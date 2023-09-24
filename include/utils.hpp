#pragma once

#include <memory>

template <typename T>
class DefaultCtor {
 public:
  T operator()() const { return T{}; }
};

template <typename T, typename CTOR = DefaultCtor<T>>
class Singleton {
 public:
  static T& instance() {
    static T instance = CTOR{}();
    return instance;
  }
};
