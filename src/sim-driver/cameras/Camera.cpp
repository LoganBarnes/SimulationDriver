#include <sim-driver/cameras/Camera.hpp>

#define GLFORCE_RADIANS_

#include <glm/gtc/matrix_transform.hpp>

namespace sim
{

template<typename T>
TCamera<T>::TCamera()
{
    lookAt(
            glm::tvec3<T>(0.0, 2.0, 5.0),
            glm::tvec3<T>(0.0, 0.0, 0.0)
    );
    perspective(60.0, 1.0, 1.0, 1000.0);
    ortho(-1.0, 1.0, -1.0, 1.0);
}

template<typename T>
void
TCamera<T>::lookAt(
        const glm::tvec3<T> &eye,
        const glm::tvec3<T> &point,
        const glm::tvec3<T> &up
)
{
    eye_ = eye;
    lookVector_ = glm::normalize(point - eye_);
    upVector_ = up;
    rightVector_ = glm::cross(lookVector_, upVector_);
    viewMatrix_ = glm::lookAt(eye_, point, upVector_);

    perspectiveProjectionViewMatrix_ = perspectiveMatrix_ * viewMatrix_;
}

template<typename T>
void
TCamera<T>::perspective(
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
void
TCamera<T>::ortho(
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
void
TCamera<T>::setEye(const glm::tvec3<T> &eye)
{
    lookAt(eye, eye + lookVector_, upVector_);
}

template<typename T>
void
TCamera<T>::setLook(const glm::tvec3<T> &look)
{
    lookAt(eye_, eye_ + look, upVector_);
}

template<typename T>
void
TCamera<T>::setUp(const glm::tvec3<T> &up)
{
    lookAt(eye_, eye_ + lookVector_, up);
}

template<typename T>
void
TCamera<T>::setFovYDegrees(T fovYDegrees)
{
    perspective(fovYDegrees, aspectRatio_, nearPlane_, farPlane_);
}

template<typename T>
void
TCamera<T>::setAspectRatio(T aspectRatio)
{
    perspective(fovYDegrees_, aspectRatio, nearPlane_, farPlane_);
}

template<typename T>
void
TCamera<T>::setNearPlaneDistance(T near)
{
    perspective(fovYDegrees_, aspectRatio_, near, farPlane_);
}

template<typename T>
void
TCamera<T>::setFarPlaneDistance(T far)
{
    perspective(fovYDegrees_, aspectRatio_, nearPlane_, far);
}

template<typename T>
void
TCamera<T>::setOrthoLeft(T left)
{
    ortho(left, orthoRight_, orthoBottom_, orthoTop_);
}

template<typename T>
void
TCamera<T>::setOrthoRight(T right)
{
    ortho(orthoLeft_, right, orthoBottom_, orthoTop_);
}

template<typename T>
void
TCamera<T>::setOrthoBottom(T bottom)
{
    ortho(orthoLeft_, orthoRight_, bottom, orthoTop_);
}

template<typename T>
void
TCamera<T>::setOrthoTop(T top)
{
    ortho(orthoLeft_, orthoRight_, orthoBottom_, top);
}

template
class TCamera<float>;

template
class TCamera<double>;

} // namespace sim
