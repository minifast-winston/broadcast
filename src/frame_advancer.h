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
  uint32_t fps_;

  pp::CompletionCallbackFactory<FrameAdvancer> callback_factory_;

  std::vector<FrameObserver*> listeners_;

public:
  FrameAdvancer(uint32_t fps, PPB_Core* core) : encoding_(false),
                                                paused_(true),
                                                fps_(fps),
                                                callback_factory_(this),
                                                listeners_() {
    core_ = core;
  };

  FrameAdvancer(uint32_t fps): encoding_(false),
                               paused_(true),
                               fps_(fps),
                               callback_factory_(this),
                               listeners_() {
    core_ = static_cast<const PPB_Core*>(
      pp::Module::Get()->GetBrowserInterface(PPB_CORE_INTERFACE));
  };

  void SetRemote(RemoteControl *remote);
  void OnFrameTick(FrameObserver *observer);
  void ClearListeners();

  void OnEncodingChange(bool encoding);
  void OnPausedChange(bool paused);

  PP_Time GetFrameDelay(PP_Time now, PP_Time back_then, int32_t frames);
  void ScheduleTick(int32_t result, PP_Time back_then, int32_t frames);
  virtual void Delay(PP_Time delay, pp::CompletionCallback callback);
  void Tick(int32_t frames);
};
