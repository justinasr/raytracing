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
#define SAMPLES_PER_PIXEL 500
#define MAX_HITS 5

#define ARR_LEN(a) (sizeof(a)/sizeof(a[0]))

#define COLOR_R(hex) ((hex>>16)&0xFF)
#define COLOR_G(hex) ((hex>> 8)&0xFF)
#define COLOR_B(hex) ((hex>> 0)&0xFF)

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

typedef struct {
    Vec3 center;
    double r;
    uint32_t color;
} Sphere;

#define make_sphere(_x, _y, _z, _r, _c) (Sphere){.center=(Vec3){.x=_x, .y=_y, .z=_z}, .r=_r, .color=_c}

Vec3 vec3_random_unit() {
    return vec3_as_unit(vec3_random(-1.0, 1.0));
}

Vec3 vec3_on_surface(Vec3 v, Vec3 n) {
    if (vec3_dot(v, n) > 0.0) return v;
    return vec3_mult(v, -1.0);
}

Vec3 vec3_random_unit_on_surface(Vec3 n) {
    return vec3_on_surface(vec3_random_unit(), n);
}

Vec3 vec3_color_blend(Vec3 a, Vec3 b) {
    Vec3 r;
    r.x = sqrt(a.x * b.x);
    r.y = sqrt(a.y * b.y);
    r.z = sqrt(a.z * b.z);
    return r;
}

bool hit_sphere(Sphere sphere, Ray ray, RayHit *ray_hit) {
    Vec3 oc = vec3_sub(ray.origin, sphere.center);
    double a = vec3_dot(ray.direction, ray.direction);
    double b = 2.0 * vec3_dot(oc, ray.direction);
    double c = vec3_dot(oc, oc) - sphere.r * sphere.r;
    // discriminant
    double d = b * b - 4 * a * c;

    if (d < EPSILON) {
        return false;
    }
    // distance on ray
    double t = (-b - sqrt(d)) / (2.0 * a);
    if (t < EPSILON) {
        return false;
    }

    // point on ray
    ray_hit->t = t;
    ray_hit->point = ray_at(ray, t);
    // hit normal facing outwards
    ray_hit->normal = vec3_div(vec3_sub(ray_hit->point, sphere.center), sphere.r);
    // ray should be hitting outside of the sphere
    assert(vec3_dot(ray.direction, ray_hit->normal) < 0.0);

    return true;
}

Vec3 get_ray_color(Ray ray, Sphere spheres[], size_t spheres_n, size_t max_hits) {
    if (max_hits == 0) {
        return (Vec3){.x=0.0, .y=0.0, .z=0.0};
    }
    RayHit ray_hit;
    size_t sphere_i;
    bool hit = false;
    for (size_t i = 0; i < spheres_n; ++i) {
        RayHit tmp_ray_hit;
        if (hit_sphere(spheres[i], ray, &tmp_ray_hit)) {
            if (!hit || tmp_ray_hit.t < ray_hit.t) {
                ray_hit.t = tmp_ray_hit.t;
                ray_hit.normal = tmp_ray_hit.normal;
                ray_hit.point = tmp_ray_hit.point;
                hit = true;
                sphere_i = i;
            }
        }
    }

    if (hit) {
        Vec3 my_color = (Vec3) {
            // .x=0.5 * (ray_hit.normal.x + 1),
            // .y=0.5 * (ray_hit.normal.y + 1),
            // .z=0.5 * (ray_hit.normal.z + 1),
            .x=COLOR_R(spheres[sphere_i].color)/255.0,
            .y=COLOR_G(spheres[sphere_i].color)/255.0,
            .z=COLOR_B(spheres[sphere_i].color)/255.0,
        };
        Vec3 reflection = vec3_random_unit_on_surface(ray_hit.normal);
        // Vec3 reflection = ray_hit.normal;
        Ray random_ray = (Ray){.origin=ray_hit.point, reflection};
        Vec3 ray_color = get_ray_color(random_ray, spheres, spheres_n, max_hits - 1);
        // return vec3_mult(ray_color, 0.5);
        return vec3_color_blend(my_color, ray_color);
    }
    double a = 0.5 * (ray.direction.y + 1.0);
    return (Vec3) {
        .x=((1.0-a) + a * 0.4),
        .y=((1.0-a) + a * 0.6),
        .z=((1.0-a) + a * 1.0),
    };
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

    Sphere spheres[] = {
        make_sphere(-0.9, 0.0, -1.0, 0.5, 0xe74c3c),
        make_sphere( 0.0, 0.0, -1.0, 0.5, 0x3498db),
        make_sphere( 0.9, 0.0, -1.0, 0.5, 0x27ae60),

        make_sphere(-0.45, -0.4,  -1.0, 0.1, 0xf1c40f),
        make_sphere(-0.25, -0.4,  -0.5, 0.1, 0xe67e22),
        make_sphere(  0.0, -0.4, -0.25, 0.1, 0x8e44ad),
        make_sphere( 0.25, -0.4,  -0.5, 0.1, 0x34495e),
        make_sphere( 0.45, -0.4,  -1.0, 0.1, 0xecf0f1),

        make_sphere( 0.0, -50.5, -1.0, 50.0, 0xbababa)
    };
    size_t spheres_n = ARR_LEN(spheres);

    for (size_t y = 0; y < IMAGE_HEIGHT; ++y) {
        printf("Line %d/%d\n", y + 1, IMAGE_HEIGHT);
        for (size_t x = 0; x < IMAGE_WIDTH; ++x) {
            Vec3 pixel_center = vec3_add3(
                pixel_00,
                vec3_mult(pixel_delta_u, x),
                vec3_mult(pixel_delta_v, y)
            );

            Vec3 ray_direction = vec3_sub(pixel_center, camera);
            Vec3 ray_direction_unit = vec3_as_unit(ray_direction);

            Ray ray = (Ray){.origin=camera, .direction=ray_direction_unit};

            Vec3 color = (Vec3){0};
            for (size_t i = 0; i < SAMPLES_PER_PIXEL; i++) {
                color = vec3_add(color, get_ray_color(ray, spheres, spheres_n, MAX_HITS));
            }
            color = vec3_div(color, (double)SAMPLES_PER_PIXEL);

            int r = color.x * 255.0;
            int g = color.y * 255.0;
            int b = color.z * 255.0;

            assert(0 <= r && r <= 255);
            assert(0 <= g && g <= 255);
            assert(0 <= b && b <= 255);

            fprintf(file, "%d %d %d\n", r, g, b);
        }
    }

    fclose(file);
    return 0;
}
