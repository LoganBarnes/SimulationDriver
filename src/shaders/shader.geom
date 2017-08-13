#version 410
#extension GL_ARB_separate_shader_objects : enable

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} vertex_in[];

uniform mat4 screen_from_world = mat4(1.0);
uniform float normal_scale = 1.0;

out Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} vertex;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main()
{
    gl_Position = screen_from_world * vec4(vertex_in[0].position, 1.0);
    vertex.position = vertex_in[0].position;
    vertex.normal = vertex_in[0].normal;
    vertex.texCoords = vertex_in[0].texCoords;
    EmitVertex();

    gl_Position = screen_from_world * vec4(vertex_in[0].position + vertex_in[0].normal * normal_scale, 1.0);
    vertex.position = vertex_in[0].position;
    vertex.normal = vertex_in[0].normal;
    vertex.texCoords = vertex_in[0].texCoords;
    EmitVertex();

    EndPrimitive();
}
