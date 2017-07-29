#version 410
#extension GL_ARB_separate_shader_objects : enable

in Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} vertex;

uniform int displayMode = 1;
uniform vec3 shapeColor = vec3(0.7);
uniform sampler2D tex;
uniform vec3 lightDir = normalize(vec3(0.7, 0.85, 1.0));

layout( location = 0 ) out vec4 outColor;

void main(void)
{
    vec3 color = vec3(1);

    vec3 normal = gl_FrontFacing ? vertex.normal : -vertex.normal;

    switch(displayMode)
    {
    case 0:
        color = vertex.position;
        break;
    case 1:
        color = normal * 0.5 + 0.5;
        break;
    case 2:
        color = vec3(vertex.texCoords, 1.0);
        break;
    case 3:
        color = shapeColor;
        break;
    case 4:
        color = texture(tex, vertex.texCoords).rgb;
        break;
    case 5:
    {
        float ambient = 0.1;
        float intensity = max(ambient, dot(normal, lightDir));
        color = shapeColor * intensity;
    }
        break;
    case 6:
    {
        float ambient = 0.1;
        float intensity = max(ambient, dot(normal, lightDir));
        color = shapeColor * intensity;
    }
        break;
    default:
        break;
    }

    outColor = vec4(color, 1.0);
}
