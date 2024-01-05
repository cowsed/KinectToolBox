#include "myfreenectdevice.h"
#include <iostream>

MyFreenectDevice::MyFreenectDevice(freenect_context *_ctx, int _index)
    : Freenect::FreenectDevice(_ctx, _index)
    , m_buffer_video(freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB).bytes)
    , m_buffer_depth(
          freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_REGISTERED).bytes / 2)
    , m_new_rgb_frame(false)
    , m_new_depth_frame(false)
    , current_video_type(VideoType::RGB)
{
    setDepthFormat(FREENECT_DEPTH_REGISTERED);
}
// Do not call directly, even in child
void MyFreenectDevice::VideoCallback(void *_rgb, uint32_t timestamp) {
  std::scoped_lock<std::mutex> lock(m_rgb_mutex);
  uint8_t *rgb = static_cast<uint8_t *>(_rgb);
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
  uint16_t *depth = static_cast<uint16_t *>(_depth);
  copy(depth, depth + getDepthBufferSize() / 2, m_buffer_depth.begin());
  m_new_depth_frame = true;
  depth_timestamp = timestamp;
  depth_count++;
  depth_callback(std::span<uint16_t>(m_buffer_depth.begin(), m_buffer_depth.end()));
}

std::span<uint8_t> MyFreenectDevice::color_data()
{
  return std::span(m_buffer_video);
}
std::span<uint16_t> MyFreenectDevice::depth_data()
{
  return std::span(m_buffer_depth);
}

void MyFreenectDevice::install_depth_callback(depth_callback_t cb)
{
  depth_callback = cb;
}

void MyFreenectDevice::install_color_callback(color_callback_t cb)
{
  rgb_callback = cb;
}

uint64_t MyFreenectDevice::rgb_samples()
{
  return rgb_count;
}
uint64_t MyFreenectDevice::depth_samples()
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
  std::vector<uint8_t> rgb(640 * 480 * 3);
  std::vector<uint16_t> depth(640 * 480);

  std::copy(m_buffer_depth.begin(), m_buffer_depth.end(), depth.begin());

  if (current_video_type == VideoType::RGB) {
      std::copy(m_buffer_video.begin(), m_buffer_video.end(), rgb.begin());
  } else {
      for (int i = 0; i < 640 * 480; i++) {
          rgb[3 * i] = m_buffer_video[i];
          rgb[3 * i + 1] = m_buffer_video[i];
          rgb[3 * i + 2] = m_buffer_video[i];
      }
  }
  return {.rgb = rgb, .depth = depth};
}
