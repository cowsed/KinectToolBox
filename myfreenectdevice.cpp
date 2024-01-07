#include "myfreenectdevice.h"
#include <iostream>

MyFreenectDevice::MyFreenectDevice(freenect_context *_ctx, int _index)
    : Freenect::FreenectDevice(_ctx, _index)
    , m_buffer_video(freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB).bytes)
    , m_buffer_depth(
          freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_REGISTERED).bytes / 2)
    , current_video_type(VideoType::RGB)
{
    setDepthFormat(FREENECT_DEPTH_REGISTERED);
}
// Do not call directly, even in child
void MyFreenectDevice::VideoCallback(void *_rgb, uint32_t timestamp) {
  std::scoped_lock<std::mutex> lock(m_rgb_mutex);
  auto *rgb = static_cast<uint8_t *>(_rgb);
  copy(rgb, rgb + getVideoBufferSize(), m_buffer_video.begin());
  m_new_rgb_frame = true;
  rgb_timestamp = timestamp;
  rgb_count++;
  rgb_callback(std::span<uint8_t>(m_buffer_video.begin(), m_buffer_video.end()));
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

std::span<uint8_t> MyFreenectDevice::color_data()
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

VideoType MyFreenectDevice::video_mode()
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
  std::vector<uint8_t> rgb(rgb_buffer_size);
  std::vector<uint16_t> depth(depth_buffer_size);

  std::copy(m_buffer_depth.begin(), m_buffer_depth.end(), depth.begin());

  if (current_video_type == VideoType::RGB) {
      std::copy(m_buffer_video.begin(), m_buffer_video.end(), rgb.begin());
  } else {
      for (size_t i = 0; i < num_pixels; i++) {
          rgb[3 * i] = m_buffer_video[i];
          rgb[3 * i + 1] = m_buffer_video[i];
          rgb[3 * i + 2] = m_buffer_video[i];
      }
  }
  return {.rgb = rgb, .depth = depth};
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
