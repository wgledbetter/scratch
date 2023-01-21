// NOTE: I'm not doing the windows-specific dllexport shit because I am an adult so I use linux.

#pragma once

#include <fastcdr/Cdr.h>
#include <fmt/format.h>

#include <fastrtps/utils/fixed_size_string.hpp>
#include <string>

#include "settings.h"

class MySuperCoolMessage {
 public:
  using Cdr = eprosima::fastcdr::Cdr;

  // Static Values ===========================================================================================
  static constexpr const char* Name = MY_MESSAGE_NAME;

  // Constructors ============================================================================================
  MySuperCoolMessage() {
    this->index   = 0;
    this->message = "";
  }

  MySuperCoolMessage(const MySuperCoolMessage& other) {
    this->index   = other.index;
    this->message = other.message;
  };

  MySuperCoolMessage(MySuperCoolMessage&& other) noexcept {
    this->index   = other.index;
    this->message = std::move(other.message);
  };

  // Destructor ==============================================================================================

  ~MySuperCoolMessage() {};

  // Operators ===============================================================================================

  inline MySuperCoolMessage& operator=(const MySuperCoolMessage& other) {
    this->index   = other.index;
    this->message = other.message;

    return *this;
  }

  inline MySuperCoolMessage& operator=(MySuperCoolMessage&& other) noexcept {
    this->index   = other.index;
    this->message = std::move(other.message);

    return *this;
  }

  inline bool operator==(const MySuperCoolMessage& other) const {
    return (this->index == other.index && this->message == other.message);
  }

  inline bool operator!=(const MySuperCoolMessage& other) const {
    return !(*this == other);
  }

  // Printer =================================================================================================

  inline std::string toString() const {
    return fmt::format("IDX: {}, Message: {}.", this->index, this->message);
  }

  // Message Content Accessors ===============================================================================

  // index ---------------------------------------------------------------------------------------------------

  inline void setIndex(uint32_t idx) {
    this->index = idx;
  }

  inline uint32_t getIndex() const {
    return this->index;
  }

  inline uint32_t& getIndex() {
    return this->index;
  }

  // message -------------------------------------------------------------------------------------------------

  inline void setMessage(const std::string& msg) {
    this->message = msg;
  }

  inline void setMessage(std::string&& msg) {
    this->message = std::move(msg);
  }

  inline const std::string& getMessage() const {
    return this->message;
  }

  inline std::string& getMessage() {
    return this->message;
  }

  // Serialization / Deserialization =========================================================================

  static size_t getMaxCdrSerializedSize(size_t currentAlignment = 0) {
    size_t initialAlignment = currentAlignment;

    // Assuming this is the uint32_t portion?
    currentAlignment += 4 + Cdr::alignment(currentAlignment, 4);

    // Assuming this is the std::string portion?
    currentAlignment += 4 + Cdr::alignment(currentAlignment, 4) + 255 + 1;

    return currentAlignment - initialAlignment;
  }

  // ---------------------------------------------------------------------------------------------------------

  static size_t getCdrSerializedSize(const MySuperCoolMessage& data, size_t currentAlignment = 0) {
    (void) data;
    size_t initialAlignment = currentAlignment;

    // Current size of "index"
    currentAlignment += 4 + Cdr::alignment(currentAlignment, 4);

    // Current size of "message"
    currentAlignment += 4 + Cdr::alignment(currentAlignment, 4) + data.getMessage().size() + 1;

    return currentAlignment - initialAlignment;
  }

  // ---------------------------------------------------------------------------------------------------------

  inline void serialize(Cdr& cdr) const {
    cdr << this->index;
    cdr << this->message.c_str();
  }

  // ---------------------------------------------------------------------------------------------------------

  inline void deserialize(Cdr& cdr) {
    cdr >> this->index;
    cdr >> this->message;
  }

  // ---------------------------------------------------------------------------------------------------------

  static size_t getKeyMaxCdrSerializedSize(size_t currentAlignment = 0) {
    return currentAlignment;
  }

  // ---------------------------------------------------------------------------------------------------------

  static bool isKeyDefined() {
    return false;
  }

  // ---------------------------------------------------------------------------------------------------------

  inline void serializeKey(Cdr& cdr) const {
    (void) cdr;
  }

 private:
  // Member Variables ========================================================================================
  uint32_t    index;
  std::string message;
};
