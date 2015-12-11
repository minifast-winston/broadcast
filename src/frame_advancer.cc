#include "frame_advancer.h"
#include "ppapi/cpp/module.h"
#include <algorithm>

void FrameAdvancer::SetRemote(RemoteControl *remote) {
  remote->OnChange(this);
}

void FrameAdvancer::OnFrameTick(FrameObserver *observer) {
  listeners_.push_back(observer);
}

void FrameAdvancer::ClearListeners() {
  listeners_.clear();
}

void FrameAdvancer::OnEncodingChange(bool encoding) {
  encoding_ = encoding;
  ScheduleTick(PP_OK);
}

void FrameAdvancer::OnPausedChange(bool paused) {
  paused_ = paused;
  ScheduleTick(PP_OK);
}

PP_Time FrameAdvancer::GetFrameDelay() {
  PP_Time now = core_->GetTime();
  PP_Time then = last_frame_at_;
  PP_Time tick = 1.0 / fps_;
  PP_Time scheduled = then + tick;
  PP_Time delay = std::max(std::min(scheduled - now, tick), 0.0);
  return delay;
}

void FrameAdvancer::ScheduleTick(int32_t result) {
  if (result != PP_OK) { return; }
  if (encoding_ && !paused_) {
    Tick();
    Delay(
      callback_factory_.NewCallback(&FrameAdvancer::ScheduleTick),
      GetFrameDelay() * 1000);
  }
  last_frame_at_ = core_->GetTime();
}

void FrameAdvancer::Tick() {
  std::vector<FrameObserver*>::iterator it;
  frame_++;
  for (it = listeners_.begin(); it != listeners_.end(); ++it) {
    (*it)->OnFrameTick(frame_);
  }
}

void FrameAdvancer::Delay(pp::CompletionCallback callback, PP_Time delay) {
  pp::Module::Get()->core()->CallOnMainThread(delay, callback, 0);
}
