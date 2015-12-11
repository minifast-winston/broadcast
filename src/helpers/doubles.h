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

class TestFrameAdvancerListener: public FrameObserver {
  int32_t value_;
  bool called_;
public:
  TestFrameAdvancerListener(): called_(false), value_(0) {}
  ~TestFrameAdvancerListener() {}
  int32_t GetValue() { return value_; }
  void OnFrameTick(int32_t value) { value_ = value; called_ = true; }
};

class MockFrameAdvancer: public FrameAdvancer {
  PP_Time delay_;
  pp::CompletionCallback callback_;
public:
  MockFrameAdvancer(uint32_t fps, PPB_Core* core): FrameAdvancer(fps, core) {};
  PP_Time GetDelay() { return delay_; }
  void Delay(pp::CompletionCallback callback, PP_Time delay) { delay_ = delay; }
};
