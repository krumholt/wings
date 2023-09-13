#include "wings/base/math.c"
#include "wings/base/types.c"
#include "wings/graphics/camera.c"
#include "wings/input/keyboard.c"
#include "wings/input/mouse.c"

struct third_person_controller
{
    struct v3 target;
    f32       angle_xy;
    f32       angle_z;
    f32       min_angle_z;
    f32       max_angle_z;
    f32       distance;

    f32 min_distance;
    f32 max_distance;

    f32 speed_distance;
    f32 speed_angle_xy;
    f32 speed_angle_z;
    f32 speed;
} third_person_controller_default_settings = {
    .target         = {0.0f, 0.0f, 0.0f},
    .angle_xy       = 0.0f,
    .angle_z        = 0.0f,
    .min_angle_z    = -89.0f,
    .max_angle_z    = 89.0f,
    .distance       = 10.0f,
    .min_distance   = 1.0f,
    .max_distance   = 1000.0f,
    .speed_distance = 30.0f,
    .speed_angle_xy = 270.0f,
    .speed_angle_z  = 270.0f,
    .speed          = 300.0f,
};

void
update_camera_from_controller(struct camera *cam, struct third_person_controller *controller, f32 delta_in_seconds)
{
    controller->angle_z  = clamp_f32(controller->angle_z, controller->min_angle_z, controller->max_angle_z);
    controller->distance = clamp_f32(controller->distance, controller->min_distance, controller->max_distance);
    cam->position        = make_v3(
        controller->distance * sinf(PI * (180.0f - controller->angle_xy) / 180.0f) * cosf(PI * controller->angle_z / 180.0f),
        controller->distance * cosf(PI * (180.0f - controller->angle_xy) / 180.0f) * cosf(PI * controller->angle_z / 180.0f),
        controller->distance * sinf(PI * controller->angle_z / 180.0f));
    cam->position = add_v3(cam->position, controller->target);
    cam->forward  = normalize_v3(sub_v3(controller->target, cam->position));
    cam->right    = normalize_v3(cross_v3(cam->forward, make_v3(0.0f, 0.0f, 1.0f)));
    cam->up       = normalize_v3(cross_v3(cam->right, cam->forward));
    cam->view     = make_look_at_matrix_RH(cam->position, controller->target, cam->up);
    if (right_mouse_button_held())
    {
        controller->angle_xy -= mouse.position_delta.x * controller->speed_angle_xy * delta_in_seconds;
        controller->angle_z += mouse.position_delta.y * controller->speed_angle_z * delta_in_seconds;
    }
    struct v3 forward = cam->forward;
    forward.z         = 0;
    forward           = normalize_v3(forward);
    struct v3 right   = cam->right;
    if (key_held(keycode_A))
    {
        controller->target
            = add_v3(controller->target,
                     mul_f32_v3(-controller->speed * delta_in_seconds, right));
    }
    if (key_held(keycode_W))
    {
        controller->target
            = add_v3(controller->target,
                     mul_f32_v3(controller->speed * delta_in_seconds, forward));
    }
    if (key_held(keycode_S))
    {
        controller->target
            = add_v3(controller->target,
                     mul_f32_v3(-controller->speed * delta_in_seconds, forward));
    }
    if (key_held(keycode_D))
    {
        controller->target
            = add_v3(controller->target,
                     mul_f32_v3(controller->speed * delta_in_seconds, right));
    }
    controller->distance -= controller->speed_distance * mouse.wheel_delta;
}

inline void
rotate_up(struct third_person_controller *controller, f32 amount)
{
    controller->angle_z += controller->speed_angle_z * amount;
}

inline void
rotate_down(struct third_person_controller *controller, f32 amount)
{
    controller->angle_z -= controller->speed_angle_z * amount;
}

inline void
rotate_left(struct third_person_controller *controller, f32 amount)
{
    controller->angle_xy -= controller->speed_angle_xy * amount;
}

inline void
rotate_right(struct third_person_controller *controller, f32 amount)
{
    controller->angle_xy += controller->speed_angle_xy * amount;
}

inline void
move_closer(struct third_person_controller *controller, f32 amount)
{
    controller->distance -= controller->speed_distance * amount;
}

inline void
move_away(struct third_person_controller *controller, f32 amount)
{
    controller->distance += controller->speed_distance * amount;
}
