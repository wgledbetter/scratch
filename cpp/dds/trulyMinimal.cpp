// The class-based message organization and the splitting of publisher and subscriber classes is 100% the
// correct way to do it, but I want the absolute bare minimum all in one file so I can see what's going on.

#include <fmt/format.h>

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

#include "DDSTypes.h"
#include "MySuperCoolMessage.h"

using namespace eprosima::fastdds::dds;

// Required Listener Classes /////////////////////////////////////////////////////////////////////////////////

template<class MessageClass>
struct SubListener : public DataReaderListener {
  inline void on_data_available(DataReader* reader) override {
    SampleInfo   info;
    ReturnCode_t getMsgSuccess = reader->take_next_sample(&this->msg, &info);
    if (getMsgSuccess == ReturnCode_t::RETCODE_OK) {
      fmt::print("SubListener::on_data_available got message: {}.\n", this->msg.toString());
    }
  }

  inline void on_subscription_matched(DataReader* reader, const SubscriptionMatchedStatus& info) override {
    fmt::print("Subscription Matched.\n");
  }

  // Member Variables ========================================================================================
  MessageClass msg;
};

// The purpose of this class is to detect whether a subscriber is available. The availability of subscribers
// is used as a trigger to send messages.
struct PubListener : public DataWriterListener {
  inline void on_publication_matched(DataWriter* writer, const PublicationMatchedStatus& info) override {
    fmt::print("Entering PubListener::on_publication_matched.\n");
    if (info.current_count_change == 1) {
      this->matched           = info.total_count;
      this->firstHasConnected = true;
      fmt::print("Publisher matched inside PubListener::on_publication_matched.\n");
    } else if (info.current_count_change == -1) {
      this->matched = info.total_count;
      fmt::print("Publisher un-matched inside PubListener::on_publication_matched.\n");
    } else {
      fmt::print("PublicationMatchedStatus::current_count_change value of {} is invalid.\n",
                 info.current_count_change);
    }
  }

  // Member Variables ----------------------------------------------------------------------------------------
  int  matched           = 0;
  bool firstHasConnected = false;
};

// Main //////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
  const char* TopicName   = "TopicalConversation";
  const char* MessageName = "DefinitelyNotCoolMessage";

  // Common Setup ============================================================================================
  DDSTypes<MySuperCoolMessage> myType(MessageName);
  TypeSupport                  type(&myType);
  TopicQos                     topicQos = TOPIC_QOS_DEFAULT;

  auto factory = DomainParticipantFactory::get_instance();

  // Publisher Setup =========================================================================================
  DomainParticipantQos pubPartQos = PARTICIPANT_QOS_DEFAULT;
  pubPartQos.name("Pub");
  DomainParticipant* pubParticipant = factory->create_participant(0, pubPartQos);
  type.register_type(pubParticipant);
  PublisherQos  pubQos    = PUBLISHER_QOS_DEFAULT;
  Publisher*    publisher = pubParticipant->create_publisher(pubQos, nullptr);
  Topic*        pubTopic  = pubParticipant->create_topic(TopicName, MessageName, topicQos);
  DataWriterQos writerQos = DATAWRITER_QOS_DEFAULT;
  PubListener   pubListener;
  DataWriter*   writer = publisher->create_datawriter(pubTopic, writerQos, &pubListener);

  // Subscriber Setup ========================================================================================
  DomainParticipantQos subPartQos = PARTICIPANT_QOS_DEFAULT;
  subPartQos.name("Sub");
  DomainParticipant* subParticipant = factory->create_participant(0, subPartQos);
  type.register_type(subParticipant);
  SubscriberQos subQos         = SUBSCRIBER_QOS_DEFAULT;
  Subscriber*   subscriber     = subParticipant->create_subscriber(subQos, nullptr);
  Topic*        subTopic       = subParticipant->create_topic(TopicName, MessageName, topicQos);
  DataReaderQos readerQos      = DATAREADER_QOS_DEFAULT;
  readerQos.reliability().kind = RELIABLE_RELIABILITY_QOS;
  SubListener<MySuperCoolMessage> subListener;
  DataReader*                     reader = subscriber->create_datareader(subTopic, readerQos, &subListener);

  // Send Message ============================================================================================
  MySuperCoolMessage msg;
  msg.setIndex(69);
  msg.setMessage("Hey there.");
  bool sent = false;
  while (true) {
    if (pubListener.firstHasConnected || pubListener.matched > 0) {
      fmt::print("Got good conditions to send.\n");
      writer->write(&msg);
      fmt::print("Sent message.\n");
      break;
    }
    fmt::print("Stalling...\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  // Cleanup =================================================================================================
  if (reader != nullptr) {
    subscriber->delete_datareader(reader);
  }
  if (writer != nullptr) {
    publisher->delete_datawriter(writer);
  }

  if (subTopic != nullptr) {
    subParticipant->delete_topic(subTopic);
  }
  if (pubTopic != nullptr) {
    pubParticipant->delete_topic(pubTopic);
  }

  if (subscriber != nullptr) {
    subParticipant->delete_subscriber(subscriber);
  }
  if (publisher != nullptr) {
    pubParticipant->delete_publisher(publisher);
  }

  factory->delete_participant(subParticipant);
  factory->delete_participant(pubParticipant);

  return 0;
}
