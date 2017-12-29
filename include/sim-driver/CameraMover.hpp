#pragma once

#include <sim-driver/Camera.hpp>

namespace sim {

template <typename T>
class TCameraMover
{
public:
    explicit TCameraMover(TCamera<T> camera);

    void yaw(T angleDegrees);
    void pitch(T angleDegrees);
    void zoom(T scale);

    bool isUsingOrbitMode() const;
    T getOrbitOffsetDistance() const;
    const glm::tvec3<T>& getOrbitOrigin() const;
    T getOrbitYawAngle() const;
    T getOrbitPitchAngle() const;

    void setUsingOrbitMode(bool usingOrbitMode);
    void setOrbitOffsetDistance(T orbitOffsetDistance);
    void setOrbitOrigin(const glm::tvec3<T>& orbitOrigin);
    void setOrbitYawAngle(T orbitYawAngle);
    void setOrbitPitchAngle(T orbitPitchAngle);

    TCamera<T> camera;

private:
    // orbit variables
    bool usingOrbitMode_{false};
    T orbitOffsetDistance_{0};
    glm::tvec3<T> orbitOrigin_{0};

    T orbitYawAngle_{0};
    T orbitPitchAngle_{0};

    void updateOrbitSettings();
};

using CameraMover = TCameraMover<float>;
using CameraMoverD = TCameraMover<double>;

} // namespace sim
