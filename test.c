/*
  Tests just to be sure there are no silly mistakes
*/

#include <assert.h>
#include <math.h>
#include "vec3.h"

#define f_assert_eq(a,b) assert(fabs(fabs(a)-fabs(b))<0.00001)
#define f_assert_neq(a,b) assert(fabs(fabs(a)-fabs(b))>=0.00001)

void test_vec3() {
    Vec3 a = (Vec3){.x= 1.5, .y= 2.5, .z= 3.5};
    Vec3 b = (Vec3){.x= 4.1, .y= 4.2, .z= 4.3};
    Vec3 c = (Vec3){.x=-2.9, .y=-2.8, .z=-2.7};
    double x = 0.3;
    Vec3 e;
    Vec3 r;

    // Addition
    e = (Vec3){.x=5.6, .y=6.7, .z=7.8};
    r = vec3_add(a, b);
    f_assert_eq(e.x, r.x);
    f_assert_eq(e.y, r.y);
    f_assert_eq(e.z, r.z);

    // Addition add3
    e = (Vec3){.x=2.7, .y=3.9, .z=5.1};
    r = vec3_add3(a, b, c);
    f_assert_eq(e.x, r.x);
    f_assert_eq(e.y, r.y);
    f_assert_eq(e.z, r.z);

    // Subtraction
    e = (Vec3){.x=-2.6, .y=-1.7, .z=-0.8};
    r = vec3_sub(a, b);
    f_assert_eq(e.x, r.x);
    f_assert_eq(e.y, r.y);
    f_assert_eq(e.z, r.z);

    // Subtraction sub3
    e = (Vec3){.x=0.3, .y=1.1, .z=1.9};
    r = vec3_sub3(a, b, c);
    f_assert_eq(e.x, r.x);
    f_assert_eq(e.y, r.y);
    f_assert_eq(e.z, r.z);

    // Multiplication
    e = (Vec3){.x=0.45, .y=0.75, .z=1.05};
    r = vec3_mult(a, x);
    f_assert_eq(e.x, r.x);
    f_assert_eq(e.y, r.y);
    f_assert_eq(e.z, r.z);

    // Multiplication with another vector
    e = (Vec3){.x=6.15, .y=10.5, .z=15.05};
    r = vec3_mult_vec3(a, b);
    f_assert_eq(e.x, r.x);
    f_assert_eq(e.y, r.y);
    f_assert_eq(e.z, r.z);

    // Division
    e = (Vec3){.x=5.0, .y=8.333333, .z=11.666667};
    r = vec3_div(a, x);
    f_assert_eq(e.x, r.x);
    f_assert_eq(e.y, r.y);
    f_assert_eq(e.z, r.z);

    // Length squared
    f_assert_eq(vec3_len_sqr(a), 20.75);
    f_assert_eq(vec3_len_sqr(b), 52.94);
    f_assert_eq(vec3_len_sqr(c), 23.54);

    // Length
    f_assert_eq(vec3_len(a), 4.555217);
    f_assert_eq(vec3_len(b), 7.275988);
    f_assert_eq(vec3_len(c), 4.851804);

    // Dot
    f_assert_eq(vec3_dot(a, b), 31.7);
    f_assert_eq(vec3_dot(b, c), -35.26);
    f_assert_eq(vec3_dot(a, c), -20.8);

    // Unit
    e = (Vec3){.x=0.329293, .y=0.548821, .z=0.768350};
    r = vec3_as_unit(a);
    f_assert_eq(e.x, r.x);
    f_assert_eq(e.y, r.y);
    f_assert_eq(e.z, r.z);
    f_assert_neq(vec3_len(a), 1.0);
    f_assert_eq(vec3_len(r), 1.0);

    // Random
    r = vec3_random(-1.0, 1.0);
    assert(r.x >= -1.0 && r.x <= 1.0);
    assert(r.y >= -1.0 && r.y <= 1.0);
    assert(r.z >= -1.0 && r.z <= 1.0);
    r = vec3_random(11.0, 12.0);
    assert(r.x >= 11.0 && r.x <= 12.0);
    assert(r.y >= 11.0 && r.y <= 12.0);
    assert(r.z >= 11.0 && r.z <= 12.0);
}

int main() {
    test_vec3();
    return 0;
}
