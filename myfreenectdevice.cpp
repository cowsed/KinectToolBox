#include "myfreenectdevice.h"


MyFreenectDevice::MyFreenectDevice(freenect_context *_ctx, int _index)
    : Freenect::FreenectDevice(_ctx, _index),
    m_buffer_video(freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM,
                                            FREENECT_VIDEO_RGB).bytes),
    m_buffer_depth(freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM,
                                            FREENECT_DEPTH_REGISTERED).bytes/2),
    m_new_rgb_frame(false), m_new_depth_frame(false) {
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
}

// Do not call directly, even in child
void MyFreenectDevice::DepthCallback(void *_depth, uint32_t timestamp) {
  std::scoped_lock lock(m_depth_mutex);
  uint16_t *depth = static_cast<uint16_t *>(_depth);
  copy(depth, depth + getDepthBufferSize() / 2, m_buffer_depth.begin());
  m_new_depth_frame = true;
  depth_timestamp = timestamp;
  depth_count++;
}


uint64_t MyFreenectDevice::rgb_samples(){
  return rgb_count;
}
uint64_t MyFreenectDevice::depth_samples(){
  return depth_count;
}
