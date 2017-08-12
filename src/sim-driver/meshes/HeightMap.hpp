#pragma once

#include <vector>
#include <cassert>

namespace sim
{

class HeightMap
{
public:
    HeightMap(int width, int height);
    HeightMap(int width, int height, std::vector<float> column_major_data);

    HeightMap(int width,
              int height,
              glm::vec3 world_origin,
              glm::vec3 world_dimensions,
              std::vector<float> column_major_data);

    float safeGet(int x, int y, float paddingVal = 0);
    float *operator[](int x);

    glm::vec3 safeGetWorld(int x, int y, float paddingVal = 0);

    template<typename Fun>
    void forEach(Fun fun);

    const std::vector<float> &data();

    glm::vec3 HeightMap::to_world(glm::vec3 p) const;

    const int width() const
    { return width_; }

    const int height() const
    { return width_; }

private:
    std::vector<float> data_; // column major so it can be accessed with [x][y]

    int width_, height_;
    glm::vec3 world_origin_{0};
    glm::vec3 world_dimensions_;
};

HeightMap::HeightMap(int width, int height)
    : HeightMap(width, height, std::vector<float>(static_cast<std::size_t>(width * height)))
{}

HeightMap::HeightMap(int width, int height, std::vector<float> column_major_data)
    : data_{std::move(column_major_data)},
      width_{width},
      height_{height},
      world_origin_{0.0f},
      world_dimensions_{width, 1.0f, height}
{
    assert(width * height == data_.size());
}

HeightMap::HeightMap(int width,
                     int height,
                     glm::vec3 world_origin,
                     glm::vec3 world_dimensions,
                     std::vector<float> column_major_data)
    : data_{std::move(column_major_data)},
      width_{width},
      height_{height},
      world_origin_{world_origin},
      world_dimensions_{world_dimensions}
{
    assert(width * height == data_.size());
}

float HeightMap::safeGet(int x, int y, float paddingVal)
{
    if (0 > x || x >= width_ || 0 > y || y >= height_)
    {
        return paddingVal;
    }
    return data_[x * height_ + y];
}

float *HeightMap::operator[](int x)
{
    return &(data_[x * height_]);
}

glm::vec3 HeightMap::safeGetWorld(int x, int y, float paddingVal)
{
    float height = paddingVal;
    if (0 <= x && x < width_ && 0 <= y && y < height_)
    {
        height = data_[x * height_ + y];
    }
    return to_world(glm::vec3{x, height, y});
}

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
};

const std::vector<float> &HeightMap::data()
{
    return data_;
}

glm::vec3 HeightMap::to_world(glm::vec3 p) const
{
    p.x /= glm::max(1.0f, width_ - 1.0f);
    p.z /= glm::max(1.0f, height_ - 1.0f);
    return p * world_dimensions_ + world_origin_;
}

} // namespace sim

