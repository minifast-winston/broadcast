#include <math.h>
#include <deque>

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/var_array_buffer.h"
#include "ppapi/cpp/var_dictionary.h"
#include "ppapi/utility/completion_callback_factory.h"

#include "ppapi/cpp/media_stream_video_track.h"
#include "ppapi/cpp/video_encoder.h"
#include "ppapi/cpp/video_frame.h"

#include "ivf_writer.h"
#include "remote_control.h"
#include "frame_advancer.h"

class CaptureInstance : public pp::Instance,
                        public FrameObserver {
  bool track_configured_;
  bool encoder_initialized_;

  RemoteControl remote_;
  FrameAdvancer advancer_;
  IVFWriter ivf_writer_;

  pp::VideoEncoder video_encoder_;
  pp::MediaStreamVideoTrack video_track_;
  pp::VideoFrame current_track_frame_;
  pp::Size frame_size_;
  pp::Size encoder_size_;
  pp::CompletionCallbackFactory<CaptureInstance> callback_factory_;
  std::deque<uint64_t> frames_timestamps_;

  int32_t encoded_frames_;

public:
  explicit CaptureInstance(PP_Instance i) : pp::Instance(i),
                                            callback_factory_(this),
                                            track_configured_(false),
                                            encoder_initialized_(false),
                                            remote_(),
                                            ivf_writer_(),
                                            advancer_(15),
                                            encoded_frames_(0),
                                            video_encoder_(this) {
    advancer_.SetRemote(&remote_);
    advancer_.OnFrameTick(this);
  }
  virtual ~CaptureInstance() {}

  void SetTrack(pp::Resource track, uint32_t width, uint32_t height) {
    video_track_ = pp::MediaStreamVideoTrack(track);

    if (encoder_size_.IsEmpty()) {
      frame_size_ = pp::Size(width, height);
    } else {
      frame_size_ = encoder_size_;
    }

    int32_t attrib_list[] = {PP_MEDIASTREAMVIDEOTRACK_ATTRIB_FORMAT,
                             PP_VIDEOFRAME_FORMAT_I420,
                             PP_MEDIASTREAMVIDEOTRACK_ATTRIB_WIDTH,
                             frame_size_.width(),
                             PP_MEDIASTREAMVIDEOTRACK_ATTRIB_HEIGHT,
                             frame_size_.height(),
                             PP_MEDIASTREAMVIDEOTRACK_ATTRIB_NONE};

    video_track_.Configure(
      attrib_list,
      callback_factory_.NewCallback(&CaptureInstance::OnTrackConfigured));
  }

  void OnTrackConfigured(int32_t result) {
    if (result != PP_OK) { return; }
    track_configured_ = true;
    video_encoder_.Initialize(
      PP_VIDEOFRAME_FORMAT_I420, frame_size_, PP_VIDEOPROFILE_VP8_ANY, 2000000,
      PP_HARDWAREACCELERATION_WITHFALLBACK,
      callback_factory_.NewCallback(&CaptureInstance::OnEncoderInitialized));
  }

  void OnEncoderInitialized(int32_t result) {
    if (result != PP_OK) { return; }
    if (video_encoder_.GetFrameCodedSize(&encoder_size_) != PP_OK) { return; }
    encoder_initialized_ = true;
  }

  void OnBitstreamBuffer(int32_t result, PP_BitstreamBuffer buffer) {
    if (result != PP_OK) { return; }

    pp::VarDictionary dictionary;
    pp::VarArrayBuffer array_buffer;

    uint8_t* data_ptr;
    uint32_t data_offset = 0;
    uint32_t frame_offset = 0;
    uint64_t timestamp = frames_timestamps_.front();
    frames_timestamps_.pop_front();

    dictionary.Set(pp::Var("name"), pp::Var("frame"));
    dictionary.Set(pp::Var("frame"), pp::Var(encoded_frames_++));
    dictionary.Set(pp::Var("timestamp"), pp::Var(pp::Module::Get()->core()->GetTime()));

    if (encoded_frames_ == 1) {
      array_buffer = pp::VarArrayBuffer(
          buffer.size + ivf_writer_.GetFileHeaderSize() +
          ivf_writer_.GetFrameHeaderSize());
      data_ptr = static_cast<uint8_t*>(array_buffer.Map());
      frame_offset = ivf_writer_.WriteFileHeader(
          data_ptr, "VP8",
          frame_size_.width(), frame_size_.height());
    } else {
      array_buffer = pp::VarArrayBuffer(
          buffer.size + ivf_writer_.GetFrameHeaderSize());
      data_ptr = static_cast<uint8_t*>(array_buffer.Map());
    }

    data_offset =
        frame_offset +
        ivf_writer_.WriteFrameHeader(data_ptr + frame_offset, timestamp, buffer.size);

    memcpy(data_ptr + data_offset, buffer.buffer, buffer.size);
    video_encoder_.RecycleBitstreamBuffer(buffer);
    array_buffer.Unmap();
    dictionary.Set(pp::Var("data"), array_buffer);
    PostMessage(dictionary);
  }

  void OnEncodeComplete(int32_t result) {
    if (result != PP_OK) { return; }
    video_encoder_.GetBitstreamBuffer(callback_factory_.NewCallbackWithOutput(
        &CaptureInstance::OnBitstreamBuffer));
  }

  void OnEncoderFrame(int32_t result, pp::VideoFrame encoder_frame, pp::VideoFrame track_frame) {
    if (result != PP_OK) {
      video_track_.RecycleFrame(track_frame);
      return;
    }

    encoder_frame.SetTimestamp(track_frame.GetTimestamp());
    memcpy(encoder_frame.GetDataBuffer(), track_frame.GetDataBuffer(), track_frame.GetDataBufferSize());
    frames_timestamps_.push_back(
      static_cast<uint64_t>(track_frame.GetTimestamp() * 1000));
    video_track_.RecycleFrame(track_frame);
    video_encoder_.Encode(encoder_frame, PP_FALSE, callback_factory_.NewCallback(
      &CaptureInstance::OnEncodeComplete));
  }

  void OnTrackFrame(int32_t result, pp::VideoFrame track_frame) {
    track_frame.detach();
    video_encoder_.GetVideoFrame(callback_factory_.NewCallbackWithOutput(
      &CaptureInstance::OnEncoderFrame, track_frame));
  }

  void OnFrameTick(int32_t currentFrame) {
    video_track_.GetFrame(callback_factory_.NewCallbackWithOutput(
        &CaptureInstance::OnTrackFrame));
  }

  virtual void HandleMessage(const pp::Var& var_message) {
    pp::VarDictionary dict_message(var_message);
    std::string command = dict_message.Get("command").AsString();

    if (command == "configure") {
      SetTrack(dict_message.Get("track").AsResource(),
               dict_message.Get("width").AsInt(),
               dict_message.Get("height").AsInt());
    } else if (command == "start") {
      remote_.SetEncoding(true);
    } else if (command == "pause") {
      remote_.SetPaused(true);
    } else if (command == "resume") {
      remote_.SetPaused(false);
    } else if (command == "stop") {
      remote_.SetEncoding(false);
    }
  }
};

class CaptureModule : public pp::Module {
  public:
  CaptureModule() : pp::Module() {}
  virtual ~CaptureModule() {}
  virtual pp::Instance* CreateInstance(PP_Instance i) {
    return new CaptureInstance(i);
  }
};

namespace pp {
  Module* CreateModule() { return new CaptureModule(); }
}
