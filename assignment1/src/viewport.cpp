#include "viewport.h"

#include "CMU462.h"

namespace CMU462 {

void ViewportImp::set_viewbox( float x, float y, float span ) {

  // Task 4 (part 2):
  // Set svg to normalized device coordinate transformation. Your input
  // arguments are defined as SVG canvas coordinates.
  Matrix3x3 svg_2_norm = get_canvas_to_norm();
  Matrix3x3 translation = Matrix3x3::identity();
  translation(0, 0) = this->span / span;
  translation(1, 1) = this->span / span;
  translation(0, 2) = (this->span / span) * (this->x - x) / this->span / 2 - (this->span / span - 1) / 2;
  translation(1, 2) = (this->span / span) * (this->y - y) / this->span / 2 - (this->span / span - 1) / 2;
  set_canvas_to_norm(translation * svg_2_norm);
  this->x = x;
  this->y = y;
  this->span = span;

}

void ViewportImp::update_viewbox( float dx, float dy, float scale ) {

  Matrix3x3 svg_2_norm = get_canvas_to_norm();
  Matrix3x3 translation = Matrix3x3::identity();
  translation(0, 0) = 1 / scale;
  translation(1, 1) = 1 / scale;
  translation(0, 2) = dx / span / 2 - (1 / scale - 1) / 2;
  translation(1, 2) = dy / span / 2 - (1 / scale - 1) / 2;
  set_canvas_to_norm(translation * svg_2_norm);

  this->x -= dx;
  this->y -= dy;
  this->span *= scale;
  set_viewbox( x, y, span );

}

} // namespace CMU462
