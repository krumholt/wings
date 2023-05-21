#ifndef __camera_cpp__
#define __camera_cpp__

#include "wings_math.c"

struct camera
{
    struct v3    position;
    struct v3    forward;
    struct v3    right;
    struct v3    up;
    struct mat4  view;
    struct mat4  projection;
};

void
update_view(struct camera *camera)
{
    camera->forward = normalize_v3(camera->forward);
    camera->right = normalize_v3(cross_v3(camera->forward, camera->up));
    camera->up    = normalize_v3(cross_v3(camera->right, camera->forward));
    camera->view  = make_look_at_matrix_RH(camera->position,
                                           add_v3(camera->position, camera->forward),
                                           camera->up);
}

struct camera
make_camera(struct v3 position, struct v3 forward, struct v3 up, struct mat4 projection)
{
    struct camera camera   = {0};
    camera.position   = position;
    camera.forward    = normalize_v3(forward);
    camera.up         = normalize_v3(up);
    camera.right      = cross_v3(forward, up);
    camera.projection = projection;
    update_view(&camera);

    return(camera);
}

void
move_forward(struct camera *camera, f32 amount)
{
    camera->position = add_v3(camera->position,
                              mul_v3_f32(camera->forward, amount));
}

void
move_backward(struct camera *camera, f32 amount)
{
    camera->position = sub_v3(camera->position,
                              mul_v3_f32(camera->forward, amount));
}

void
move_left(struct camera *camera, f32 amount)
{
    camera->position = sub_v3(camera->position,
                              mul_v3_f32(camera->right, amount));
}

void
move_right(struct camera *camera, f32 amount)
{
    camera->position = add_v3(camera->position,
                              mul_v3_f32(camera->right, amount));
}

void
turn_right(struct camera *camera, f32 amount)
{
    if (camera->up.z < 0.0f) amount = -amount;
    struct mat3 rotation = mat4_to_mat3(make_rotation_z_axis_mat4(amount));
    camera->forward = normalize_v3(mul_mat3_v3(rotation, camera->forward));
    camera->right   = normalize_v3(mul_mat3_v3(rotation, camera->right));
    camera->up      = normalize_v3(mul_mat3_v3(rotation, camera->up));
}

void
turn_left(struct camera *camera, f32 amount)
{
    turn_right(camera, -amount);
}


void
turn_up(struct camera *camera, f32 amount)
{
    struct quaternion rotation   = make_rotation_quaternion(camera->right, amount);
    camera->forward = rotate_vector_by_quaternion(rotation, camera->forward);
    camera->up      = normalize_v3(cross_v3(camera->right, camera->forward));
}

void
turn_down(struct camera *camera, f32 amount)
{
    struct quaternion rotation   = make_rotation_quaternion(camera->right, -amount);
    camera->forward = rotate_vector_by_quaternion(rotation, camera->forward);
    camera->up      = normalize_v3(cross_v3(camera->right, camera->forward));
}


#endif
