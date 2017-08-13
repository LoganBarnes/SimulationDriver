#version 410
#extension GL_ARB_separate_shader_objects : enable

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in Vertex
{
    vec3 world_position;
    vec3 world_normal;
    vec2 tex_coords;
} vertex_in[];

uniform mat4 screen_from_world = mat4(1.0);
uniform float normal_scale = 1.0;

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

void main()
{
    gl_Position = screen_from_world * vec4(vertex_in[0].world_position, 1.0);
    vertex.world_position = vertex_in[0].world_position;
    vertex.world_normal = vertex_in[0].world_normal;
    vertex.tex_coords = vertex_in[0].tex_coords;
    EmitVertex();

    gl_Position = screen_from_world * vec4(vertex_in[0].world_position + vertex_in[0].world_normal * normal_scale, 1.0);
    vertex.world_position = vertex_in[0].world_position;
    vertex.world_normal = vertex_in[0].world_normal;
    vertex.tex_coords = vertex_in[0].tex_coords;
    EmitVertex();

    EndPrimitive();
}
