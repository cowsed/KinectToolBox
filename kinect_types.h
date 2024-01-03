#ifndef KINECT_TYPES_H
#define KINECT_TYPES_H

#include <cstdint>
#include <functional>
#include <span>
#include <vector>

struct vec3 {
    float x, y, z;
};

struct DepthPoint
{
    uint8_t r, g, b;
    uint16_t depth;
};

struct Point
{
    uint8_t r, g, b;
    vec3 pos;
};

using PointCapture = std::vector<DepthPoint>;

struct VideoCapture
{
    std::vector<uint8_t> rgb;
    std::vector<uint16_t> depth;
};

using PointSupply = std::vector<std::span<Point>>;
using PointSupplier = std::function<PointSupply()>;

enum class VideoType {
    RGB,
    IR,
};

#endif // KINECT_TYPES_H
