#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CMU462 {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

  // TODO:
  // Implement ray - bounding box intersection test
  // If the ray intersected the bouding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.

  double t0x = (min.x - r.o.x) / r.d.x;
  double t1x = (max.x - r.o.x) / r.d.x;
  double t0y = (min.y - r.o.y) / r.d.y;
  double t1y = (max.y - r.o.y) / r.d.y;
  double t0z = (min.z - r.o.z) / r.d.z;
  double t1z = (max.z - r.o.z) / r.d.z;

  double tx_max = std::max(t0x, t1x);
  double tx_min = std::min(t0x, t1x);
  double ty_max = std::max(t0y, t1y);
  double ty_min = std::min(t0y, t1y);
  double tz_max = std::max(t0z, t1z);
  double tz_min = std::min(t0z, t1z);

  double t0_new = std::max(std::max(tx_min, ty_min), tz_min);
  double t1_new = std::min(std::min(tx_max, ty_max), tz_max);

  if(t1_new >= t0_new && t1_new <= t1 && t0_new >= t0){
      t0 = t0_new;
      t1 = t1_new;
      return true;
  }
  return false;

  //std::cout << min << max << r.o << r.d << t0 << " " << t1 << std::endl;
  //return t1 >= t0 && (t1 >= r.min_t || t0 <= r.max_t);

}

void BBox::draw(Color c) const {

  glColor4f(c.r, c.g, c.b, c.a);

	// top
	glBegin(GL_LINE_STRIP);
	glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
	glEnd();

	// bottom
	glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
	glEnd();

	// side
	glBegin(GL_LINES);
	glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
	glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
	glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
	glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
	glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CMU462
