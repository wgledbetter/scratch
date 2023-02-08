#pragma once

#include <fmt/format.h>

#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

#include "common/ThreadSafeQueue.h"

template<class MessageClass>
struct AccumulatingSubListener : public eprosima::fastdds::dds::DataReaderListener {
  using DataReader                = eprosima::fastdds::dds::DataReader;
  using DataReaderListener        = eprosima::fastdds::dds::DataReaderListener;
  using SampleInfo                = eprosima::fastdds::dds::SampleInfo;
  using SubscriptionMatchedStatus = eprosima::fastdds::dds::SubscriptionMatchedStatus;

  // Constructor =============================================================================================
  AccumulatingSubListener() {};

  // Destructor ==============================================================================================
  ~AccumulatingSubListener() override {};

  // On Data Available =======================================================================================
  inline void on_data_available(DataReader* reader) override {
    if (reader->take_next_sample(&this->msg, &this->info) == ReturnCode_t::RETCODE_OK) {
      if (this->info.instance_state == eprosima::fastdds::dds::ALIVE_INSTANCE_STATE) {
        this->queue.push(this->msg);
      }
    }
  }

  // On Subscription Matched =================================================================================
  inline void on_subscription_matched(DataReader* reader, const SubscriptionMatchedStatus& info) override {
    //
  }

  // Member Variables ========================================================================================
  ThreadSafeQueue<MessageClass> queue;

 protected:
  MessageClass msg;
  SampleInfo   info;
};
