#version 410
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in ivec2 indices;

uniform mat4 projection_from_world = mat4(1.0);

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

//out Vertex
//{
//    ivec2 indices;
//    vec3 worldPos;
//} vertex;

out gl_PerVertex
{
  vec4 gl_Position;
};

void main(void)
{
//    vertex.indices = indices;
    vec2 texCoords = (vec2(indices) + 0.5) / vec2(texSize);

    float height = texture(heights, texCoords).r;

    vertex.position = vec3(indices.x / max(1.0, texSize.x - 1.0), height, indices.y / max(1.0, texSize.y - 1.0));
    vertex.position = vertex.position * worldDimensions + worldOrigin;

    vertex.normal = vec3(0, 1, 0);
    vertex.texCoords = texCoords;

    gl_Position = projection_from_world * vec4(vertex.position, 1.0);
}
