#include "csg.h"

struct CSGNode {
    void InitLeaf(Primitive p) {
        primitive = p;
        oper = -1;
    }
    void InitInterior(int operation, CSGNode *c0, CSGNode *c1) {
        children.push_back(c0);
        children.push_back(c1);
        oper = operation;
    }
    std::vector<CSGNode*> children;
    Primitive primitive;
    int oper;
};

void deleteNode(CSGNode* node) {
    if (node->children.size() != 0) {
        for (auto i : node->children) deleteNode(i);
    }
    delete node;
}

CSG::~CSG() {
    deleteNode(root);
}

CSG::CSG() {

}

bool CSG::Intersect(const Ray &ray, Intersection *isect) const {

}
