#ifndef MYFREENECTDEVICE_H
#define MYFREENECTDEVICE_H

#include <libfreenect.hpp>
#include <mutex>
#include <vector>



class MyFreenectDevice : public Freenect::FreenectDevice {
public:
  MyFreenectDevice(freenect_context *_ctx, int _index);

  uint64_t rgb_samples();
  uint64_t depth_samples();
// private:
  // Do not call directly, even in child
  // Callback for libfreenect
  void VideoCallback(void *_rgb, uint32_t timestamp);
  void DepthCallback(void *_depth, uint32_t timestamp);

  // rgb picture
  std::mutex m_rgb_mutex;
  std::vector<uint8_t> m_buffer_video;
  bool m_new_rgb_frame = false;
  uint32_t rgb_timestamp = 0;
  uint64_t rgb_count = 0;

  // depth picture
  std::mutex m_depth_mutex;
  std::vector<uint16_t> m_buffer_depth;
  bool m_new_depth_frame = false;
  uint32_t depth_timestamp = 0;
  uint64_t depth_count = 0;
};

#endif // MYFREENECTDEVICE_H
