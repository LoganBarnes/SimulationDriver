#include "OptiXScene.hpp"
#include <optixu/optixu_math_stream_namespace.h>

namespace sim
{

OptiXScene::OptiXScene(optix::Context &context)
    : material_(context->createMaterial())
{
    std::vector<float> data = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f};

    // context
    context->setRayTypeCount(2);
    context->setEntryPointCount(1);
    context->setStackSize(4640);

    context["surface_ray_type"]->setUint(0);
    context["trimming_ray_type"]->setUint(1);
    context["scene_epsilon"]->setFloat(1.0e-3f);

    // Starting program to generate rays
    context->setRayGenerationProgram(0,
                                     context->createProgramFromPTXFile("ptx/SimDriverOptiX_generated_Cameras.cu.ptx",
                                                                       "pinhole_camera"));
    // What to do when something messes up
    context["error_color"]->setFloat(1.0f, 0.0f, 0.0f);
    context->setExceptionProgram(0,
                                 context->createProgramFromPTXFile("ptx/SimDriverOptiX_generated_Cameras.cu.ptx",
                                                                   "exception"));
    // What to do when rays don't intersect with anything
    context["bg_color"]->setFloat(0.0f, 0.0f, 1.0f);
    context->setMissProgram(0,
                            context->createProgramFromPTXFile("ptx/SimDriverOptiX_generated_Cameras.cu.ptx", "miss"));

    material_->setClosestHitProgram(0,
                                    context->createProgramFromPTXFile("ptx/SimDriverOptiX_generated_Brdf.cu.ptx",
                                                                      "closest_hit_normals"));

    // box
    std::string box_ptx("ptx/SimDriverOptiX_generated_Box.cu.ptx");
    optix::Program box_bounds = context->createProgramFromPTXFile(box_ptx, "box_bounds");
    optix::Program box_intersect = context->createProgramFromPTXFile(box_ptx, "box_intersect");

    optix::Geometry box = context->createGeometry();

    box->setPrimitiveCount(1u);
    box->setBoundingBoxProgram(box_bounds);
    box->setIntersectionProgram(box_intersect);
    box["boxmin"]->setFloat(-1, -1, -1);
    box["boxmax"]->setFloat(1, 1, 1);

    optix::GeometryGroup boxGroup = context->createGeometryGroup();
    boxGroup->setChildCount(1);

    optix::GeometryInstance gi = context->createGeometryInstance();
    gi->setGeometry(box);
    gi->setMaterialCount(1);
    gi->setMaterial(0, material_);

    boxGroup->setChild(0, gi);
    boxGroup->setAcceleration(context->createAcceleration("NoAccel", "NoAccel"));

    optix::Transform trans = context->createTransform();

    trans->setChild(boxGroup);

    optix::Matrix4x4 T = optix::Matrix4x4::translate(optix::make_float3(0, 0, -5.5f));
    optix::Matrix4x4 S = optix::Matrix4x4::scale(optix::make_float3(1, 1, 1));
    optix::Matrix4x4 R = optix::Matrix4x4::rotate(0, optix::make_float3(0, 1, 0));

    optix::Matrix4x4 M = T * R * S;

    trans->setMatrix(false, M.getData(), 0);

    // scene bvh start
    optix::Group topGroup = context->createGroup();
    topGroup->setChildCount(1);

    //  topGroup->setChild( 0, boxGroup );
    topGroup->setChild(0, trans);

    topGroup->setAcceleration(context->createAcceleration("Bvh", "Bvh"));

    context["top_object"]->set(topGroup);
}

} // namespace sim
