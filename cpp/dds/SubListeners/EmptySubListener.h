#pragma once

#include <fmt/format.h>

#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

template<class MessageClass>
struct EmptySubListener : public eprosima::fastdds::dds::DataReaderListener {
  using DataReader                = eprosima::fastdds::dds::DataReader;
  using DataReaderListener        = eprosima::fastdds::dds::DataReaderListener;
  using SampleInfo                = eprosima::fastdds::dds::SampleInfo;
  using SubscriptionMatchedStatus = eprosima::fastdds::dds::SubscriptionMatchedStatus;

  // Constructor =============================================================================================
  EmptySubListener() {};

  // Destructor ==============================================================================================
  ~EmptySubListener() override {};

  // On Data Available =======================================================================================
  inline void on_data_available(DataReader* reader) override {
    //
  }

  // On Subscription Matched =================================================================================
  inline void on_subscription_matched(DataReader* reader, const SubscriptionMatchedStatus& info) override {
    //
  }
};
