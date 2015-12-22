#include "event.h"

template <typename... Emissions>
void Event<Emissions...>::Listen(std::function<void(Emissions...)> const &listener) {
  listeners_.push_back(listener);
}

template <typename... Emissions>
void Event<Emissions...>::Send(Emissions... e) {
  for (auto listener : listeners_) {
    listener(e...);
  }
}

template <typename... Emissions>
std::future<void> Event<Emissions...>::SendAsync(Emissions... e) {
  return std::async(std::launch::async, [this, e...](){ Send(e...); });
}
