#version 410
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

uniform mat4 projectionView = mat4(1.0);
uniform mat4 model          = mat4(1.0);
uniform mat3 normalMatrix   = mat3(1.0);

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

void main(void)
{
    vertex.position = vec3(model * vec4(inPosition, 1.0));
    vertex.normal = normalMatrix * inNormal;
    vertex.texCoords = inTexCoords;

    gl_Position = projectionView * vec4(vertex.position, 1.0);
}
