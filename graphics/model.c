#ifndef MODEL_C_
#define MODEL_C_

#include "wings/base/allocators.c"
#include "wings/base/math.c"
#include "wings/base/types.c"
#include "wings/graphics/opengl.c"
#include "wings/os/file.c"

struct skinned_mesh_vertex
{
    struct v3  position;
    struct v3  normal;
    struct v2  uv;
    struct v4s joint_id;
    struct v4  joint_weight;
};

struct mesh_vertex
{
    struct v3 position;
    struct v3 normal;
    struct v2 uv;
};

struct meshi
{
    s32         number_of_vertices;
    b32         has_joints;
    struct v3  *position;
    struct v3  *normal;
    struct v2  *uv;
    struct v4s *joint_id;
    struct v4  *joint_weight;
};

struct gpu_mesh
{
    s32 number_of_vertices;
    u32 va;
    u32 vb;
};

struct joint
{
    struct quaternion rotation;
    struct v3         position;
    float             _padding;
};

struct skeleton
{
    struct mat4 *inverse_bind_translation;
    struct mat4 *interpolated_translation;
    struct mat4 *final_translation;
    s32         *parent_joint_index;
    s32          number_of_joints;
};

struct animation
{
    f32          *keyframe_time;
    struct joint *joint;
    s32           number_of_keyframes;
};

struct animation_collection
{
    struct skeleton   skeleton;
    struct animation *animation;
    char            **name;
    s32               number_of_animations;
};

struct material
{
    u32 texture;
};

struct model
{
    struct animation_collection animations;
    struct skeleton             skeleton;
    struct meshi                mesh;
    struct gpu_mesh             gpu_mesh;
};

struct phong_shader
{
    u32 handle;
    s32 model;
    s32 view;
    s32 projection;
    s32 camera_position;
    s32 palette;
} phong_shader = { 0 };

struct skinning_phong_shader
{
    u32         handle;
    s32         model;
    s32         view;
    s32         projection;
    s32         camera_position;
    s32         palette;
    s32         joints;
    const char *vertex_shader_text;
    const char *fragment_shader_text;
} skinning_phong_shader = { 0 };

error
load_phong_shader(struct allocator *allocator)
{
    struct buffer buffer    = { 0 };
    b32           error     = 0;
    char         *file_name = "phong.vs";

    error = read_file(&buffer, file_name, 1, allocator);
    if (error)
        return (error);
    char *vs  = (char *)buffer.base;
    file_name = "phong.fs";
    error     = read_file(&buffer, file_name, 1, allocator);
    if (error)
        return (error);
    char *fs = (char *)buffer.base;

    error = compile_shader_program(&phong_shader.handle, vs, fs);
    assert(error == 0 && phong_shader.handle);

    phong_shader.model           = glGetUniformLocation(phong_shader.handle, "model");
    phong_shader.view            = glGetUniformLocation(phong_shader.handle, "view");
    phong_shader.projection      = glGetUniformLocation(phong_shader.handle, "projection");
    phong_shader.camera_position = glGetUniformLocation(phong_shader.handle, "camera_position");
    phong_shader.palette         = glGetUniformLocation(phong_shader.handle, "palette");
    /*
    assert(phong_shader.palette    != -1 &&
           phong_shader.model      != -1 &&
           phong_shader.camera_position != -1 &&
           phong_shader.view       != -1 &&
           phong_shader.projection != -1);
           */
    return (NO_ERROR);
}

error
load_skinning_phong_shader(struct allocator *allocator)
{
    struct buffer buffer    = { 0 };
    b32           error     = 0;
    char         *file_name = "skinning_phong.vs";
    error                   = read_file(&buffer, file_name, 1, allocator);
    if (error)
        return (error);
    char *vs  = (char *)buffer.base;
    file_name = "skinning_phong.fs";
    error     = read_file(&buffer, file_name, 1, allocator);
    if (error)
        return (error);
    char *fs = (char *)buffer.base;
    error    = compile_shader_program(&skinning_phong_shader.handle, vs, fs);
    assert(error == 0 && skinning_phong_shader.handle);

    skinning_phong_shader.model           = glGetUniformLocation(skinning_phong_shader.handle, "model");
    skinning_phong_shader.view            = glGetUniformLocation(skinning_phong_shader.handle, "view");
    skinning_phong_shader.projection      = glGetUniformLocation(skinning_phong_shader.handle, "projection");
    skinning_phong_shader.camera_position = glGetUniformLocation(skinning_phong_shader.handle, "camera_position");
    skinning_phong_shader.palette         = glGetUniformLocation(skinning_phong_shader.handle, "palette");
    skinning_phong_shader.joints          = glGetUniformLocation(skinning_phong_shader.handle, "joint");
    assert(skinning_phong_shader.palette != -1 && skinning_phong_shader.model != -1 && skinning_phong_shader.camera_position != -1 && skinning_phong_shader.view != -1 && skinning_phong_shader.projection != -1);
    return (NO_ERROR);
}

struct gpu_mesh
upload_skinning_mesh(struct skinned_mesh_vertex *vertex, s32 number_of_vertices)
{
    struct gpu_mesh mesh    = { 0 };
    mesh.number_of_vertices = number_of_vertices;
    glGenVertexArrays(1, &mesh.va);
    glBindVertexArray(mesh.va);
    glGenBuffers(1, &mesh.vb);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vb);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct skinned_mesh_vertex), (void *)offsetof(struct skinned_mesh_vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct skinned_mesh_vertex), (void *)offsetof(struct skinned_mesh_vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct skinned_mesh_vertex), (void *)offsetof(struct skinned_mesh_vertex, uv));
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(struct skinned_mesh_vertex), (void *)offsetof(struct skinned_mesh_vertex, joint_id));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(struct skinned_mesh_vertex), (void *)offsetof(struct skinned_mesh_vertex, joint_weight));

    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(struct skinned_mesh_vertex) * number_of_vertices), vertex, GL_STATIC_DRAW);

    glBindVertexArray(0);

    return (mesh);
}

struct gpu_mesh
upload_meshi(struct mesh_vertex *vertex, s32 number_of_vertices)
{
    struct gpu_mesh mesh    = { 0 };
    mesh.number_of_vertices = number_of_vertices;
    glGenVertexArrays(1, &mesh.va);
    glBindVertexArray(mesh.va);
    glGenBuffers(1, &mesh.vb);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vb);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct mesh_vertex), (void *)offsetof(struct mesh_vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct mesh_vertex), (void *)offsetof(struct mesh_vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct mesh_vertex), (void *)offsetof(struct mesh_vertex, uv));

    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(sizeof(struct mesh_vertex) * number_of_vertices), vertex, GL_STATIC_DRAW);

    glBindVertexArray(0);

    return (mesh);
}

void
render_model(struct model model, struct phong_shader shader, struct mat4 projection, struct mat4 view, struct v3 camera_position, u32 palette, struct mat4 transform)
{
    glEnable(GL_CULL_FACE);
    glUseProgram(shader.handle);
    glUniform1i(shader.palette, 0);
    glUniformMatrix4fv(shader.projection, 1, 0, &projection.m00);
    glUniformMatrix4fv(shader.view, 1, 0, &view.m00);
    glUniform3fv(shader.camera_position, 1, &camera_position.x);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, palette);
    {
        struct gpu_mesh mesh = model.gpu_mesh;
        glBindVertexArray(mesh.va);
        struct mat4 model_matrix = transform;
        glUniformMatrix4fv(shader.model, 1, 0, &model_matrix.m00);

        glDrawArrays(GL_TRIANGLES, 0, mesh.number_of_vertices);
        glBindVertexArray(0);
    }
    // GL_ASSERT(0UL);
}

error
render_model_skinned(struct model model, struct skinning_phong_shader shader, struct mat4 projection, struct mat4 view, struct v3 camera_position, u32 palette, struct mat4 transform)
{
    glEnable(GL_CULL_FACE);
    glUseProgram(shader.handle);
    glUniform1i(shader.palette, 0);
    glUniformMatrix4fv(shader.projection, 1, 0, &projection.m00);
    glUniformMatrix4fv(shader.view, 1, 0, &view.m00);
    glUniform3fv(shader.camera_position, 1, &camera_position.x);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, palette);

    {
        struct gpu_mesh mesh = model.gpu_mesh;
        glBindVertexArray(mesh.va);
        struct mat4 model_matrix = transform;
        glUniformMatrix4fv(shader.model, 1, 0, &model_matrix.m00);
        glUniformMatrix4fv(shader.joints,
                           model.skeleton.number_of_joints,
                           0,
                           (float *)model.skeleton.final_translation);

        glDrawArrays(GL_TRIANGLES, 0, mesh.number_of_vertices);
        glBindVertexArray(0);
    }
    GL_CHECK(1);
    return (NO_ERROR);
}

struct animation
make_animation(s32               number_of_joints,
               s32               number_of_keyframes,
               struct joint     *joint,
               float            *keyframe_time,
               struct allocator *allocator)
{
    struct animation animation    = { 0 };
    animation.number_of_keyframes = number_of_keyframes;

    error error = NO_ERROR;
    //@TODO: errors not handled
    error = allocate_array(&animation.keyframe_time,
                           allocator,
                           number_of_keyframes,
                           float);
    ASSERT(!error);
    error = allocate_array(&animation.joint,
                           allocator,
                           number_of_keyframes * number_of_joints, struct joint);
    ASSERT(!error);

    for (s32 index = 0;
         index < number_of_keyframes;
         ++index)
    {
        animation.keyframe_time[index] = keyframe_time[index];
    }
    for (s32 index = 0;
         index < number_of_keyframes * number_of_joints;
         ++index)
    {
        animation.joint[index] = joint[index];
    }
    return (animation);
}

struct v2s
find_frame_indices(float *keyframe_time, s32 number_of_keyframes, f32 time)
{
    s32 current_frame = 0;
    for (; current_frame < number_of_keyframes - 1;
         ++current_frame)
    {
        f32 next_keyframe_time = keyframe_time[current_frame + 1];
        if (time < next_keyframe_time)
            return make_v2s(current_frame, current_frame + 1);
    }
    return make_v2s(current_frame, current_frame);
}

void
update_joints_1(struct mat4 *translation, struct skeleton skeleton, struct animation animation, float time)
{
    struct v2s frame_indices = find_frame_indices(animation.keyframe_time, animation.number_of_keyframes, time);
    s32        last_frame    = frame_indices.x;
    s32        next_frame    = frame_indices.y;
    f32        fraction      = map_f32(time,
                                       animation.keyframe_time[last_frame],
                                       animation.keyframe_time[next_frame],
                                       0.0f,
                                       1.0f);
    if (last_frame == next_frame)
        fraction = 0.5f;
    struct joint *last_pose = animation.joint + (last_frame * skeleton.number_of_joints);
    struct joint *next_pose = animation.joint + (next_frame * skeleton.number_of_joints);
    for (int joint_index = 0;
         joint_index < skeleton.number_of_joints;
         ++joint_index)
    {
        struct joint      last_joint = last_pose[joint_index];
        struct joint      next_joint = next_pose[joint_index];
        struct v3         position   = lerp_v3(last_joint.position, next_joint.position, fraction);
        struct quaternion rotation   = nlerp(last_joint.rotation, next_joint.rotation, fraction);
        translation[joint_index]     = mul_mat4(make_translation_matrix(position), quaternion_to_matrix(rotation));
    }
}

void
update_joints(struct skeleton skeleton, struct animation animation, float time)
{
    struct v2s frame_indices = find_frame_indices(animation.keyframe_time, animation.number_of_keyframes, time);
    s32        last_frame    = frame_indices.x;
    s32        next_frame    = frame_indices.y;
    float      fraction      = map_f32(time,
                                       animation.keyframe_time[last_frame],
                                       animation.keyframe_time[next_frame],
                                       0.0f,
                                       1.0f);
    if (last_frame == next_frame)
        fraction = 0.5f;
    struct joint *last_pose = animation.joint + (last_frame * skeleton.number_of_joints);
    struct joint *next_pose = animation.joint + (next_frame * skeleton.number_of_joints);

    skeleton.interpolated_translation[0] = mat4_identity;

    for (int joint_index = 0;
         joint_index < skeleton.number_of_joints;
         ++joint_index)
    {
        struct joint      last_joint                     = last_pose[joint_index];
        struct joint      next_joint                     = next_pose[joint_index];
        struct v3         position                       = lerp_v3(last_joint.position, next_joint.position, fraction);
        struct quaternion rotation                       = nlerp(last_joint.rotation, next_joint.rotation, fraction);
        struct mat4       local_interpolated_translation = mul_mat4(make_translation_matrix(position), quaternion_to_matrix(rotation));

        skeleton.interpolated_translation[joint_index] = mul_mat4(skeleton.interpolated_translation[skeleton.parent_joint_index[joint_index]],
                                                                  local_interpolated_translation);
        skeleton.final_translation[joint_index]        = mul_mat4(skeleton.interpolated_translation[joint_index], skeleton.inverse_bind_translation[joint_index]);
    }
}

#endif
