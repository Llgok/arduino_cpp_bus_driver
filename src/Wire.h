/*
 * @Description: Wire
 * @Author: LILYGO_L
 * @Date: 2025-08-05 11:23:28
 * @LastEditTime: 2026-04-29 15:31:37
 * @License: GPL 3.0
 */
#pragma once

#include "arduino_cpp_bus_driver.h"

// WIRE_HAS_BUFFER_SIZE means Wire has setBufferSize()
// #define WIRE_HAS_BUFFER_SIZE 1
// // WIRE_HAS_END means Wire has end()
// #define WIRE_HAS_END 1

// #ifndef I2C_BUFFER_LENGTH
// #define I2C_BUFFER_LENGTH 128 // Default size, if none is set using
// Wire::setBuffersize(size_t) #endif typedef void (*user_onRequest)(void);
// typedef void (*user_onReceive)(uint8_t *, int);

class TwoWire {
 public:
  TwoWire(uint8_t bus_num) : num_(bus_num) {}

  // call setPins() first, so that begin() can be called without arguments from
  // libraries
  bool setPins(int sda, int scl);

  void set_freq(uint32_t freq_hz);
  bool set_bus_handle(i2c_master_bus_handle_t bus_handle);

  bool begin(int sda, int scl,
      uint32_t frequency = -1);  // returns true, if successful init of i2c bus
  // bool begin(uint8_t slaveAddr, int sda, int scl, uint32_t frequency);
  // Explicit Overload for Arduino MainStream API compatibility
  inline bool begin() { return begin(sda_, scl_, freq_hz_); }
  // inline bool begin(uint8_t addr)
  // {
  //     return begin(addr, -1, -1, -1);
  // }
  // inline bool begin(int addr)
  // {
  //     return begin(static_cast<uint8_t>(addr), -1, -1, 0);
  // }
  bool end() { return false; }
  bool end(bool delete_bus);

  // size_t setBufferSize(size_t bSize);

  // void setTimeOut(uint16_t timeOutMillis); // default timeout of i2c
  // transactions is 50ms uint16_t getTimeOut();

  // bool setClock(uint32_t);
  // uint32_t getClock();

  void beginTransmission(uint16_t address);
  void beginTransmission(uint8_t address);
  void beginTransmission(int address);

  uint8_t endTransmission(bool sendStop);
  uint8_t endTransmission(void);

  size_t requestFrom(uint16_t address, size_t size, bool sendStop);
  uint8_t requestFrom(uint16_t address, uint8_t size, bool sendStop);
  uint8_t requestFrom(uint16_t address, uint8_t size, uint8_t sendStop);
  size_t requestFrom(uint8_t address, size_t len, bool stopBit);
  uint8_t requestFrom(uint16_t address, uint8_t size);
  uint8_t requestFrom(uint8_t address, uint8_t size, uint8_t sendStop);
  uint8_t requestFrom(uint8_t address, uint8_t size);
  uint8_t requestFrom(int address, int size, int sendStop);
  uint8_t requestFrom(int address, int size);

  size_t write(uint8_t);
  size_t write(const uint8_t*, size_t);
  int available(void);
  int read(void);
  size_t readBytes(uint8_t* buffer, size_t length);
  // int peek(void);
  void flush(void);

  inline size_t write(const char* s) { return write((uint8_t*)s, strlen(s)); }
  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }

  // void onReceive(void (*)(int));
  // void onRequest(void (*)(void));
  // size_t slaveWrite(const uint8_t *, size_t);

 private:
  int32_t sda_ = -1;
  int32_t scl_ = -1;
  uint32_t freq_hz_ = 100000;

  bool init_flag_ = false;
  uint8_t num_;

  // size_t bufferSize;
  std::unique_ptr<uint8_t[]> rx_buffer_;
  size_t rx_index_ = 0;
  size_t rx_length_ = 0;

  std::vector<uint8_t> tx_buffer_;
  // uint16_t txAddress;
  // uint32_t _timeOutMillis;
  // bool nonStop;

  std::shared_ptr<cpp_bus_driver::HardwareI2c> bus_;
  i2c_master_bus_handle_t bus_handle_ = nullptr;

  // bool is_slave;
  // void (*user_onRequest)(void);
  // void (*user_onReceive)(int);
  // static void onRequestService(uint8_t, void *);
  // static void onReceiveService(uint8_t, uint8_t *, size_t, bool, void *);
  // bool initPins(int sdaPin, int sclPin);
  // bool allocateWireBuffer(void);
  void freeWireBuffer(void);
};

extern TwoWire Wire;
extern TwoWire Wire1;
