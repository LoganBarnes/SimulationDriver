#version 410
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in mat3 inHeights;
layout(location = 1) in vec3 inScale;
layout(location = 2) in ivec2 inIndices;

uniform mat4 projectionView = mat4(1.0);
uniform mat4 model          = mat4(1.0);
uniform mat3 normalMatrix   = mat3(1.0);

out Vertex
{
    mat3 heights;
    vec3 scale;
    ivec2 indices;
} vertex;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main(void)
{
    vertex.heights = inHeights;
    vertex.scale = inScale;
    vertex.indices = inIndices;

    gl_Position = projectionView * vec4(inHeights[1][1] * inScale, 1.0);
}
