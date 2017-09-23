#include <sim-driver/CameraMover.hpp>

#define GLM_ENABLE_EXPERIMENTAL
//#pragma warning(push, 0)
//#include <glm/gtx/string_cast.hpp>
//#pragma warning(pop)

#include <glm/gtx/rotate_vector.hpp>
#include <iostream>

namespace sim
{

template<typename T>
TCameraMover<T>::TCameraMover(TCamera<T> cam)
    : camera{std::move(cam)}
{}

template<typename T>
void TCameraMover<T>::yaw(T angleDegrees)
{
    if (usingOrbitMode_) {
        orbitYawAngle_ += glm::radians(angleDegrees);
        updateOrbitSettings();
    }
}

template<typename T>
void TCameraMover<T>::pitch(T angleDegrees)
{
    if (usingOrbitMode_) {
        orbitPitchAngle_ += glm::radians(angleDegrees);
        constexpr auto eps = T(1e-4f);
        orbitPitchAngle_ = glm::clamp(orbitPitchAngle_, eps - glm::half_pi<T>(), glm::half_pi<T>() - eps);
        updateOrbitSettings();
    }
}

template<typename T>
void TCameraMover<T>::zoom(T scale)
{
    if (usingOrbitMode_) {
        constexpr auto eps = T(1.0e-3);
        scale *= std::max(orbitOffsetDistance_, T(0.1)) * T(0.01);
        orbitOffsetDistance_ = T(std::fmax(orbitOffsetDistance_ + scale, T(0)));
        if (orbitOffsetDistance_ < eps) {
            orbitOffsetDistance_ = 0.0f;
        }
        updateOrbitSettings();
    }
}

template<typename T>
bool TCameraMover<T>::isUsingOrbitMode() const
{
    return usingOrbitMode_;
}
template<typename T>
T TCameraMover<T>::getOrbitOffsetDistance() const
{
    return orbitOffsetDistance_;
}
template<typename T>
const glm::tvec3<T> &TCameraMover<T>::getOrbitOrigin() const
{
    return orbitOrigin_;
}
template<typename T>
T TCameraMover<T>::getOrbitYawAngle() const
{
    return orbitYawAngle_;
}
template<typename T>
T TCameraMover<T>::getOrbitPitchAngle() const
{
    return orbitPitchAngle_;
}

template<typename T>
void TCameraMover<T>::setUsingOrbitMode(bool usingOrbitMode)
{
    usingOrbitMode_ = usingOrbitMode;
    updateOrbitSettings();
}
template<typename T>
void TCameraMover<T>::setOrbitOffsetDistance(T orbitOffsetDistance)
{
    orbitOffsetDistance_ = orbitOffsetDistance;
    updateOrbitSettings();
}
template<typename T>
void TCameraMover<T>::setOrbitOrigin(const glm::tvec3<T> &orbitOrigin)
{
    orbitOrigin_ = orbitOrigin;
    updateOrbitSettings();
}
template<typename T>
void TCameraMover<T>::setOrbitYawAngle(T orbitYawAngle)
{
    orbitYawAngle_ = orbitYawAngle;
    updateOrbitSettings();
}
template<typename T>
void TCameraMover<T>::setOrbitPitchAngle(T orbitPitchAngle)
{
    orbitPitchAngle_ = orbitPitchAngle;
    updateOrbitSettings();
}

template<typename T>
void TCameraMover<T>::updateOrbitSettings()
{
    if (usingOrbitMode_) {
        glm::tvec3<T> eye{0, 0, orbitOffsetDistance_};
        eye = glm::rotate(eye, orbitPitchAngle_, {1, 0, 0});
        eye = glm::rotate(eye, orbitYawAngle_, {0, 1, 0});
        eye += orbitOrigin_;

        glm::tvec3<T> point{0, 0, -1};
        point = glm::rotate(point, orbitPitchAngle_, {1, 0, 0});
        point = glm::rotate(point, orbitYawAngle_, {0, 1, 0});
        point += orbitOrigin_;

        camera.lookAt(eye, point, {0, 1, 0});
    }
}

template
class TCameraMover<float>;

template
class TCameraMover<double>;

} // namespace sim
