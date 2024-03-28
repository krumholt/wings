#ifndef WINGS_BASE_ASSET_IMPORTERS_C_
#define WINGS_BASE_ASSET_IMPORTERS_C_

#include "types.h"
#include "colors.h"
#include "errors.h"
#include "allocators.h"
#include "wings_file_reader.c"
#include "os/file.c"
#include "graphics/mesh.c"

error
load_mesh(struct mesh *mesh, const char *filename, struct allocator *allocator)
{
   error         error  = NO_ERROR;
   struct buffer buffer = { 0 };

   error = file_read(&buffer, filename, 0, allocator);
   if (error)
      return (ec_base_asset_importers__failed_to_read_file);

   struct wings_file_parser parser = { 0 };

   error = make_wings_file_parser(&parser, buffer);
   if (error)
      return (ec_base_asset_importers__failed_to_parse_file);

   error = set_to_next_chunk_with_name(&parser, "blender");
   if (error)
      return (ec_base_asset_importers__failed_to_parse_file);

   error = set_to_next_chunk_with_name(&parser, "mesh");
   if (error)
      return (ec_base_asset_importers__failed_to_parse_file);

   struct wings_file_mesh_chunk *mesh_chunk = parser.mesh_chunk;
   enum mesh_attribute           attributes = 0;
   if (mesh_chunk->has_positions)
      attributes |= mesh_attribute_position;
   if (mesh_chunk->has_normals)
      attributes |= mesh_attribute_normal;
   if (mesh_chunk->has_uvs)
      attributes |= mesh_attribute_uv;
   if (mesh_chunk->has_colors)
      attributes |= mesh_attribute_color;
   if (mesh_chunk->has_joint_ids)
      attributes |= mesh_attribute_joint_id;
   if (mesh_chunk->has_joint_weights)
      attributes |= mesh_attribute_joint_weight;

   attributes |= mesh_attribute_color;
   make_mesh(mesh,
             attributes,
             mesh_chunk->number_of_vertices,
             allocator);
   mesh->used = mesh->size;

   error = set_to_next_chunk_with_name(&parser, "positions");
   if (error)
      return (ec_base_asset_importers__failed_to_parse_file);

   struct v3 *positions = (struct v3 *)(mesh->data + mesh->positions_offset);
   for (u32 index = 0; index < mesh->used; ++index)
   {
      positions[index] = parser.positions_chunk->data[index];
   }

   error = set_to_next_chunk_with_name(&parser, "normals");
   if (error)
      return (ec_base_asset_importers__failed_to_parse_file);

   struct v3 *normals = (struct v3 *)(mesh->data + mesh->normals_offset);
   for (u32 index = 0; index < mesh->used; ++index)
   {
      normals[index] = parser.normals_chunk->data[index];
   }

   error = set_to_next_chunk_with_name(&parser, "uvs");
   if (error)
      return (ec_base_asset_importers__failed_to_parse_file);

   struct v2 *uvs = (struct v2 *)(mesh->data + mesh->uvs_offset);
   for (u32 index = 0; index < mesh->used; ++index)
   {
      uvs[index] = parser.uvs_chunk->data[index];
   }

   struct v4 *colorios = (struct v4 *)(mesh->data + mesh->colors_offset);
   for (u32 index = 0; index < mesh->used; ++index)
   {
      colorios[index] = color_pink;
   }

   return (ec__no_error);
}

#endif
