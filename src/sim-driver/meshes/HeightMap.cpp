#include <sim-driver/meshes/HeightMap.hpp>

namespace sim
{

HeightMap::HeightMap(int width, int height)
        : HeightMap(width, height, std::vector<float>(static_cast<std::size_t>(width * height))) {}

HeightMap::HeightMap(int width, int height, std::vector<float> column_major_data)
        : data_{std::move(column_major_data)},
          width_{width},
          height_{height},
          worldOrigin_{0.0f},
          worldDimensions_{width, 1.0f, height}
{
    assert(width * height == data_.size());
}

HeightMap::HeightMap(int width,
                     int height,
                     glm::vec3 worldOrigin,
                     glm::vec3 worldDimensions,
                     std::vector<float> column_major_data)
        : data_{std::move(column_major_data)},
          width_{width},
          height_{height},
          worldOrigin_{worldOrigin},
          worldDimensions_{worldDimensions}
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
    return worldFromLocal(glm::vec3{x, height, y});
}

glm::vec3 HeightMap::worldFromLocal(glm::vec3 p) const
{
    p.x /= glm::max(1.0f, width_ - 1.0f);
    p.z /= glm::max(1.0f, height_ - 1.0f);
    return p * worldDimensions_ + worldOrigin_;
}

const std::vector<float> &HeightMap::getData() const { return data_; }
int HeightMap::getWidth() const { return width_; }
int HeightMap::getHeight() const { return height_; }
const glm::vec3 &HeightMap::getWorldOrigin() const { return worldOrigin_; }
const glm::vec3 &HeightMap::getWorldDimensions() const { return worldDimensions_; }

void HeightMap::setWorldOrigin(const glm::vec3 &worldOrigin) { worldOrigin_ = worldOrigin; }
void HeightMap::setWorldDimensions(const glm::vec3 &worldDimensions) { worldDimensions_ = worldDimensions; }

} // namespace sim

