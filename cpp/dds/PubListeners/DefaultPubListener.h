#pragma once

#include <fmt/format.h>

#include <fastdds/dds/publisher/DataWriter.hpp>

template<class MessageClass>
struct DefaultPubListener : public eprosima::fastdds::dds::DataWriterListener {
  using DataWriter               = eprosima::fastdds::dds::DataWriter;
  using PublicationMatchedStatus = eprosima::fastdds::dds::PublicationMatchedStatus;

  // Constructor -------------------------------------------------------------------------------------------
  DefaultPubListener() : matched(0), firstConnected(false) {
    fmt::print("Constructed DefaultPubListener.\n");
  }

  // Destructor --------------------------------------------------------------------------------------------
  ~DefaultPubListener() override {
    fmt::print("Destroyed DefaultPubListener.\n");
  }

  // on_publication_matched --------------------------------------------------------------------------------
  inline void on_publication_matched(DataWriter* writer, const PublicationMatchedStatus& info) override {
    fmt::print("Entering DefaultPubListener::on_publication_matched.\n");
    if (info.current_count_change == 1) {
      this->matched        = info.total_count;
      this->firstConnected = true;
      fmt::print("Publisher matched inside DefaultPubListener::on_publication_matched.\n");
    } else if (info.current_count_change == -1) {
      this->matched = info.total_count;
      fmt::print("Publisher un-matched inside DefaultPubListener::on_publication_matched.\n");
    } else {
      fmt::print("PublicationMatchedStatus::current_count_change value of {} is invalid.\n",
                 info.current_count_change);
    }
  }

  // Member Variables --------------------------------------------------------------------------------------
  int  matched;
  bool firstConnected;
};
