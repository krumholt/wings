#ifndef WINGS_BASE_SHAPES_C_
#define WINGS_BASE_SHAPES_C_

#if !defined WINGS_BASE_ALLOCATORS_C
#include "wings/base/allocators.c"
#endif
#if !defined WINGS_BASE_MATH_C
#include "wings/base/math.c"
#endif

struct triangle2
{
    struct v2 a;
    struct v2 b;
    struct v2 c;
};

struct aab2
{
    struct v2 min;
    struct v2 max;
};

struct circle
{
    struct v2 position;
    f32       radius;
    f32       rotation;
};

struct box2
{
    struct v2 position;
    struct v2 half_extend;
    f32       rotation;
};
//        e1
//   v2 ------ v1
//    |        |
// e2 |        | e4
//    |        |
//   v3 ------ v4
//        e3
enum box2_axis
{
    box2_axis_FACE_A_X,
    box2_axis_FACE_A_Y,
    box2_axis_FACE_B_X,
    box2_axis_FACE_B_Y,
};

enum box2_edge_number
{
    box2_edge_number_NO_EDGE = 0,
    box2_edge_number_EDGE_1,
    box2_edge_number_EDGE_2,
    box2_edge_number_EDGE_3,
    box2_edge_number_EDGE_4,
};

struct line_segment2
{
    struct v2 start;
    struct v2 end;
};

struct poly_line2
{
    struct v2 *points;
    s32        number_of_points;
    s32        max_number_of_points;
};

struct contact_point
{
    struct v2 position;
    struct v2 normal;
    f32       spacing;
};

f32 distance_line_point(struct v2 a, struct v2 b, struct v2 point);
f32 distance_line_segment_point(struct v2 a, struct v2 b, struct v2 point);

struct poly_line2
make_poly_line2(s32 max_number_of_points, struct allocator *allocator)
{
    struct poly_line2 line    = { 0 };
    line.max_number_of_points = max_number_of_points;
    error error = allocate_array(&line.points, allocator, max_number_of_points, struct v2);
	if (error) // @TODO: @FIXME
		printf("ERROR\n");
    return (line);
}

inline struct v2
get_center_aab2(struct aab2 aab)
{
    struct v2 center = mul_f32_v2(0.5f, add_v2(aab.min, aab.max));
    return (center);
}

inline struct aab2
move_aab2(struct aab2 box, struct v2 amount)
{
    struct v2   new_min = add_v2(box.min, amount);
    struct v2   new_max = add_v2(box.max, amount);
    struct aab2 result  = { new_min, new_max };
    return (result);
}

inline struct aab2
grow_aab2(struct aab2 r, float value)
{
    struct v2   growth  = { value, value };
    struct v2   new_min = sub_v2(r.min, growth);
    struct v2   new_max = add_v2(r.max, growth);
    struct aab2 result  = { new_min, new_max };
    return (result);
}

inline struct aab2
shrink_aab2(struct aab2 r, float value)
{
    struct aab2 result = grow_aab2(r, -value);
    return (result);
}

b32
is_point_in_aab2(struct v2 point, struct aab2 aab)
{
    return (point.x >= aab.min.x && point.x <= aab.max.x && point.y >= aab.min.y && point.y <= aab.max.y);
}

b32
is_point_in_box2(struct v2 point, struct box2 box)
{
    struct mat2 rotation            = make_rotation_mat2(box.rotation);
    struct mat2 rotation_transposed = transpose_mat2(rotation);

    struct v2 center_to_point = sub_v2(point, box.position);
    struct v2 d_a             = mul_mat2_v2(rotation_transposed, center_to_point);

    struct v2 penetration = sub_v2(abs_v2(d_a),
                                   box.half_extend);
    return penetration.x <= 0 && penetration.y <= 0;
}

b32
_is_point_in_circle(struct v2 point, struct v2 position, f32 radius)
{
    struct v2 point_to_circle = sub_v2(position, point);
    return (length_v2(point_to_circle) <= radius);
}

inline b32
is_point_in_circle(struct v2 point, struct circle circle)
{
    return (_is_point_in_circle(point, circle.position, circle.radius));
}

b32
is_point_left_of_segment(struct v2 point,
                         struct v2 a,
                         struct v2 b)
{
    return (cross_v2(sub_v2(b, a), sub_v2(point, a)) >= 0);
}

b32
is_point_in_triangle(struct v2 point,
                     struct v2 a,
                     struct v2 b,
                     struct v2 c)
{
    b32 b1 = is_point_left_of_segment(point, a, b);
    b32 b2 = is_point_left_of_segment(point, b, c);
    b32 b3 = is_point_left_of_segment(point, c, a);
    return b1 && b2 && b3;
}

b32
is_point_in_triangles(struct v2  point,
                      struct v2 *vertex,
                      s32        number_of_vertices,
                      struct v2  position)
{
    assert(number_of_vertices % 3 == 0);
    b32 result = 0;
    for (s32 index = 0; index < number_of_vertices; index += 3)
    {
        struct v2 a = add_v2(vertex[index + 0], position);
        struct v2 b = add_v2(vertex[index + 1], position);
        struct v2 c = add_v2(vertex[index + 2], position);
        result      = is_point_in_triangle(point, a, b, c);
        if (result)
            return (result);
    }
    return (0);
}

b32
line_segment_line_segment_intersection(
    struct v2  a_start,
    struct v2  a_end,
    struct v2  b_start,
    struct v2  b_end,
    struct v2 *result)
{
    struct v2 a = sub_v2(a_end, a_start);
    if (a.x == 0.0f && a.y == 0.0f)
        return 0; //@TODO: point, line segement could technically still collide
    struct v2 b = sub_v2(b_end, b_start);
    if (b.x == 0.0f && b.y == 0.0f)
        return 0; //@TODO: point, line segement could technically still collide

    f32 x1          = a_start.x;
    f32 x2          = a_end.x;
    f32 x3          = b_start.x;
    f32 x4          = b_end.x;
    f32 y1          = a_start.y;
    f32 y2          = a_end.y;
    f32 y3          = b_start.y;
    f32 y4          = b_end.y;
    f32 denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (denominator == 0.0f)
        return 0;

    f32 t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
    f32 u = ((x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2)) / denominator;
    if (!(0.0 <= t && t <= 1.0f && 0.0f <= u && u <= 1.0f))
        return 0;
    result->x = x1 + t * (x2 - x1);
    result->y = y1 + t * (y2 - y1);
    return 1;
}

b32
line_line_intersection(
    struct v2  a_start,
    struct v2  a_end,
    struct v2  b_start,
    struct v2  b_end,
    struct v2 *intersection_point)
{
    struct v2 a = sub_v2(a_end, a_start);
    if (a.x == 0.0f && a.y == 0.0f)
        return 0;
    struct v2 b = sub_v2(b_end, b_start);
    if (b.x == 0.0f && b.y == 0.0f)
        return 0;

    f32 x1          = a_start.x;
    f32 x2          = a_end.x;
    f32 x3          = b_start.x;
    f32 x4          = b_end.x;
    f32 y1          = a_start.y;
    f32 y2          = a_end.y;
    f32 y3          = b_start.y;
    f32 y4          = b_end.y;
    f32 denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (denominator == 0.0f)
        return 0;

    f32 numerator_x       = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
    f32 numerator_y       = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);
    intersection_point->x = numerator_x / denominator;
    intersection_point->y = numerator_y / denominator;
    return 1;
}

b32
_circle_circle_intersection(struct v2 center_a, f32 radius_a,
                            struct v2 center_b, f32 radius_b,
                            struct v2 *normal,
                            f32       *separation)
{
    struct v2 a_to_b           = sub_v2(center_b, center_a);
    f32       distance_squared = dot_v2(a_to_b, a_to_b);
    f32       radius_squared   = (radius_a + radius_b) * (radius_a + radius_b);
    if (distance_squared > radius_squared)
        return 0;
    *normal     = mul_f32_v2(1.0f / sqrtf(distance_squared), a_to_b);
    *separation = sqrtf(distance_squared) - radius_a - radius_b;
    return 1;
}

s32
_circle_circle_contact_points(struct v2 center_a, f32 radius_a,
                              struct v2 center_b, f32 radius_b,
                              struct contact_point *contact_points)
{
    struct v2 a_to_b           = sub_v2(center_b, center_a);
    f32       distance_squared = dot_v2(a_to_b, a_to_b);
    f32       radius_squared   = (radius_a + radius_b) * (radius_a + radius_b);
    if (distance_squared > radius_squared)
        return 0;
    contact_points[0].normal   = mul_f32_v2(1.0f / sqrtf(distance_squared), a_to_b);
    contact_points[0].spacing  = sqrtf(distance_squared) - radius_a - radius_b;
    contact_points[0].position = add_v2(center_a,
                                        mul_f32_v2(radius_a,
                                                   contact_points[0].normal));
    return 1;
}

inline s32
circle_circle_contact_points(struct circle         circle_a,
                             struct circle         circle_b,
                             struct contact_point *contact_points)
{
    return (
        _circle_circle_contact_points(
            circle_a.position, circle_a.radius,
            circle_b.position, circle_b.radius,
            contact_points));
}

b32
_box2_circle_intersection(struct v2  box_position,
                          struct v2  box_half_extend,
                          f32        box_rotation,
                          struct v2  center,
                          f32        radius,
                          struct v2 *normal,
                          f32       *separation)
{
    struct mat2 rotation            = make_rotation_mat2(box_rotation);
    struct mat2 rotation_transposed = transpose_mat2(rotation);

    struct v2 box_to_circle = sub_v2(center, box_position);
    struct v2 d_box         = mul_mat2_v2(rotation_transposed, box_to_circle);

    struct v2 closest_point = { 0 };
    closest_point.x         = max(-box_half_extend.x,
                                  min(box_half_extend.x, d_box.x));
    closest_point.y         = max(-box_half_extend.y,
                                  min(box_half_extend.y, d_box.y));
    closest_point           = mul_mat2_v2(rotation, closest_point);
    closest_point           = add_v2(closest_point, box_position);
    struct v2 offset        = sub_v2(center, closest_point);
    *separation             = length_v2(offset) - radius;
    *normal                 = normalize_v2(offset);

    return (*separation < 0.0f);
}

inline s32
box2_circle_intersection(struct box2   box,
                         struct circle circle,
                         struct v2    *normal,
                         f32          *separation)
{
    return _box2_circle_intersection(box.position,
                                     box.half_extend,
                                     box.rotation,
                                     circle.position,
                                     circle.radius,
                                     normal,
                                     separation);
}

s32
_box2_circle_contact_points(struct v2             box_position,
                            struct v2             box_half_extend,
                            f32                   box_rotation,
                            struct v2             circle_position,
                            f32                   circle_radius,
                            struct contact_point *contact_points)
{
    struct mat2 rotation            = make_rotation_mat2(box_rotation);
    struct mat2 rotation_transposed = transpose_mat2(rotation);

    struct v2 box_to_circle = sub_v2(circle_position, box_position);
    struct v2 d_box         = mul_mat2_v2(rotation_transposed, box_to_circle);

    struct v2 closest_point = { 0 };
    closest_point.x         = max(-box_half_extend.x,
                                  min(box_half_extend.x, d_box.x));
    closest_point.y         = max(-box_half_extend.y,
                                  min(box_half_extend.y, d_box.y));
    if (closest_point.x == d_box.x && closest_point.y == d_box.y)
    {
    }
    closest_point = mul_mat2_v2(rotation, closest_point);
    closest_point = add_v2(closest_point, box_position);
    // printf("%f, %f\n", closest_point.x, closest_point.y);
    struct v2 offset  = sub_v2(circle_position, closest_point);
    f32       spacing = length_v2(offset) - circle_radius;
    if (spacing > 0.0f)
        return 0;
    struct v2 normal = normalize_v2(offset);

    contact_points[0].position = closest_point;
    contact_points[0].normal   = normal;
    contact_points[0].spacing  = spacing;
    return (1);
}

inline s32
box2_circle_contact_points(struct box2           box,
                           struct circle         circle,
                           struct contact_point *contact_points)
{
    return (_box2_circle_contact_points(box.position,
                                        box.half_extend,
                                        box.rotation,
                                        circle.position,
                                        circle.radius,
                                        contact_points));
}

inline b32
circle_circle_intersection(struct circle c1, struct circle c2, struct v2 *normal, f32 *separation)
{
    return (_circle_circle_intersection(c1.position, c1.radius,
                                        c2.position, c2.radius,
                                        normal, separation));
}

b32
_box2_box2_intersection(struct v2       box_a_position,
                        struct v2       box_a_half_extend,
                        f32             box_a_rotation,
                        struct v2       box_b_position,
                        struct v2       box_b_half_extend,
                        f32             box_b_rotation,
                        struct v2      *normal,
                        f32            *separation,
                        enum box2_axis *axis)
{
    struct v2   position_a       = box_a_position;
    struct v2   position_b       = box_b_position;
    struct mat2 rot_a            = make_rotation_mat2(box_a_rotation);
    struct mat2 rot_b            = make_rotation_mat2(box_b_rotation);
    struct mat2 rot_a_transposed = transpose_mat2(rot_a);
    struct mat2 rot_b_transposed = transpose_mat2(rot_b);

    struct v2   a_to_b          = sub_v2(position_b, position_a);
    struct v2   d_a             = mul_mat2_v2(rot_a_transposed, a_to_b);
    struct v2   d_b             = mul_mat2_v2(rot_b_transposed, a_to_b);
    struct mat2 from_b_to_a     = mul_mat2(rot_a_transposed, rot_b);
    struct mat2 abs_from_b_to_a = abs_mat2(from_b_to_a);
    struct mat2 abs_from_a_to_b = transpose_mat2(abs_from_b_to_a);

    struct v2 face_a = sub_v2(sub_v2(abs_v2(d_a),
                                     box_a_half_extend),
                              mul_mat2_v2(abs_from_b_to_a, box_b_half_extend));
    if (face_a.x > 0.0f || face_a.y > 0.0f)
        return 0;

    struct v2 face_b = sub_v2(sub_v2(abs_v2(d_b),
                                     box_b_half_extend),
                              mul_mat2_v2(abs_from_a_to_b, box_a_half_extend));
    if (face_b.x > 0.0f || face_b.y > 0.0f)
        return 0;

    *axis       = box2_axis_FACE_A_X;
    *separation = face_a.x;
    if (d_a.x > 0.0f)
        *normal = make_v2(rot_a.m00, rot_a.m01);
    else
        *normal = make_v2(-rot_a.m00, -rot_a.m01);

    const f32 relative_tolerance = 0.95f;
    const f32 absolute_tolerance = 0.01f;

    if (face_a.y > relative_tolerance * *separation
                       + absolute_tolerance * box_a_half_extend.y)
    {
        *axis       = box2_axis_FACE_A_Y;
        *separation = face_a.y;
        *normal     = d_a.y > 0.0f ? make_v2(rot_a.m10, rot_a.m11)
                                   : make_v2(-rot_a.m10, -rot_a.m11);
    }

    if (face_b.x > relative_tolerance * *separation
                       + absolute_tolerance * box_b_half_extend.x)
    {
        *axis       = box2_axis_FACE_B_X;
        *separation = face_b.x;
        *normal     = d_b.x > 0.0f ? make_v2(rot_b.m00, rot_b.m01)
                                   : make_v2(-rot_b.m00, -rot_b.m01);
    }

    if (face_b.y > relative_tolerance * *separation
                       + absolute_tolerance * box_b_half_extend.y)
    {
        *axis       = box2_axis_FACE_B_Y;
        *separation = face_b.y;
        *normal     = d_b.y > 0.0f ? make_v2(rot_b.m10, rot_b.m11)
                                   : make_v2(-rot_b.m10, -rot_b.m11);
    }
    return 1;
}

inline b32
box2_box2_intersection(struct box2     box_a,
                       struct box2     box_b,
                       struct v2      *normal,
                       f32            *separation,
                       enum box2_axis *axis)
{
    return _box2_box2_intersection(box_a.position,
                                   box_a.half_extend,
                                   box_a.rotation,
                                   box_b.position,
                                   box_b.half_extend,
                                   box_b.rotation,
                                   normal,
                                   separation,
                                   axis);
}

s32
clip_segment_to_line(struct v2             out[2],
                     struct v2             in[2],
                     struct v2             normal,
                     f32                   offset,
                     enum box2_edge_number clip_edge)
{
	UNUSED(clip_edge);
    s32 number_of_points = 0;

    f32 distance_0 = dot_v2(normal, in[0]) - offset;
    f32 distance_1 = dot_v2(normal, in[1]) - offset;

    if (distance_0 <= 0.0f)
        out[number_of_points++] = in[0];
    if (distance_1 <= 0.0f)
        out[number_of_points++] = in[1];

    if (distance_0 * distance_1 < 0.0f)
    {
        f32 interpolated      = distance_0 / (distance_0 - distance_1);
        out[number_of_points] = add_v2(
            in[0],
            mul_f32_v2(interpolated, sub_v2(in[1], in[0])));
        ++number_of_points;
    }
    return (number_of_points);
}
void
compute_incident_edge(struct v2   edge[2],
                      struct v2   half_extend,
                      struct v2   position,
                      struct mat2 rotation,
                      struct v2   normal)
{
    struct mat2 rotation_inverse    = transpose_mat2(rotation);
    struct v2   normal_in_box_frame = mul_f32_v2(
        -1.0f, mul_mat2_v2(rotation_inverse, normal));
    struct v2 normal_in_box_frame_abs = abs_v2(normal_in_box_frame);

    if (normal_in_box_frame_abs.x > normal_in_box_frame_abs.y)
    {
        if (normal_in_box_frame.x >= 0.0f)
        {
            edge[0] = make_v2(half_extend.x, -half_extend.y);

            edge[1] = make_v2(half_extend.x, half_extend.y);
        }
        else
        {
            edge[0] = make_v2(-half_extend.x, half_extend.y);

            edge[1] = make_v2(-half_extend.x, -half_extend.y);
        }
    }
    else
    {
        if (normal_in_box_frame.y >= 0.0f)
        {
            edge[0] = make_v2(half_extend.x, half_extend.y);

            edge[1] = make_v2(-half_extend.x, half_extend.y);
        }
        else
        {
            edge[0] = make_v2(-half_extend.x, -half_extend.y);

            edge[1] = make_v2(half_extend.x, -half_extend.y);
        }
    }
    edge[0] = add_v2(position, mul_mat2_v2(rotation, edge[0]));
    edge[1] = add_v2(position, mul_mat2_v2(rotation, edge[1]));
}

s32
_box2_box2_contact_points(struct v2             position_a,
                          struct v2             half_extend_a,
                          f32                   rotation_a,
                          struct v2             position_b,
                          struct v2             half_extend_b,
                          f32                   rotation_b,
                          struct contact_point *contact_points)
{
    struct mat2 rot_a            = make_rotation_mat2(rotation_a);
    struct mat2 rot_b            = make_rotation_mat2(rotation_b);
    struct mat2 rot_a_transposed = transpose_mat2(rot_a);
    struct mat2 rot_b_transposed = transpose_mat2(rot_b);

    struct v2   a_to_b          = sub_v2(position_b, position_a);
    struct v2   d_a             = mul_mat2_v2(rot_a_transposed, a_to_b);
    struct v2   d_b             = mul_mat2_v2(rot_b_transposed, a_to_b);
    struct mat2 from_b_to_a     = mul_mat2(rot_a_transposed, rot_b);
    struct mat2 abs_from_b_to_a = abs_mat2(from_b_to_a);
    struct mat2 abs_from_a_to_b = transpose_mat2(abs_from_b_to_a);

    struct v2 face_a = sub_v2(sub_v2(abs_v2(d_a),
                                     half_extend_a),
                              mul_mat2_v2(abs_from_b_to_a, half_extend_b));
    if (face_a.x > 0.0f || face_a.y > 0.0f)
        return 0;

    struct v2 face_b = sub_v2(sub_v2(abs_v2(d_b),
                                     half_extend_b),
                              mul_mat2_v2(abs_from_a_to_b, half_extend_a));
    if (face_b.x > 0.0f || face_b.y > 0.0f)
        return 0;

    enum box2_axis axis    = box2_axis_FACE_A_X;
    f32            spacing = face_a.x;
    struct v2      normal  = { 0 };
    if (d_a.x > 0.0f)
        normal = make_v2(rot_a.m00, rot_a.m01);
    else
        normal = make_v2(-rot_a.m00, -rot_a.m01);

    const f32 relative_tolerance = 0.95f;
    const f32 absolute_tolerance = 0.01f;

    if (face_a.y > relative_tolerance * spacing
                       + absolute_tolerance * half_extend_a.y)
    {
        axis    = box2_axis_FACE_A_Y;
        spacing = face_a.y;
        normal  = d_a.y > 0.0f ? make_v2(rot_a.m10, rot_a.m11)
                               : make_v2(-rot_a.m10, -rot_a.m11);
    }

    if (face_b.x > relative_tolerance * spacing
                       + absolute_tolerance * half_extend_b.x)
    {
        axis    = box2_axis_FACE_B_X;
        spacing = face_b.x;
        normal  = d_b.x > 0.0f ? make_v2(rot_b.m00, rot_b.m01)
                               : make_v2(-rot_b.m00, -rot_b.m01);
    }

    if (face_b.y > relative_tolerance * spacing
                       + absolute_tolerance * half_extend_b.y)
    {
        axis    = box2_axis_FACE_B_Y;
        spacing = face_b.y;
        normal  = d_b.y > 0.0f ? make_v2(rot_b.m10, rot_b.m11)
                               : make_v2(-rot_b.m10, -rot_b.m11);
    }

    struct v2             front_normal, side_normal;
    struct v2             incident_edge[2];
    f32                   front, negative_side, positive_side;
    enum box2_edge_number negative_edge, positive_edge;

    switch (axis)
    {
    case box2_axis_FACE_A_X:
    {
        front_normal  = normal;
        front         = dot_v2(position_a, front_normal) + half_extend_a.x;
        side_normal   = make_v2(rot_a.m10, rot_a.m11);
        f32 side      = dot_v2(position_a, side_normal);
        negative_side = -side + half_extend_a.y;
        positive_side = side + half_extend_a.y;
        negative_edge = box2_edge_number_EDGE_3;
        positive_edge = box2_edge_number_EDGE_1;
        compute_incident_edge(incident_edge, half_extend_b, position_b, rot_b, front_normal);
    }
    break;
    case box2_axis_FACE_A_Y:
    {
        front_normal  = normal;
        front         = dot_v2(position_a, front_normal) + half_extend_a.y;
        side_normal   = make_v2(rot_a.m00, rot_a.m01);
        f32 side      = dot_v2(position_a, side_normal);
        negative_side = -side + half_extend_a.x;
        positive_side = side + half_extend_a.x;
        negative_edge = box2_edge_number_EDGE_2;
        positive_edge = box2_edge_number_EDGE_4;
        compute_incident_edge(incident_edge, half_extend_b, position_b, rot_b, front_normal);
    }
    break;
    case box2_axis_FACE_B_X:
    {
        front_normal  = mul_f32_v2(-1.0f, normal);
        front         = dot_v2(position_b, front_normal) + half_extend_b.x;
        side_normal   = make_v2(rot_b.m10, rot_b.m11);
        f32 side      = dot_v2(position_b, side_normal);
        negative_side = -side + half_extend_b.y;
        positive_side = side + half_extend_b.y;
        negative_edge = box2_edge_number_EDGE_3;
        positive_edge = box2_edge_number_EDGE_1;
        compute_incident_edge(incident_edge, half_extend_a, position_a, rot_a, front_normal);
    }
    break;
    case box2_axis_FACE_B_Y:
    {
        front_normal  = mul_f32_v2(-1.0f, normal);
        front         = dot_v2(position_b, front_normal) + half_extend_b.y;
        side_normal   = make_v2(rot_b.m00, rot_b.m01);
        f32 side      = dot_v2(position_b, side_normal);
        negative_side = -side + half_extend_b.x;
        positive_side = side + half_extend_b.x;
        negative_edge = box2_edge_number_EDGE_2;
        positive_edge = box2_edge_number_EDGE_4;
        compute_incident_edge(incident_edge, half_extend_a, position_a, rot_a, front_normal);
    }
    break;
    }

    struct v2 clip_points_1[2];
    struct v2 clip_points_2[2];

    s32 number_of_points;

    number_of_points = clip_segment_to_line(
        clip_points_1,
        incident_edge,
        mul_f32_v2(-1.0f, side_normal),
        negative_side,
        negative_edge);

    if (number_of_points < 2)
        return 0;

    number_of_points = clip_segment_to_line(
        clip_points_2,
        clip_points_1,
        side_normal,
        positive_side,
        positive_edge);

    if (number_of_points < 2)
        return 0;

    s32 number_of_contacts = 0;
    for (s32 index = 0;
         index < 2;
         ++index)
    {
        f32 spacing = dot_v2(front_normal, clip_points_2[index]) - front;

        if (spacing <= 0)
        {
            contact_points[number_of_contacts].position = clip_points_2[index];
            contact_points[number_of_contacts].normal   = normal;
            contact_points[number_of_contacts].spacing  = spacing;
            ++number_of_contacts;
        }
    }
    return number_of_contacts;
}

inline s32
box2_box2_contact_points(struct box2          box_a,
                         struct box2          box_b,
                         struct contact_point contact_points[2])
{
    return (_box2_box2_contact_points(box_a.position,
                                      box_a.half_extend,
                                      box_a.rotation,
                                      box_b.position,
                                      box_b.half_extend,
                                      box_b.rotation,
                                      contact_points));
}

s32
circle_poly_line2_contact_points(struct circle        circle,
                                 struct poly_line2    line,
                                 struct contact_point contact_points[2])
{
    //f32 closest_distance   = FLT_MAX;
    s32 number_of_contacts = 0;
    for (s32 index = 0;
         index < line.number_of_points - 1;
         ++index)
    {
        struct v2 start = line.points[index];
        struct v2 end   = line.points[index + 1];

        struct v2 start_to_end            = sub_v2(end, start);
        struct v2 start_to_end_normalized = normalize_v2(start_to_end);
        struct v2 start_to_center         = sub_v2(circle.position, start);

        f32       point_on_line = dot_v2(start_to_center, start_to_end_normalized);
        struct v2 closest_point = add_v2(start,
                                         mul_f32_v2(point_on_line,
                                                    start_to_end_normalized));
        if (point_on_line < 0.0f)
            closest_point = start;
        if (point_on_line > length_v2(start_to_end))
            closest_point = end;

        struct v2 normal   = normalize_v2(sub_v2(circle.position, closest_point)); //{-start_to_end_normalized.y, start_to_end_normalized.x};
        f32       distance = length_v2(sub_v2(closest_point, circle.position));
        f32       spacing  = distance - circle.radius;
        if (spacing <= 0.0f)
        {
            if (dot_v2(normal, sub_v2(closest_point, circle.position)) >= 0.0f)
            {
                spacing = -distance - circle.radius;
            }
            contact_points[number_of_contacts].position = closest_point;
            contact_points[number_of_contacts].normal   = normal;
            contact_points[number_of_contacts].spacing  = spacing;
            number_of_contacts++;
            if (number_of_contacts == 2)
                return 2;
        }
    }
    return number_of_contacts;
}

struct aab2
get_cell_aab2(struct aab2 area, s32 columns, s32 rows, s32 index)
{
    s32 x = index % columns;
    s32 y = index / columns;

    struct v2   size      = sub_v2(area.max, area.min);
    struct v2   cell_size = { size.x / columns, size.y / rows };
    struct v2   start     = { cell_size.x * x + area.min.x,
                              cell_size.y * y + area.min.y };
    struct v2   end       = add_v2(start, cell_size);
    struct aab2 cell      = { start, end };
    return (cell);
}

f32
get_width_aab2(struct aab2 box)
{
    return (box.max.x - box.min.x);
}

f32
get_height_aab2(struct aab2 box)
{
    return (box.max.y - box.min.y);
}

f32
distance_line_point(struct v2 a, struct v2 b, struct v2 point)
{
    if (a.x == b.x && a.y == b.y)
        return length_v2(sub_v2(a, point));
    struct v2 a_to_b   = sub_v2(b, a);
    f32       distance = cross_v2(sub_v2(a, point), a_to_b) / length_v2(a_to_b);

    return fabsf(distance);
}

struct v2
project_point_on_line_segment(struct v2 point, struct v2 a, struct v2 b)
{
    struct v2 a_to_b            = sub_v2(b, a);
    struct v2 a_to_b_normalized = normalize_v2(a_to_b);
    struct v2 a_to_point        = sub_v2(point, a);
    f32       point_on_a_to_b   = dot_v2(a_to_point, a_to_b_normalized);
    if (point_on_a_to_b < 0.0f)
        return a;
    if (point_on_a_to_b > length_v2(a_to_b))
        return b;
    struct v2 target = mul_f32_v2(point_on_a_to_b, a_to_b_normalized);
    target           = add_v2(target, a);
    return (target);
}

f32
distance_line_segment_point(struct v2 a, struct v2 b, struct v2 point)
{
    struct v2 point_on_segment = project_point_on_line_segment(point, a, b);
    return length_v2(sub_v2(point_on_segment, point));
}

#endif
