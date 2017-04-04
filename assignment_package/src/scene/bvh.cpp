#include "bvh.h"

struct BVHPrimitiveInfo {
    BVHPrimitiveInfo() {}
    BVHPrimitiveInfo(QString name, int primitiveNumber, const Bounds3f &bounds)
        : name(name), primitiveNumber(primitiveNumber),
          bounds(bounds),
          centroid(.5f * bounds.min + .5f * bounds.max) {}
    int primitiveNumber;
    Bounds3f bounds;
    Point3f centroid;
    QString name;
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
    BucketInfo() {}
    int count = 0;
    Bounds3f bounds;
};

struct LinearBVHNode {
    LinearBVHNode() {}

    Bounds3f bounds;
    union {
        int primitivesOffset;   // leaf
        int secondChildOffset;  // interior
    };
    unsigned short nPrimitives;  // 0 -> interior node, 16 bytes
    unsigned char axis;          // interior node: xyz, 8 bytes
    unsigned char pad[1];        // ensure 32 byte total size
};

BVHAccel::~BVHAccel() {}

BVHBuildNode* BVHAccel::recursiveBuild(
    std::vector<BVHPrimitiveInfo> &primitiveInfo,
    int start, int end, int totalNodes,
    std::vector<std::shared_ptr<Primitive>> &orderedPrims) {

    BVHBuildNode* node;
    totalNodes++;

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

    // corner case when center bounds has no volume (create leaf)
    if (center_bounds.max[splitAxis] == center_bounds.min[splitAxis]) {
        for (int i = start; i < end; i ++) {
            int prim_num = primitiveInfo[i].primitiveNumber;
            orderedPrims.push_back(primitives[prim_num]);
        }
        node->InitLeaf(offset, num_prims, bounds);
        return node;


    } else {
        // partition into buckets
        int numBuckets = 12;
        std::vector<BucketInfo> buckets;
        buckets.resize(numBuckets);
        for (int i = start; i < end; i++) {
            int b = numBuckets * center_bounds.Offset(primitiveInfo[i].centroid)[splitAxis];
            if (b == numBuckets) b = numBuckets - 1;
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
            if (cost[i] <= minCost) {
                minCost = cost[i];
                minBucket = i;
            }
        }
        // calculate mid split
        float leafCost = num_prims;
        int middle = (start + end)/2;
        if (num_prims > maxPrimsInNode || minCost < leafCost) {
            BVHPrimitiveInfo *pmid = std::partition(&primitiveInfo[start],
                                                    &primitiveInfo[end - 1] + 1,
                [=](const BVHPrimitiveInfo &pi) {
                    int b = numBuckets * center_bounds.Offset(pi.centroid)[splitAxis];
                    if (b == numBuckets) b = numBuckets - 1;
                    return b <= minBucket;
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
        BVHBuildNode *left = recursiveBuild(primitiveInfo, start, middle, totalNodes, orderedPrims);
        BVHBuildNode *right = recursiveBuild(primitiveInfo, middle, end, totalNodes, orderedPrims);
        node->InitInterior(splitAxis, left, right);
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

    int totalNodes = 0;
    std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
    for (int i = 0; i < primitives.size(); i++) {
        primitiveInfo[i] = BVHPrimitiveInfo(primitives[i]->name, i, primitives[i]->WorldBound());
    }
    std::vector<std::shared_ptr<Primitive>> orderedPrims;
    root = recursiveBuild(primitiveInfo, 0, primitives.size(), totalNodes, orderedPrims);
    primitives.swap(orderedPrims);

    int offset = 0;
    nodes = std::vector<std::shared_ptr<LinearBVHNode>>(totalNodes);
    flattenBVHTree(root, offset);

    std::cout << "BVH Build Time: " + timer.elapsed();
}

int BVHAccel::flattenBVHTree(BVHBuildNode *node, int offset) {
    LinearBVHNode linearNode;
    nodes[offset] = std::make_shared<LinearBVHNode>();
    linearNode.bounds.min = node->bounds.min;
    linearNode.bounds.max = node->bounds.max;
    int myOffset = offset++;
    // leaf node
    if (node->nPrimitives > 0) {
        linearNode.primitivesOffset = node->firstPrimOffset;
        linearNode.nPrimitives = node->nPrimitives;

    // interior node (depth first search array)
    } else {
        linearNode.axis = node->splitAxis;
        linearNode.nPrimitives = 0;
        flattenBVHTree(node->children[0], offset);
        linearNode.secondChildOffset = flattenBVHTree(node->children[1], offset);
    }
    return myOffset;
}

bool BVHAccel::Intersect(const Ray &ray, Intersection *isect) const
{
    //TODO
    if (root->nPrimitives == 0) return false;
    bool hit = false;
    Vector3f invDir(1.f/ray.direction.x, 1.f/ray.direction.y, 1.f/ray.direction.z);
    int dirIsNeg[3] = {invDir.x < 0.f, invDir.y < 0.f, invDir.z < 0.f};
    int toVisit = 0, currIndex = 0;
    int nodes2visit[64]; // stack
    while (true) {
        std::shared_ptr<LinearBVHNode> node = nodes[currIndex];
        float t;
        if (node->bounds.Intersect(ray, &t)) {
            // leaf node
            if (node->nPrimitives > 0) {
                for (int i = 0; i < node->nPrimitives; i++) {
                    if (primitives[node->primitivesOffset + i]->Intersect(ray, isect)) hit = true;
                }
                if (toVisit == 0) break;
                currIndex = nodes2visit[--toVisit];
            // interior node
            } else {
                if (dirIsNeg[node->axis]) {
                    nodes2visit[toVisit++] = currIndex +1;
                    currIndex = node->secondChildOffset;
                } else {
                    nodes2visit[toVisit++] = node->secondChildOffset;
                    currIndex = currIndex +1;
                }
            }
        } else {
            if (toVisit == 0) break;
            currIndex = nodes2visit[--toVisit];
        }
    }
    return hit;
}
