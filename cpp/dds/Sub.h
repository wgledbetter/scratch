#pragma once

#include <fastdds/rtps/transport/TCPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/TCPv6TransportDescriptor.h>
#include <fastrtps/utils/IPLocator.h>
#include <fmt/format.h>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <string>
#include <vector>

#include "DDSTypes.h"
#include "settings.h"

template<class MessageClass>
struct Sub {
  using DataReader                = eprosima::fastdds::dds::DataReader;
  using DataReaderQos             = eprosima::fastdds::dds::DataReaderQos;
  using DataReaderListener        = eprosima::fastdds::dds::DataReaderListener;
  using Subscriber                = eprosima::fastdds::dds::Subscriber;
  using SubscriberQos             = eprosima::fastdds::dds::SubscriberQos;
  using SubscriptionMatchedStatus = eprosima::fastdds::dds::SubscriptionMatchedStatus;
  using DomainParticipant         = eprosima::fastdds::dds::DomainParticipant;
  using DomainParticipantFactory  = eprosima::fastdds::dds::DomainParticipantFactory;
  using DomainParticipantQos      = eprosima::fastdds::dds::DomainParticipantQos;
  using SampleInfo                = eprosima::fastdds::dds::SampleInfo;
  using Topic                     = eprosima::fastdds::dds::Topic;
  using TopicQos                  = eprosima::fastdds::dds::TopicQos;
  using TypeSupport               = eprosima::fastdds::dds::TypeSupport;
  using TCPv4TransportDescriptor  = eprosima::fastdds::rtps::TCPv4TransportDescriptor;
  using TCPv6TransportDescriptor  = eprosima::fastdds::rtps::TCPv6TransportDescriptor;

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
  }

  // Destructor ==============================================================================================

  ~Sub() {
    if (this->reader != nullptr) {
      this->subscriber->delete_datareader(this->reader);
    }
    if (this->topic != nullptr) {
      this->participant->delete_topic(this->topic);
    }
    if (this->subscriber != nullptr) {
      this->participant->delete_subscriber(this->subscriber);
    }
    DomainParticipantFactory::get_instance()->delete_participant(this->participant);

    fmt::print("Destroyed Sub.\n");
  }

  // Init Default ============================================================================================

  inline bool init() {
    DomainParticipantQos dpQos = eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT;
    dpQos.name("Sub");
    auto factory = DomainParticipantFactory::get_instance();

    this->participant = factory->create_participant(0, dpQos);
    if (this->participant == nullptr) {
      return false;
    }

    fmt::print("Registering type {} with the Sub participant.\n", this->type.get_type_name());
    this->type.register_type(this->participant);

    SubscriberQos subQos = eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT;
    this->subscriber     = this->participant->create_subscriber(subQos, nullptr);
    if (this->subscriber == nullptr) {
      return false;
    }

    TopicQos topicQos = eprosima::fastdds::dds::TOPIC_QOS_DEFAULT;
    this->topic       = this->participant->create_topic(MY_TOPIC_NAME, MY_MESSAGE_NAME, topicQos);
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

  // Init TCPv4 ==============================================================================================

  inline bool initTCPv4(const std::string&              ip,
                        unsigned short                  port,
                        const std::vector<std::string>& whitelist) {
    eprosima::fastdds::rtps::Locator initialPeerLocator;
    initialPeerLocator.kind = LOCATOR_KIND_TCPv4;
    initialPeerLocator.port = port;

    std::shared_ptr<TCPv4TransportDescriptor> descriptor = std::make_shared<TCPv4TransportDescriptor>();
    for (std::string ip: whitelist) {
      descriptor->interfaceWhiteList.push_back(ip);
      fmt::print("Publisher whitelisted IP {}.\n", ip);
    }
    if (!ip.empty()) {
      eprosima::fastrtps::rtps::IPLocator::setIPv4(initialPeerLocator, ip);
    } else {
      eprosima::fastrtps::rtps::IPLocator::setIPv4(initialPeerLocator, "127.0.0.1");
    }

    DomainParticipantQos dpQos;
    dpQos.wire_protocol().builtin.initialPeersList.push_back(initialPeerLocator);
    dpQos.wire_protocol().builtin.discovery_config.leaseDuration = eprosima::fastrtps::c_TimeInfinite;
    dpQos.wire_protocol().builtin.discovery_config.leaseDuration_announcementperiod =
        eprosima::fastrtps::Duration_t(5, 0);
    dpQos.name("SubTCPv4");
    dpQos.transport().use_builtin_transports = false;
    dpQos.transport().user_transports.push_back(descriptor);

    this->participant = DomainParticipantFactory::get_instance()->create_participant(0, dpQos);
    if (this->participant == nullptr) {
      return false;
    }

    this->type.register_type(this->participant);

    this->subscriber = this->participant->create_subscriber(eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT);
    if (this->subscriber == nullptr) {
      return false;
    }

    TopicQos topicQos = eprosima::fastdds::dds::TOPIC_QOS_DEFAULT;
    this->topic       = this->participant->create_topic(MY_TOPIC_NAME, MY_MESSAGE_NAME, topicQos);
    if (this->topic == nullptr) {
      return false;
    }

    DataReaderQos readerQos;
    readerQos.history().kind                      = HISTORY_KIND;
    readerQos.resource_limits().max_samples       = RESOURCE_MAX_SAMPLES;
    readerQos.resource_limits().allocated_samples = RESOURCE_ALLOCATED_SAMPLES;
    readerQos.reliability().kind                  = RELIABILITY_KIND;
    readerQos.durability().kind                   = DURABILITY_KIND;

    this->reader = this->subscriber->create_datareader(this->topic, readerQos, &(this->listener));
    if (this->reader == nullptr) {
      return false;
    }

    return true;
  }

  // Run =====================================================================================================

  inline void run() {
    while (true) {
      // If this loop is empty, it'll get optimized away and break things.
      std::this_thread::sleep_for(std::chrono::microseconds(1));
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
