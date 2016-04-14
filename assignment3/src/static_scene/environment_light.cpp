#include "environment_light.h"
#include <iostream>

namespace CMU462 { namespace StaticScene {

EnvironmentLight::EnvironmentLight(const HDRImageBuffer* envMap)
    : envMap(envMap) {
  // TODO: initialize things here as needed
}

Spectrum EnvironmentLight::sample_L(const Vector3D& p, Vector3D* wi,
                                    float* distToLight,
                                    float* pdf) const {
  // TODO: Implement
  UniformHemisphereSampler3D sampler;
  Vector3D dir = sampler.get_sample();
  *wi = (double)(std::rand()) / RAND_MAX > 0.5 ? dir : -dir;
  *distToLight = INF_D;
  *pdf = 1.0 / (4.0 * M_PI);
  double theta = acos(wi->y / wi->norm());
  double phi = atan2(wi->z, wi->x);
  phi += phi >= 0 ? 0 : 2 * M_PI;
  double theta_norm = theta / M_PI * envMap->h;
  double phi_norm = phi / 2 / M_PI * envMap->w;
  int theta_floor = floor(theta_norm);
  int theta_ceil = ceil(theta_norm);
  int phi_floor = floor(phi_norm);
  int phi_ceil = ceil(phi_norm);
  double theta_alpha = theta_norm - theta_floor;
  double phi_alpha = phi_norm - phi_floor;
  return (envMap -> data[theta_floor * envMap->w + phi_floor] * (1 - phi_alpha) +
         envMap -> data[theta_floor * envMap->w + phi_ceil] * phi_alpha) * (1 - theta_alpha) +
         (envMap -> data[theta_ceil * envMap->w + phi_floor] * (1 - phi_alpha) +
         envMap -> data[theta_ceil * envMap->w + phi_ceil] * phi_alpha) * theta_alpha;
}

Spectrum EnvironmentLight::sample_dir(const Ray& r) const {
  // TODO: Implement
  double theta = acos(r.d.y / r.d.norm());
  double phi = atan2(r.d.z, r.d.x);
  phi += phi >= 0 ? 0 : 2 * M_PI;
  double theta_norm = theta / M_PI * envMap->h;
  double phi_norm = phi / 2 / M_PI * envMap->w;
  int theta_floor = floor(theta_norm);
  int theta_ceil = ceil(theta_norm);
  int phi_floor = floor(phi_norm);
  int phi_ceil = ceil(phi_norm);
  double theta_alpha = theta_norm - theta_floor;
  double phi_alpha = phi_norm - phi_floor;
  return (envMap -> data[theta_floor * envMap->w + phi_floor] * (1 - phi_alpha) +
         envMap -> data[theta_floor * envMap->w + phi_ceil] * phi_alpha) * (1 - theta_alpha) +
         (envMap -> data[theta_ceil * envMap->w + phi_floor] * (1 - phi_alpha) +
         envMap -> data[theta_ceil * envMap->w + phi_ceil] * phi_alpha) * theta_alpha;
}

} // namespace StaticScene
} // namespace CMU462
