#version 410
#extension GL_ARB_separate_shader_objects : enable

layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

float biLerp(in float u1v1, in float u2v1, in float u1v2, in float u2v2, in float t, in float s)
{
  float v1 = mix(u1v1, u2v1, t);
  float v2 = mix(u1v2, u2v2, t);
  return mix(v1, v2, s);
}

float halfLerp(in float u1v1, in float u2v1, in float u1v2, in float u2v2)
{
  float v1 = (u1v1 + u2v1) * 0.5;
  float v2 = (u1v2 + u2v2) * 0.5;
  return (v1 + v2) * 0.5;
}

in Vertex
{
    ivec2 indices;
    vec3 worldPos;
} vertex_in[];

uniform mat4 screen_from_world = mat4(1.0);

uniform sampler2D heights;
uniform ivec2 texSize;
uniform vec3 worldOrigin;
uniform vec3 worldDimensions;

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

    vertex.position = vertex_in[0].worldPos + vec3(-0.5, 0, -0.5);
    vertex.normal = vec3(0, 1, 0);
    vertex.texCoords = vertex_in[0].indices / (texSize - 1.0);
    gl_Position = screen_from_world * vec4(vertex.position, 1.0);
    EmitVertex();

    vertex.position = vertex_in[0].worldPos + vec3(0.5, 0, -0.5);
    vertex.normal = vec3(0, 1, 0);
    vertex.texCoords = vertex_in[0].indices / (texSize - 1.0);
    gl_Position = screen_from_world * vec4(vertex.position, 1.0);
    EmitVertex();

    vertex.position = vertex_in[0].worldPos + vec3(0.0, 0, 0.5);
    vertex.normal = vec3(0, 1, 0);
    vertex.texCoords = vertex_in[0].indices / (texSize - 1.0);
    gl_Position = screen_from_world * vec4(vertex.position, 1.0);
    EmitVertex();

    EndPrimitive();

//    mat3 heights = vertex_in[0].heights;
//
//    float x1z1height = halfLerp(heights[0][0], heights[1][0], heights[0][1], heights[1][1]);
//    vec3 x1z1 = vec3(vertex_in[0].indices.x - 0.5, x1z1height, vertex_in[0].indices.y - 0.5);
//    x1z1 *= vertex_in[0].scale;
//
//    float x2z1height = halfLerp(heights[1][0], heights[2][0], heights[1][1], heights[2][1]);
//    vec3 x2z1 = vec3(vertex_in[0].indices.x + 0.5, x1z1height, vertex_in[0].indices.y - 0.5);
//    x2z1 *= vertex_in[0].scale;
//
//    float x1z2height = halfLerp(heights[0][1], heights[1][1], heights[0][2], heights[1][2]);
//    vec3 x1z2 = vec3(vertex_in[0].indices.x - 0.5, x1z1height, vertex_in[0].indices.y + 0.5);
//    x1z2 *= vertex_in[0].scale;
//
//    float x2z2height = halfLerp(heights[1][1], heights[2][1], heights[1][2], heights[2][2]);
//    vec3 x2z2 = vec3(vertex_in[0].indices.x + 0.5, x1z1height, vertex_in[0].indices.y + 0.5);
//    x2z2 *= vertex_in[0].scale;
//
//    vec3 n11 = normalize(cross(x1z2 - x1z1, x2z1 - x1z1));
//    vec3 n21 = normalize(cross(x1z1 - x2z1, x2z1 - x2z1));
//    vec3 n12 = normalize(cross(x2z2 - x1z2, x1z1 - x1z2));
//    vec3 n22 = normalize(cross(x2z1 - x2z2, x1z2 - x2z2));
//
//    gl_Position = projectionView * vec4(x1z1, 1.0);
//    vertex.position = x1z1;
//    vertex.normal = n11;
//    vertex.texCoords = vertex_in[0].indices / heightMapSize;
//    EmitVertex();
//
//    gl_Position = projectionView * vec4(x1z2, 1.0);
//    vertex.position = x1z2;
//    vertex.normal = n12;
//    vertex.texCoords = vertex_in[0].indices / heightMapSize;
//    EmitVertex();
//
//    gl_Position = projectionView * vec4(x2z2, 1.0);
//    vertex.position = x2z2;
//    vertex.normal = n22;
//    vertex.texCoords = vertex_in[0].indices / heightMapSize;
//    EmitVertex();
//
//    EndPrimitive();
}
