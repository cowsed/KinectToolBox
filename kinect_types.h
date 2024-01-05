#ifndef KINECT_TYPES_H
#define KINECT_TYPES_H

#include <QDataStream>
#include <QList>
#include "pcl/impl/point_types.hpp"
#include "pcl/point_cloud.h"
#include <cstdint>
#include <functional>
#include <future>
#include <iostream>
#include <optional>
#include <span>
#include <vector>

struct vec3 {
    float x, y, z;
};

// QDataStream& operator<<(QDataStream& out, const vec3& p);
// QDataStream& operator>>(QDataStream& in, vec3& p);

struct DepthPoint
{
    uint8_t r, g, b;
    uint16_t depth;
};

using Point = pcl::PointXYZRGB;

// QDataStream& operator<<(QDataStream& out, const Point& p);
// QDataStream& operator>>(QDataStream& out, Point& p);

using DepthPointCapture = std::vector<DepthPoint>;


struct VideoCapture
{
    std::vector<uint8_t> rgb;
    std::vector<uint16_t> depth;
};

// QDataStream& operator<<(QDataStream& out, const VideoCapture& vc);
// QDataStream& operator>>(QDataStream& out, VideoCapture& vc);

using PointCloud = pcl::PointCloud<Point>;

using PointSupply = std::vector<PointCloud::Ptr>;
using PointSupplier = std::function<PointSupply()>;

enum class VideoType {
    RGB,
    IR,
};


#endif // KINECT_TYPES_H
