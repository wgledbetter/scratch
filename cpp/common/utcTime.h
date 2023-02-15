#pragma once

#include <sys/time.h>

inline double utcTime() {
  timeval tv;
  gettimeofday(&tv, nullptr);

  return tv.tv_sec + (tv.tv_usec / 1e6);
}
