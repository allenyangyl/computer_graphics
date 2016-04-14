#include "bvh.h"

#include "CMU462/CMU462.h"
#include "static_scene/triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CMU462 { namespace StaticScene {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  this->primitives = _primitives;

  // TODO:
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

  BBox bb;
  for (size_t i = 0; i < primitives.size(); ++i) {
    bb.expand(primitives[i]->get_bbox());
  }

  root = new BVHNode(bb, 0, primitives.size());
  newNode(root, primitives, 0, primitives.size(), max_leaf_size);

}

void BVHAccel::newNode(BVHNode*& root, const std::vector<Primitive*>& primitives, size_t start, size_t end, size_t max_leaf_size) {
  if(end - start > max_leaf_size){
      size_t mid = (start + end) / 2;
      BBox bb_left, bb_right;
      for (size_t i = start; i < mid; ++i) {
        bb_left.expand(primitives[i]->get_bbox());
      }
      for (size_t i = mid; i < end; ++i) {
        bb_right.expand(primitives[i]->get_bbox());
      }

      BVHNode* left = new BVHNode(bb_left, start, mid - start);
      BVHNode* right = new BVHNode(bb_right, mid, end - mid);
      newNode(left, primitives, start, mid, max_leaf_size);
      newNode(right, primitives, mid, end, max_leaf_size);
      root -> l = left;
      root -> r = right;
  }
  return;
}

BVHAccel::~BVHAccel() {

  // TODO:
  // Implement a proper destructor for your BVH accelerator aggregate
  deleteNode(root);
}

void BVHAccel::deleteNode(BVHNode*& root){
    if(root -> isLeaf()){
        delete root;
    }
    else{
        deleteNode(root -> l);
        deleteNode(root -> r);
    }
}

BBox BVHAccel::get_bbox() const {
  return root->bb;
}

bool BVHAccel::intersect(const Ray &ray) const {

  // TODO:
  // Implement ray - bvh aggregate intersection test. A ray intersects
  // with a BVH aggregate if and only if it intersects a primitive in
  // the BVH that is not an aggregate.
  Intersection i;
  return closestHit(root, ray, &i);

}

bool BVHAccel::intersect(const Ray &ray, Intersection *i) const {

  // TODO:
  // Implement ray - bvh aggregate intersection test. A ray intersects
  // with a BVH aggregate if and only if it intersects a primitive in
  // the BVH that is not an aggregate. When an intersection does happen.
  // You should store the non-aggregate primitive in the intersection data
  // and not the BVH aggregate itself.

  return closestHit(root, ray, i);

}

bool BVHAccel::closestHit(const BVHNode* root, const Ray &ray, Intersection* isect) const{
    if(root -> isLeaf()) {
        bool hit = false;
        for (size_t p = root -> start; p < root -> start + root -> range; ++p) {
          if(primitives[p]->intersect(ray, isect)){
              hit = true;
          }
        }
        return hit;
    }
    else{
        double t0_left = -INF_D, t1_left = INF_D, t0_right = -INF_D, t1_right = INF_D, t_max;
        bool hitLeft, hitRight, hit;
        hitLeft = (root -> l -> bb).intersect(ray, t0_left, t1_left);
        hitRight = (root -> r -> bb).intersect(ray, t0_right, t1_right);
        if(hitLeft && hitRight){
            BVHNode* first = (t0_left <= t0_right) ? root -> l : root -> r;
            BVHNode* second = (t0_left <= t0_right) ? root -> r : root -> l;
            t_max = max(t0_left, t0_right);
            hit = closestHit(first, ray, isect);
            if(!hit){
                hit = closestHit(second, ray, isect);
            }
            else if(t_max < isect -> t){
                closestHit(second, ray, isect);
            }
            return hit;
        }
        else if(hitLeft){
            return closestHit(root -> l, ray, isect);
        }
        else if(hitRight){
            return closestHit(root -> r, ray, isect);
        }
        else{
            return false;
        }
    }
}

}  // namespace StaticScene
}  // namespace CMU462
