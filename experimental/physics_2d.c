#include "geometry_2d.c"
#include "types.h"
#include "wings_math.c"

struct rigid_body2
{
   struct v2 velocity;
   struct v2 force;
   f32       angular_velocity;
   f32       torque;
   f32       mass;
   f32       inverse_mass;
   f32       friction;
   f32       inertia;
   f32       inverse_inertia;
   f32       restitution;
};

struct contact2
{
   struct contact_point contact_points[2];
   struct v2            r1[2];
   struct v2            r2[2];
   s32                  body_1;
   s32                  body_2;
   f32                  mass_normal[2];
   f32                  mass_tangent[2];
   f32                  bias[2];
   s32                  number_of_contact_points;
};

struct circle_joint2
{
   s32 circle_1;
   s32 circle_2;
   s32 body_1;
   s32 body_2;
   f32 mass_normal;
   f32 distance;
};

struct circle_joint
{
   struct mat2 M;
   struct v2   local_anchor_1;
   struct v2   local_anchor_2;
   struct v2   r1;
   struct v2   r2;
   struct v2   bias;
   struct v2   P;
   s32         circle_1;
   s32         circle_2;
   f32         bias_factor;
   f32         softness;
};

struct physics_2d_world
{
   struct contact2   *contacts;
   struct box2       *boxes;
   struct circle     *circles;
   struct poly_line2 *lines;

   struct rigid_body2   *bodies;
   struct circle_joint  *circle_joints;
   struct circle_joint2 *circle_joints2;

   s32 *box_bodies;
   s32 *circle_bodies;
   s32 *line_bodies;
   s32  max_number_of_boxes;
   s32  number_of_boxes;
   s32  max_number_of_circles;
   s32  number_of_circles;
   s32  max_number_of_lines;
   s32  number_of_lines;
   s32  max_number_of_contacts;
   s32  number_of_contacts;
   s32  max_number_of_circle_joints;
   s32  number_of_circle_joints;
   s32  max_number_of_circle_joints2;
   s32  number_of_circle_joints2;
   s32  number_of_bodies;

   struct v2 gravity;
};

struct physics_2d_world
make_physics_2d_world(s32            max_number_of_boxes,
                      s32            max_number_of_circles,
                      s32            max_number_of_lines,
                      s32            max_number_of_circle_joints,
                      s32            max_number_of_circle_joints2,
                      struct memory *memory)
{
   struct physics_2d_world world      = { 0 };
   world.max_number_of_boxes          = max_number_of_boxes;
   world.max_number_of_circles        = max_number_of_circles;
   world.max_number_of_lines          = max_number_of_lines;
   world.max_number_of_circle_joints  = max_number_of_circle_joints;
   world.max_number_of_circle_joints2 = max_number_of_circle_joints2;
   world.max_number_of_contacts       = (max_number_of_boxes
                                   + max_number_of_circles)
                                  * 10;
   world.boxes          = allocate_array(memory, max_number_of_boxes, struct box2);
   world.box_bodies     = allocate_array(memory, max_number_of_boxes, s32);
   world.circles        = allocate_array(memory, max_number_of_circles, struct circle);
   world.circle_bodies  = allocate_array(memory, max_number_of_circles, s32);
   world.lines          = allocate_array(memory, max_number_of_lines, struct poly_line2);
   world.circle_joints  = allocate_array(memory,
                                         max_number_of_circle_joints,
                                         struct circle_joint);
   world.circle_joints2 = allocate_array(memory,
                                         max_number_of_circle_joints2,
                                         struct circle_joint2);
   world.line_bodies    = allocate_array(memory, max_number_of_lines, s32);
   world.contacts       = allocate_array(memory,
                                         world.max_number_of_contacts,
                                         struct contact2);
   world.bodies         = allocate_array(memory,
                                         max_number_of_boxes + max_number_of_circles,
                                         struct rigid_body2);
   return (world);
}

void
add_circle_joint2(struct physics_2d_world *world, s32 circle_1, s32 circle_2, f32 distance)
{
   s32                  body_1    = world->circle_bodies[circle_1];
   s32                  body_2    = world->circle_bodies[circle_2];
   struct circle_joint2 new_joint = {
      .circle_1    = circle_1,
      .circle_2    = circle_2,
      .body_1      = body_1,
      .body_2      = body_2,
      .mass_normal = 0.01f,
      .distance    = 0.01f,
   };
   s32 joint_index                    = world->number_of_circle_joints2++;
   world->circle_joints2[joint_index] = new_joint;
}

void
add_circle_joint(struct physics_2d_world *world, s32 c1, s32 c2, struct v2 anchor)
{
   struct circle      circle_1              = world->circles[c1];
   struct circle      circle_2              = world->circles[c2];
   struct rigid_body2 body_1                = world->bodies[world->circle_bodies[c1]];
   struct rigid_body2 body_2                = world->bodies[world->circle_bodies[c2]];
   struct mat2        rotation_1            = make_rotation_mat2(circle_1.rotation);
   struct mat2        rotation_2            = make_rotation_mat2(circle_2.rotation);
   struct mat2        rotation_1_transposed = transpose_mat2(rotation_1);
   struct mat2        rotation_2_transposed = transpose_mat2(rotation_2);
   struct v2          local_anchor_1        = mul_mat2_v2(rotation_1_transposed,
                                                          sub_v2(anchor, circle_1.position));
   struct v2          local_anchor_2        = mul_mat2_v2(rotation_2_transposed,
                                                          sub_v2(anchor, circle_2.position));

   struct circle_joint new_joint = {
      .M              = { 0.0f, 0.0f, 0.0f, 0.0f },
      .local_anchor_1 = local_anchor_1,
      .local_anchor_2 = local_anchor_2,
      .r1             = { 0.0f, 0.0f },
      .r2             = { 0.0f, 0.0f },
      .bias           = { 0.0f, 0.0f },
      .P              = { 0.0f, 0.0f },
      .circle_1       = c1,
      .circle_2       = c2,
      .bias_factor    = 0.2f,
      .softness       = 0.8f,
   };
   s32 joint_index                   = world->number_of_circle_joints++;
   world->circle_joints[joint_index] = new_joint;
}

s32
p2d_clip_segment_to_line(struct v2             out[2],
                         struct v2             in[2],
                         struct v2             normal,
                         f32                   offset,
                         enum box2_edge_number clip_edge)
{
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
p2d_compute_incident_edge(struct v2   edge[2],
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

/*
void
_physics_2d_collision_box_box(struct rigid_body2 *body_1,
                              struct rigid_body2 *body_2,
                              struct contact2 *contact)
{
    contact->number_of_contact_points =
        _box2_box2_contact_points(body_1->position,
                                  body_1->shape.half_extend,
                                  body_1->rotation,
                                  body_2->position,
                                  body_2->shape.half_extend,
                                  body_2->rotation,
                                  contact->contact_points);
    contact->body_1 = body_1;
    contact->body_2 = body_2;
}

void
_physics_2d_collision_box_circle(struct rigid_body2 *body_1,
                                 struct rigid_body2 *body_2,
                                 struct contact2 *contact)
{
    contact->number_of_contact_points =
        _box2_circle_contact_points(body_1->position,
                                    body_1->shape.half_extend,
                                    body_1->rotation,
                                    body_2->position,
                                    body_2->shape.radius,
                                    contact->contact_points);
    contact->body_1 = body_1;
    contact->body_2 = body_2;
}

void
_physics_2d_collision_circle_circle(struct rigid_body2 *body_1,
                                    struct rigid_body2 *body_2,
                                    struct contact2 *contact)
{
    contact->number_of_contact_points =
        _circle_circle_contact_points(body_1->position,
                                    body_1->shape.radius,
                                    body_2->position,
                                    body_2->shape.radius,
                                    contact->contact_points);
    contact->body_1 = body_1;
    contact->body_2 = body_2;
}

void
physics_2d_collision(struct rigid_body2 *body_1,
                     struct rigid_body2 *body_2,
                     struct contact2 *contacts)
{
    enum rigid_body2_shape_type type_1 = body_1->shape_type;
    enum rigid_body2_shape_type type_2 = body_2->shape_type;
    switch(type_1 | type_2)
    {
    case rigid_body2_shape_type_circle:
    {
        _physics_2d_collision_circle_circle(body_1, body_2, contacts);
    } break;
    case rigid_body2_shape_type_box:
    {
        _physics_2d_collision_box_box(body_1, body_2, contacts);
    } break;
    case 3:
    {
        if (type_1 == rigid_body2_shape_type_box)
            _physics_2d_collision_box_circle(body_1, body_2, contacts);
        else
            _physics_2d_collision_box_circle(body_2, body_1, contacts);
    } break;
    }
}
*/

f32
calculate_inertia(f32 mass, f32 width, f32 height)
{
   return (mass * (width * width + height * height) / 12.0f);
}

struct rigid_body2
make_rigid_body2(f32 mass, f32 inertia)
{
   // if (mass <= 0.0f)
   // log_and_exit("make_rigid_body2 mass can not be <= 0.0f\n");

   f32 inverse_mass    = 0.0f;
   f32 inverse_inertia = 0.0f;
   if (mass < FLT_MAX)
   {
      inverse_mass    = 1.0f / mass;
      inverse_inertia = 1.0f / inertia;
   }
   else
   {
      inverse_mass    = 0.0f;
      inertia         = FLT_MAX;
      inverse_inertia = 0.0f;
   }
   struct rigid_body2 body = {
      .velocity         = { 0 },
      .force            = { 0 },
      .angular_velocity = 0.0f,
      .torque           = 0.0f,
      .mass             = mass,
      .inverse_mass     = inverse_mass,
      .friction         = 0.1f,
      .inertia          = inertia,
      .inverse_inertia  = inverse_inertia,
      .restitution      = 0.1f,
   };
   return (body);
}

s32
p2d_add_poly_line2(struct physics_2d_world *world,
                   struct poly_line2        line)
{
   if (world->number_of_lines >= world->max_number_of_lines)
   {
      printf("too many poly lines\n");
      return 0;
   }
   struct rigid_body2 body   = { 0 };
   body.velocity             = make_v2(0.0f, 0.0f);
   body.force                = make_v2(0.0f, 0.0f);
   body.angular_velocity     = 0.0f;
   body.torque               = 0.0f;
   body.mass                 = FLT_MAX;
   body.inverse_mass         = 0.0f;
   body.friction             = 0.2f;
   body.inertia              = FLT_MAX;
   body.inverse_inertia      = 0.0f;
   s32 index                 = world->number_of_lines++;
   world->lines[index]       = line;
   s32 body_index            = world->number_of_bodies++;
   world->bodies[body_index] = body;
   world->line_bodies[index] = body_index;
   return (index);
}

s32
p2d_add_circle(struct physics_2d_world *world,
               struct circle            circle,
               f32                      mass)
{
   if (world->number_of_circles >= world->max_number_of_circles)
   {
      printf("too many circles\n");
      return 0;
   }
   f32 inverse_mass = 0.0f;
   f32 inertia, inverse_inertia;
   f32 width  = 2.0f * circle.radius;
   f32 height = 2.0f * circle.radius;
   if (mass < FLT_MAX)
   {
      inverse_mass    = 1.0f / mass;
      inertia         = mass * (width * width + height * height) / 12.0f;
      inverse_inertia = 1.0f / inertia;
   }
   else
   {
      inverse_mass    = 0.0f;
      inertia         = FLT_MAX;
      inverse_inertia = 0.0f;
   }
   struct rigid_body2 body     = { 0 };
   body.velocity               = make_v2(0.0f, 0.0f);
   body.force                  = make_v2(0.0f, 0.0f);
   body.angular_velocity       = 0.0f;
   body.torque                 = 0.0f;
   body.mass                   = mass;
   body.inverse_mass           = inverse_mass;
   body.friction               = 0.2f;
   body.inertia                = inertia;
   body.inverse_inertia        = inverse_inertia;
   s32 index                   = world->number_of_circles++;
   world->circles[index]       = circle;
   s32 body_index              = world->number_of_bodies++;
   world->bodies[body_index]   = body;
   world->circle_bodies[index] = body_index;
   return (index);
}

s32
p2d_add_box(struct physics_2d_world *world,
            struct box2              box,
            f32                      mass)
{
   if (world->number_of_boxes == world->max_number_of_boxes)
   {
      printf("too many boxes\n");
      return 0;
   }
   f32 inverse_mass = 0.0f;
   f32 inertia, inverse_inertia;
   f32 width  = 2.0f * box.half_extend.x;
   f32 height = 2.0f * box.half_extend.y;
   if (mass < FLT_MAX)
   {
      inverse_mass    = 1.0f / mass;
      inertia         = mass * (width * width + height * height) / 12.0f;
      inverse_inertia = 1.0f / inertia;
   }
   else
   {
      inverse_mass    = 0.0f;
      inertia         = FLT_MAX;
      inverse_inertia = 0.0f;
   }
   struct rigid_body2 body   = { 0 };
   body.velocity             = make_v2(0.0f, 0.0f);
   body.force                = make_v2(0.0f, 0.0f);
   body.angular_velocity     = 0.0f;
   body.torque               = 0.0f;
   body.mass                 = mass;
   body.inverse_mass         = inverse_mass;
   body.friction             = 0.2f;
   body.inertia              = inertia;
   body.inverse_inertia      = inverse_inertia;
   s32 index                 = world->number_of_boxes++;
   world->boxes[index]       = box;
   s32 body_index            = world->number_of_bodies++;
   world->bodies[body_index] = body;
   world->box_bodies[index]  = body_index;
   return (index);
}

void
pre_step(struct physics_2d_world *world, f32 inverse_dt)
{
   f32 allowed_penetration = 0.01f;
   f32 bias_factor         = 0.2f;

   for (s32 index = 0;
        index < world->number_of_contacts;
        ++index)
   {
      struct contact2 *contact = world->contacts + index;
      // @TODO: block solver?
      for (s32 point_index = 0;
           point_index < contact->number_of_contact_points;
           ++point_index)
      {
         struct v2          r1     = contact->r1[point_index];
         struct v2          r2     = contact->r2[point_index];
         struct rigid_body2 body_1 = world->bodies[contact->body_1];
         struct rigid_body2 body_2 = world->bodies[contact->body_2];

         // Precompute normal mass, tangent mass, and bias.
         f32 rn1     = dot_v2(r1, contact->contact_points[point_index].normal);
         f32 rn2     = dot_v2(r2, contact->contact_points[point_index].normal);
         f32 kNormal = body_1.inverse_mass + body_2.inverse_mass;
         kNormal += body_1.inverse_inertia * (dot_v2(r1, r1) - rn1 * rn1)
                    + body_2.inverse_inertia * (dot_v2(r2, r2) - rn2 * rn2);
         contact->mass_normal[point_index] = 1.0f / kNormal;

         struct v2 tangent  = make_v2(contact->contact_points[point_index].normal.y, -contact->contact_points[point_index].normal.x);
         f32       rt1      = dot_v2(r1, tangent);
         f32       rt2      = dot_v2(r2, tangent);
         f32       kTangent = body_1.inverse_mass + body_2.inverse_mass;
         kTangent += body_1.inverse_inertia * (dot_v2(r1, r1) - rt1 * rt1)
                     + body_2.inverse_inertia * (dot_v2(r2, r2) - rt2 * rt2);
         contact->mass_tangent[point_index] = 1.0f / kTangent;

         contact->bias[point_index] = -bias_factor * inverse_dt * min(0.0f, contact->contact_points[point_index].spacing + allowed_penetration);
      }
   }
   for (s32 index = 0;
        index < world->number_of_circle_joints2;
        ++index)
   {
      struct circle_joint2 *joint    = world->circle_joints2 + index;
      struct circle         circle_1 = world->circles[joint->circle_1];
      struct circle         circle_2 = world->circles[joint->circle_2];
      struct rigid_body2    body_1   = world->bodies[world->circle_bodies[joint->circle_1]];
      struct rigid_body2    body_2   = world->bodies[world->circle_bodies[joint->circle_2]];

      struct v2 a_to_b = sub_v2(circle_2.position, circle_1.position);
      struct v2 normal = normalize_v2(a_to_b);
      struct v2 r1     = mul_f32_v2(0.5f, a_to_b);
      struct v2 r2     = mul_f32_v2(-0.5f, a_to_b);
      f32       rn1    = dot_v2(r1, normal);
      f32       rn2    = dot_v2(r2, normal);

      f32 kNormal = body_1.inverse_mass + body_2.inverse_mass;
      kNormal += body_1.inverse_inertia * (dot_v2(r1, r1) - rn1 * rn1)
                 + body_2.inverse_inertia * (dot_v2(r2, r2) - rn2 * rn2);
      joint->mass_normal = 1.0f / kNormal;
   }
   for (s32 index = 0;
        index < world->number_of_circle_joints;
        ++index)
   {
      struct circle_joint *joint    = world->circle_joints + index;
      struct circle        circle_1 = world->circles[joint->circle_1];
      struct circle        circle_2 = world->circles[joint->circle_2];
      struct rigid_body2   body_1   = world->bodies[world->circle_bodies[joint->circle_1]];
      struct rigid_body2   body_2   = world->bodies[world->circle_bodies[joint->circle_2]];

      struct mat2 rotation_1 = make_rotation_mat2(circle_1.rotation);
      struct mat2 rotation_2 = make_rotation_mat2(circle_2.rotation);

      joint->r1 = mul_mat2_v2(rotation_1, joint->local_anchor_1);
      joint->r2 = mul_mat2_v2(rotation_2, joint->local_anchor_2);

      struct mat2 k1 = { 0 };
      k1.m00         = body_1.inverse_mass + body_2.inverse_mass;
      k1.m01         = 0.0f;
      k1.m10         = 0.0f;
      k1.m11         = body_1.inverse_mass + body_2.inverse_mass;

      struct mat2 k2 = { 0 };
      k2.m00         = body_1.inverse_inertia * joint->r1.y * joint->r1.y;
      k2.m01         = -body_1.inverse_inertia * joint->r1.x * joint->r1.y;
      k2.m10         = -body_1.inverse_inertia * joint->r1.x * joint->r1.y;
      k2.m11         = body_1.inverse_inertia * joint->r1.x * joint->r1.x;

      struct mat2 k3 = { 0 };
      k3.m00         = body_2.inverse_inertia * joint->r2.y * joint->r2.y;
      k3.m01         = -body_2.inverse_inertia * joint->r2.x * joint->r2.y;
      k3.m10         = -body_2.inverse_inertia * joint->r2.x * joint->r2.y;
      k3.m11         = body_2.inverse_inertia * joint->r2.x * joint->r2.x;

      struct mat2 k = add_mat2(add_mat2(k1, k2), k3);
      k.m00 += joint->softness;
      k.m11 += joint->softness;

      joint->M = invert_mat2(k);

      struct v2 p1 = add_v2(circle_1.position, joint->r1);
      struct v2 p2 = add_v2(circle_2.position, joint->r2);
      struct v2 dp = sub_v2(p2, p1);

      joint->bias = mul_f32_v2(-joint->bias_factor * inverse_dt, dp);

      joint->P = make_v2(0.0f, 0.0f);
   }
}

void
apply_impulses(struct physics_2d_world *world)
{
   for (s32 index = 0;
        index < world->number_of_contacts;
        ++index)
   {
      struct contact2    *contact = world->contacts + index;
      struct rigid_body2 *body_1  = world->bodies + contact->body_1;
      struct rigid_body2 *body_2  = world->bodies + contact->body_2;
      if (body_1->inverse_mass == 0.0f
          && body_2->inverse_mass == 0.0f)
         continue;
      for (s32 point_index = 0;
           point_index < contact->number_of_contact_points;
           ++point_index)
      {
         struct v2 r1 = contact->r1[point_index];
         struct v2 r2 = contact->r2[point_index];

         struct v2 dv = body_2->velocity;
         dv           = add_v2(dv, make_v2(-body_2->angular_velocity * r2.y, body_2->angular_velocity * r2.x));
         dv           = sub_v2(dv, body_1->velocity);
         dv           = sub_v2(dv, make_v2(-body_1->angular_velocity * r1.y, body_1->angular_velocity * r1.x));

         f32 vn = dot_v2(dv, contact->contact_points[point_index].normal);

         f32 restitution = (body_1->restitution + body_2->restitution) * 0.5f;
         f32 dPn         = contact->mass_normal[point_index] * (-vn * (1.0f + restitution) + contact->bias[point_index]);
         dPn             = max(dPn, 0.0f);

         struct v2 Pn = mul_f32_v2(dPn, contact->contact_points[point_index].normal);

         body_1->velocity = sub_v2(body_1->velocity, mul_f32_v2(body_1->inverse_mass, Pn));
         body_1->angular_velocity -= body_1->inverse_inertia * cross_v2(r1, Pn);

         body_2->velocity = add_v2(body_2->velocity, mul_f32_v2(body_2->inverse_mass, Pn));
         body_2->angular_velocity += body_2->inverse_inertia * cross_v2(r2, Pn);

         dv = body_2->velocity;
         dv = add_v2(dv, make_v2(-body_2->angular_velocity * r2.y, body_2->angular_velocity * r2.x));
         dv = sub_v2(dv, body_1->velocity);
         dv = sub_v2(dv, make_v2(-body_1->angular_velocity * r1.y, body_1->angular_velocity * r1.x));

         struct v2 tangent = make_v2(-contact->contact_points[point_index].normal.y, contact->contact_points[point_index].normal.x);
         f32       vt      = dot_v2(dv, tangent);
         f32       dPt     = contact->mass_tangent[point_index] * (-vt);

         // Compute friction impulse
         f32 friction = sqrtf(body_1->friction * body_2->friction);
         f32 maxPt    = friction * dPn;
         // dPt = max(-maxPt, min(dPt, maxPt));
         dPt          = clamp_f32(dPt, -maxPt, maxPt);
         struct v2 Pt = mul_f32_v2(dPt, tangent);

         body_1->velocity = sub_v2(body_1->velocity, mul_f32_v2(body_1->inverse_mass, Pt));
         body_1->angular_velocity -= body_1->inverse_inertia * cross_v2(r1, Pt);

         body_2->velocity = add_v2(body_2->velocity, mul_f32_v2(body_2->inverse_mass, Pt));
         body_2->angular_velocity += body_2->inverse_inertia * cross_v2(r2, Pt);
      }
   }
   for (s32 index = 0;
        index < world->number_of_circle_joints;
        ++index)
   {
      struct circle_joint *joint    = world->circle_joints + index;
      struct circle        circle_1 = world->circles[joint->circle_1];
      struct circle        circle_2 = world->circles[joint->circle_2];
      struct rigid_body2  *body_1   = world->bodies + world->circle_bodies[joint->circle_1];
      struct rigid_body2  *body_2   = world->bodies + world->circle_bodies[joint->circle_2];

      struct v2 dv = body_2->velocity;
      dv           = add_v2(dv, make_v2(-body_2->angular_velocity * joint->r2.y, body_2->angular_velocity * joint->r2.x));
      dv           = sub_v2(dv, body_1->velocity);
      dv           = sub_v2(dv, make_v2(-body_1->angular_velocity * joint->r1.y, body_1->angular_velocity * joint->r1.x));

      joint->P          = sub_v2(joint->bias, dv);
      joint->P          = sub_v2(joint->P, mul_f32_v2(joint->softness, joint->P));
      struct v2 impulse = mul_mat2_v2(joint->M, joint->P);

      body_1->velocity = sub_v2(body_1->velocity,
                                mul_f32_v2(body_1->inverse_mass, impulse));
      body_1->angular_velocity -= body_1->inverse_inertia * cross_v2(joint->r1, impulse);

      body_2->velocity = add_v2(body_2->velocity,
                                mul_f32_v2(body_2->inverse_mass, impulse));
      body_2->angular_velocity += body_2->inverse_inertia * cross_v2(joint->r2, impulse);

      joint->P = add_v2(joint->P, impulse);
   }
   for (s32 index = 0;
        index < world->number_of_circle_joints2;
        ++index)
   {
      struct circle_joint2 *joint    = world->circle_joints2 + index;
      struct circle         circle_1 = world->circles[joint->circle_1];
      struct circle         circle_2 = world->circles[joint->circle_2];
      struct rigid_body2   *body_1   = world->bodies + joint->body_1;
      struct rigid_body2   *body_2   = world->bodies + joint->body_2;

      struct v2 normal = normalize_v2(sub_v2(circle_2.position,
                                             circle_1.position));
      struct v2 dv     = sub_v2(body_2->velocity, body_1->velocity);
      dv               = sub_v2(dv, body_1->velocity);

      f32 vn = dot_v2(dv, normal);

      f32 dPn = joint->mass_normal * (-vn /*+ joint->bias*/);

      struct v2 Pn = mul_f32_v2(dPn, normal);

      body_1->velocity = sub_v2(body_1->velocity, mul_f32_v2(body_1->inverse_mass, Pn));

      body_2->velocity = add_v2(body_2->velocity, mul_f32_v2(body_2->inverse_mass, Pn));
   }
}

void
find_contacts(struct physics_2d_world *world)
{
   world->number_of_contacts = 0;
   for (s32 index_1 = 0;
        index_1 < world->number_of_boxes;
        ++index_1)
   {
      for (s32 index_2 = index_1 + 1;
           index_2 < world->number_of_boxes;
           ++index_2)
      {
         struct contact2 *contact          = world->contacts + world->number_of_contacts;
         struct box2      box_1            = world->boxes[index_1];
         struct box2      box_2            = world->boxes[index_2];
         contact->number_of_contact_points = box2_box2_contact_points(box_1, box_2, contact->contact_points);
         if (contact->number_of_contact_points > 0)
         {
            world->number_of_contacts += 1;
            contact->body_1 = world->box_bodies[index_1];
            contact->body_2 = world->box_bodies[index_2];
            for (s32 point_index = 0;
                 point_index < contact->number_of_contact_points;
                 ++point_index)
            {
               contact->r1[point_index] = sub_v2(contact->contact_points[point_index].position,
                                                 box_1.position);
               contact->r2[point_index] = sub_v2(contact->contact_points[point_index].position,
                                                 box_2.position);

               assert(world->number_of_contacts <= world->max_number_of_contacts);
            }
         }
      }
   }
   for (s32 index_1 = 0;
        index_1 < world->number_of_circles;
        ++index_1)
   {
      for (s32 index_2 = index_1 + 1;
           index_2 < world->number_of_circles;
           ++index_2)
      {
         struct contact2 *contact          = world->contacts + world->number_of_contacts;
         struct circle    circle_1         = world->circles[index_1];
         struct circle    circle_2         = world->circles[index_2];
         contact->number_of_contact_points = circle_circle_contact_points(circle_1, circle_2, contact->contact_points);
         if (contact->number_of_contact_points > 0)
         {
            contact->body_1 = world->circle_bodies[index_1];
            contact->body_2 = world->circle_bodies[index_2];
            world->number_of_contacts += 1;
            contact->r1[0] = sub_v2(contact->contact_points[0].position,
                                    circle_1.position);
            contact->r2[0] = sub_v2(contact->contact_points[0].position,
                                    circle_2.position);

            assert(world->number_of_contacts <= world->max_number_of_contacts);
         }
      }
      for (s32 index_2 = 0;
           index_2 < world->number_of_lines;
           ++index_2)
      {
         struct contact2  *contact         = world->contacts + world->number_of_contacts;
         struct circle     circle          = world->circles[index_1];
         struct poly_line2 line            = world->lines[index_2];
         contact->number_of_contact_points = circle_poly_line2_contact_points(circle, line, contact->contact_points);
         if (contact->number_of_contact_points > 0)
         {
            contact->body_1 = world->line_bodies[index_2];
            contact->body_2 = world->circle_bodies[index_1];
            world->number_of_contacts += 1;
            contact->r1[0] = make_v2(0.0f, 0.0f);
            contact->r2[0] = sub_v2(contact->contact_points[0].position,
                                    circle.position);

            assert(world->number_of_contacts <= world->max_number_of_contacts);
         }
      }
   }
   for (s32 index_1 = 0;
        index_1 < world->number_of_boxes;
        ++index_1)
   {
      for (s32 index_2 = 0;
           index_2 < world->number_of_circles;
           ++index_2)
      {
         struct contact2 *contact          = world->contacts + world->number_of_contacts;
         struct box2      box              = world->boxes[index_1];
         struct circle    circle           = world->circles[index_2];
         contact->number_of_contact_points = box2_circle_contact_points(box, circle, contact->contact_points);
         if (contact->number_of_contact_points > 0)
         {
            contact->body_1 = world->box_bodies[index_1];
            contact->body_2 = world->circle_bodies[index_2];
            world->number_of_contacts += 1;
            contact->r1[0] = sub_v2(contact->contact_points[0].position,
                                    box.position);
            contact->r2[0] = sub_v2(contact->contact_points[0].position,
                                    circle.position);

            assert(world->number_of_contacts <= world->max_number_of_contacts);
         }
      }
   }
}

void
physics_2d_step_world(struct physics_2d_world *world)
{
   f32 delta_in_seconds         = 1.0f / 60.0f;
   f32 dt                       = delta_in_seconds;
   f32 inverse_delta_in_seconds = 60.f;
   f32 inverse_dt               = inverse_delta_in_seconds;

   find_contacts(world);

   f32 linear_damping  = 0.05f;
   f32 angular_damping = 0.05f;
   for (s32 index = 0;
        index < world->number_of_boxes + world->number_of_circles;
        ++index)
   {
      struct rigid_body2 *body = world->bodies + index;
      if (body->inverse_mass == 0.0f)
         continue;

      struct v2 acceleration       = mul_f32_v2(body->inverse_mass, body->force);
      struct v2 total_acceleration = add_v2(world->gravity, acceleration);
      body->velocity               = add_v2(body->velocity, mul_f32_v2(dt, total_acceleration));
      body->angular_velocity += dt * body->inverse_mass * body->torque;

      // damping
      body->velocity = mul_f32_v2(1.0f / (1.0f + dt * linear_damping), body->velocity);
      body->angular_velocity *= 1.0f / (1.0f + dt * angular_damping);
   }

   pre_step(world, inverse_dt);
   s32 number_of_iterations = 15;
   for (s32 index = 0;
        index < number_of_iterations;
        ++index)
   {
      apply_impulses(world);
   }
   for (s32 index = 0;
        index < world->number_of_boxes;
        ++index)
   {
      struct rigid_body2 *body     = world->bodies + world->box_bodies[index];
      world->boxes[index].position = add_v2(world->boxes[index].position,
                                            mul_f32_v2(dt, body->velocity));
      world->boxes[index].rotation = world->boxes[index].rotation
                                     + dt * body->angular_velocity;
      body->force.x = 0.0f;
      body->force.y = 0.0f;
      body->torque  = 0.0f;
   }
   for (s32 index = 0;
        index < world->number_of_circles;
        ++index)
   {
      struct rigid_body2 *body       = world->bodies + world->circle_bodies[index];
      world->circles[index].position = add_v2(world->circles[index].position,
                                              mul_f32_v2(dt, body->velocity));
      world->circles[index].rotation = world->circles[index].rotation
                                       + dt * body->angular_velocity;
      body->force.x = 0.0f;
      body->force.y = 0.0f;
      body->torque  = 0.0f;
   }
}
