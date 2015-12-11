class Timecop {
  static Timecop *instance_;
  PP_Time now_;
public:
  Timecop(): now_(0.0){};
  static Timecop *instance() {
    if (!instance_) {
      instance_ = new Timecop();
    }
    return instance_;
  }
  static PP_Time GetTime() { return instance()->Get(); }
  static void ResetTime() { instance()->Set(0.0); }
  static void AdvanceTime(PP_Time interval) { instance()->Delta(interval); }
  static void RewindTime(PP_Time interval) { instance()->Delta(-1 * interval); }

  void Set(PP_Time time) { now_ = time; }
  void Delta(PP_Time interval) { now_ += interval; }
  PP_Time Get() { return now_; }
};
Timecop *Timecop::instance_ = 0;
