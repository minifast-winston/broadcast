#include "configurer.h"

void Configurer::SetEncoder(pp::VideoEncoder *encoder) {
  video_encoder_ = encoder;
}

void Configurer::OnConfigure(ConfigurationObserver *listener) {
  listeners_.push_back(listener);
}

void Configurer::ClearListeners() {
  listeners_.clear();
}

void Configurer::Configure(pp::MediaStreamVideoTrack *video_track, uint32_t width, uint32_t height) {
  pp::Size frame_size = pp::Size(width, height);

  int32_t attrib_list[] = {PP_MEDIASTREAMVIDEOTRACK_ATTRIB_FORMAT,
                           PP_VIDEOFRAME_FORMAT_I420,
                           PP_MEDIASTREAMVIDEOTRACK_ATTRIB_WIDTH,
                           frame_size.width(),
                           PP_MEDIASTREAMVIDEOTRACK_ATTRIB_HEIGHT,
                           frame_size.height(),
                           PP_MEDIASTREAMVIDEOTRACK_ATTRIB_NONE};

  video_track->Configure(
    attrib_list,
    callback_factory_.NewCallback(&Configurer::OnTrackConfigured, frame_size));
}

void Configurer::OnTrackConfigured(int32_t result, pp::Size frame_size) {
  if (result != PP_OK) { return; }
  video_encoder_->Initialize(
    PP_VIDEOFRAME_FORMAT_I420,
    frame_size,
    PP_VIDEOPROFILE_VP8_ANY, 2000000,
    PP_HARDWAREACCELERATION_WITHFALLBACK,
    callback_factory_.NewCallback(&Configurer::OnEncoderInitialized, frame_size));
}

void Configurer::OnEncoderInitialized(int32_t result, pp::Size frame_size) {
  std::vector<ConfigurationObserver *>::iterator it;
  if (result != PP_OK) { return; }
  if (video_encoder_->GetFrameCodedSize(&encoder_size_) != PP_OK) { return; }
  for (it = listeners_.begin(); it != listeners_.end(); ++it) {
    (*it)->OnConfigure(encoder_size_);
  }
}
