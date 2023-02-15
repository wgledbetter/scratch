#pragma once

#include <fmt/format.h>

void printing_callback(int code, const char* description) {
  fmt::print("GLFW had error code {}, which is {}.", code, description);
}
