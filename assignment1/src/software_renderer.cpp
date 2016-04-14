#include "software_renderer.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

#include "triangulation.h"

using namespace std;

namespace CMU462 {


// Implements SoftwareRenderer //

void SoftwareRendererImp::draw_svg( SVG& svg ) {

  // set top level transformation
  transformation = canvas_to_screen;

  // set render target
  set_render_target( this -> render_target, this -> target_w, this -> target_h );

  // draw all elements
  for ( size_t i = 0; i < svg.elements.size(); ++i ) {
    draw_element(svg.elements[i]);
}

  // draw canvas outline
  Vector2D a = transform(Vector2D(    0    ,     0    )); a.x--; a.y--;
  Vector2D b = transform(Vector2D(svg.width,     0    )); b.x++; b.y--;
  Vector2D c = transform(Vector2D(    0    ,svg.height)); c.x--; c.y++;
  Vector2D d = transform(Vector2D(svg.width,svg.height)); d.x++; d.y++;

  rasterize_line(a.x, a.y, b.x, b.y, Color::Black);
  rasterize_line(a.x, a.y, c.x, c.y, Color::Black);
  rasterize_line(d.x, d.y, b.x, b.y, Color::Black);
  rasterize_line(d.x, d.y, c.x, c.y, Color::Black);

  // resolve and send to render target
  resolve();

}

void SoftwareRendererImp::set_sample_rate( size_t sample_rate ) {

  // Task 3:
  // You may want to modify this for supersampling support
  this->sample_rate = sample_rate;

}

void SoftwareRendererImp::set_render_target( unsigned char* render_target,
                                             size_t width, size_t height ) {

  // Task 3:
  // You may want to modify this for supersampling support
  this->render_target = render_target;
  this->target_w = width;
  this->target_h = height;
  unsigned char* supersample_target = new unsigned char [4 * sample_rate * this -> target_w * sample_rate * this -> target_h];
  this -> supersample_target = supersample_target;
  clear_target();

}

void SoftwareRendererImp::draw_element( SVGElement* element ) {

  // Task 4 (part 1):
  // Modify this to implement the transformation stack
  Matrix3x3 transformation_temp = transformation;
  transformation = transformation * element->transform;

  switch(element->type) {
    case POINT:
      draw_point(static_cast<Point&>(*element));
      break;
    case LINE:
      draw_line(static_cast<Line&>(*element));
      break;
    case POLYLINE:
      draw_polyline(static_cast<Polyline&>(*element));
      break;
    case RECT:
      draw_rect(static_cast<Rect&>(*element));
      break;
    case POLYGON:
      draw_polygon(static_cast<Polygon&>(*element));
      break;
    case ELLIPSE:
      draw_ellipse(static_cast<Ellipse&>(*element));
      break;
    case IMAGE:
      draw_image(static_cast<Image&>(*element));
      break;
    case GROUP:
      draw_group(static_cast<Group&>(*element));
      break;
    default:
      break;
  }

  transformation = transformation_temp;

}


// Primitive Drawing //

void SoftwareRendererImp::draw_point( Point& point ) {

  Vector2D p = transform(point.position);
  rasterize_point( p.x, p.y, point.style.fillColor );

}

void SoftwareRendererImp::draw_line( Line& line ) {

  Vector2D p0 = transform(line.from);
  Vector2D p1 = transform(line.to);
  rasterize_line( p0.x, p0.y, p1.x, p1.y, line.style.strokeColor );

}

void SoftwareRendererImp::draw_polyline( Polyline& polyline ) {

  Color c = polyline.style.strokeColor;

  if( c.a != 0 ) {
    int nPoints = polyline.points.size();
    for( int i = 0; i < nPoints - 1; i++ ) {
      Vector2D p0 = transform(polyline.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polyline.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_rect( Rect& rect ) {

  Color c;

  // draw as two triangles
  float x = rect.position.x;
  float y = rect.position.y;
  float w = rect.dimension.x;
  float h = rect.dimension.y;

  Vector2D p0 = transform(Vector2D(   x   ,   y   ));
  Vector2D p1 = transform(Vector2D( x + w ,   y   ));
  Vector2D p2 = transform(Vector2D(   x   , y + h ));
  Vector2D p3 = transform(Vector2D( x + w , y + h ));

  // draw fill
  c = rect.style.fillColor;
  if (c.a != 0 ) {
    rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    rasterize_triangle( p2.x, p2.y, p1.x, p1.y, p3.x, p3.y, c );
  }

  // draw outline
  c = rect.style.strokeColor;
  if( c.a != 0 ) {
    rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    rasterize_line( p1.x, p1.y, p3.x, p3.y, c );
    rasterize_line( p3.x, p3.y, p2.x, p2.y, c );
    rasterize_line( p2.x, p2.y, p0.x, p0.y, c );
  }

}

void SoftwareRendererImp::draw_polygon( Polygon& polygon ) {

  Color c;

  // draw fill
  c = polygon.style.fillColor;
  if( c.a != 0 ) {

    // triangulate
    vector<Vector2D> triangles;
    triangulate( polygon, triangles );

    // draw as triangles
    for (size_t i = 0; i < triangles.size(); i += 3) {
      Vector2D p0 = transform(triangles[i + 0]);
      Vector2D p1 = transform(triangles[i + 1]);
      Vector2D p2 = transform(triangles[i + 2]);
      rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    }
  }

  // draw outline
  c = polygon.style.strokeColor;
  if( c.a != 0 ) {
    int nPoints = polygon.points.size();
    for( int i = 0; i < nPoints; i++ ) {
      Vector2D p0 = transform(polygon.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polygon.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_ellipse( Ellipse& ellipse ) {

  // Extra credit

}

void SoftwareRendererImp::draw_image( Image& image ) {

  Vector2D p0 = transform(image.position);
  Vector2D p1 = transform(image.position + image.dimension);

  rasterize_image( p0.x, p0.y, p1.x, p1.y, image.tex );
}

void SoftwareRendererImp::draw_group( Group& group ) {

  for ( size_t i = 0; i < group.elements.size(); ++i ) {
    draw_element(group.elements[i]);
  }

}

// Rasterization //

// The input arguments in the rasterization functions
// below are all defined in screen space coordinates

void SoftwareRendererImp::rasterize_point( float x, float y, Color color ) {

  // fill in the nearest pixel
  int sx = (int) floor(x);
  int sy = (int) floor(y);

  // check bounds
  if ( sx < 0 || sx >= target_w ) return;
  if ( sy < 0 || sy >= target_h ) return;

  // fill sample - NOT doing alpha blending!
  /*render_target[4 * (sx + sy * target_w)    ] = (uint8_t) (color.r * 255);
  render_target[4 * (sx + sy * target_w) + 1] = (uint8_t) (color.g * 255);
  render_target[4 * (sx + sy * target_w) + 2] = (uint8_t) (color.b * 255);
  render_target[4 * (sx + sy * target_w) + 3] = (uint8_t) (color.a * 255);*/

  for (int x_samp = 0; x_samp < sample_rate; x_samp++){
      for (int y_samp = 0; y_samp < sample_rate; y_samp++){
          float a = (1 - (1 - color.a) * (1 - (float) supersample_target[4 * (sx * sample_rate + x_samp + (sy * sample_rate + y_samp) * sample_rate * target_w) + 3] / 255));
          supersample_target[4 * (sx * sample_rate + x_samp + (sy * sample_rate + y_samp) * sample_rate * target_w)    ] =
            (uint8_t) (((1 - color.a) * (float) supersample_target[4 * (sx * sample_rate + x_samp + (sy * sample_rate + y_samp) * sample_rate * target_w)    ] * a / 255 + color.a * color.r) * 255);
          supersample_target[4 * (sx * sample_rate + x_samp + (sy * sample_rate + y_samp) * sample_rate * target_w) + 1] =
            (uint8_t) (((1 - color.a) * (float) supersample_target[4 * (sx * sample_rate + x_samp + (sy * sample_rate + y_samp) * sample_rate * target_w) + 1] * a / 255 + color.a * color.g) * 255);
          supersample_target[4 * (sx * sample_rate + x_samp + (sy * sample_rate + y_samp) * sample_rate * target_w) + 2] =
            (uint8_t) (((1 - color.a) * (float) supersample_target[4 * (sx * sample_rate + x_samp + (sy * sample_rate + y_samp) * sample_rate * target_w) + 2] * a / 255 + color.a * color.b) * 255);
          supersample_target[4 * (sx * sample_rate + x_samp + (sy * sample_rate + y_samp) * sample_rate * target_w) + 3] =
            (uint8_t) (a * 255);
      }
  }

}

void SoftwareRendererImp::rasterize_line( float x0, float y0,
                                          float x1, float y1,
                                          Color color) {

  // Task 1:
  // Implement line rasterization
  bool incre = abs(y1 - y0) > abs(x1 - x0);
  // swap x and y axis if abs(slope) > 1
  if ( incre ){
      swap(x0, y0);
      swap(x1, y1);
  }
  // swap the points if x0 > x1
  if ( x0 > x1 ){
      swap(x0, x1);
      swap(y0, y1);
  }
  // initialize the locations
  float slope = (y1 - y0) / (x1 - x0);
  int x_start = (int) floor(x0);
  int x_end = (int) floor(x1);
  int y = (int) round(y0 - slope * (x0 - x_start - 0.5));
  float error = y0 - y - slope * (x0 - x_start - 0.5);
  float gap;
  // increase x from start to end
  for(int x = x_start; x <= x_end; x++){
      // end points
      if( x == x_start ){ gap = 0.5 - x + x_start; }
      else if( x == x_end ) { gap = 0.5 + x - x_end; }
      else { gap = 1; }
      // rasterization
      if ( incre ){
          rasterize_point( y - 1, x, color * (0.5 - error) * gap );
          rasterize_point( y, x, color * (0.5 + error) * gap );
      }
      else{
          rasterize_point( x, y - 1, color * (0.5 - error) * gap );
          rasterize_point( x, y, color * (0.5 + error) * gap );
      }
      // update error and y
      error += slope;
      if(error >= 0.5){
          error -= 1;
          y++;
      }
      if(error < -0.5){
          error += 1;
          y--;
      }
  }

}

void SoftwareRendererImp::rasterize_triangle( float x0, float y0,
                                              float x1, float y1,
                                              float x2, float y2,
                                              Color color ) {
  // Task 2:
  // Implement triangle rasterization
  if((x1 - x0) * (y2 - y0) < (x2 - x0) * (y1 - y0)){
      swap(x1, x2);
      swap(y1, y2);
  }

  float x_min = min(x0, min(x1, x2));
  float x_max = max(x0, max(x1, x2));
  float y_min = min(y0, min(y1, y2));
  float y_max = max(y0, max(y1, y2));

  for(int x = (int) floor(x_min); x <= (int) ceil(x_max); x++){
      for(int y = (int) floor(y_min); y <= (int) ceil(y_max); y++){
          for (int x_samp = 0; x_samp < sample_rate; x_samp++){
              float x_coor = (float) (2 * x_samp + 1) / (2 * sample_rate);
              for (int y_samp = 0; y_samp < sample_rate; y_samp++){
                  float y_coor = (float) (2 * y_samp + 1) / (2 * sample_rate);
                  if( (x + x_coor - x0) * (y1 - y0) <= (y + y_coor - y0) * (x1 - x0) &&
                  (x + x_coor - x1) * (y2 - y1) <= (y + y_coor - y1) * (x2 - x1) &&
                  (x + x_coor - x2) * (y0 - y2) <= (y + y_coor - y2) * (x0 - x2) ){
                      float a = (1 - (1 - color.a) * (1 - (float) supersample_target[4 * (x * sample_rate + x_samp + (y * sample_rate + y_samp) * sample_rate * target_w) + 3] / 255));
                      supersample_target[4 * (x * sample_rate + x_samp + (y * sample_rate + y_samp) * sample_rate * target_w)    ] =
                        (uint8_t) (((1 - color.a) * (float) supersample_target[4 * (x * sample_rate + x_samp + (y * sample_rate + y_samp) * sample_rate * target_w)    ] * a / 255 + color.a * color.r) * 255);
                      supersample_target[4 * (x * sample_rate + x_samp + (y * sample_rate + y_samp) * sample_rate * target_w) + 1] =
                        (uint8_t) (((1 - color.a) * (float) supersample_target[4 * (x * sample_rate + x_samp + (y * sample_rate + y_samp) * sample_rate * target_w) + 1] * a / 255 + color.a * color.g) * 255);
                      supersample_target[4 * (x * sample_rate + x_samp + (y * sample_rate + y_samp) * sample_rate * target_w) + 2] =
                        (uint8_t) (((1 - color.a) * (float) supersample_target[4 * (x * sample_rate + x_samp + (y * sample_rate + y_samp) * sample_rate * target_w) + 2] * a / 255 + color.a * color.b) * 255);
                      supersample_target[4 * (x * sample_rate + x_samp + (y * sample_rate + y_samp) * sample_rate * target_w) + 3] =
                        (uint8_t) (a * 255);
                  }
              }
          }
      }
  }

}

void SoftwareRendererImp::rasterize_image( float x0, float y0,
                                           float x1, float y1,
                                           Texture& tex ) {
  // Task 5:
  // Implement image rasterization
  Sampler2DImp sampler;
  sampler.generate_mips(tex, 0);
  int x_start = (int) round(x0);
  int x_end = (int) round(x1);
  int y_start = (int) round(y0);
  int y_end = (int) round(y1);
  float u_scale = tex.mipmap[0].width / (x1 - x0);
  float v_scale = tex.mipmap[0].height / (y1 - y0);
  for(int x = x_start; x < x_end; x++){
      for(int y = y_start; y < y_end; y++){
          //rasterize_point( x, y, sampler.sample_bilinear(tex, (x + 0.5 - x0) / (x1 - x0), (y + 0.5 - y0) / (y1 - y0), 0) );
          rasterize_point( x, y, sampler.sample_trilinear(tex, (x + 0.5 - x0) / (x1 - x0), (y + 0.5 - y0) / (y1 - y0), u_scale, v_scale) );
      }
  }

}

// resolve samples to render target
void SoftwareRendererImp::resolve( void ) {

  // Task 3:
  // Implement supersampling
  // You may also need to modify other functions marked with "Task 3".

  unsigned char* supersample_target = this -> supersample_target;

  for(int x = 0; x < this -> target_w; x++){
      for(int y = 0; y < this -> target_h; y++){
          float r = 0, g = 0, b = 0, a = 0;
          for (int x_samp = 0; x_samp < sample_rate; x_samp++){
              for (int y_samp = 0; y_samp < sample_rate; y_samp++){
                      r += supersample_target[4 * (x * sample_rate + x_samp + (y * sample_rate + y_samp) * sample_rate * target_w)    ];
                      g += supersample_target[4 * (x * sample_rate + x_samp + (y * sample_rate + y_samp) * sample_rate * target_w) + 1];
                      b += supersample_target[4 * (x * sample_rate + x_samp + (y * sample_rate + y_samp) * sample_rate * target_w) + 2];
                      a += supersample_target[4 * (x * sample_rate + x_samp + (y * sample_rate + y_samp) * sample_rate * target_w) + 3];
                  }
              }
          render_target[4 * (x + y * target_w)    ] = (uint8_t) (r / sample_rate / sample_rate);
          render_target[4 * (x + y * target_w) + 1] = (uint8_t) (g / sample_rate / sample_rate);
          render_target[4 * (x + y * target_w) + 2] = (uint8_t) (b / sample_rate / sample_rate);
          render_target[4 * (x + y * target_w) + 3] = (uint8_t) (a / sample_rate / sample_rate);
      }
  }
  delete[] supersample_target;

  return;

}


} // namespace CMU462
