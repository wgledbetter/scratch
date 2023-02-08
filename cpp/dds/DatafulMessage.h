#pragma once

#include <fastcdr/Cdr.h>
#include <fmt/format.h>

#include "settings.h"

struct DatafulMessage {
  using Cdr = eprosima::fastcdr::Cdr;

  // Static Values ===========================================================================================
  static constexpr const char* Name = "Sorta Like a TM Packet";

  // Constructors ============================================================================================
  DatafulMessage() {
    this->t = 0;
    this->x = 0;
    this->y = 0;
    this->z = 0;
    this->u = 0;
    this->v = 0;
    this->w = 0;
  }

  DatafulMessage(const DatafulMessage& other) {
    this->t = other.t;
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    this->u = other.u;
    this->v = other.v;
    this->w = other.w;
  }

  DatafulMessage(DatafulMessage&& other) noexcept {
    this->t = other.t;
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    this->u = other.u;
    this->v = other.v;
    this->w = other.w;
  }

  // Operators ===============================================================================================

  inline DatafulMessage& operator=(const DatafulMessage& other) {
    this->t = other.t;
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    this->u = other.u;
    this->v = other.v;
    this->w = other.w;

    return *this;
  }

  inline DatafulMessage& operator=(DatafulMessage&& other) noexcept {
    this->t = other.t;
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    this->u = other.u;
    this->v = other.v;
    this->w = other.w;

    return *this;
  }

  inline bool operator==(const DatafulMessage& other) const {
    return (this->t == other.t && this->x == other.x && this->y == other.y && this->z == other.z
            && this->u == other.u && this->v == other.v && this->w == other.w);
  }

  inline bool operator!=(const DatafulMessage& other) const {
    return !(*this == other);
  }

  // Printer =================================================================================================

  inline std::string toString() const {
    return fmt::format("Time: {}, Position: [{}, {}, {}], Velocity: [{}, {}, {}].",
                       this->t,
                       this->x,
                       this->y,
                       this->z,
                       this->u,
                       this->v,
                       this->w);
  }

  // Message Content Accessors ===============================================================================

  // Time ----------------------------------------------------------------------------------------------------

  inline void setStart(double t0t0) {
    this->t0 = t0t0;
  }

  inline double getStart() const {
    return this->t0;
  }

  inline void setTime(double tt) {
    this->t = tt;
  }

  inline double getTime() const {
    return this->t;
  }

  // Position ------------------------------------------------------------------------------------------------

  inline void setPosition(double xx, double yy, double zz) {
    this->x = xx;
    this->y = yy;
    this->z = zz;
  }

  inline void setX(double xx) {
    this->x = xx;
  }

  inline void setY(double yy) {
    this->y = yy;
  }

  inline void setZ(double zz) {
    this->z = zz;
  }

  inline double getX() const {
    return this->x;
  }

  inline double getY() const {
    return this->y;
  }

  inline double getZ() const {
    return this->z;
  }

  // Velocity ------------------------------------------------------------------------------------------------

  inline void setVelocity(double uu, double vv, double ww) {
    this->u = uu;
    this->v = vv;
    this->w = ww;
  }

  inline void setU(double uu) {
    this->u = uu;
  }

  inline void setV(double vv) {
    this->v = vv;
  }

  inline void setW(double ww) {
    this->w = ww;
  }

  inline double getU() const {
    return this->u;
  }

  inline double getV() const {
    return this->v;
  }

  inline double getW() const {
    return this->w;
  }

  // Serialization / Deserialization =========================================================================

  static size_t getMaxCdrSerializedSize(size_t currentAlignment = 0) {
    size_t initialAlignment = currentAlignment;

    // Ten doubles, each 8 bytes
    currentAlignment += 10 * 8 + Cdr::alignment(currentAlignment, 8);

    return currentAlignment - initialAlignment;
  }

  // ---------------------------------------------------------------------------------------------------------

  static size_t getCdrSerializedSize(const DatafulMessage& data, size_t currentAlignment = 0) {
    return getMaxCdrSerializedSize(currentAlignment);
  }

  // ---------------------------------------------------------------------------------------------------------

  inline void serialize(Cdr& cdr) const {
    cdr << this->t0;
    cdr << this->t;
    cdr << this->x;
    cdr << this->y;
    cdr << this->z;
    cdr << this->u;
    cdr << this->v;
    cdr << this->w;
  }

  // ---------------------------------------------------------------------------------------------------------

  inline void deserialize(Cdr& cdr) {
    cdr >> this->t0;
    cdr >> this->t;
    cdr >> this->x;
    cdr >> this->y;
    cdr >> this->z;
    cdr >> this->u;
    cdr >> this->v;
    cdr >> this->w;
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
  double t0, t, x, y, z, u, v, w;
};
