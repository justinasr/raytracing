/*
  Vec3 - vector struct with 3 components - x, y, z
*/

#ifndef VEC3_H_
#define VEC3_H_

typedef struct {
  double x;
  double y;
  double z;
} Vec3;

/*
  Add two vectors, component by component
*/
Vec3 vec3_add(Vec3 a, Vec3 b);

/*
  Add three vectors, component by component
*/
Vec3 vec3_add3(Vec3 a, Vec3 b, Vec3 c);

/*
  Subtract vector b from a, component by component
*/
Vec3 vec3_sub(Vec3 a, Vec3 b);

/*
  Subtract vectors b and c from a, compoent by component
*/
Vec3 vec3_sub3(Vec3 a, Vec3 b, Vec3 c);

/*
  Multiply each component of vector by a constant
*/
Vec3 vec3_mult(Vec3 a, double x);

/*
  Divide each component of vector by a constant
*/
Vec3 vec3_div(Vec3 a, double x);

/*
  Return square of vector's length
*/
double vec3_len_sqr(Vec3 a);

/*
  Length of vector
*/
double vec3_len(Vec3 a);

/*
  Dot product of two vectors:
  a.x*b.x + a.y*b.y + a.z*b.z
*/
double vec3_dot(Vec3 a, Vec3 b);

/*
  Normalize vector so its length would be 1
*/
Vec3 vec3_as_unit(Vec3 a);

/*
  Return a vector with values being random min <= x < max
*/
Vec3 vec3_random(double min, double max);

#endif // VEC3_H_
