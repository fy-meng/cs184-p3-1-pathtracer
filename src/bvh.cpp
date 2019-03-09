#include "bvh.h"

#include "CGL/CGL.h"
#include "static_scene/triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL { namespace StaticScene {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  root = construct_bvh(_primitives, max_leaf_size);

}

BVHAccel::~BVHAccel() {
  if (root) delete root;
}

BBox BVHAccel::get_bbox() const {
  return root->bb;
}

void BVHAccel::draw(BVHNode *node, const Color& c, float alpha) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->draw(c, alpha);
  } else {
    draw(node->l, c, alpha);
    draw(node->r, c, alpha);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color& c, float alpha) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->drawOutline(c, alpha);
  } else {
    drawOutline(node->l, c, alpha);
    drawOutline(node->r, c, alpha);
  }
}

BVHNode *BVHAccel::construct_bvh(const std::vector<Primitive*>& prims, size_t max_leaf_size) {
  
  // (Part 2.1):
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

  BBox bbox;
  for (Primitive *p : prims) {
      BBox bb = p->get_bbox();
      bbox.expand(bb);
  }

  auto *node = new BVHNode(bbox);

  if (prims.size() > max_leaf_size) {
    // compute the axis with largest extent
    int idx = bbox.extent[0] < bbox.extent[1] ? 1 : 0;
    idx = bbox.extent[idx] < bbox.extent[2] ? 2 : idx;

    double split = 0;
    for (Primitive *p : prims)
      split += p->get_bbox().centroid()[idx];
    split /= prims.size();

    auto *left = new vector<Primitive *>(), *right = new vector<Primitive *>();
    for (Primitive *p : prims)
      (p->get_bbox().centroid()[idx] <= split ? left : right)->push_back(p);

    if (left->empty() || right->empty())
      // this only happens if all centroid are the same
      // and thus no point to split anymore
      return node;

    node->l = construct_bvh(*left, max_leaf_size);
    node->r = construct_bvh(*right, max_leaf_size);
  } else  // leaf node
    node->prims = new vector<Primitive *>(prims);

  return node;
}


bool BVHAccel::intersect(const Ray& r, BVHNode *node) const {

  // (Part 2.3):
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.

  double t1, t2;
  if (node->bb.intersect(r, t1, t2)) {
    if (node->isLeaf()) {
      for (Primitive *p : *(node->prims))
        if (p->intersect(r))
          return true;
      return false;
    } else {
      return intersect(r, node->l) || intersect(r, node->r);
    }
  } else
    return false;
}

bool BVHAccel::intersect(const Ray& r, Intersection* i, BVHNode *node) const {

  // (Part 2.3):
  // Fill in the intersect function.

  double t1, t2;
  if (node->bb.intersect(r, t1, t2)) {
    bool hit = false;
    if (node->isLeaf()) {
      for (Primitive *p : *(node->prims)) {
        total_isects++;
        if (p->intersect(r, i))
          hit = true;
      }
      return hit;
    } else {
      // intersection will be automatically set by recursion
      if (intersect(r, i, node->l))
        hit = true;
      if (intersect(r, i, node->r))
        hit = true;
      return hit;
    }
  } else
    return false;
}

}  // namespace StaticScene
}  // namespace CGL
