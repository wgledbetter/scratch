// Idea stolen from https://www.youtube.com/watch?v=6pXhQ28FVlU
// NOTE: Only got this to work in C++20

#pragma once

#include <fmt/format.h>
#include <netdb.h>

#include <array>
#include <stdexcept>
#include <string>

#include "common/stoi.h"

// IPv4Addr //////////////////////////////////////////////////////////////////////////////////////////////////

struct IPv4Addr {

  // Custom Quote Operator ===================================================================================
  inline friend constexpr IPv4Addr operator""_ip4(const char* text, unsigned long length);

  // Conversions =============================================================================================
  inline operator addrinfo() const;

  // Printing ================================================================================================
  inline std::string toString() const {
    return fmt::format("{}.{}.{}.{}", this->addr[0], this->addr[1], this->addr[2], this->addr[3]);
  }

 private:
  std::array<uint8_t, 4> addr;
};

inline constexpr IPv4Addr operator""_ip4(const char* text, unsigned long length) {
  // I apologize...

  IPv4Addr out;

  int      filled   = 0;
  int      lastDot  = 0;
  int      subIndex = 0;
  long int tempAddrPortion;
  char*    strtolEnd;
  char     addrPortion[4];
  addrPortion[0] = '\0';
  addrPortion[1] = '\0';
  addrPortion[2] = '\0';
  addrPortion[3] = '\0';  // Gurantee null termination
  for (int i = 0; i < length; i++) {
    if (filled > 3) {
      throw "compile-time-error: You passed too many digits.";
    } else if (text[i] == '.') {
      if (subIndex == 0) {
        throw "compile-time-error: You didn't pass any digits between dots.";
      } else {
        tempAddrPortion = stoi(&addrPortion[0]);
        if (tempAddrPortion < 256) {
          out.addr[filled] = tempAddrPortion;
          filled++;
          subIndex       = 0;
          addrPortion[0] = '\0';
          addrPortion[1] = '\0';
          addrPortion[2] = '\0';
        } else {
          throw "compile-time-error: The address portion is too large for an 8-bit integer.";
        }
      }
    } else {
      if (subIndex < 3) {
        addrPortion[subIndex] = text[i];
        subIndex++;
      } else {
        throw "compile-time-error: You passed too many digits between dots.";
      }
    }
  }

  if (subIndex == 0) {
    throw "compile-time-error: You didn't pass any digits between dots.";
  } else {
    tempAddrPortion = stoi(&addrPortion[0]);
    if (tempAddrPortion < 256) {
      out.addr[filled] = tempAddrPortion;
      filled++;
      subIndex       = 0;
      addrPortion[0] = '\0';
      addrPortion[1] = '\0';
      addrPortion[2] = '\0';
    } else {
      throw "compile-time-error: The address portion is too large for an 8-bit integer.";
    }
  }

  return out;
}
