import sys
import bpy
from array import array
import struct
import time
import shutil
from pathlib import Path

ALIGNMENT = 8
string_storage = bytearray()


class Mesh:
    def __init__(self):
        self.texture_file_name_index = 0
        self.positions = bytearray()
        self.normals = bytearray()
        self.colors = bytearray()
        self.uvs = bytearray()
        self.joint_ids = bytearray()
        self.joint_weights = bytearray()


def skeleton_to_bytes(skeleton):
    result = bytearray()
    result.extend(skeleton["inverse_bind_translation"])
    result.extend(skeleton["parent_joint_index"])
    return result


actions_keyframes = {}
for action in bpy.data.actions:
    keyframes = set()
    for fcurve in action.fcurves:
        for keyframe_point in fcurve.keyframe_points:
            keyframes.add(keyframe_point.co.x)
    as_list = list(keyframes)
    as_list.sort()
    actions_keyframes[action.name] = array('f', as_list)


def get_keyframes_per_action(obj):
    actions = {}
    action_names = set()
    obj = obj.parent
    if not obj or obj.type != "ARMATURE":
        return actions
    for track in obj.animation_data.nla_tracks:
        for strip in track.strips:
            action_names.add(strip.action.name)
    for action_name in action_names:
        actions[action_name] = actions_keyframes[action_name]
    return actions


def print_debug(output_vertices,
                output_normals,
                output_texture_coordinates,
                output_joint_indices,
                output_joint_weights):
    output_vertices_iter = iter(output_vertices)
    vi = list(zip(output_vertices_iter,
                  output_vertices_iter, output_vertices_iter))
    print("vertices ({})".format(len(vi)))
    print(vi)

    output_normals_iter = iter(output_normals)
    ni = list(zip(output_normals_iter,
                  output_normals_iter,
                  output_normals_iter))
    print("normals ({})".format(len(ni)))
    print(ni)

    output_texture_coordinates_iter = iter(output_texture_coordinates)
    ti = list(zip(output_texture_coordinates_iter,
                  output_texture_coordinates_iter))
    print("texture coordinates ({})".format(len(ti)))
    print(ti)

    output_joint_indices_iter = iter(output_joint_indices)
    ji = list(zip(output_joint_indices_iter, output_joint_indices_iter,
                  output_joint_indices_iter, output_joint_indices_iter))
    print("joint indices ({})".format(len(ji)))
    print(ji)

    output_joint_weights_iter = iter(output_joint_weights)
    wi = list(zip(output_joint_weights_iter, output_joint_weights_iter,
                  output_joint_weights_iter, output_joint_weights_iter))
    print("joint weights ({})".format(len(wi)))
    print(wi)


def get_skeleton(obj):
    inverse_bind_translation = array('f')
    parent_joint_index = array('i')
    joint_name = []

    if not obj.parent or obj.parent.type != "ARMATURE":
        return {"inverse_bind_translation": inverse_bind_translation,
                "parent_joint_index": parent_joint_index, "joint_name": joint_name}

    bones = obj.parent.data.bones
    joint_index = 0
    for bone in bones:
        joint_name.append(bone.name)

    to_obj_space = obj.matrix_local
    for bone in bones:
        parent = bone.parent
        if parent:
            parent_joint_index.append(joint_name.index(parent.name))
        else:
            parent_joint_index.append(0)

        inverse_bind = bone.matrix_local.inverted() @ to_obj_space
        for col in inverse_bind.col:
            inverse_bind_translation.extend(col)

        joint_index += 1
    return {"inverse_bind_translation": inverse_bind_translation,
            "parent_joint_index": parent_joint_index,
            "joint_name": joint_name}


def get_bone_mapping(obj):
    if not obj.parent or obj.parent.type != "ARMATURE":
        return {}
    vertex_group_to_bone_index_mapping = {}
    number_of_bones = 0
    bones = obj.parent.pose.bones
    for bone in bones:
        vertex_group_to_bone_index_mapping[bone.name] = number_of_bones
        number_of_bones += 1

    return vertex_group_to_bone_index_mapping


# 0,p1,r1,
def get_animations(obj):
    all_actions = {}
    actions_and_keyframes = get_keyframes_per_action(obj)

    for action_name, frames in actions_and_keyframes.items():
        animation = {}
        obj.parent.animation_data.action = bpy.data.actions.get(action_name)
        bones_out = array('f')
        for frame in frames:
            bpy.context.scene.frame_set(int(frame))
            bones = obj.parent.pose.bones
            for bone in bones:
                if bone.parent:
                    parent_matrix = bone.parent.matrix.inverted()
                else:
                    parent_matrix = obj.parent.matrix_world

                rotation = (parent_matrix @ bone.matrix).to_quaternion()
                translation = (parent_matrix @ bone.matrix).to_translation()
                bones_out.extend(rotation)
                bones_out.extend(translation)
                bones_out.append(0.0)  # padding
        animation["joints"] = bones_out
        animation["keyframes"] = frames
        animation["name"] = store_string(action_name)

        all_actions[action_name] = animation
        # bones_iter = iter(bones_out)
        # print(list(zip(bones_iter,
        #                bones_iter,
        #                bones_iter, bones_iter, bones_iter, bones_iter,
        #                bones_iter, bones_iter)))

    return all_actions


def animations_to_bytes(animations):
    result = array('B')
    for animation_name, animation in animations.items():
        number_of_keyframes = len(animation["keyframes"])
        result.extend(number_of_keyframes.to_bytes(4, 'little'))
        result.extend(animation["name"].to_bytes(4, 'little'))
        result.extend(animation["keyframes"].tobytes())
        result.extend(animation["joints"].tobytes())
    return result


def get_mesh(obj):
    mesh = obj.data
    mesh.calc_loop_triangles()
    mesh.calc_normals_split()
    number_of_vertices = 0
    result_mesh = Mesh()
    vertices = mesh.vertices
    uvs = mesh.uv_layers[0].data
    bone_mapping = get_bone_mapping(obj)
    for triangle in mesh.loop_triangles:
        indices = triangle.vertices
        normals = triangle.split_normals
        uv_indices = triangle.loops
        for index in range(0, 3):
            if len(vertices[indices[index]].groups) > 4:
                print("ERROR: Vertex {} has more than 4 vertex groups.".format(
                    indices[index]))
                return -1

            vertex = vertices[indices[index]]
            number_of_vertices += 1
            result_mesh.positions.extend(struct.pack("fff", *vertex.co))
            result_mesh.normals.extend(struct.pack("fff", *normals[index]))
            fliped_uvs = uvs[uv_indices[index]].uv.copy()
            fliped_uvs.y = 1.0 - fliped_uvs.y
            result_mesh.uvs.extend(struct.pack(
                "ff", *fliped_uvs))
            if bone_mapping:
                for group in vertex.groups:
                    g = obj.vertex_groups[group.group]
                    vertex_group_index = bone_mapping[g.name]
                    result_mesh.joint_ids.append(
                        vertex_group_index.to_bytes(4, 'little'))
                    result_mesh.joint_weights.append(
                        group.weight.to_bytes(4, 'little'))
                for i in range(0, 4 - len(vertex.groups)):
                    result_mesh.joint_ids.extend(struct.pack("i", 0))
                    result_mesh.joint_weights.extend(struct.pack("f", 0.0))
    return (result_mesh)


def calculate_padding(size_in_bytes):
    padding = size_in_bytes % ALIGNMENT
    if padding != 0:
        padding = ALIGNMENT - padding
    return padding


def align(size_in_bytes):
    return size_in_bytes + calculate_padding(size_in_bytes)


def B(number, size):
    return number.to_bytes(size, 'little')


def chunk_header(name, size, id, parent_id):
    result = bytearray()
    result.extend(name.ljust(16, '\0').encode('utf-8'))
    result.extend(B(size, 8))
    result.extend(B(id, 4))
    result.extend(B(parent_id, 4))
    return result


def make_model_chunk(id, obj):
    name_index = store_string(obj.name)
    # @Todo: Currently models have exactly one mesh
    number_of_meshes = 1

    chunk = bytearray()
    chunk.extend(chunk_header("model", 8, id, 0))

    chunk.extend(B(number_of_meshes, 4))
    chunk.extend(B(name_index, 4))
    return (chunk)


def make_mesh_chunk(id, model_chunk_id, mesh):
    number_of_vertices = len(mesh.positions) // 12
    has_positions = 1 if len(mesh.positions) > 0 else 0
    has_normals = 1 if len(mesh.normals) > 0 else 0
    has_uvs = 1 if len(mesh.uvs) > 0 else 0
    has_colors = 1 if len(mesh.colors) > 0 else 0
    has_joint_ids = 1 if len(mesh.joint_ids) > 0 else 0
    has_joint_weights = 1 if len(mesh.joint_weights) > 0 else 0

    texture_file_name = store_string("TODO")
    chunk = bytearray()
    chunk.extend(chunk_header("mesh", 4+4+1+1+1+1+1+1+2, id, model_chunk_id))
    chunk.extend(B(number_of_vertices, 4))
    chunk.extend(B(texture_file_name, 4))
    chunk.extend(B(has_positions, 1))
    chunk.extend(B(has_normals, 1))
    chunk.extend(B(has_uvs, 1))
    chunk.extend(B(has_colors, 1))
    chunk.extend(B(has_joint_ids, 1))
    chunk.extend(B(has_joint_weights, 1))
    chunk.extend(B(0, 2))
    return (chunk)


def make_data_chunk(id, parent_id, name, data):
    chunk = bytearray()
    chunk.extend(chunk_header(name, len(data), id, parent_id))
    chunk.extend(data)
    return (chunk)


def store_string(string):
    global string_storage
    index = len(string_storage)
    string_storage.extend(string.encode('utf-8'))
    string_storage.extend((0).to_bytes(1, 'little'))
    padding = calculate_padding(len(string_storage))
    string_storage.extend(("X"*padding).encode('utf-8'))
    return index


def make_strings_chunk(id):
    global string_storage
    result = bytearray()
    result.extend(chunk_header("strings", len(string_storage), id, 0))
    result.extend(string_storage)
    return result


def make_blender_chunk(id):
    creation_time = time.time_ns() // 1000000000
    result = bytearray()
    result.extend(chunk_header("blender", 8+8+4+4, id, 0))

    # @Todo: currently only one model gets exported per file
    number_of_models = 1
    source_filename = store_string(
        bpy.path.basename(bpy.context.blend_data.filepath))
    blender_version = store_string(bpy.app.version_string)
    result.extend(number_of_models.to_bytes(8, 'little'))
    result.extend(creation_time.to_bytes(8, 'little'))
    result.extend(source_filename.to_bytes(4, 'little'))
    result.extend(blender_version.to_bytes(4, 'little'))
    return result


def export_object(obj):
    mesh = get_mesh(obj)
    if mesh == -1:
        return -1

    STRINGS_CHUNK_ID = 0
    BLENDER_CHUNK_ID = 1
    MODEL_CHUNK_ID = 2
    MESH_CHUNK_ID = 3
    blender_chunk = make_blender_chunk(BLENDER_CHUNK_ID)
    model_chunk = make_model_chunk(MODEL_CHUNK_ID, obj)
    mesh_chunk = make_mesh_chunk(MESH_CHUNK_ID, MODEL_CHUNK_ID, mesh)

    positions_chunk = make_data_chunk(
        5, MESH_CHUNK_ID, "positions", mesh.positions)
    normals_chunk = make_data_chunk(
        5, MESH_CHUNK_ID, "normals", mesh.normals)
    uvs_chunk = make_data_chunk(
        5, MESH_CHUNK_ID, "uvs", mesh.uvs)
    if (len(mesh.joint_ids) > 0):
        joint_ids_chunk = make_data_chunk(
            5, MESH_CHUNK_ID, "joint_ids", mesh.joint_ids)
        joint_weights_chunk = make_data_chunk(
            5, MESH_CHUNK_ID, "joint_weights", mesh.joint_weights)

    strings_chunk = make_strings_chunk(STRINGS_CHUNK_ID)
    object_name = obj.name.replace(" ", "_")
    file_name = "{}.wings".format(object_name)
    file = open(file_name, "wb")

    file.write("WINGS\0\0\0".encode('utf-8'))
    file.write((1).to_bytes(4, 'little'))  # major version
    file.write((0).to_bytes(4, 'little'))  # minor version
    if (len(string_storage) > 0):
        file.write(strings_chunk)
    file.write(blender_chunk)
    file.write(model_chunk)
    file.write(mesh_chunk)
    file.write(positions_chunk)
    file.write(normals_chunk)
    file.write(uvs_chunk)
    if (len(mesh.joint_ids) > 0):
        file.write(joint_ids_chunk)
        file.write(joint_weights_chunk)
    file.flush()
    file.close()

    path = Path(bpy.data.filepath)
    albedo_image = Path(str(path.parent.absolute()) +
                        '/' + object_name + '_albedo.png')
    if (Path.exists(albedo_image)):
        shutil.copy(albedo_image, object_name+'_albedo.png')
    return 0


def main():
    global string_storage
    number_of_arguments = 0
    if "--" in sys.argv:
        start_of_arguments = sys.argv.index("--") + 1
        number_of_arguments = len(sys.argv) - start_of_arguments

    bpy.ops.object.mode_set(mode="OBJECT")
    scene = bpy.context.scene

    if number_of_arguments > 0:
        for obj_name in sys.argv[start_of_arguments:]:
            if scene.objects[obj_name]:
                export_object(scene.objects[obj_name])
    else:
        for obj in scene.objects:
            if obj.type == "MESH":
                export_object(obj)


main()
