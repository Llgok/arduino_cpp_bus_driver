/*
 * @Description: SPI
 * @Author: LILYGO_L
 * @Date: 2025-08-05 11:23:01
 * @LastEditTime: 2026-04-27 16:04:15
 * @License: GPL 3.0
 */
#pragma once

#include "arduino_cpp_bus_driver.h"

class SPISettings {
 public:
  SPISettings()
      : clock_(1000000), bit_order_(SPI_MSBFIRST), data_mode_(SPI_MODE0) {}
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t data_mode)
      : clock_(clock), bit_order_(bitOrder), data_mode_(data_mode) {}
  uint32_t clock_;
  uint8_t bit_order_;
  uint8_t data_mode_;
};

class SPIClass {
 public:
  SPIClass(uint8_t spi_bus = 1)
      : spi_num_(
            spi_bus) /*, _use_hw_ss(false), _div(0), _inTransaction(false)*/
  {}

  void begin(
      int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1, int8_t ss = -1);
  // void end();

  // void setHwCs(bool use);
  // void setBitOrder(uint8_t bitOrder);
  // void setDataMode(uint8_t data_mode);
  void setFrequency(uint32_t freq);
  void set_bus_init_flag(bool enable) { bus_init_flag_ = enable; }
  // void setClockDivider(uint32_t clockDiv);

  // uint32_t getClockDivider();

  void beginTransaction(SPISettings settings);
  void endTransaction(void);
  void transfer(void* data, uint32_t size);
  uint8_t transfer(uint8_t data);
  // uint16_t transfer16(uint16_t data);
  // uint32_t transfer32(uint32_t data);

  void transferBytes(const uint8_t* data, uint8_t* out, uint32_t size);
  // void transferBits(uint32_t data, uint32_t *out, uint8_t bits);

  // void write(uint8_t data);
  // void write16(uint16_t data);
  // void write32(uint32_t data);
  void writeBytes(const uint8_t* data, uint32_t size);
  // void writePixels(const void *data, uint32_t size); // ili9341 compatible
  // void writePattern(const uint8_t *data, uint8_t size, uint32_t repeat);

  int8_t pinSS() { return cs_; }

 private:
  int32_t mosi_, sclk_, miso_, cs_, freq_hz_;
  bool init_flag_ = false;
  bool bus_init_flag_ = false;
  int8_t spi_num_;
  // bool _use_hw_ss;
  // uint32_t _div;
  //     bool _inTransaction;
  // #if !CONFIG_DISABLE_HAL_LOCKS
  //     SemaphoreHandle_t paramLock = NULL;
  // #endif
  // void writePattern_(const uint8_t *data, uint8_t size, uint8_t repeat);
  std::vector<uint8_t> tx_buffer_;
  uint8_t* rx_buffer_;
  size_t rx_length_;
  std::shared_ptr<cpp_bus_driver::HardwareSpi> bus_;
};

extern SPIClass SPI;
