#pragma once

#include <vector>

namespace sim
{

template<typename T>
class HeightMap
{
public:
    explicit HeightMap(int width, int height);
    explicit HeightMap(int width, int height, std::vector<T> data);

    T *operator[](int x);

    const std::vector<T> &getData();

private:
    std::vector<T> data_;

    int width_, height_;
};

template<typename T>
HeightMap<T>::HeightMap(int width, int height)
    : HeightMap(width, height, std::vector<T>(width * height))
{}

template<typename T>
HeightMap<T>::HeightMap(int width, int height, std::vector<T> data)
    : width_{width},
      height_{height},
      data_{std::move(data)}
{}

template<typename T>
T *HeightMap<T>::operator[](int x)
{
    return &data_[x * width_];
}

template<typename T>
const std::vector<T> &HeightMap<T>::getData()
{
    return data_;
}

} // namespace sim

