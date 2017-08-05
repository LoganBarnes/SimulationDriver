#version 410
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_storage_buffer_object : require

in Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} vertex;

const float PI = 3.141592653589793;

layout(std430, binding = 0) buffer lightData
{
    vec4 lights[];
};

uniform int displayMode = 5;
uniform vec3 shapeColor = vec3(1, 0.9, 0.7);
uniform sampler2D tex;
uniform vec3 lightDir = normalize(vec3(0.7, 0.85, 1.0));

uniform float roughness = 0.2;
uniform vec3  IOR = vec3(1.5145, 1.5208, 1.5232);
uniform vec3  eye;

layout(location = 0) out vec4 outColor;


////////////non-polaraized and non-magnetic////////////
float fresnel(in  vec3  w_i,
              in  vec3  n,
              in  float n1,
              in  float n2,
              out vec3  w_r,
              out vec3  w_t)
{
    w_r = reflect( -w_i, n );
    w_t = refract( -w_i, n, n1 / n2 );

    float cosI = dot( w_i, n );
    float cosT = dot( w_t, -n );

    float n1CosI = n1 * cosI;
    float n2CosT = n2 * cosT;

    float n1CosT = n1 * cosT;
    float n2CosI = n2 * cosI;

    float Rs = ( n1CosI - n2CosT ) / ( n1CosI + n2CosT );
    Rs *= Rs;

    float Rp = ( n1CosT - n2CosI ) / ( n1CosT + n2CosI );
    Rp *= Rp;

    return 0.5 * ( Rs + Rp );
}



////////////Cook-Torrance////////////
vec3 calcBRDF(in vec3 V,
              in vec3 N,
              in vec3 L)
{
    // roughness == m in cook-torrance lingo
    float m = roughness;

    vec3 R, T;
    vec3 F = vec3(fresnel( V, N, 1.0, IOR.x, R, T ),
                  fresnel( V, N, 1.0, IOR.y, R, T ),
                  fresnel( V, N, 1.0, IOR.z, R, T ));

    vec3 H = normalize( V + L );

    float cosNV = dot( N, V );
    float cosNH = dot( N, H );
    float cosNL = dot( N, L );
    float cosVH = dot( V, H );

    // geometric attenuation
    float G = min( 1.0, min( 2.0 * cosNH * cosNV / cosVH, 2.0 * cosNH * cosNL / cosVH ) );

    // microfacet slope distribution
    float cosNHPow2 = cosNH * cosNH;
    float mPo2 = m * m;

    float D = ( 1.0 / ( PI * mPo2 * cosNHPow2 * cosNHPow2 ) )
              * exp( ( cosNHPow2 - 1.0 ) / ( mPo2 * cosNHPow2 ) );

    // return shapeColor * ( F * D * G ) / ( PI * cosNL * cosNV );
    // return shapeColor * F;
    vec3 specular = shapeColor * ( F * D * G ) / ( PI * cosNL * cosNV );

    vec3 diffuse = shapeColor * ( 1.0 - F ) / PI;

    return diffuse + specular;
}

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
        vec3 w_v = normalize(eye - vertex.position);

        vec3 intensity = vec3(0.1); // ambient

        for (int i = 0; i < lights.length(); ++i)
        {
            vec3 w_l = normalize(lights[i].xyz);

            intensity += calcBRDF(w_v, vertex.normal, w_l)
                         * lights[i].w
                         * max(0.0, dot(normal, w_l));
        }
        color = shapeColor * intensity;
    }
        break;
    default:
        break;
    }

    outColor = vec4(color, 1.0);
}
