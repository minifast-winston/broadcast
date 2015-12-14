#include <vector>
#include "ppapi/utility/completion_callback_factory.h"
#include "ppapi/cpp/size.h"
#include "ppapi/cpp/video_encoder.h"
#include "ppapi/cpp/media_stream_video_track.h"

#ifndef BROADCAST_CONFIGURER_H_
#define BROADCAST_CONFIGURER_H_

class ConfigurationObserver {
public:
  virtual void OnConfigure(pp::Size size) = 0;
};

class Configurer {
  std::vector<ConfigurationObserver*> listeners_;
  pp::CompletionCallbackFactory<Configurer> callback_factory_;

  pp::Size encoder_size_;
  pp::VideoEncoder *video_encoder_;
public:
  explicit Configurer() : listeners_(), callback_factory_(this) {}
  void SetEncoder(pp::VideoEncoder *encoder);
  void OnConfigure(ConfigurationObserver *listener);
  void ClearListeners();
  void Configure(pp::MediaStreamVideoTrack *track, uint32_t width, uint32_t height);
  void OnTrackConfigured(int32_t result, pp::Size frame_size);
  void OnEncoderInitialized(int32_t result, pp::Size frame_size);
};

#endif
