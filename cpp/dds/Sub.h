#pragma once

#include <fmt/format.h>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include "DDSTypes.h"
#include "settings.h"

template<class MessageClass>
struct Sub {
  using DataReader                = eprosima::fastdds::dds::DataReader;
  using DataReaderQos             = eprosima::fastdds::dds::DataReaderQos;
  using DataReaderListener        = eprosima::fastdds::dds::DataReaderListener;
  using SampleInfo                = eprosima::fastdds::dds::SampleInfo;
  using SubscriptionMatchedStatus = eprosima::fastdds::dds::SubscriptionMatchedStatus;
  using DomainParticipant         = eprosima::fastdds::dds::DomainParticipant;
  using DomainParticipantFactory  = eprosima::fastdds::dds::DomainParticipantFactory;
  using DomainParticipantQos      = eprosima::fastdds::dds::DomainParticipantQos;
  using Subscriber                = eprosima::fastdds::dds::Subscriber;
  using SubscriberQos             = eprosima::fastdds::dds::SubscriberQos;
  using Topic                     = eprosima::fastdds::dds::Topic;
  using TopicQos                  = eprosima::fastdds::dds::TopicQos;
  using TypeSupport               = eprosima::fastdds::dds::TypeSupport;

  // Listener ================================================================================================

  struct Listener : public DataReaderListener {
    // Constructor -------------------------------------------------------------------------------------------
    Listener() : matched(0), samples() {
      fmt::print("Constructed Sub::Listener.\n");
    };

    // Destructor --------------------------------------------------------------------------------------------
    ~Listener() override {
      fmt::print("Destroyed Sub::Listener.\n");
    };

    // on_data_available -------------------------------------------------------------------------------------
    inline void on_data_available(DataReader* reader) override {
      fmt::print("Entering Sub::Listener::on_data_available.\n");
      SampleInfo info;
      if (reader->take_next_sample(&msg, &info) == ReturnCode_t::RETCODE_OK) {
        if (info.instance_state == eprosima::fastdds::dds::ALIVE_INSTANCE_STATE) {
          this->samples++;
          fmt::print("I am in Sub::Listener::on_data_available, and the 'sample' I just read is {}.\n",
                     this->msg.toString());
        }
      }
    }

    // on_subscription_matched -------------------------------------------------------------------------------
    inline void on_subscription_matched(DataReader* reader, const SubscriptionMatchedStatus& info) override {
      fmt::print("Entering Sub::Listener::on_subscription_matched.\n");
      if (info.current_count_change == 1) {
        this->matched = info.total_count;
        fmt::print("Subscriber matched inside Sub::Listener::on_subscription_matched.\n");
      } else if (info.current_count_change == -1) {
        this->matched = info.total_count;
        fmt::print("Subscriber un-matched inside Sub::Listener::on_subscription_matched.\n");
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

  // Constructor =============================================================================================

  Sub()
      : participant(nullptr),
        subscriber(nullptr),
        topic(nullptr),
        reader(nullptr),
        type(new DDSTypes<MessageClass>()) {
    fmt::print("Constructed Sub.\n");
  };

  // Destructor ==============================================================================================

  ~Sub() {
    fmt::print("Destroyed Sub.\n");
  }

  // Init ====================================================================================================

  inline bool init() {
    DomainParticipantQos dpQos = eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT;
    dpQos.name("Sub");
    auto factory = DomainParticipantFactory::get_instance();

    this->participant = factory->create_participant(0, dpQos);
    if (this->participant == nullptr) {
      return false;
    }

    fmt::print("Registering type {} with the participant.\n", this->type.get_type_name());
    this->type.register_type(this->participant);

    SubscriberQos subQos = eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT;
    this->subscriber     = this->participant->create_subscriber(subQos, nullptr);
    if (this->subscriber == nullptr) {
      return false;
    }

    TopicQos topicQos = eprosima::fastdds::dds::TOPIC_QOS_DEFAULT;
    this->topic = this->participant->create_topic("MyReallyInterestingTopic", MY_MESSAGE_NAME, topicQos);
    if (this->topic == nullptr) {
      return false;
    }

    DataReaderQos readerQos      = eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT;
    readerQos.reliability().kind = eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS;

    this->reader = this->subscriber->create_datareader(this->topic, readerQos, &(this->listener));
    if (this->reader == nullptr) {
      return false;
    }

    return true;
  }

  // Run =====================================================================================================

  inline void run() {
    while (true) {
      //
    }
  }

 protected:
  // Member Variables ========================================================================================
  DomainParticipant* participant;
  Subscriber*        subscriber;
  Topic*             topic;
  DataReader*        reader;
  TypeSupport        type;

  Listener listener;
};
