#pragma once

#define MY_MESSAGE_NAME "DoesntHaveToMatchTheObjectName"
#define MY_TOPIC_NAME   "AReallyInterestingTopic"

#define HISTORY_KIND     eprosima::fastdds::dds::KEEP_ALL_HISTORY_QOS
#define RELIABILITY_KIND eprosima::fastdds::dds::RELIABLE_RELIABILITY_QOS
#define DURABILITY_KIND  eprosima::fastdds::dds::TRANSIENT_LOCAL_DURABILITY_QOS

#define WRITER_HEARTBEAT_SECONDS     1
#define WRITER_HEARTBEAT_NANOSECONDS 200000000

enum class DDSExampleModes {
  Default,
  TCPv4,
  TCPv6,
  UDPv4,
  UDPv6
};
