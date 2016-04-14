#include "texture.h"

#include <assert.h>
#include <iostream>
#include <algorithm>

using namespace std;

namespace CMU462 {

inline void uint8_to_float( float dst[4], unsigned char* src ) {
  uint8_t* src_uint8 = (uint8_t *)src;
  dst[0] = src_uint8[0] / 255.f;
  dst[1] = src_uint8[1] / 255.f;
  dst[2] = src_uint8[2] / 255.f;
  dst[3] = src_uint8[3] / 255.f;
}

inline void float_to_uint8( unsigned char* dst, float src[4] ) {
  uint8_t* dst_uint8 = (uint8_t *)dst;
  dst_uint8[0] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[0])));
  dst_uint8[1] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[1])));
  dst_uint8[2] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[2])));
  dst_uint8[3] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[3])));
}

void Sampler2DImp::generate_mips(Texture& tex, int startLevel) {

  // NOTE(sky):
  // The starter code allocates the mip levels and generates a level
  // map simply fills each level with a color that differs from its
  // neighbours'. The reference solution uses trilinear filtering
  // and it will only work when you have mipmaps.

  // Task 6: Implement this

  // check start level
  if ( startLevel >= tex.mipmap.size() ) {
    std::cerr << "Invalid start level";
  }

  // allocate sublevels
  int baseWidth  = tex.mipmap[startLevel].width;
  int baseHeight = tex.mipmap[startLevel].height;
  int numSubLevels = (int)(log2f( (float)max(baseWidth, baseHeight)));

  numSubLevels = min(numSubLevels, kMaxMipLevels - startLevel - 1);
  tex.mipmap.resize(startLevel + numSubLevels + 1);

  int width  = baseWidth;
  int height = baseHeight;
  for (int i = 1; i <= numSubLevels; i++) {

    MipLevel& level = tex.mipmap[startLevel + i];

    // handle odd size texture by rounding down
    width  = max( 1, width  / 2); assert(width  > 0);
    height = max( 1, height / 2); assert(height > 0);

    level.width = width;
    level.height = height;
    level.texels = vector<unsigned char>(4 * width * height);

  }

  // fill all 0 sub levels with interchanging colors
  /*Color colors[3] = { Color(1,0,0,1), Color(0,1,0,1), Color(0,0,1,1) };
  for(size_t i = 1; i < tex.mipmap.size(); ++i) {

    Color c = colors[i % 3];
    MipLevel& mip = tex.mipmap[i];

    for(size_t i = 0; i < 4 * mip.width * mip.height; i += 4) {
      float_to_uint8( &mip.texels[i], &c.r );
    }
}*/

  for(size_t i = 1; i < tex.mipmap.size(); ++i) {
    MipLevel& mip_last = tex.mipmap[i - 1];
    MipLevel& mip = tex.mipmap[i];
    for(int x = 0; x < mip.width; x++) {
        for(int y = 0; y < mip.height; y++) {
            mip.texels[4 * (x + y * mip.width)    ] = (uint8_t) ((
                (float) mip_last.texels[4 * (2 * x +     (2 * y    ) * mip_last.width)    ] +
                (float) mip_last.texels[4 * (2 * x + 1 + (2 * y    ) * mip_last.width)    ] +
                (float) mip_last.texels[4 * (2 * x +     (2 * y + 1) * mip_last.width)    ] +
                (float) mip_last.texels[4 * (2 * x + 1 + (2 * y + 1) * mip_last.width)    ] ) / 4);
            mip.texels[4 * (x + y * mip.width) + 1] = (uint8_t) ((
                (float) mip_last.texels[4 * (2 * x +     (2 * y    ) * mip_last.width) + 1] +
                (float) mip_last.texels[4 * (2 * x + 1 + (2 * y    ) * mip_last.width) + 1] +
                (float) mip_last.texels[4 * (2 * x +     (2 * y + 1) * mip_last.width) + 1] +
                (float) mip_last.texels[4 * (2 * x + 1 + (2 * y + 1) * mip_last.width) + 1] ) / 4);
            mip.texels[4 * (x + y * mip.width) + 2] = (uint8_t) ((
                (float) mip_last.texels[4 * (2 * x +     (2 * y    ) * mip_last.width) + 2] +
                (float) mip_last.texels[4 * (2 * x + 1 + (2 * y    ) * mip_last.width) + 2] +
                (float) mip_last.texels[4 * (2 * x +     (2 * y + 1) * mip_last.width) + 2] +
                (float) mip_last.texels[4 * (2 * x + 1 + (2 * y + 1) * mip_last.width) + 2] ) / 4);
            mip.texels[4 * (x + y * mip.width) + 3] = (uint8_t) ((
                (float) mip_last.texels[4 * (2 * x +     (2 * y    ) * mip_last.width) + 3] +
                (float) mip_last.texels[4 * (2 * x + 1 + (2 * y    ) * mip_last.width) + 3] +
                (float) mip_last.texels[4 * (2 * x +     (2 * y + 1) * mip_last.width) + 3] +
                (float) mip_last.texels[4 * (2 * x + 1 + (2 * y + 1) * mip_last.width) + 3] ) / 4);
        }
    }
  }

}

Color Sampler2DImp::sample_nearest(Texture& tex,
                                   float u, float v,
                                   int level) {

  // Task 5: Implement nearest neighbour interpolation

  // return magenta for invalid level
  if ( level >= tex.mipmap.size() ) {
      return Color(1,0,1,1);
  }

  MipLevel thisLevel = tex.mipmap[level];
  int x = (int) round(u * thisLevel.width);
  int y = (int) round(v * thisLevel.height);
  return Color(
      (float) thisLevel.texels[4 * (x + y * thisLevel.width)    ] / 255,
      (float) thisLevel.texels[4 * (x + y * thisLevel.width) + 1] / 255,
      (float) thisLevel.texels[4 * (x + y * thisLevel.width) + 2] / 255,
      (float) thisLevel.texels[4 * (x + y * thisLevel.width) + 3] / 255);

}

Color Sampler2DImp::sample_bilinear(Texture& tex,
                                    float u, float v,
                                    int level) {

  // Task 5: Implement bilinear filtering

  // return magenta for invalid level
  if ( level >= tex.mipmap.size() ) {
      return Color(1,0,1,1);
  }

  MipLevel thisLevel = tex.mipmap[level];
  int x_floor = (int) floor(u * thisLevel.width - 0.5);
  int x_ceil = (int) ceil(u * thisLevel.width - 0.5);
  int y_floor = (int) floor(v * thisLevel.height - 0.5);
  int y_ceil = (int) ceil(v * thisLevel.height - 0.5);
  float x_dist = u * thisLevel.width - x_floor - 0.5;
  float y_dist = v * thisLevel.height - y_floor - 0.5;
  int index_top_left = 4 * (x_floor + y_floor * thisLevel.width);
  int index_top_right = 4 * (x_ceil + y_floor * thisLevel.width);
  int index_bottom_left = 4 * (x_floor + y_ceil * thisLevel.width);
  int index_bottom_right = 4 * (x_ceil + y_ceil * thisLevel.width);
  Color top_left = Color(
      (float) thisLevel.texels[index_top_left    ] / 255,
      (float) thisLevel.texels[index_top_left + 1] / 255,
      (float) thisLevel.texels[index_top_left + 2] / 255,
      (float) thisLevel.texels[index_top_left + 3] / 255);
  Color top_right = Color(
      (float) thisLevel.texels[index_top_right    ] / 255,
      (float) thisLevel.texels[index_top_right + 1] / 255,
      (float) thisLevel.texels[index_top_right + 2] / 255,
      (float) thisLevel.texels[index_top_right + 3] / 255);
  Color bottom_left = Color(
      (float) thisLevel.texels[index_bottom_left    ] / 255,
      (float) thisLevel.texels[index_bottom_left + 1] / 255,
      (float) thisLevel.texels[index_bottom_left + 2] / 255,
      (float) thisLevel.texels[index_bottom_left + 3] / 255);
  Color bottom_right = Color(
      (float) thisLevel.texels[index_bottom_right    ] / 255,
      (float) thisLevel.texels[index_bottom_right + 1] / 255,
      (float) thisLevel.texels[index_bottom_right + 2] / 255,
      (float) thisLevel.texels[index_bottom_right + 3] / 255);
  return (1 - y_dist) * ((1 - x_dist) * top_left + x_dist * top_right) + y_dist * ((1 - x_dist) * bottom_left + x_dist * bottom_right);

}

Color Sampler2DImp::sample_trilinear(Texture& tex,
                                     float u, float v,
                                     float u_scale, float v_scale) {

  // Task 6: Implement trilinear filtering
  float d = log2f(max(u_scale, v_scale));
  int d_floor = (int) floor(d);
  int d_ceil = (int) ceil(d);
  float d_dist = d - d_floor;

  if( d_ceil <= 0 ){
      return sample_bilinear(tex, u, v, 0);
  }

  // return magenta for invalid level
  if ( d_ceil >= tex.mipmap.size() ) {
      return Color(1,0,1,1);
  }

  return (1 - d_dist) * sample_bilinear(tex, u, v, d_floor) + d_dist * sample_bilinear(tex, u, v, d_ceil);

}

} // namespace CMU462
