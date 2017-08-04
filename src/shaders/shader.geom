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

uniform mat4 projectionView = mat4(1.0);
uniform float normalScale = 1.0;

out Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} vertex;

void main()
{
    gl_Position = projectionView * vec4(vertex_in[0].position, 1.0);
    vertex.position = vertex_in[0].position;
    vertex.normal = vertex_in[0].normal;
    vertex.texCoords = vertex_in[0].texCoords;
    EmitVertex();

    gl_Position = projectionView * vec4(vertex_in[0].position + vertex_in[0].normal * normalScale, 1.0);
    vertex.position = vertex_in[0].position;
    vertex.normal = vertex_in[0].normal;
    vertex.texCoords = vertex_in[0].texCoords;
    EmitVertex();

    EndPrimitive();
}