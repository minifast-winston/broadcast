#include "remote_control.h"

void RemoteControl::OnChange(RemoteControlObserver *observer) {
  views_.push_back(observer);
}

void RemoteControl::SetEncoding(bool encoding) {
  encoding_ = encoding;
  for (int i = 0; i < views_.size(); ++i) {
    views_[i]->OnEncodingChange(encoding_);
  }
}

void RemoteControl::SetPaused(bool paused) {
  paused_ = paused;
  for (int i = 0; i < views_.size(); ++i) {
    views_[i]->OnPausedChange(paused_);
  }
}

bool RemoteControl::IsRunning() {
  return encoding_ && !paused_;
}
