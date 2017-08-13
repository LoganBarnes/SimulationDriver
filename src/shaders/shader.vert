#version 410
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

uniform mat4 screen_from_world        = mat4(1.0);
uniform mat4 world_from_local         = mat4(1.0);
uniform mat3 world_from_local_normals = mat3(1.0);

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
    vertex.position = vec3(world_from_local * vec4(inPosition, 1.0));
    vertex.normal = world_from_local_normals * inNormal;
    vertex.texCoords = inTexCoords;

    gl_Position = screen_from_world * vec4(vertex.position, 1.0);
}
