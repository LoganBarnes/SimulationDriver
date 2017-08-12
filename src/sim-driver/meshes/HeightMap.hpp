#pragma once

#include <vector>
#include <cassert>

namespace sim
{

template<typename T = float>
class HeightMap
{
public:
    explicit HeightMap(int width, int height);
    explicit HeightMap(int width, int height, std::vector<T> column_major_data);

    T safeGet(int x, int y, T paddingVal = 0);
    T *operator[](int x);

    template<typename Fun>
    void forEach(Fun fun);

    const std::vector<T> &data();

    const int width() const { return width_; }
    const int height() const { return width_; }

private:
    std::vector<T> data_; // column major so it can be accessed with [x][y]

    int width_, height_;
};

template<typename T>
HeightMap<T>::HeightMap(int width, int height)
    : HeightMap(width, height, std::vector<T>(width * height))
{}

template<typename T>
HeightMap<T>::HeightMap(int width, int height, std::vector<T> column_major_data)
    : width_{width},
      height_{height},
      data_{std::move(column_major_data)}
{
    assert(width * height == data_.size());
}

template<typename T>
T HeightMap<T>::safeGet(int x, int y, T paddingVal)
{
    if (0 > x || x >= width_ || 0 > y || y >= height_)
    {
        return paddingVal;
    }
    return data_.at(x * height_ + y);
}

template<typename T>
T *HeightMap<T>::operator[](int x)
{
    return &(data_[x * height_]);
}

template<typename T>
template<typename Fun>
void HeightMap<T>::forEach(Fun fun)
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

template<typename T>
const std::vector<T> &HeightMap<T>::data()
{
    return data_;
}

} // namespace sim

