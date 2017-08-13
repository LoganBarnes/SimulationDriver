#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace sim
{

class HeightMap
{
public:
    HeightMap(int width, int height);

    HeightMap(int width, int height, std::vector<float> column_major_data);

    HeightMap(int width,
              int height,
              glm::vec3 worldOrigin,
              glm::vec3 worldDimensions,
              std::vector<float> column_major_data);

    float safeGet(int x, int y, float paddingVal = 0);

    float *operator[](int x);

    glm::vec3 safeGetWorld(int x, int y, float paddingVal = 0);

    template<typename Fun>
    void forEach(Fun fun);

    glm::vec3 worldFromLocal(glm::vec3 p) const;

    const std::vector<float> &getData() const;
    int getWidth() const;
    int getHeight() const;
    const glm::vec3 &getWorldOrigin() const;
    const glm::vec3 &getWorldDimensions() const;

    void setWorldOrigin(const glm::vec3 &worldOrigin);
    void setWorldDimensions(const glm::vec3 &worldDimensions);

private:
    std::vector<float> data_; // column major so it can be accessed with [x][y]

    int width_, height_;
    glm::vec3 worldOrigin_{0};
    glm::vec3 worldDimensions_;
};

template<typename Fun>
void HeightMap::forEach(Fun fun)
{
    int index = 0;
    for (int xi = 0; xi < width_; ++xi)
    {
        for (int yi = 0; yi < height_; ++yi)
        {
            fun(xi, yi, data_[index++]);
        }
    }
}

} // namespace sim

