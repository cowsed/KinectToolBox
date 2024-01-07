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

constexpr size_t kinect_video_width = 640;
constexpr size_t kinect_video_height = 480;
constexpr size_t num_pixels = kinect_video_width * kinect_video_height;
constexpr size_t depth_buffer_size = num_pixels;
constexpr size_t rgb_buffer_size = 3 * num_pixels;
constexpr size_t rgba_buffer_size = 4 * num_pixels;

enum class KinectConnectionStatus {
    Connected,
    Disconnected,
    Unknown,
};

struct vec3 {
    float x, y, z;
};

struct rgb
{
    uint8_t r, g, b;
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

enum class VideoType {
    RGB,
    IR,
};

struct VideoCapture
{
    std::vector<rgb> rgb;
    std::vector<uint16_t> depth;
};

// QDataStream& operator<<(QDataStream& out, const VideoCapture& vc);
// QDataStream& operator>>(QDataStream& out, VideoCapture& vc);

using PointCloud = pcl::PointCloud<Point>;

using PointSupply = std::vector<PointCloud::Ptr>;
using PointSupplier = std::function<PointSupply()>;



#endif // KINECT_TYPES_H
