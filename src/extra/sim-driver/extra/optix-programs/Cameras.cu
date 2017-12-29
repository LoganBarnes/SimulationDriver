#include <sim-driver/extra/OptiXCommon.hpp>

rtDeclareVariable(PerRayData, prd_current, rtPayload,

                  );

rtDeclareVariable(optix::Ray, ray, rtCurrentRay,

                  );

rtDeclareVariable(uint2, launch_index, rtLaunchIndex,

                  );

rtDeclareVariable(unsigned int, surface_ray_type, ,

                  );

rtDeclareVariable(float, scene_epsilon, ,

                  );

rtDeclareVariable(rtObject,
                  top_object,

                  , );

rtBuffer<float4, 2> output_buffer;

RT_PROGRAM
void pinhole_camera()
{
    float3 ray_origin = make_float3(0.0f, 0.0f, -5.0f);
    float3 ray_direction = make_float3(0.0f, 0.0f, 1.0f);

    PerRayData prd;

    prd.result = make_float3(0.f);

    optix::Ray ray(ray_origin, ray_direction, surface_ray_type, scene_epsilon);

    rtTrace(top_object, ray, prd);

    output_buffer[launch_index] = make_float4(prd.result, 1.0);
} // pinhole_camera

rtDeclareVariable(float3,
                  bg_color,

                  , );

RT_PROGRAM
void miss()
{
    prd_current.result = bg_color;
}

rtDeclareVariable(float3,
                  error_color,

                  , );

RT_PROGRAM
void exception()
{
    output_buffer[launch_index] = make_float4(error_color, 1.0);
}
