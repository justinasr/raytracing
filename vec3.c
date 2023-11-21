#include <math.h>
#include <stdlib.h>
#include "vec3.h"

Vec3 vec3_add(Vec3 a, Vec3 b) {
    Vec3 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

Vec3 vec3_add3(Vec3 a, Vec3 b, Vec3 c) {
    Vec3 r;
    r.x = a.x + b.x + c.x;
    r.y = a.y + b.y + c.y;
    r.z = a.z + b.z + c.z;
    return r;
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    Vec3 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    return r;
}

Vec3 vec3_sub3(Vec3 a, Vec3 b, Vec3 c) {
    Vec3 r;
    r.x = a.x - b.x - c.x;
    r.y = a.y - b.y - c.y;
    r.z = a.z - b.z - c.z;
    return r;
}

Vec3 vec3_mult(Vec3 a, double x) {
    Vec3 r;
    r.x = a.x * x;
    r.y = a.y * x;
    r.z = a.z * x;
    return r;
}

Vec3 vec3_mult_vec3(Vec3 a, Vec3 b) {
    Vec3 r;
    r.x = a.x * b.x;
    r.y = a.y * b.y;
    r.z = a.z * b.z;
    return r;
}

Vec3 vec3_div(Vec3 a, double x) {
    Vec3 r;
    r.x = a.x / x;
    r.y = a.y / x;
    r.z = a.z / x;
    return r;
}

double vec3_len_sqr(Vec3 a) {
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

double vec3_len(Vec3 a) {
    return sqrt(vec3_len_sqr(a));
}

double vec3_dot(Vec3 a, Vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vec3 vec3_as_unit(Vec3 a) {
    double l = vec3_len(a);
    return vec3_div(a, l);
}

Vec3 vec3_random(double min, double max) {
    double range = max - min;
    double x = min + (1.0 * rand() / RAND_MAX) * range;
    double y = min + (1.0 * rand() / RAND_MAX) * range;
    double z = min + (1.0 * rand() / RAND_MAX) * range;
    return (Vec3){.x=x, .y=y, .z=z};
}
