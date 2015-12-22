#ifndef BROADCAST_EVENT_H
#define BROADCAST_EVENT_H

#include <functional>
#include <vector>
#include <future>

template <typename... Emissions>
class Event {
  std::vector<std::function<void(Emissions...)>> listeners_;
public:
  Event(): listeners_(){};
  void Listen(std::function<void(Emissions...)> const &listener);
  void Send(Emissions... e);
  std::future<void> SendAsync(Emissions... e);
};

#include "event.tcc"

#endif
