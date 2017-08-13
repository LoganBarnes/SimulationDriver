#include <sim-driver/cameras/Camera.hpp>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace sim
{

template<typename T>
TCamera<T>::TCamera()
        : orbitOffsetDistance_{0},
          orbitOrigin_{0},
          usingOrbitMode_{false}
{
    lookAt(glm::tvec3<T>(0, 0, 0), glm::tvec3<T>(0, 0, -1));
    perspective(60, 1, 1, 1000);
    ortho(-1, 1, -1, 1);
}

template<typename T>
void TCamera<T>::lookAt(const glm::tvec3<T> &eye,
                        const glm::tvec3<T> &point,
                        const glm::tvec3<T> &up,
                        bool updateOrbitPoint)
{
    eyeVector_ = eye;
    lookVector_ = glm::normalize(point - eyeVector_);
    upVector_ = up;
    rightVector_ = glm::cross(lookVector_, upVector_);
    viewFromWorldMatrix_ = glm::lookAt(eyeVector_, point, upVector_);

    perspectiveScreenFromWorldMatrix_ = perspectiveScreenFromViewMatrix_ * viewFromWorldMatrix_;

    if (updateOrbitPoint)
    {
        orbitOrigin_ = point;
        orbitOffsetDistance_ = glm::length(orbitOrigin_ - eyeVector_);
    }
}

template<typename T>
void TCamera<T>::perspective(T fovyDegrees,
                             T aspect,
                             T zNear,
                             T zFar)
{
    fovYDegrees_ = fovyDegrees;
    fovYRadians_ = glm::radians(fovYDegrees_);
    aspectRatio_ = aspect;
    nearPlane_ = zNear;
    farPlane_ = zFar;
    perspectiveScreenFromViewMatrix_ = glm::perspective(fovYRadians_, aspectRatio_, nearPlane_, farPlane_);

    perspectiveScreenFromWorldMatrix_ = perspectiveScreenFromViewMatrix_ * viewFromWorldMatrix_;
}

template<typename T>
void TCamera<T>::ortho(T left,
                       T right,
                       T bottom,
                       T top)
{
    orthoLeft_ = left;
    orthoRight_ = right;
    orthoBottom_ = bottom;
    orthoTop_ = top;
    orthographicScreenFromViewMatrix_ = glm::ortho(orthoLeft_, orthoRight_, orthoBottom_, orthoTop_);

    perspectiveScreenFromWorldMatrix_ = perspectiveScreenFromViewMatrix_ * viewFromWorldMatrix_;
}

template<typename T>
void TCamera<T>::yaw(T angleRadians)
{
    glm::tvec3<T> newLook = glm::rotate(lookVector_, angleRadians, upVector_);
    if (usingOrbitMode_)
    {
        updateOrbit(newLook);
    }
    else
    {
        lookAt(eyeVector_, eyeVector_ + newLook, upVector_);
    }
}

template<typename T>
void TCamera<T>::pitch(T angleRadians)
{
    glm::tvec3<T> newLook = glm::rotate(lookVector_, angleRadians, rightVector_);
    upVector_ = glm::cross(rightVector_, newLook);
    if (usingOrbitMode_)
    {
        updateOrbit(newLook);
    }
    else
    {
        lookAt(eyeVector_, eyeVector_ + newLook, upVector_);
    }
}

template<typename T>
const glm::tvec3<T> &TCamera<T>::getEyeVector() const { return eyeVector_; }
template<typename T>
const glm::tvec3<T> &TCamera<T>::getLookVector() const { return lookVector_; }
template<typename T>
const glm::tvec3<T> &TCamera<T>::getUpVector() const { return upVector_; }
template<typename T>
const glm::tvec3<T> &TCamera<T>::getRightVector() const { return rightVector_; }
template<typename T>
T TCamera<T>::getOrbitOffsetDistance() const { return orbitOffsetDistance_; }
template<typename T>
const glm::tvec3<T> &TCamera<T>::getOrbitOrigin() const { return orbitOrigin_; }
template<typename T>
const glm::tmat4x4<T> &TCamera<T>::getViewFromWorldMatrix() const { return viewFromWorldMatrix_; }
template<typename T>
T TCamera<T>::getFovYDegrees() const { return fovYDegrees_; }
template<typename T>
T TCamera<T>::getFovYRadians() const { return fovYRadians_; }
template<typename T>
T TCamera<T>::getAspectRatio() const { return aspectRatio_; }
template<typename T>
T TCamera<T>::getNearPlane() const { return nearPlane_; }
template<typename T>
T TCamera<T>::getFarPlane() const { return farPlane_; }
template<typename T>
const glm::tmat4x4<T> &
TCamera<T>::getPerspectiveScreenFromViewMatrix() const { return perspectiveScreenFromViewMatrix_; }
template<typename T>
T TCamera<T>::getOrthoLeft() const { return orthoLeft_; }
template<typename T>
T TCamera<T>::getOrthoRight() const { return orthoRight_; }
template<typename T>
T TCamera<T>::getOrthoBottom() const { return orthoBottom_; }
template<typename T>
T TCamera<T>::getOrthoTop() const { return orthoTop_; }
template<typename T>
const glm::tmat4x4<T> &
TCamera<T>::getOrthographicScreenFromViewMatrix() const { return orthographicScreenFromViewMatrix_; }
template<typename T>
const glm::tmat4x4<T> &
TCamera<T>::getPerspectiveScreenFromWorldMatrix() const { return perspectiveScreenFromWorldMatrix_; }
template<typename T>
const glm::tmat4x4<T> &TCamera<T>::getOrthoScreenFromWorldMatrix() const { return orthoScreenFromWorldMatrix_; }
template<typename T>
bool TCamera<T>::isUsingOrbitMode() const { return usingOrbitMode_; }

template<typename T>
void TCamera<T>::setEyeVector(const glm::tvec3<T> &eyeVector)
{
    lookAt(eyeVector, eyeVector + lookVector_, upVector_);
}
template<typename T>
void TCamera<T>::setLookVector(const glm::tvec3<T> &lookVector)
{
    lookAt(eyeVector_, eyeVector_ + lookVector, upVector_);
}
template<typename T>
void TCamera<T>::setUpVector(const glm::tvec3<T> &upVector)
{
    lookAt(eyeVector_, eyeVector_ + lookVector_, upVector);
}
template<typename T>
void TCamera<T>::setFovYDegrees(T fovYDegrees)
{
    perspective(fovYDegrees, aspectRatio_, nearPlane_, farPlane_);
}
template<typename T>
void TCamera<T>::setAspectRatio(T aspectRatio)
{
    perspective(fovYDegrees_, aspectRatio, nearPlane_, farPlane_);
}
template<typename T>
void TCamera<T>::setNearPlane(T nearPlane)
{
    perspective(fovYDegrees_, aspectRatio_, nearPlane, farPlane_);
}
template<typename T>
void TCamera<T>::setFarPlane(T farPlane)
{
    perspective(fovYDegrees_, aspectRatio_, nearPlane_, farPlane);
}
template<typename T>
void TCamera<T>::setOrthoLeft(T orthoLeft)
{
    ortho(orthoLeft, orthoRight_, orthoBottom_, orthoTop_);
}
template<typename T>
void TCamera<T>::setOrthoRight(T orthoRight)
{
    ortho(orthoLeft_, orthoRight, orthoBottom_, orthoTop_);
}
template<typename T>
void TCamera<T>::setOrthoBottom(T orthoBottom)
{
    ortho(orthoLeft_, orthoRight_, orthoBottom, orthoTop_);
}
template<typename T>
void TCamera<T>::setOrthoTop(T orthoTop)
{
    ortho(orthoLeft_, orthoRight_, orthoBottom_, orthoTop);
}
template<typename T>
void TCamera<T>::setUsingOrbitMode(bool usingOrbitMode)
{
    usingOrbitMode_ = usingOrbitMode;
    updateOrbitSettings();
}
template<typename T>
void TCamera<T>::setOrbitOffsetDistance(T orbitOffsetDistance)
{
    orbitOffsetDistance_ = orbitOffsetDistance;
    updateOrbitSettings();
}
template<typename T>
void TCamera<T>::setOrbitOrigin(const glm::tvec3<T> &orbitOrigin)
{
    orbitOrigin_ = orbitOrigin;
    updateOrbitSettings();
}

template<typename T>
void TCamera<T>::updateOrbitSettings()
{
    if (usingOrbitMode_)
    {
        glm::tvec3<T> newLook = orbitOrigin_ - eyeVector_;
        T lookLen = glm::length(newLook);
        if (lookLen > 1.0e-3f)
        {
            newLook /= lookLen;
        }
        else
        {
            newLook = lookVector_;
        }
        updateOrbit(newLook);
    }
}

template<typename T>
void TCamera<T>::updateOrbit(glm::tvec3<T> newLook)
{
    glm::tvec3<T> eye = orbitOrigin_ - newLook * orbitOffsetDistance_;
    lookAt(eye, eye + newLook, glm::vec3{0, 1, 0});
}

template
class TCamera<float>;

template
class TCamera<double>;

} // namespace sim
