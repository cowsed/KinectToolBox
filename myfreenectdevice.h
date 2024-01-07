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

    void install_depth_callback(depth_callback_t callback);
    void install_color_callback(color_callback_t callback);

    // numbers of rgb samples we've received
    [[nodiscard]] uint64_t rgb_samples() const;
    // numbers of depth samples we've received
    [[nodiscard]] uint64_t depth_samples() const;

    VideoType video_mode();
    void set_ir();
    void set_rgb();

    std::span<uint8_t> color_data();
    std::span<uint16_t> depth_data();

    VideoCapture take_capture();

    static vec3 pixel_to_point(size_t image_x, size_t image_y, float depth);

private:
    // Do not call directly, even in child
    // Callback for libfreenect
    void VideoCallback(void *_rgb, uint32_t timestamp) override;
    void DepthCallback(void *_depth, uint32_t timestamp) override;

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
