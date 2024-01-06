#ifndef MYFREENECTDEVICE_H
#define MYFREENECTDEVICE_H

#include "kinect_types.h"
#include <functional>
#include <libfreenect.hpp>
#include <mutex>
#include <span>
#include <vector>

class MyFreenectDevice : public Freenect::FreenectDevice {
public:
    using color_callback_t = std::function<void(std::span<uint8_t>)>;
    using depth_callback_t = std::function<void(std::span<uint16_t>)>;
    MyFreenectDevice(freenect_context* _ctx, int _index);

    void install_depth_callback(depth_callback_t cb);
    void install_color_callback(color_callback_t cb);
    // numbers of rgb samples we've received
    uint64_t rgb_samples();
    // numbers of depth samples we've received
    uint64_t depth_samples();

    static vec3 pixel_to_point(int x, int y, float depth)
    {
        float f = 595.f;
        // Convert from image plane coordinates to world coordinates
        return {
            .x = -(x - (640 - 1) / 2.f) * depth / f, // X = (x - cx) * d / fx
            .y = -(y - (480 - 1) / 2.f) * depth / f, // Y = (y - cy) * d / fy
            .z = depth                               // Z = d
        };
    }
    VideoType video_mode();
    void set_ir();
    void set_rgb();

    std::span<uint8_t> color_data();
    std::span<uint16_t> depth_data();

    VideoCapture take_capture();

private:
    // Do not call directly, even in child
    // Callback for libfreenect
    void VideoCallback(void* _rgb, uint32_t timestamp);
    void DepthCallback(void* _depth, uint32_t timestamp);

    VideoType current_video_type;

    // rgb picture
    std::mutex m_rgb_mutex;
    std::vector<uint8_t> m_buffer_video;
    bool m_new_rgb_frame = false;
    uint32_t rgb_timestamp = 0;
    uint64_t rgb_count = 0;
    color_callback_t rgb_callback;

    // depth picture
    std::mutex m_depth_mutex;
    std::vector<uint16_t> m_buffer_depth;
    bool m_new_depth_frame = false;
    uint32_t depth_timestamp = 0;
    uint64_t depth_count = 0;
    depth_callback_t depth_callback;
};

#endif // MYFREENECTDEVICE_H
