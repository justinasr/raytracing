#include "ray.h"

Vec3 ray_at(Ray r, double t) {
    return vec3_add(r.origin, vec3_mult(r.direction, t));
}
