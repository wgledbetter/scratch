// https://stackoverflow.com/a/25195711/10386618

#pragma once

inline constexpr bool is_digit(char c) {
  return c <= '9' && c >= '0';
}

inline constexpr int stoi_impl(const char* str, int value = 0) {
  return *str ? is_digit(*str) ? stoi_impl(str + 1, (*str - '0') + value * 10)
                               : throw "compile-time-error: not a digit"
              : value;
}

inline constexpr int stoi(const char* str) {
  return stoi_impl(str);
}
