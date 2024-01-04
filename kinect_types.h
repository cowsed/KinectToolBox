#ifndef KINECT_TYPES_H
#define KINECT_TYPES_H

#include <QDataStream>
#include <QList>
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

QDataStream& operator<<(QDataStream& out, const vec3& p);
QDataStream& operator>>(QDataStream& in, vec3& p);

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

QDataStream& operator<<(QDataStream& out, const Point& p);
QDataStream& operator>>(QDataStream& out, Point& p);

using DepthPointCapture = std::vector<DepthPoint>;

using PointCapture = std::vector<Point>;
void to_pcd(const std::string& fname, const PointCapture& cap);

struct VideoCapture
{
    std::vector<uint8_t> rgb;
    std::vector<uint16_t> depth;
};

QDataStream& operator<<(QDataStream& out, const VideoCapture& vc);
QDataStream& operator>>(QDataStream& out, VideoCapture& vc);

using PointSupply = std::vector<std::span<Point>>;
using PointSupplier = std::function<PointSupply()>;

enum class VideoType {
    RGB,
    IR,
};

template<typename T>
class MultiFuture
{
public:
    MultiFuture(T t)
        : thing(t)
        , runner([]() {})
    {}
    // MultiFuture(std::function<T()> f)
    // {
    //     std::packaged_task<T()> task(f);
    //     result = task.get_future();
    //     runner(std::move(task));
    // }
    void set_work(std::function<T()> f)
    {
        runner.join();

        thing = std::nullopt;
        std::packaged_task<T()> task(f);
        result = task.get_future();
        runner = std::thread(std::move(task));
        std::cout << "run" << std::endl;
    }
    T& get()
    {
        if (!thing.has_value()) {
            runner.join();
            if (!result.valid()) {
                std::cout << "INVALID" << std::endl;
            }
            thing = result.get();
        }

        return thing.value();
    }

private:
    std::thread runner;
    std::future<T> result;
    std::optional<T> thing;
};

#endif // KINECT_TYPES_H
