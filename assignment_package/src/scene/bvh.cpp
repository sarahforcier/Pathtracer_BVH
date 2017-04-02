#include "bvh.h"

struct BVHPrimitiveInfo {
    BVHPrimitiveInfo() {}
    BVHPrimitiveInfo(int primitiveNumber, const Bounds3f &bounds)
        : primitiveNumber(primitiveNumber),
          bounds(bounds),
          centroid(.5f * bounds.min + .5f * bounds.max) {}
    int primitiveNumber;
    Bounds3f bounds;
    Point3f centroid;
};

struct BVHBuildNode {
    // BVHBuildNode Public Methods
    void InitLeaf(int first, int n, const Bounds3f &b) {
        firstPrimOffset = first;
        nPrimitives = n;
        bounds = b;
        children[0] = children[1] = nullptr;
    }
    void InitInterior(int axis, BVHBuildNode *c0, BVHBuildNode *c1) {
        children[0] = c0;
        children[1] = c1;
        bounds = Union(c0->bounds, c1->bounds);
        splitAxis = axis;
        nPrimitives = 0;
    }

    Bounds3f bounds;
    BVHBuildNode *children[2];
    int splitAxis, firstPrimOffset, nPrimitives;
};

struct BucketInfo {
    int count = 0;
    Bounds3f bounds;
};

struct LinearBVHNode {
    Bounds3f bounds;
    union {
        int primitivesOffset;   // leaf
        int secondChildOffset;  // interior
    };
    unsigned short nPrimitives;  // 0 -> interior node, 16 bytes
    unsigned char axis;          // interior node: xyz, 8 bytes
    unsigned char pad[1];        // ensure 32 byte total size
};

BVHAccel::~BVHAccel()
{
    delete [] nodes;
}

BVHBuildNode* BVHAccel::recursiveBuild(
    std::vector<BVHPrimitiveInfo> &primitiveInfo,
    int start, int end, std::vector<std::shared_ptr<Primitive>> &orderedPrims) {

    BVHBuildNode* node;

    // compute bounds of all primitives
    Bounds3f bounds;
    for (int i = start; i < end; i++) {
        bounds = Union(bounds, primitiveInfo[i].bounds);
    }

    // base case
    int num_prims = end - start;
    int offset = orderedPrims.size();
    if (num_prims == 1) {
        for (int i = start; i < end; i ++) {
            int prim_num = primitiveInfo[i].primitiveNumber;
            orderedPrims.push_back(primitives[prim_num]);
        }
        node->InitLeaf(offset, num_prims, bounds);
        return node;
    }

    // find longest axis
    Bounds3f center_bounds;
    for (int i = start; i < end; i++) {
        center_bounds = Union(center_bounds, primitiveInfo[i].centroid);
    }
    int splitAxis = center_bounds.MaximumExtent();
    int middle = (start + end)/2;

    // corner case when center bounds has no volume (create leaf)
    if (center_bounds.max[splitAxis] == center_bounds.max[splitAxis]) {
        for (int i = start; i < end; i ++) {
            int prim_num = primitiveInfo[i].primitiveNumber;
            orderedPrims.push_back(primitives[prim_num]);
        }
        node->InitLeaf(offset, num_prims, bounds);
        return node;


    } else {
        // partition into buckets
        int numBuckets = 12;
        BucketInfo buckets[numBuckets];
        for (int i = start; i < end; i++) {
            int b = numBuckets * center_bounds.Offset(primitiveInfo[i].centroid)[splitAxis];
            if (b = numBuckets) b = numBuckets - 1;
            buckets[b].count ++;
            buckets[b].bounds = Union(buckets[b].bounds, primitiveInfo[i].bounds);
        }
        // compute cost
        float cost[numBuckets - 1];
        for (int i = 0; i < numBuckets; i++) {
            Bounds3f left; int countL = 0;
            for (int j = 0; j <= i; j++) { // left side
                left = Union(left, buckets[j].bounds);
                countL += buckets[j].count;
            }
            Bounds3f right; int countR = 0;
            for (int j = i + 1; j < numBuckets; j++) { // right side
                right = Union(right, buckets[j].bounds);
                countR += buckets[j].count;
            }
            cost[i] = 0.125f + (countL * left.SurfaceArea() +
                                countR * right.SurfaceArea()) / bounds.SurfaceArea();
        }
        // choose split to minimize cost
        float minCost = cost[0];
        int minBucket = 0;
        for (int i = 1; i < numBuckets; i ++) {
            if (cost[i] < minCost) {
                minCost = cost[i];
                minBucket = i;
            }
        }
        // calculate mid split
        float leafCost = num_prims;
        if (num_prims > maxPrimsInNode || minCost < leafCost) {
            BVHPrimitiveInfo *pmid = std::partition(&primitiveInfo[start],
                                                    &primitiveInfo[end] + 1,
                [=](const BVHPrimitiveInfo &pi) {
                    int b = numBuckets * center_bounds.Offset(pi.centroid)[splitAxis];
                    if (b == numBuckets) b = numBuckets - 1;
                    return b <= minCost;
                });
            middle = pmid - &primitiveInfo[0];
        } else {
            for (int i = start; i < end; i ++) {
                int prim_num = primitiveInfo[i].primitiveNumber;
                orderedPrims.push_back(primitives[prim_num]);
            }
            node->InitLeaf(offset, num_prims, bounds);
            return node;
        }
        // create node
        node->InitInterior(splitAxis,
                recursiveBuild(primitiveInfo, start, middle, orderedPrims),
                recursiveBuild(primitiveInfo, middle, end, orderedPrims));
    }

    return node;
}

// Constructs an array of BVHPrimitiveInfos, recursively builds a node-based BVH
// from the information, then optimizes the memory of the BVH
BVHAccel::BVHAccel(const std::vector<std::shared_ptr<Primitive> > &p, int maxPrimsInNode)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), primitives(p)
{
    //TODO
    QTime timer = QTime();
    timer.start();
    if (primitives.size() == 0) {
        std::cout << "BVH Build Time: " + timer.elapsed(); return;
    }

    std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
    for (int i = 0; i < primitives.size(); i++) {
        primitiveInfo[i] = {i, primitives[i]->WorldBound()};
    }
    std::vector<std::shared_ptr<Primitive>> orderedPrims;
    root = recursiveBuild(primitiveInfo, 0, primitives.size(), orderedPrims);
    primitives.swap(orderedPrims);

    int offset = 0;
    flattenBVHTree(root, &offset);

    std::cout << "BVH Build Time: " + timer.elapsed();
}

int BVHAccel::flattenBVHTree(BVHBuildNode *node, int *offset) {
    return 0;
}

bool BVHAccel::Intersect(const Ray &ray, Intersection *isect) const
{
    //TODO
    return false;
}
