#include "test_helper.h"
#include "remote_control.h"

class TestRemoteControlListener: public RemoteControlObserver {
  bool called_;
  bool value_;
public:
  TestRemoteControlListener(): called_(false) {}
  ~TestRemoteControlListener() {}
  bool GetCalled() { return called_; }
  bool GetValue() { return value_; }

  void OnEncodingChange(bool encoding) { called_ = true; value_ = encoding; };
  void OnPausedChange(bool paused) { called_ = true; value_ = paused; };
};

TEST(RemoteControl, OnChangePaused) {
  TestRemoteControlListener *pause_listener = new TestRemoteControlListener();
  RemoteControl control = RemoteControl();
  control.OnChange(pause_listener);
  control.SetPaused(false);
  EXPECT_EQ(pause_listener->GetCalled(), true);
  EXPECT_EQ(pause_listener->GetValue(), false);
  control.SetEncoding(true);
  EXPECT_EQ(pause_listener->GetValue(), true);
  control.ClearListeners();
}

TEST(RemoteControl, IsRunning) {
  RemoteControl control = RemoteControl();
  EXPECT_EQ(control.IsRunning(), false);

  control.SetEncoding(true);
  control.SetPaused(false);
  EXPECT_EQ(control.IsRunning(), true);
}
