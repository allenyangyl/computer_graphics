#include "bsdf.h"

#include <iostream>
#include <algorithm>
#include <utility>

using std::min;
using std::max;
using std::swap;

namespace CMU462 {

void make_coord_space(Matrix3x3& o2w, const Vector3D& n) {

    Vector3D z = Vector3D(n.x, n.y, n.z);
    Vector3D h = z;
    if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z)) h.x = 1.0;
    else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z)) h.y = 1.0;
    else h.z = 1.0;

    z.normalize();
    Vector3D y = cross(h, z);
    y.normalize();
    Vector3D x = cross(z, y);
    x.normalize();

    o2w[0] = x;
    o2w[1] = y;
    o2w[2] = z;
}

// Diffuse BSDF //

Spectrum DiffuseBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return albedo * (1.0 / PI);
}

Spectrum DiffuseBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  UniformHemisphereSampler3D sampler;
  *wi = sampler.get_sample();
  *pdf = 1.0 / (2.0 * M_PI);
  return albedo * (1.0 / PI);
}

// Mirror BSDF //

Spectrum MirrorBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum MirrorBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO:
  // Implement MirrorBSDF
  reflect(wo, wi);
  *pdf = 1;
  return reflectance;
}

// Glossy BSDF //

/*
Spectrum GlossyBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlossyBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *pdf = 1.0f;
  return reflect(wo, wi, reflectance);
}
*/

// Refraction BSDF //

Spectrum RefractionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum RefractionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO:
  // Implement RefractionBSDF
  if(refract(wo, wi, ior)){
      *pdf = 1;
      return transmittance;
  }
  else{
      *pdf = 1;
      return Spectrum();
  }
}

// Glass BSDF //

Spectrum GlassBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlassBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO:
  // Compute Fresnel coefficient and either reflect or refract based on it.
  if(!refract(wo, wi, ior)){
      reflect(wo, wi);
      *pdf = 1;
      return reflectance;
  }
  else{
      float ior_p = wo.z >= 0 ? ior : 1 / ior;
      double delta = ior_p * ior_p * (wo.x * wo.x + wo.y * wo.y + wo.z * wo.z) - wo.x * wo.x - wo.y * wo.y;
      double prob = 1 - ((fabs(wo.z) - ior_p * sqrt(delta)) * (fabs(wo.z) - ior_p * sqrt(delta)) / (fabs(wo.z) + ior_p * sqrt(delta)) / (fabs(wo.z) + ior_p * sqrt(delta)) +
                    (ior_p * fabs(wo.z) - sqrt(delta)) * (ior_p * fabs(wo.z) - sqrt(delta)) / (ior_p * fabs(wo.z) + sqrt(delta)) / (ior_p * fabs(wo.z) + sqrt(delta))) / 2;
      if((double)(std::rand()) / RAND_MAX  > prob){
          reflect(wo, wi);
          *pdf = 1;
          return reflectance;
      }
      else{
          refract(wo, wi, ior);
          *pdf = 1;
          return transmittance;
      }
  }
  return Spectrum();
}

void BSDF::reflect(const Vector3D& wo, Vector3D* wi) {

  // TODO:
  // Implement reflection of wo about normal (0,0,1) and store result in wi.
  wi->x = -wo.x;
  wi->y = -wo.y;
  wi->z = wo.z;
  return;
}

bool BSDF::refract(const Vector3D& wo, Vector3D* wi, float ior) {

  // TODO:
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When dot(wo,n) is positive, then wo corresponds to a
  // ray entering the surface through vacuum.
  if(wo.z >= 0){
      // in vacuum
      double delta = ior * ior * (wo.x * wo.x + wo.y * wo.y + wo.z * wo.z) - wo.x * wo.x - wo.y * wo.y;
      wi->x = -wo.x / sqrt(delta);
      wi->y = -wo.y / sqrt(delta);
      wi->z = -1;
      wi->normalize();
  }
  else{
      // in surface
      double delta = (wo.x * wo.x + wo.y * wo.y + wo.z * wo.z) / (ior * ior) - wo.x * wo.x - wo.y * wo.y;
      if(delta > 0){
          wi->x = -wo.x / sqrt(delta);
          wi->y = -wo.y / sqrt(delta);
          wi->z = 1;
          wi->normalize();
      }
      else{
          return false;
      }
  }

  return true;

}

// Emission BSDF //

Spectrum EmissionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum EmissionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *wi  = sampler.get_sample(pdf);
  return Spectrum();
}

} // namespace CMU462
