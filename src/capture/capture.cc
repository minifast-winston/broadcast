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

#include "bitstream_writer.h"
#include "remote_control.h"
#include "frame_advancer.h"
#include "configurer.h"

class CaptureInstance : public pp::Instance,
                        public FrameObserver,
                        public ConfigurationObserver {
  RemoteControl remote_;
  FrameAdvancer advancer_;
  Configurer configurer_;
  pp::VideoEncoder video_encoder_;

  pp::CompletionCallbackFactory<CaptureInstance> callback_factory_;

  pp::MediaStreamVideoTrack video_track_;
  pp::Size frame_size_;

public:
  explicit CaptureInstance(PP_Instance i) : pp::Instance(i),
                                            remote_(),
                                            advancer_(15),
                                            configurer_(),
                                            video_encoder_(this),
                                            callback_factory_(this) {
    advancer_.SetRemote(&remote_);
    advancer_.OnFrameTick(this);
    configurer_.OnConfigure(this);
    configurer_.SetEncoder(&video_encoder_);
  }
  virtual ~CaptureInstance() {}

  void OnBitstreamBuffer(int32_t result, PP_BitstreamBuffer buffer, PP_TimeDelta timestamp, int32_t frame_count) {
    if (result != PP_OK) { return; }

    pp::VarDictionary dictionary;
    BitstreamWriter writer(&buffer, frame_count == 1, timestamp, frame_size_);
    pp::VarArrayBuffer array_buffer(writer.GetSize());

    dictionary.Set(pp::Var("name"), pp::Var("frame"));
    dictionary.Set(pp::Var("frame"), pp::Var(frame_count));
    dictionary.Set(pp::Var("timestamp"), pp::Var(timestamp));
    dictionary.Set(pp::Var("width"), pp::Var(1.0 * frame_size_.width()));
    dictionary.Set(pp::Var("height"), pp::Var(1.0 * frame_size_.height()));
    dictionary.Set(pp::Var("buffer"), pp::Var(1.0 * buffer.size));
    dictionary.Set(pp::Var("keyframe"), pp::Var(buffer.key_frame));
    dictionary.Set(pp::Var("size"), pp::Var(1.0 * writer.GetSize()));

    writer.WriteTo(static_cast<uint8_t*>(array_buffer.Map()));
    array_buffer.Unmap();
    video_encoder_.RecycleBitstreamBuffer(buffer);
    dictionary.Set(pp::Var("data"), array_buffer);
    PostMessage(dictionary);
  }

  void OnEncodeComplete(int32_t result, PP_TimeDelta timestamp, int32_t frame_count) {
    if (result != PP_OK) { return; }
    video_encoder_.GetBitstreamBuffer(callback_factory_.NewCallbackWithOutput(
        &CaptureInstance::OnBitstreamBuffer, timestamp, frame_count));
  }

  void OnEncoderFrame(int32_t result, pp::VideoFrame encoder_frame, pp::VideoFrame track_frame, int32_t frame_count) {
    if (result != PP_OK) {
      video_track_.RecycleFrame(track_frame);
      return;
    }

    encoder_frame.SetTimestamp(track_frame.GetTimestamp());
    memcpy(encoder_frame.GetDataBuffer(), track_frame.GetDataBuffer(), track_frame.GetDataBufferSize());
    video_track_.RecycleFrame(track_frame);

    video_encoder_.Encode(
      encoder_frame,
      PP_FALSE,
      callback_factory_.NewCallback(
        &CaptureInstance::OnEncodeComplete,
        encoder_frame.GetTimestamp(),
        frame_count));
  }

  void OnTrackFrame(int32_t result, pp::VideoFrame track_frame, int32_t frame_count) {
    track_frame.detach();
    video_encoder_.GetVideoFrame(callback_factory_.NewCallbackWithOutput(
      &CaptureInstance::OnEncoderFrame, track_frame, frame_count));
  }

  void OnFrameTick(int32_t frame_count) {
    video_track_.GetFrame(callback_factory_.NewCallbackWithOutput(
        &CaptureInstance::OnTrackFrame, frame_count));
  }

  void OnConfigure(pp::Size size) {
    frame_size_ = size;
    remote_.SetEncoding(true);
  }

  virtual void HandleMessage(const pp::Var& var_message) {
    pp::VarDictionary dict_message(var_message);
    std::string command = dict_message.Get("command").AsString();

    if (command == "configure") {
      if (!dict_message.Get("track").is_resource()) { return; }
      video_track_ = pp::MediaStreamVideoTrack(dict_message.Get("track").AsResource());
      configurer_.Configure(&video_track_,
                            dict_message.Get("width").AsInt(),
                            dict_message.Get("height").AsInt());
    } else if (command == "pause") {
      remote_.SetPaused(true);
    } else if (command == "resume") {
      remote_.SetPaused(false);
    } else if (command == "stop") {
      video_encoder_ = pp::VideoEncoder(this);
      configurer_.SetEncoder(&video_encoder_);
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
