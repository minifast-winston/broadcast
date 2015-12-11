#include <vector>
#include "ppapi/utility/completion_callback_factory.h"
#include "remote_control.h"

class FrameObserver {
public:
  virtual void OnFrameTick(int32_t frame) {};
};

class FrameAdvancer: public RemoteControlObserver {
  const PPB_Core* core_;
  bool encoding_;
  bool paused_;
  int32_t frame_;
  uint32_t fps_;

  PP_Time last_frame_at_;
  pp::CompletionCallbackFactory<FrameAdvancer> callback_factory_;

  std::vector<FrameObserver*> listeners_;

public:
  FrameAdvancer(uint32_t fps, PPB_Core* core):
    callback_factory_(this),
    last_frame_at_(0),
    frame_(0),
    encoding_(false),
    paused_(true),
    fps_(fps) {
    core_ = core;
  };
  FrameAdvancer(uint32_t fps): callback_factory_(this),
                                        last_frame_at_(0),
                                        frame_(0),
                                        encoding_(false),
                                        paused_(true),
                                        fps_(fps) {
    core_ = static_cast<const PPB_Core*>(
      pp::Module::Get()->GetBrowserInterface(PPB_CORE_INTERFACE));
  };

  void SetRemote(RemoteControl *remote);
  void OnFrameTick(FrameObserver *observer);
  void ClearListeners();

  void OnEncodingChange(bool encoding);
  void OnPausedChange(bool paused);
  PP_Time GetFrameDelay();
  void ScheduleTick(int32_t result);
  virtual void Delay(pp::CompletionCallback callback, PP_Time delay);
  void Tick();
};
