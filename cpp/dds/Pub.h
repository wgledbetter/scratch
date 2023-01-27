#pragma once

#include <fastdds/rtps/transport/TCPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/TCPv6TransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/UDPv6TransportDescriptor.h>
#include <fastrtps/utils/IPLocator.h>
#include <fmt/format.h>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <magic_enum.hpp>
#include <string>
#include <vector>

#include "DDSTypes.h"
#include "settings.h"

template<class MessageClass>
struct Pub {
  using DataWriter               = eprosima::fastdds::dds::DataWriter;
  using DataWriterQos            = eprosima::fastdds::dds::DataWriterQos;
  using DataWriterListener       = eprosima::fastdds::dds::DataWriterListener;
  using Publisher                = eprosima::fastdds::dds::Publisher;
  using PublisherQos             = eprosima::fastdds::dds::PublisherQos;
  using PublicationMatchedStatus = eprosima::fastdds::dds::PublicationMatchedStatus;
  using DomainParticipant        = eprosima::fastdds::dds::DomainParticipant;
  using DomainParticipantQos     = eprosima::fastdds::dds::DomainParticipantQos;
  using DomainParticipantFactory = eprosima::fastdds::dds::DomainParticipantFactory;
  using Topic                    = eprosima::fastdds::dds::Topic;
  using TopicQos                 = eprosima::fastdds::dds::TopicQos;
  using TypeSupport              = eprosima::fastdds::dds::TypeSupport;
  using TCPv4TransportDescriptor = eprosima::fastdds::rtps::TCPv4TransportDescriptor;
  using TCPv6TransportDescriptor = eprosima::fastdds::rtps::TCPv6TransportDescriptor;
  using UDPv4TransportDescriptor = eprosima::fastdds::rtps::UDPv4TransportDescriptor;
  using UDPv6TransportDescriptor = eprosima::fastdds::rtps::UDPv6TransportDescriptor;

  // Listener ================================================================================================

  struct Listener : public DataWriterListener {
    // Constructor -------------------------------------------------------------------------------------------
    Listener() : matched(0), firstConnected(false) {
      fmt::print("Constructed Pub::Listener.\n");
    }

    // Destructor --------------------------------------------------------------------------------------------
    ~Listener() override {
      fmt::print("Destroyed Pub::Listener.\n");
    }

    // on_publication_matched --------------------------------------------------------------------------------
    inline void on_publication_matched(DataWriter* writer, const PublicationMatchedStatus& info) override {
      fmt::print("Entering Pub::Listener::on_publication_matched.\n");
      if (info.current_count_change == 1) {
        this->matched        = info.total_count;
        this->firstConnected = true;
        fmt::print("Publisher matched inside Pub::Listener::on_publication_matched.\n");
      } else if (info.current_count_change == -1) {
        this->matched = info.total_count;
        fmt::print("Publisher un-matched inside Pub::Listener::on_publication_matched.\n");
      } else {
        fmt::print("PublicationMatchedStatus::current_count_change value of {} is invalid.\n",
                   info.current_count_change);
      }
    }

    // Member Variables --------------------------------------------------------------------------------------
    int  matched;
    bool firstConnected;
  };

  // Constructor =============================================================================================

  Pub()
      : participant(nullptr),
        publisher(nullptr),
        topic(nullptr),
        writer(nullptr),
        type(new DDSTypes<MessageClass>()) {
    fmt::print("Constructed Pub.\n");
  }

  // Destructor ==============================================================================================

  ~Pub() {
    fmt::print("Destroyed Pub.\n");
  }

  // Utility Functions =======================================================================================

  template<class TpDesc>
  void setupDescriptor(DomainParticipantQos&           dpQos,
                       const std::string&              ip,
                       unsigned short                  port,
                       const std::vector<std::string>& whitelist) const {
    std::shared_ptr<TpDesc> descriptor = std::make_shared<TpDesc>();
    for (std::string wlip: whitelist) {
      descriptor->interfaceWhiteList.push_back(wlip);
      fmt::print("Publisher is allowed to talk on IP {}.\n", wlip);
    }
    descriptor->sendBufferSize    = 0;
    descriptor->receiveBufferSize = 0;
    if constexpr (std::is_same<TpDesc, TCPv4TransportDescriptor>::value) {
      if (!ip.empty()) {
        descriptor->set_WAN_address(ip);
      }
    }
    if constexpr (std::is_same<TpDesc, TCPv4TransportDescriptor>::value
                  || std::is_same<TpDesc, TCPv6TransportDescriptor>::value) {
      descriptor->add_listener_port(port);
    }
    dpQos.transport().user_transports.push_back(descriptor);
  }

  // Init Default ============================================================================================

  inline bool init() {
    this->msg.setIndex(0);
    this->msg.setMessage("This message is from Pub::init.\n");

    DomainParticipantQos dpQos = eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT;
    dpQos.name("Pub");
    auto factory = DomainParticipantFactory::get_instance();

    this->participant = factory->create_participant(0, dpQos);
    if (this->participant == nullptr) {
      return false;
    }

    fmt::print("Registering type {} with the Pub participant.\n", this->type.get_type_name());
    // This function is sort of inverted because it calls the participant
    this->type.register_type(this->participant);

    PublisherQos pubQos = eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT;
    this->publisher     = this->participant->create_publisher(pubQos, nullptr);
    if (this->publisher == nullptr) {
      return false;
    }

    TopicQos topicQos = eprosima::fastdds::dds::TOPIC_QOS_DEFAULT;
    this->topic       = this->participant->create_topic(MY_TOPIC_NAME, MY_MESSAGE_NAME, topicQos);
    if (this->topic == nullptr) {
      return false;
    }

    DataWriterQos writerQos = eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT;
    this->writer            = this->publisher->create_datawriter(this->topic, writerQos, &(this->listener));
    if (this->writer == nullptr) {
      return false;
    }

    return true;
  }

  // Init Network ============================================================================================

  inline bool initNetwork(const DDSExampleModes           mode,
                          const std::string&              ip,
                          unsigned short                  port,
                          const std::vector<std::string>& whitelist,
                          const int                       historyLength) {
    // Set Default Message -----------------------------------------------------------------------------------
    this->msg.setIndex(0);
    this->msg.setMessage("This message is from Pub::initNetwork.\n");

    // Setup DPQoS -------------------------------------------------------------------------------------------

    DomainParticipantQos dpQos;
    dpQos.wire_protocol().builtin.discovery_config.leaseDuration = eprosima::fastrtps::c_TimeInfinite;
    dpQos.wire_protocol().builtin.discovery_config.leaseDuration_announcementperiod =
        eprosima::fastrtps::Duration_t(5, 0);
    dpQos.name(fmt::format("Pub{}", magic_enum::enum_name(mode)));
    dpQos.transport().use_builtin_transports = false;

    if (mode == DDSExampleModes::TCPv4) {
      this->setupDescriptor<TCPv4TransportDescriptor>(dpQos, ip, port, whitelist);
    } else if (mode == DDSExampleModes::TCPv6) {
      this->setupDescriptor<TCPv6TransportDescriptor>(dpQos, ip, port, whitelist);
    } else if (mode == DDSExampleModes::UDPv4) {
      this->setupDescriptor<UDPv4TransportDescriptor>(dpQos, ip, port, whitelist);
    } else if (mode == DDSExampleModes::UDPv6) {
      this->setupDescriptor<UDPv6TransportDescriptor>(dpQos, ip, port, whitelist);
    } else {
      return false;
    }

    // Remaining Common Setup --------------------------------------------------------------------------------

    this->participant = DomainParticipantFactory::get_instance()->create_participant(0, dpQos);
    if (this->participant == nullptr) {
      return false;
    }

    this->type.register_type(this->participant);

    this->publisher = this->participant->create_publisher(eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT);
    if (this->publisher == nullptr) {
      return false;
    }

    TopicQos topicQos = eprosima::fastdds::dds::TOPIC_QOS_DEFAULT;
    this->topic       = this->participant->create_topic(MY_TOPIC_NAME, MY_MESSAGE_NAME, topicQos);
    if (this->topic == nullptr) {
      return false;
    }

    DataWriterQos writerQos;
    writerQos.history().kind                                      = HISTORY_KIND;
    writerQos.resource_limits().max_samples                       = historyLength;
    writerQos.resource_limits().allocated_samples                 = historyLength;
    writerQos.reliable_writer_qos().times.heartbeatPeriod.seconds = WRITER_HEARTBEAT_SECONDS;
    writerQos.reliable_writer_qos().times.heartbeatPeriod.nanosec = WRITER_HEARTBEAT_NANOSECONDS;
    writerQos.reliability().kind                                  = RELIABILITY_KIND;

    this->writer = this->publisher->create_datawriter(this->topic, writerQos, &(this->listener));
    if (this->writer == nullptr) {
      return false;
    }

    return true;
  }

  // Run =====================================================================================================

  inline void run(
      int         nMessages         = 10,
      int         delayMilliseconds = 1,
      std::string msgContent = "This is a default message defined as a default argument to Pub::run.") {
    fmt::print("Entering Pub::run with nMessages = {}, delayMilliseconds = {}, and msgContent = {}.\n",
               nMessages,
               delayMilliseconds,
               msgContent);

    bool pubSuccess;

    // Lambda publish function -------------------------------------------------------------------------------

    auto publishFunc = [this, &msgContent](bool wait) {
      fmt::print("Trying to construct and send message.\n");
      if (this->listener.firstConnected || !wait || this->listener.matched > 0) {
        this->msg.setIndex(this->msg.getIndex() + 1);
        this->msg.setMessage(msgContent);

        this->writer->write(&this->msg);
        return true;
      }
      return false;
    };

    // Execute -----------------------------------------------------------------------------------------------

    if (nMessages == 0) {
      fmt::print("User requested to send 0 messages, but I will send INFINITE messages. HAHAHAHAHA.\n");
      while (!this->stop) {
        pubSuccess = publishFunc(false);
        if (pubSuccess) {
          fmt::print("Publishing succeeded. Message should be: {}.\n", this->msg.toString());
        } else {
          fmt::print("Publishing failed, but I'm sending infinite messages so it's not a big deal.");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
      }
    } else {
      for (int i = 0; i < nMessages; ++i) {
        pubSuccess = publishFunc(false);
        if (!pubSuccess) {
          fmt::print("Publishing failed, so this message doesn't count. I'll try again.\n");
          --i;
        } else {
          fmt::print("Publishing succeeded. Message should be: {}.\n", this->msg.toString());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
      }
    }
  }

 protected:
  // Member Variables ========================================================================================
  DomainParticipant* participant;
  Publisher*         publisher;
  Topic*             topic;
  DataWriter*        writer;
  TypeSupport        type;

  bool stop;

  Listener listener;

  MessageClass msg;
};
