#ifndef REMOTE_CONTROL_H_
#define REMOTE_CONTROL_H_

#include <vector>

class RemoteControlObserver {
public:
  virtual void OnEncodingChange(bool encoding) {};
  virtual void OnPausedChange(bool paused) {};
};

class RemoteControl {
  bool encoding_;
  bool paused_;
  std::vector<RemoteControlObserver*> listeners_;
public:
  explicit RemoteControl(): encoding_(false), paused_(true) {}
  void OnChange(RemoteControlObserver *observer);
  void ClearListeners();

  void SetEncoding(bool encoding);
  void SetPaused(bool paused);
  bool IsRunning();
};

#endif
