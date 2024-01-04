#include "kinect_types.h"
#include <format>

#include <pcl/impl/point_types.hpp>
#include <pcl/io/impl/pcd_io.hpp>
#include <pcl/point_cloud.h>

void to_pcd(const std::string& fname, const PointCapture& cap)
{
    pcl::PointCloud<pcl::PointXYZRGB> cloud;
    cloud.is_dense = false;
    cloud.reserve(cap.size());
    for (Point p : cap) {
        pcl::PointXYZRGB np;
        np.x = p.pos.x;
        np.y = p.pos.y;
        np.z = p.pos.z;
        np.r = p.r;
        np.g = p.g;
        np.b = p.b;

        cloud.push_back(np);
    }
    pcl::PCDWriter pw;
    pw.writeASCII(fname, cloud);
}

QDataStream& operator<<(QDataStream& out, const Point& p)
{
    out << p.r << p.g << p.b << p.pos;
    return out;
}

QDataStream& operator<<(QDataStream& out, const vec3& v)
{
    out << v.x << v.y << v.z;
    return out;
}

QDataStream& operator>>(QDataStream& in, vec3& p)
{
    in >> p.x;
    in >> p.y;
    in >> p.z;
    return in;
}

QDataStream& operator>>(QDataStream& in, Point& p)
{
    in >> p.r;
    in >> p.g;
    in >> p.b;
    in >> p.pos;
    return in;
}

QDataStream& operator<<(QDataStream& out, const VideoCapture& vc)
{
    QList<uint8_t> rgb(vc.rgb.size());
    QList<uint16_t> depth(vc.rgb.size());

    std::copy(vc.rgb.begin(), vc.rgb.end(), rgb.begin());
    std::copy(vc.depth.begin(), vc.depth.end(), depth.begin());

    out << rgb << depth;
    return out;
}
QDataStream& operator>>(QDataStream& in, VideoCapture& vc)
{
    QList<uint8_t> rgb;
    QList<uint16_t> depth;

    in >> rgb;
    in >> depth;
    return in;
}
