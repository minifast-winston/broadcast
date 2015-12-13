#include "test_helper.h"
#include "frame_advancer.h"

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
  ~MockFrameAdvancer() {}
  PP_Time GetDelay() { return delay_; }
  void Delay(PP_Time delay, pp::CompletionCallback callback) { delay_ = delay; }
};

TEST(FrameAdvancer, GetFrameDelay) {
  PPB_Core *core = 0;
  MockFrameAdvancer advancer(1, core);
  EXPECT_EQ(advancer.GetFrameDelay(0, 0, 1), 1);
  EXPECT_EQ(advancer.GetFrameDelay(1, 0, 2), 1);
  advancer.ClearListeners();
}

TEST(FrameAdvancer, Tick) {
  PPB_Core *core = 0;
  MockFrameAdvancer advancer(1, core);
  TestFrameAdvancerListener *frame_listener = new TestFrameAdvancerListener();
  advancer.OnFrameTick(frame_listener);
  advancer.Tick(1);
  EXPECT_EQ(frame_listener->GetValue(), 1);
  advancer.ClearListeners();
}

TEST(FrameAdvancer, ScheduleTick) {
  PPB_Core core;
  core.GetTime = &Timecop::GetTime;
  MockFrameAdvancer advancer(1, &core);
  TestFrameAdvancerListener *frame_listener = new TestFrameAdvancerListener();
  advancer.OnFrameTick(frame_listener);

  advancer.OnEncodingChange(true);           // user configures the video
  EXPECT_EQ(frame_listener->GetValue(), 0); // there is no frame callback
  Timecop::AdvanceTime(15.5);              // time passes in UI land

  PP_Time back_then = Timecop::GetTime();

  advancer.OnPausedChange(false);            // user starts capturing
  EXPECT_EQ(frame_listener->GetValue(), 1); // callback gets the first frame
  EXPECT_EQ(advancer.GetDelay(), 1);       // next frame is a full second away
  Timecop::AdvanceTime(advancer.GetDelay());

  Timecop::AdvanceTime(0.1);      // timing jitter due to capturing and encoding

  advancer.ScheduleTick(PP_OK, back_then, 2);  // callback for the second frame
  EXPECT_EQ(frame_listener->GetValue(), 2);   // callback gets the first frame
  EXPECT_LT(advancer.GetDelay(), 1);         // next frame is less than a second
  EXPECT_GT(advancer.GetDelay(), 0.8);

  Timecop::ResetTime();
  advancer.ClearListeners();
}
