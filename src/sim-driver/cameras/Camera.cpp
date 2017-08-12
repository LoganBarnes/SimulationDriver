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
      orbitPoint_{0},
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
    eye_ = eye;
    lookVector_ = glm::normalize(point - eye_);
    upVector_ = up;
    rightVector_ = glm::cross(lookVector_, upVector_);
    viewMatrix_ = glm::lookAt(eye_, point, upVector_);

    perspectiveProjectionViewMatrix_ = perspectiveMatrix_ * viewMatrix_;

    if (updateOrbitPoint)
    {
        orbitPoint_ = point;
        orbitOffsetDistance_ = glm::length(orbitPoint_ - eye_);
    }
}

template<typename T>
void TCamera<T>::perspective(
    T fovyDegrees,
    T aspect,
    T zNear,
    T zFar
)
{
    fovYDegrees_ = fovyDegrees;
    fovYRadians_ = glm::radians(fovYDegrees_);
    aspectRatio_ = aspect;
    nearPlane_ = zNear;
    farPlane_ = zFar;
    perspectiveMatrix_ = glm::perspective(fovYRadians_, aspectRatio_, nearPlane_, farPlane_);

    perspectiveProjectionViewMatrix_ = perspectiveMatrix_ * viewMatrix_;
}

template<typename T>
void TCamera<T>::ortho(
    T left,
    T right,
    T bottom,
    T top
)
{
    orthoLeft_ = left;
    orthoRight_ = right;
    orthoBottom_ = bottom;
    orthoTop_ = top;
    orthographicMatrix_ = glm::ortho(orthoLeft_, orthoRight_, orthoBottom_, orthoTop_);

    perspectiveProjectionViewMatrix_ = perspectiveMatrix_ * viewMatrix_;
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
        lookAt(eye_, eye_ + newLook, upVector_);
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
        lookAt(eye_, eye_ + newLook, upVector_);
    }
}

template<typename T>
void TCamera<T>::setEye(const glm::tvec3<T> &eye)
{
    lookAt(eye, eye + lookVector_, upVector_);
}

template<typename T>
void TCamera<T>::setLook(const glm::tvec3<T> &look)
{
    lookAt(eye_, eye_ + look, upVector_);
}

template<typename T>
void TCamera<T>::setUp(const glm::tvec3<T> &up)
{
    lookAt(eye_, eye_ + lookVector_, up);
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
void TCamera<T>::setNearPlaneDistance(T near)
{
    perspective(fovYDegrees_, aspectRatio_, near, farPlane_);
}

template<typename T>
void TCamera<T>::setFarPlaneDistance(T far)
{
    perspective(fovYDegrees_, aspectRatio_, nearPlane_, far);
}

template<typename T>
void TCamera<T>::setOrthoLeft(T left)
{
    ortho(left, orthoRight_, orthoBottom_, orthoTop_);
}

template<typename T>
void TCamera<T>::setOrthoRight(T right)
{
    ortho(orthoLeft_, right, orthoBottom_, orthoTop_);
}

template<typename T>
void TCamera<T>::setOrthoBottom(T bottom)
{
    ortho(orthoLeft_, orthoRight_, bottom, orthoTop_);
}

template<typename T>
void TCamera<T>::setOrthoTop(T top)
{
    ortho(orthoLeft_, orthoRight_, orthoBottom_, top);
}

template<typename T>
void TCamera<T>::setUsingOrbitMode(bool usingOrbitMode)
{
    usingOrbitMode_ = usingOrbitMode;
    updateOrbitSettings();
}

template<typename T>
void TCamera<T>::setOrbitPoint(glm::tvec3<T> orbitPoint)
{
    orbitPoint_ = orbitPoint;
    updateOrbitSettings();
}

template<typename T>
void TCamera<T>::setOrbitOffsetDistance(T orbitOffsetDistance)
{
    orbitOffsetDistance_ = orbitOffsetDistance;
    updateOrbitSettings();
}

template<typename T>
void TCamera<T>::updateOrbitSettings()
{
    if (usingOrbitMode_)
    {
        glm::tvec3<T> newLook = orbitPoint_ - eye_;
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
    glm::tvec3<T> eye = orbitPoint_ - newLook * orbitOffsetDistance_;
    lookAt(eye, eye + newLook, glm::vec3{0, 1, 0});
}

template
class TCamera<float>;

template
class TCamera<double>;

} // namespace sim
