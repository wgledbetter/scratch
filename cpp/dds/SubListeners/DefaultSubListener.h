#pragma once

#include <fmt/format.h>

#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

template<class MessageClass>
struct DefaultSubListener : public eprosima::fastdds::dds::DataReaderListener {
  using DataReader                = eprosima::fastdds::dds::DataReader;
  using DataReaderListener        = eprosima::fastdds::dds::DataReaderListener;
  using SampleInfo                = eprosima::fastdds::dds::SampleInfo;
  using SubscriptionMatchedStatus = eprosima::fastdds::dds::SubscriptionMatchedStatus;

  // Constructor -------------------------------------------------------------------------------------------
  DefaultSubListener() : matched(0), samples() {
    fmt::print("Constructed DefaultSubListener.\n");
  };

  // Destructor --------------------------------------------------------------------------------------------
  ~DefaultSubListener() override {
    fmt::print("Destroyed DefaultSubListener.\n");
  };

  // on_data_available -------------------------------------------------------------------------------------
  inline void on_data_available(DataReader* reader) override {
    fmt::print("Entering DefaultSubListener::on_data_available.\n");
    SampleInfo info;
    if (reader->take_next_sample(&msg, &info) == ReturnCode_t::RETCODE_OK) {
      if (info.instance_state == eprosima::fastdds::dds::ALIVE_INSTANCE_STATE) {
        this->samples++;
        fmt::print("I am in DefaultSubListener::on_data_available, and the 'sample' I just read is {}.\n",
                   this->msg.toString());
      }
    }
  }

  // on_subscription_matched -------------------------------------------------------------------------------
  inline void on_subscription_matched(DataReader* reader, const SubscriptionMatchedStatus& info) override {
    fmt::print("Entering DefaultSubListener::on_subscription_matched.\n");
    if (info.current_count_change == 1) {
      this->matched = info.total_count;
      fmt::print("Subscriber matched inside DefaultSubListener::on_subscription_matched.\n");
    } else if (info.current_count_change == -1) {
      this->matched = info.total_count;
      fmt::print("Subscriber un-matched inside DefaultSubListener::on_subscription_matched.\n");
    } else {
      fmt::print("SubscriptionMatchedStatus::current_count_change value of {} is invalid.\n",
                 info.current_count_change);
    }
  }

  // Member Variables --------------------------------------------------------------------------------------
  int          matched;
  uint32_t     samples;
  MessageClass msg;
};
