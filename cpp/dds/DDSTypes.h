#pragma once

#include <fastcdr/Cdr.h>
#include <fastcdr/FastBuffer.h>

#include "settings.h"

template<class MessageClass>
class DDSTypes : public eprosima::fastdds::dds::TopicDataType {
 public:
  using SerializedPayload_t      = eprosima::fastrtps::rtps::SerializedPayload_t;
  using FastBuffer               = eprosima::fastcdr::FastBuffer;
  using Cdr                      = eprosima::fastcdr::Cdr;
  using NotEnoughMemoryException = eprosima::fastcdr::exception::NotEnoughMemoryException;
  using InstanceHandle_t         = eprosima::fastrtps::rtps::InstanceHandle_t;

  // Constructor =============================================================================================

  DDSTypes() {
    this->setName(MY_MESSAGE_NAME);
    auto type_size = MessageClass::getMaxCdrSerializedSize();
    type_size += eprosima::fastcdr::Cdr::alignment(type_size, 4);
    this->m_typeSize = static_cast<uint32_t>(type_size) + 4;

    this->m_isGetKeyDefined = MessageClass::isKeyDefined();

    size_t keyLength =
        MessageClass::getKeyMaxCdrSerializedSize() > 16 ? MessageClass::getKeyMaxCdrSerializedSize() : 16;
    this->m_keyBuffer = reinterpret_cast<unsigned char*>(malloc(keyLength));
    memset(this->m_keyBuffer, 0, keyLength);
  };

  // Destructor ==============================================================================================

  virtual ~DDSTypes() {
    if (this->m_keyBuffer != nullptr) {
      free(this->m_keyBuffer);
    }
  };

  // Serialize ===============================================================================================

  virtual bool serialize(void* data, SerializedPayload_t* payload) override {
    MessageClass* msgPtr = static_cast<MessageClass*>(data);

    FastBuffer fastBuffer(reinterpret_cast<char*>(payload->data), payload->max_size);
    Cdr serializer(fastBuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
    payload->encapsulation =
        serializer.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;
    serializer.serialize_encapsulation();

    try {
      msgPtr->serialize(serializer);
    } catch (NotEnoughMemoryException&) {
      return false;
    }

    payload->length = static_cast<uint32_t>(serializer.getSerializedDataLength());
    return true;
  }

  // DeSerialize =============================================================================================

  virtual bool deserialize(SerializedPayload_t* payload, void* data) override {
    try {
      MessageClass* msgPtr = static_cast<MessageClass*>(data);

      FastBuffer fastBuffer(reinterpret_cast<char*>(payload->data), payload->length);
      Cdr deserializer(fastBuffer, eprosima::fastcdr::Cdr::DEFAULT_ENDIAN, eprosima::fastcdr::Cdr::DDS_CDR);
      deserializer.read_encapsulation();
      payload->encapsulation =
          deserializer.endianness() == eprosima::fastcdr::Cdr::BIG_ENDIANNESS ? CDR_BE : CDR_LE;

      msgPtr->deserialize(deserializer);
    } catch (NotEnoughMemoryException&) {
      return false;
    }

    return true;
  }

  // GetSerializedSizeProvider ===============================================================================

  virtual std::function<uint32_t()> getSerializedSizeProvider(void* data) override {
    return [data]() {
      return static_cast<uint32_t>(MessageClass::getCdrSerializedSize(*static_cast<MessageClass*>(data)))
             + 4u;
    };
  }

  // GetKey ==================================================================================================

  virtual bool getKey(void* data, InstanceHandle_t* iHandle, bool force_md5 = false) override {
    if (!this->m_isGetKeyDefined) {
      return false;
    }

    MessageClass* msgPtr = static_cast<MessageClass*>(data);

    FastBuffer fastBuffer(reinterpret_cast<char*>(this->m_keyBuffer),
                          MessageClass::getKeyMaxCdrSerializedSize());
    Cdr        serializer(fastBuffer, eprosima::fastcdr::Cdr::BIG_ENDIANNESS);

    msgPtr->serializeKey(serializer);

    if (force_md5 || MessageClass::getKeyMaxCdrSerializedSize() > 16) {
      this->m_md5.init();
      this->m_md5.update(this->m_keyBuffer, static_cast<unsigned int>(serializer.getSerializedDataLength()));
      this->m_md5.finalize();
      for (uint8_t i = 0; i < 16; ++i) {
        iHandle->value[i] = this->m_md5.digest[i];
      }
    } else {
      for (uint8_t i = 0; i < 16; ++i) {
        iHandle->value[i] = this->m_keyBuffer[i];
      }
    }

    return true;
  }

  // CreateData ==============================================================================================

  virtual void* createData() override {
    // Create a new message
    return reinterpret_cast<void*>(new MessageClass());
  }

  // DeleteData ==============================================================================================

  virtual void deleteData(void* data) override {
    delete (reinterpret_cast<MessageClass*>(data));
  }

  // Conditional Functions ===================================================================================

#ifdef TOPIC_DATA_TYPE_API_HAS_IS_BOUNDED
  inline bool is_bounded() const override {
    return false;
  }
#endif

#ifdef TOPIC_DATA_TYPE_API_HAS_IS_PLAIN
  inline bool is_plain() const override {
    return false;
  }
#endif

#ifdef TOPIC_DATA_TYPE_API_HAS_CONSTRUCT_SAMPLE
  inline bool construct_sample(void* memory) const override {
    (void) memory;
    return false;
  }
#endif

  // Member Variables ========================================================================================
  MD5            m_md5;
  unsigned char* m_keyBuffer;
};
