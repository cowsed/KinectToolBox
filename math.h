#ifndef MATH_H
#define MATH_H

#include <cstdint>
#include <vector>

struct vec3 {
    float x, y, z;
};

struct Point {
    uint8_t r, g, b;
    uint16_t depth;
};

using PointCapture = std::vector<Point>;

enum class VideoType {
    RGB,
    IR,
};

#endif // MATH_H
