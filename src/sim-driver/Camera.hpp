#pragma once

#include <glm/glm.hpp>

namespace sim
{

template<typename T>
class TCamera
{
public:

    TCamera();

    void lookAt(const glm::tvec3<T> &eye,
                const glm::tvec3<T> &point,
                const glm::tvec3<T> &up = glm::tvec3<T>(0, 1, 0),
                bool updateOrbitPoint = false);

    void perspective(T fovyDegrees,
                     T aspect,
                     T zNear,
                     T zFar);

    void ortho(T left,
               T right,
               T bottom,
               T top);

    void yaw(T angleRadians);
    void pitch(T angleRadians);

    const glm::tvec3<T> &getEyeVector() const;
    const glm::tvec3<T> &getLookVector() const;
    const glm::tvec3<T> &getUpVector() const;
    const glm::tvec3<T> &getRightVector() const;
    const glm::tmat4x4<T> &getViewFromWorldMatrix() const;
    T getFovYDegrees() const;
    T getFovYRadians() const;
    T getAspectRatio() const;
    T getNearPlane() const;
    T getFarPlane() const;
    const glm::tmat4x4<T> &getPerspectiveScreenFromViewMatrix() const;
    T getOrthoLeft() const;
    T getOrthoRight() const;
    T getOrthoBottom() const;
    T getOrthoTop() const;
    const glm::tmat4x4<T> &getOrthographicScreenFromViewMatrix() const;
    const glm::tmat4x4<T> &getPerspectiveScreenFromWorldMatrix() const;
    const glm::tmat4x4<T> &getOrthoScreenFromWorldMatrix() const;
    bool isUsingOrbitMode() const;
    T getOrbitOffsetDistance() const;
    const glm::tvec3<T> &getOrbitOrigin() const;

    void setEyeVector(const glm::tvec3<T> &eyeVector);
    void setLookVector(const glm::tvec3<T> &lookVector);
    void setUpVector(const glm::tvec3<T> &upVector);
    void setFovYDegrees(T fovYDegrees);
    void setAspectRatio(T aspectRatio);
    void setNearPlane(T nearPlane);
    void setFarPlane(T farPlane);
    void setOrthoLeft(T orthoLeft);
    void setOrthoRight(T orthoRight);
    void setOrthoBottom(T orthoBottom);
    void setOrthoTop(T orthoTop);
    void setUsingOrbitMode(bool usingOrbitMode);
    void setOrbitOffsetDistance(T orbitOffsetDistance);
    void setOrbitOrigin(const glm::tvec3<T> &orbitOrigin);

private:

    // view matrix variables
    glm::tvec3<T> eyeVector_, lookVector_, upVector_, rightVector_;
    glm::tmat4x4<T> viewFromWorldMatrix_;

    // projection matrix variables
    T fovYDegrees_, fovYRadians_, aspectRatio_, nearPlane_, farPlane_;
    glm::tmat4x4<T> perspectiveScreenFromViewMatrix_;

    // orthographic matrix variables
    T orthoLeft_, orthoRight_, orthoBottom_, orthoTop_;
    glm::tmat4x4<T> orthographicScreenFromViewMatrix_;

    glm::tmat4x4<T> perspectiveScreenFromWorldMatrix_;
    glm::tmat4x4<T> orthoScreenFromWorldMatrix_;

    // orbit variables
    bool usingOrbitMode_;
    T orbitOffsetDistance_;
    glm::tvec3<T> orbitOrigin_;

    void updateOrbitSettings();

    void updateOrbit(glm::tvec3<T> newLook);

};

using Camera = TCamera<float>;
using CameraD = TCamera<double>;

} // namespace sim
