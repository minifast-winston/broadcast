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
  if (encoding != encoding_) {
    encoding_ = encoding;
    ScheduleTick(PP_OK, core_->GetTime(), 1);
  }
}

void FrameAdvancer::OnPausedChange(bool paused) {
  if (paused != paused_) {
    paused_ = paused;
    ScheduleTick(PP_OK, core_->GetTime(), 1);
  }
}

PP_Time FrameAdvancer::GetFrameDelay(PP_Time now, PP_Time back_then, int32_t frames) {
  PP_Time tick = 1.0 / fps_;
  PP_Time next = back_then + (tick * frames);
  PP_Time delay = std::max(std::min(next - now, tick), 0.0);
  return delay;
}

void FrameAdvancer::ScheduleTick(int32_t result, PP_Time back_then, int32_t frames) {
  PP_Time now;
  if (result != PP_OK) { return; }
  if (encoding_ && !paused_) {
    Tick(frames);
    now = core_->GetTime();
    Delay(
      GetFrameDelay(now, back_then, frames),
      callback_factory_.NewCallback(&FrameAdvancer::ScheduleTick, back_then, frames + 1));
  }
}

void FrameAdvancer::Tick(int32_t frames) {
  std::vector<FrameObserver*>::iterator it;
  for (it = listeners_.begin(); it != listeners_.end(); ++it) {
    (*it)->OnFrameTick(frames);
  }
}

void FrameAdvancer::Delay(PP_Time delay, pp::CompletionCallback callback) {
  pp::Module::Get()->core()->CallOnMainThread(delay * 1000, callback, 0);
}
