#include "triangle.h"

#include "CMU462/CMU462.h"
#include "GL/glew.h"

namespace CMU462 { namespace StaticScene {

Triangle::Triangle(const Mesh* mesh, size_t v1, size_t v2, size_t v3) :
    mesh(mesh), v1(v1), v2(v2), v3(v3) { }

BBox Triangle::get_bbox() const {

  // TODO:
  // compute the bounding box of the triangle
  return BBox(
              min(min(mesh->positions[v1].x, mesh->positions[v2].x), mesh->positions[v3].x),
              min(min(mesh->positions[v1].y, mesh->positions[v2].y), mesh->positions[v3].y),
              min(min(mesh->positions[v1].z, mesh->positions[v2].z), mesh->positions[v3].z),
              max(max(mesh->positions[v1].x, mesh->positions[v2].x), mesh->positions[v3].x),
              max(max(mesh->positions[v1].y, mesh->positions[v2].y), mesh->positions[v3].y),
              max(max(mesh->positions[v1].z, mesh->positions[v2].z), mesh->positions[v3].z)
          );
}

bool Triangle::intersect(const Ray& r) const {

  // TODO: implement ray-triangle intersection
  double data[9];
  data[0] = mesh->positions[v2].x - mesh->positions[v1].x;
  data[1] = mesh->positions[v3].x - mesh->positions[v1].x;
  data[2] = -r.d.x;
  data[3] = mesh->positions[v2].y - mesh->positions[v1].y;
  data[4] = mesh->positions[v3].y - mesh->positions[v1].y;
  data[5] = -r.d.y;
  data[6] = mesh->positions[v2].z - mesh->positions[v1].z;
  data[7] = mesh->positions[v3].z - mesh->positions[v1].z;
  data[8] = -r.d.z;
  Matrix3x3 M(data);
  Vector3D p = r.o - mesh->positions[v1];
  Vector3D x = M.inv() * p;
  if (x.x >= 0 && x.y >= 0 && x.x + x.y <= 1 && x.z >= r.min_t && x.z <= r.max_t){
      return true;
  }
  return false;
}

bool Triangle::intersect(const Ray& r, Intersection *isect) const {

  // TODO:
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly
  double data[9];
  data[0] = mesh->positions[v2].x - mesh->positions[v1].x;
  data[1] = mesh->positions[v3].x - mesh->positions[v1].x;
  data[2] = -r.d.x;
  data[3] = mesh->positions[v2].y - mesh->positions[v1].y;
  data[4] = mesh->positions[v3].y - mesh->positions[v1].y;
  data[5] = -r.d.y;
  data[6] = mesh->positions[v2].z - mesh->positions[v1].z;
  data[7] = mesh->positions[v3].z - mesh->positions[v1].z;
  data[8] = -r.d.z;
  Matrix3x3 M(data);
  //cout << mesh -> positions[v1] << " " << mesh -> positions[v2] << " " << mesh -> positions[v3] << endl;
  Vector3D p = r.o - mesh->positions[v1];
  Vector3D x = M.inv() * p;
  if (x.x >= 0 && x.y >= 0 && x.x + x.y <= 1 && x.z >= r.min_t && x.z <= r.max_t){
      //cout << x << endl;
      r.max_t = x.z;
      isect -> t = x.z;
      isect -> n = (1 - x.x - x.y) * mesh -> normals[v1] +
                   x.x * mesh -> normals[v2] + x.y * mesh -> normals[v3];
      (isect -> n).normalize();
      if(dot(isect -> n, r.d) > 0){
          isect -> n = -isect -> n;
      }
      isect -> primitive = this;
      isect -> bsdf = mesh -> get_bsdf();
      return true;
  }
  return false;
}

void Triangle::draw(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_TRIANGLES);
  glVertex3d(mesh->positions[v1].x,
             mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x,
             mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x,
             mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}

void Triangle::drawOutline(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_LINE_LOOP);
  glVertex3d(mesh->positions[v1].x,
             mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x,
             mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x,
             mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}



} // namespace StaticScene
} // namespace CMU462
