#pragma once

#include <glm/glm.hpp>

namespace sim
{

template<typename T>
class TCamera
{
public:

    TCamera();

    const glm::tmat4x4<T> &getViewMatrix() const
    { return viewMatrix_; }

    const glm::tmat4x4<T> &getPerspectiveProjectionMatrix() const
    { return perspectiveMatrix_; }

    const glm::tmat4x4<T> &getOrthographicProjectionMatrix() const
    { return orthographicMatrix_; }

    const glm::tmat4x4<T> &getPerspectiveProjectionViewMatrix() const
    { return perspectiveProjectionViewMatrix_; }

    const glm::tmat4x4<T> &getOrthographicProjectionViewMatrix() const
    { return orthoProjectionViewMatrix_; }

    const glm::tvec3<T> &getEyeVector() const
    { return eye_; }

    const glm::tvec3<T> &getLookVector() const
    { return lookVector_; }

    const glm::tvec3<T> &getUpVector() const
    { return upVector_; }

    const glm::tvec3<T> &getRightVector() const
    { return rightVector_; }

    const T &getFovYDegrees() const
    { return fovYDegrees_; }

    const T &getFovTRadians() const
    { return fovYRadians_; }

    const T &getAspectRatio() const
    { return aspectRatio_; }

    const T &getNearPlaneDistance() const
    { return nearPlane_; }

    const T &getFarPlaneDistance() const
    { return farPlane_; }


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

    void setEye(const glm::tvec3<T> &eye);
    void setLook(const glm::tvec3<T> &look);
    void setUp(const glm::tvec3<T> &up);

    void setFovYDegrees(T fovYDegrees);
    void setAspectRatio(T aspectRatio);
    void setNearPlaneDistance(T near);
    void setFarPlaneDistance(T far);

    void setOrthoLeft(T left);
    void setOrthoRight(T right);
    void setOrthoBottom(T bottom);
    void setOrthoTop(T top);

    void setUsingOrbitMode(bool usingOrbitMode);
    void setOrbitPoint(glm::tvec3<T> orbitPoint);
    void setOrbitOffsetDistance(T orbitOffsetDistance);

private:

    // view matrix variables
    glm::tvec3<T> eye_, lookVector_, upVector_, rightVector_;
    T orbitOffsetDistance_;
    glm::tvec3<T> orbitPoint_;
    glm::tmat4x4<T> viewMatrix_;

    // projection matrix variables
    T fovYDegrees_, fovYRadians_, aspectRatio_, nearPlane_, farPlane_;
    glm::tmat4x4<T> perspectiveMatrix_;

    // orthographic matrix variables
    T orthoLeft_, orthoRight_, orthoBottom_, orthoTop_;
    glm::tmat4x4<T> orthographicMatrix_;

    glm::tmat4x4<T> perspectiveProjectionViewMatrix_;
    glm::tmat4x4<T> orthoProjectionViewMatrix_;

    bool usingOrbitMode_;

    void updateOrbitSettings();
    void updateOrbit(glm::tvec3<T> newLook);

};

using Camera = TCamera<float>;
using CameraD = TCamera<double>;

} // namespace sim
