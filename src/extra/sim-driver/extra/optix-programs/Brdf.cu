#include <sim-driver/extra/OptiXCommon.hpp>

rtDeclareVariable( float3, shading_normal,   attribute shading_normal, );
rtDeclareVariable( float3, geometric_normal, attribute geometric_normal, );

rtDeclareVariable( PerRayData, prd_current, rtPayload, );

rtDeclareVariable( optix::Ray, ray,   rtCurrentRay, );

rtDeclareVariable( float, t_hit, rtIntersectionDistance, );
rtDeclareVariable( float, scene_epsilon, , );

RT_PROGRAM
void
closest_hit_normals()
{
    float3 worldGeoNormal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, geometric_normal));
    float3 worldShadeNormal = normalize(rtTransformNormal(RT_OBJECT_TO_WORLD, shading_normal));
    float3 ffnormal = faceforward(worldShadeNormal, -ray.direction, worldGeoNormal);

    prd_current.result = ffnormal * 0.5f + 0.5f;
    prd_current.result = make_float3(1.0f, 0.5f, 0.1f);
}
