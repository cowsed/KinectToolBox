#include "myfreenectdevice.h"

#undef emit
#include <execution>
#define emit Q_EMIT

#include <iostream>
MyFreenectDevice::MyFreenectDevice(freenect_context *_ctx, int _index)
    : Freenect::FreenectDevice(_ctx, _index)
    , m_buffer_video(num_pixels)
    , m_buffer_depth(num_pixels)
    , current_video_type(VideoType::RGB)
{
    setDepthFormat(FREENECT_DEPTH_REGISTERED);
}

static auto get_color(size_t pixel_index, std::span<uint8_t> buf) -> rgb
{
    return {
        buf[3 * pixel_index],
        buf[3 * pixel_index + 1],
        buf[3 * pixel_index + 2],
    };
}
static auto get_ir(size_t pixel_index, std::span<uint8_t> buf) -> rgb
{
    return {
        buf[pixel_index],
        buf[pixel_index],
        buf[pixel_index],
    };
}

// Do not call directly, even in child
void MyFreenectDevice::VideoCallback(void *_rgb, uint32_t timestamp) {
  std::scoped_lock<std::mutex> lock(m_rgb_mutex);

  size_t buf_size = current_video_type == VideoType::RGB ? rgba_buffer_size : depth_buffer_size;

  std::span<uint8_t> rgb_buf = std::span<uint8_t>(static_cast<uint8_t *>(_rgb), buf_size);

  for (size_t i = 0; i < num_pixels; i++) {
      rgb col = current_video_type == VideoType::RGB ? get_color(i, rgb_buf) : get_ir(i, rgb_buf);
      m_buffer_video[i] = col;
  }

  m_new_rgb_frame = true;
  rgb_timestamp = timestamp;
  rgb_count++;
  rgb_callback(std::span<rgb>(m_buffer_video.begin(), m_buffer_video.end()));
}

// Do not call directly, even in child
void MyFreenectDevice::DepthCallback(void* _depth, uint32_t timestamp)
{
  std::scoped_lock lock(m_depth_mutex);
  auto *depth = static_cast<uint16_t *>(_depth);
  copy(depth, depth + getDepthBufferSize() / 2, m_buffer_depth.begin());
  m_new_depth_frame = true;
  depth_timestamp = timestamp;
  depth_count++;
  depth_callback(std::span<uint16_t>(m_buffer_depth.begin(), m_buffer_depth.end()));
}

std::span<rgb> MyFreenectDevice::color_data()
{
  return {m_buffer_video};
}
std::span<uint16_t> MyFreenectDevice::depth_data()
{
  return {m_buffer_depth};
}

void MyFreenectDevice::install_depth_callback(depth_callback_t callback)
{
  depth_callback = std::move(callback);
}

void MyFreenectDevice::install_color_callback(color_callback_t callback)
{
  rgb_callback = std::move(callback);
}

uint64_t MyFreenectDevice::rgb_samples() const
{
  return rgb_count;
}
uint64_t MyFreenectDevice::depth_samples() const
{
  return depth_count;
}

MyFreenectDevice::VideoType MyFreenectDevice::video_mode()
{
  return current_video_type;
}
void MyFreenectDevice::set_ir()
{
  setVideoFormat(FREENECT_VIDEO_IR_8BIT);
  current_video_type = VideoType::IR;
}
void MyFreenectDevice::set_rgb()
{
  setVideoFormat(FREENECT_VIDEO_RGB);
  current_video_type = VideoType::RGB;
}

VideoCapture MyFreenectDevice::take_capture()
{
  std::scoped_lock lock(m_depth_mutex);
  std::scoped_lock lock2(m_rgb_mutex);

  std::vector<DepthPoint> buf(num_pixels);

  std::transform(m_buffer_video.cbegin(),
                 m_buffer_video.cend(),
                 m_buffer_depth.cbegin(),
                 buf.begin(),
                 [](rgb col, uint16_t depth) { return DepthPoint{.col = col, .depth = depth}; });

  return {.pix = buf};
}
vec3 MyFreenectDevice::pixel_to_point(size_t image_x, size_t image_y, float depth)
{
  constexpr float focal_point = 595.0; //  or rather lens parameter
  const float center_x = (float) (640 - 1) / 2.F;
  const float center_y = (float) (480 - 1) / 2.F;
  // Convert from image plane coordinates to world coordinates
  return {
      .x = -((float) image_x - center_x) * depth / focal_point, // X = (x - cx) * d / fx
      .y = -((float) image_y - center_y) * depth / focal_point, // Y = (y - cy) * d / fy
      .z = depth                                                // Z = d
  };
}
