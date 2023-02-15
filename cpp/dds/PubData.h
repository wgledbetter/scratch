#pragma once

#include <cmath>

#include "Pub.h"
#include "common/utcTime.h"

template<class MessageClass>
struct PubData : Pub<MessageClass, DefaultPubListener> {
  using Base = Pub<MessageClass, DefaultPubListener>;

  // Run =====================================================================================================

  inline void run(int delayMilliseconds = 2) {
    // Data Generation Functions -----------------------------------------------------------------------------

    auto xFunc = [](double t) { return std::cos(0.1 * t) * std::sin(2 * t); };
    auto yFunc = [](double t) { return 1; };
    auto zFunc = [](double t) { return 0.5; };
    auto uFunc = [](double t) { return -0.25; };
    auto vFunc = [](double t) { return -1; };
    auto wFunc = [](double t) { return -0.7; };

    // Publish Function --------------------------------------------------------------------------------------

    auto publishFunc = [this, xFunc, yFunc, zFunc, uFunc, vFunc, wFunc](double t) {
      this->msg.setTime(t);
      this->msg.setX(xFunc(t));
      this->msg.setY(yFunc(t));
      this->msg.setZ(zFunc(t));
      this->msg.setU(uFunc(t));
      this->msg.setV(vFunc(t));
      this->msg.setW(wFunc(t));

      // fmt::print("Sending: {}.\n", this->msg.toString());

      this->writer->write(&this->msg);
    };

    // Publish -----------------------------------------------------------------------------------------------

    timeval tv;

    while (!this->stop) {
      publishFunc(utcTime());
      std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
    }
  }
};
