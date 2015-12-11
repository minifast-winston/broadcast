#include "remote_control.h"

void RemoteControl::OnChange(RemoteControlObserver *observer) {
  listeners_.push_back(observer);
}

void RemoteControl::ClearListeners() {
  listeners_.clear();
}

void RemoteControl::SetEncoding(bool encoding) {
  std::vector<RemoteControlObserver*>::iterator it;
  encoding_ = encoding;
  for (it = listeners_.begin(); it != listeners_.end(); ++it) {
    (*it)->OnEncodingChange(encoding_);
  }
}

void RemoteControl::SetPaused(bool paused) {
  std::vector<RemoteControlObserver*>::iterator it;
  paused_ = paused;
  for (it = listeners_.begin(); it != listeners_.end(); ++it) {
    (*it)->OnPausedChange(encoding_);
  }
}

bool RemoteControl::IsRunning() {
  return encoding_ && !paused_;
}
