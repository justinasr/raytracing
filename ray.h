/*
  Ray - ray struct with origin point and direction
  RayHit - hitpoint struct with t, hit point and normal
*/

#include <stdbool.h>
#include "vec3.h"

typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

/*
  Vec3 as origin + t * direction point in space
*/
Vec3 ray_at(Ray r, double t);

typedef struct {
    double t;
    Vec3 point;
    Vec3 normal;
} RayHit;
