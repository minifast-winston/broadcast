#include "frame_advancer.h"
#include "ppapi/cpp/module.h"
#include <algorithm>

void FrameAdvancer::SetRemote(RemoteControl *remote) {
  remote->OnChange(this);
}

void FrameAdvancer::OnFrameTick(FrameObserver *observer) {
  listeners_.push_back(observer);
}

void FrameAdvancer::OnEncodingChange(bool encoding) {
  encoding_ = encoding;
  ScheduleTick(0);
}

void FrameAdvancer::OnPausedChange(bool paused) {
  paused_ = paused;
  ScheduleTick(0);
}

PP_Time FrameAdvancer::GetNextFrameTime() {
  PP_Time now = core_->GetTime();
  PP_Time until_next_frame = 1.0 / fps_;
  PP_Time since_last_frame = now - last_frame_at_;
  PP_Time delta = std::max(std::min(since_last_frame - until_next_frame, until_next_frame), 0.0);
  return (until_next_frame - delta) * 1000;
}

void FrameAdvancer::ScheduleTick(int32_t result) {
  if (encoding_ && !paused_) {
    Tick();
    Delay(
      callback_factory_.NewCallback(&FrameAdvancer::ScheduleTick),
      GetNextFrameTime());
  }
  last_frame_at_ = core_->GetTime();
}

void FrameAdvancer::Delay(pp::CompletionCallback callback, PP_Time delay) {
  pp::Module::Get()->core()->CallOnMainThread(delay, callback, 0);
}

void FrameAdvancer::Tick() {
  frame_++;
  for (auto &listener : listeners_) {
    listener->OnFrameTick(frame_);
  }
}
