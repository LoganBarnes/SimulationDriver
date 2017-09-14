#version 410
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 local_position;
layout(location = 1) in vec3 local_normal;
layout(location = 2) in vec2 tex_coords;

uniform mat4 screen_from_world        = mat4(1.0);
uniform mat4 world_from_local         = mat4(1.0);
uniform mat3 world_from_local_normals = mat3(1.0);

out Vertex
{
    vec3 world_position;
    vec3 world_normal;
    vec2 tex_coords;
} vertex;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main(void)
{
    vertex.world_position = vec3(world_from_local * vec4(local_position, 1.0));
    vertex.world_normal = normalize(world_from_local_normals * local_normal);
    vertex.tex_coords = tex_coords;

    gl_Position = screen_from_world * vec4(vertex.world_position, 1.0);
}
