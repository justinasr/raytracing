#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "vec3.h"
#include "ray.h"

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480

#define ASPECT_RATIO ((double)IMAGE_WIDTH/IMAGE_HEIGHT)

#define VPORT_WIDTH 2.0
#define VPORT_HEIGHT (VPORT_WIDTH/ASPECT_RATIO)

#define FOCAL_LENGTH 1.0
#define SAMPLES_PER_PIXEL 1000
#define MAX_DEPTH 100

#define ARR_LEN(a) (sizeof(a)/sizeof(a[0]))

#define COLOR_R(hex) ((hex>>16)&0xFF)
#define COLOR_G(hex) ((hex>> 8)&0xFF)
#define COLOR_B(hex) ((hex>> 0)&0xFF)

#define COLOR_TO_VEC3(r, g, b) ((Vec3){.x=r,.y=g,.z=b})
#define HEX_TO_VEC3(hex) COLOR_TO_VEC3(COLOR_R(hex)/255.0,COLOR_G(hex)/255.0,COLOR_B(hex)/255.0)
#define COLOR_BLACK COLOR_TO_VEC3(0.0,0.0,0.0)
#define COLOR_WHITE COLOR_TO_VEC3(1.0,1.0,1.0)
#define COLOR_RED COLOR_TO_VEC3(1.0,0.0,0.0)
#define COLOR_GREEN COLOR_TO_VEC3(0.0,1.0,0.0)
#define COLOR_BLUE COLOR_TO_VEC3(0.0,0.0,1.0)

#define COLOR_EMERALD HEX_TO_VEC3(0x2ecc71)
#define COLOR_CARROT HEX_TO_VEC3(0xe67e22)
#define COLOR_PETER_RIVER HEX_TO_VEC3(0x3498db)
#define COLOR_ALIZARIN HEX_TO_VEC3(0xe74c3c)
#define COLOR_WET_ASPHALT HEX_TO_VEC3(0x34495e)
#define COLOR_AMETHYST HEX_TO_VEC3(0x9b59b6)

#define make_sphere(_x, _y, _z, r, c) ((Sphere){.center=(Vec3){.x=_x,.y=_y,.z=_z},.radius=r,.color=c})
#define make_hittable(o, t) ((Hittable){.obj=o,.type=t})

#define EPSILON 0.0000001

void vec3_print(Vec3 a) {
    printf("Vec3 x=%6.3f y=%6.3f z=%6.3f", a.x, a.y, a.z);
}

void ray_print(Ray r) {
    printf(
        "Ray origin x=%6.3f y=%6.3f z=%6.3f direction x=%6.3f y=%6.3f z=%6.3f",
        r.origin.x,
        r.origin.y,
        r.origin.z,
        r.direction.x,
        r.direction.y,
        r.direction.z
    );
}

Vec3 random_in_unit_sphere() {
    while (true) {
        Vec3 v =  vec3_random(-1.0, 1.0);
        if (vec3_len_sqr(v) < 1) {
            return v;
        }
    }
}

Vec3 random_unit_vector() {
    return vec3_as_unit(random_in_unit_sphere());
}

Vec3 random_on_surface(Vec3 normal) {
    Vec3 v = random_unit_vector();
    if (vec3_dot(v, normal) > 0.0) {
        // In the same hemisphere as normal
        return v;
    } else {
        return vec3_mult(v, -1.0);
    }
}

typedef struct {
    Vec3 center;
    double radius;
    Vec3 color;
} Sphere;

typedef enum {
    SPHERE
} HittableType;

typedef struct {
    HittableType type;
    void* obj;
} Hittable;

typedef struct {
    Hittable **hittables;
    size_t count;
} Hittables;

typedef struct {
    Vec3 point; // Hit point
    Vec3 normal; // Hit normal
    double t; // Distance from ray origin
    Hittable *hittable;
} HitRecord;

bool hit_sphere(Sphere *sphere, Ray *ray, HitRecord *hit_record, double t_min, double t_max) {
    Vec3 oc = vec3_sub(ray->origin, sphere->center);
    double a = vec3_len_sqr(ray->direction);
    double half_b = vec3_dot(oc, ray->direction);
    double c = vec3_len_sqr(oc) - (sphere->radius * sphere->radius);
    double discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;

    double sqrt_d = sqrt(discriminant);
    double t1 = (-half_b - sqrt_d)/a;
    double t2 = (-half_b + sqrt_d)/a;
    if (t_min <= t1 && t1 <= t_max && t_min <= t2 && t2 <= t_max) {
        // Both t within range
        hit_record->t = t1 < t2 ? t1 : t2;
    } else if (t_min <= t1 && t1 <= t_max) {
        // Only t1 in range
        hit_record->t = t1;
    } else if (t_min <= t2 && t2 <= t_max) {
        // Only t2 in range
        hit_record->t = t2;
    } else {
        // Neither t1, nor t2 is in range
        return false;
    }

    hit_record->point = ray_at(*ray, hit_record->t);
    Vec3 outward_normal = vec3_div(vec3_sub(hit_record->point, sphere->center), sphere->radius);
    if (vec3_dot(ray->direction, outward_normal) < 0) {
        // Ray hitting from outside
        hit_record->normal = outward_normal;
    } else {
        // Ray hitting from inside
        hit_record->normal = vec3_mult(outward_normal, -1.0);
    }
    return true;
}

bool hit_one(Hittable* hittable, Ray *ray, HitRecord *hit_record, double t_min, double t_max) {
    switch (hittable->type)
    {
    case SPHERE:
        return hit_sphere((Sphere*)hittable->obj, ray, hit_record, t_min, t_max);
    }
    exit(1);
}

bool hit(Hittables *hittables, Ray *ray, HitRecord *hit_record, double t_min, double t_max) {
    HitRecord tmp_hit_record;
    double closest = t_max;
    bool did_hit = false;
    for (size_t i = 0; i < hittables->count; ++i) {
        Hittable *hittable = hittables->hittables[i];
        if (hit_one(hittable, ray, &tmp_hit_record, t_min, closest)) {
            did_hit = true;
            hit_record->normal = tmp_hit_record.normal;
            hit_record->point = tmp_hit_record.point;
            hit_record->t = tmp_hit_record.t;
            hit_record->hittable = hittable;
            closest = tmp_hit_record.t;
        }
    }
    return did_hit;
}

Vec3 ray_color(Ray *ray, Hittables *hittables, size_t depth) {
    if (depth == 0) return COLOR_BLACK;

    HitRecord hit_record;
    if (hit(hittables, ray, &hit_record, EPSILON, 100.0)) {
        // Lambertian Reflection
        Vec3 direction = vec3_add(hit_record.normal, random_on_surface(hit_record.normal));
        Ray new_ray = (Ray){.origin=hit_record.point, .direction=direction};
        Sphere *sphere = (Sphere*)hit_record.hittable->obj;
        return vec3_mult_vec3(sphere->color, ray_color(&new_ray, hittables, depth - 1));
    }

    Vec3 ray_direction_unit = vec3_as_unit(ray->direction);
    double a = 0.5 * (ray_direction_unit.y + 1.0);
    return COLOR_TO_VEC3(
        (1.0 - a) + a * 0.5,
        (1.0 - a) + a * 0.7,
        (1.0 - a) + a * 1.0
    );
}

int main() {
    const char *filename = "image.ppm";
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "ERROR: can't open %s", filename);
        return 1;
    }
    // P3 means colors in ASCII
    fprintf(file, "P3\n");
    // Image width and height
    fprintf(file, "%d %d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    // Channel values 0-255
    fprintf(file, "%d\n", 255);

    // Camera
    Vec3 camera = (Vec3){.x=0, .y=0, .z=FOCAL_LENGTH};

    // Viewport edges
    // Note negative height, because 0;0 is top left
    Vec3 vport_u = (Vec3){.x=VPORT_WIDTH, .y=0, .z=0}; // Horizontal
    Vec3 vport_v = (Vec3){.x=0, .y=-VPORT_HEIGHT, .z=0}; // Vertical
    Vec3 vport_u_half = vec3_div(vport_u, 2.0);
    Vec3 vport_v_half = vec3_div(vport_v, 2.0);

    // Pixel deltas
    Vec3 pixel_delta_u = vec3_div(vport_u, IMAGE_WIDTH);
    Vec3 pixel_delta_v = vec3_div(vport_v, IMAGE_HEIGHT);

    // Note: "negative z-axis pointing in the viewing direction"
    // Viewport is FOCAL_LENGTH away from camera
    Vec3 vport_upper_left = vec3_sub3(
        (Vec3){.x=0, .y=0, .z=0},
        vport_u_half,
        vport_v_half
    );

    Vec3 pixel_00 = vec3_add(
        vport_upper_left,
        vec3_mult(
            vec3_add(
                pixel_delta_u,
                pixel_delta_v
            ),
            0.5
        )
    );

    Sphere sphere1 = make_sphere( 0.0,    0.5, -1.0,   0.5, COLOR_EMERALD);
    Sphere sphere2 = make_sphere( 0.0,   -0.5, -1.0,   0.5, COLOR_CARROT);
    Sphere sphere3 = make_sphere( 1.0,    0.0, -1.0,   0.5, COLOR_PETER_RIVER);
    Sphere sphere4 = make_sphere(-0.5,   -0.3, -0.5,   0.2, COLOR_ALIZARIN);
    Sphere sphere5 = make_sphere(-0.8,   -0.3, -0.2,   0.2, COLOR_AMETHYST);
    Sphere sphere6 = make_sphere( 0.0, -100.5, -1.0, 100.0, COLOR_WET_ASPHALT);
    Hittable hittable1 = make_hittable(&sphere1, SPHERE);
    Hittable hittable2 = make_hittable(&sphere2, SPHERE);
    Hittable hittable3 = make_hittable(&sphere3, SPHERE);
    Hittable hittable4 = make_hittable(&sphere4, SPHERE);
    Hittable hittable5 = make_hittable(&sphere5, SPHERE);
    Hittable hittable6 = make_hittable(&sphere6, SPHERE);
    Hittable *hittable[] = {
        &hittable1,
        &hittable2,
        &hittable3,
        &hittable4,
        &hittable5,
        &hittable6
    };
    Hittables hittables = (Hittables){.hittables=hittable, .count=ARR_LEN(hittable)};

    for (size_t y = 0; y < IMAGE_HEIGHT; ++y) {
        if (y % 10 == 9) printf("Image line %lu/%d\n", y + 1, IMAGE_HEIGHT);
        for (size_t x = 0; x < IMAGE_WIDTH; ++x) {
            Vec3 pixel_center = vec3_add3(
                pixel_00,
                vec3_mult(pixel_delta_u, x),
                vec3_mult(pixel_delta_v, y)
            );

            Vec3 ray_direction = vec3_sub(pixel_center, camera);

            Ray ray = (Ray){.origin=camera, .direction=ray_direction};

            Vec3 pixel = COLOR_BLACK;
            for (size_t i = 0; i < SAMPLES_PER_PIXEL; ++i) {
                Vec3 color = ray_color(&ray, &hittables, MAX_DEPTH);
                pixel.x += color.x;
                pixel.y += color.y;
                pixel.z += color.z;
            }

            int r = (pixel.x / SAMPLES_PER_PIXEL) * 255.0;
            int g = (pixel.y / SAMPLES_PER_PIXEL) * 255.0;
            int b = (pixel.z / SAMPLES_PER_PIXEL) * 255.0;

            assert(0 <= r && r <= 255);
            assert(0 <= g && g <= 255);
            assert(0 <= b && b <= 255);

            fprintf(file, "%d %d %d\n", r, g, b);
        }
    }

    fclose(file);
    return 0;
}
