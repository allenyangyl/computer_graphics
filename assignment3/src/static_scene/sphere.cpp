#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CMU462 { namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {

  // TODO:
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.

  double a = r.d.x * r.d.x + r.d.y * r.d.y + r.d.z * r.d.z;
  double b = 2 * (r.d.x * (r.o.x - o.x) + r.d.y * (r.o.y - o.y) + r.d.z * (r.o.z - o.z));
  double c = (r.o.x - o.x) * (r.o.x - o.x) + (r.o.y - o.y) * (r.o.y - o.y) + (r.o.z - o.z) * (r.o.z - o.z) - r2;
  double delta = b * b - 4 * a * c;
  if(delta < 0){
      return false;
  }
  t1 = (-b - sqrt(delta)) / 2 / a;
  t2 = (-b + sqrt(delta)) / 2 / a;
  if(t1 >= r.min_t && t1 <= r.max_t){
      return true;
  }
  else if(t1 < r.min_t && t2 >= r.min_t && t2 <= r.max_t){
      swap(t1, t2);
      return true;
  }
  return false;

}

bool Sphere::intersect(const Ray& r) const {

  // TODO:
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.
  double t1, t2;
  return test(r, t1, t2);

}

bool Sphere::intersect(const Ray& r, Intersection *i) const {

  // TODO:
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.
  double t1, t2;
  if(test(r, t1, t2)){
      r.max_t = t1;
      i -> t = t1;
      i -> n = r.o + t1 * r.d - o;
      (i -> n).normalize();
      i -> primitive = this;
      i -> bsdf = get_bsdf();
      return true;
  }
  return false;

}

void Sphere::draw(const Color& c) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color& c) const {
    //Misc::draw_sphere_opengl(o, r, c);
}


} // namespace StaticScene
} // namespace CMU462
