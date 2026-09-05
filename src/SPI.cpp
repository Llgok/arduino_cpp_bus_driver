/*
 * @Description: None
 * @Author: LILYGO_L
 * @Date: 2025-08-05 11:23:01
 * @LastEditTime: 2026-04-20 16:53:02
 * @License: GPL 3.0
 */
#include "SPI.h"

void SPIClass::begin(int8_t sck, int8_t miso, int8_t mosi, int8_t ss) {
  std::lock_guard<std::mutex> lock(transaction_mutex_);
  if (bus_ != nullptr && !bus_->Deinit(false)) {
    return;
  }
  bus_.reset();
  tx_buffer_.clear();
  rx_buffer_ = nullptr;
  rx_length_ = 0;
  sclk_ = static_cast<int32_t>(sck);
  miso_ = static_cast<int32_t>(miso);
  mosi_ = static_cast<int32_t>(mosi);
  cs_ = static_cast<int32_t>(ss);

  init_flag_ = false;
}

// void SPIClass::end()
// {
//     // if (!_spi)
//     // {
//     //     return;
//     // }
//     // spiDetachSCK(_spi, _sck);
//     // spiDetachMISO(_spi, _miso);
//     // spiDetachMOSI(_spi, _mosi);
//     // setHwCs(false);
//     // spiStopBus(_spi);
//     // _spi = NULL;
// }

// void SPIClass::setHwCs(bool use)
// {
//     // if (use && !_use_hw_ss)
//     // {
//     //     spiAttachSS(_spi, 0, _ss);
//     //     spiSSEnable(_spi);
//     // }
//     // else if (!use && _use_hw_ss)
//     // {
//     //     spiSSDisable(_spi);
//     //     spiDetachSS(_spi, _ss);
//     // }
//     // _use_hw_ss = use;
// }

void SPIClass::setFrequency(uint32_t freq) { freq_hz_ = freq; }

bool SPIClass::setBus(const std::shared_ptr<cpp_bus_driver::HardwareSpi>& bus) {
  std::lock_guard<std::mutex> lock(transaction_mutex_);
  if (bus == nullptr || (bus_ != nullptr && !bus_->Deinit(false))) {
    return false;
  }
  bus_.reset();
  shared_bus_ = bus;
  init_flag_ = false;
  tx_buffer_.clear();
  rx_buffer_ = nullptr;
  rx_length_ = 0;
  return true;
}

// void SPIClass::setClockDivider(uint32_t clockDiv)
// {
//     // SPI_PARAM_LOCK();
//     // _div = clockDiv;
//     // spiSetClockDiv(_spi, _div);
//     // SPI_PARAM_UNLOCK();
// }

// uint32_t SPIClass::getClockDivider()
// {
//     // return spiGetClockDiv(_spi);
//     return 0;
// }

// void SPIClass::setDataMode(uint8_t data_mode)
// {
//     // spiSetDataMode(_spi, data_mode);
// }

// void SPIClass::setBitOrder(uint8_t bit_order)
// {
//     // spiSetBitOrder(_spi, bit_order);
// }

void SPIClass::beginTransaction(SPISettings settings) {
  // RFAL's IRQ task and application task must not interleave batched transfers.
  transaction_mutex_.lock();
  // SPI_PARAM_LOCK();
  // // check if last freq changed
  // uint32_t cdiv = spiGetClockDiv(_spi);
  // if (_freq != settings.clock_ || _div != cdiv)
  // {
  //     _freq = settings.clock_;
  //     _div = spiFrequencyToClockDiv(_freq);
  // }
  // spiTransaction(_spi, _div, settings.data_mode_, settings.bit_order_);
  // _inTransaction = true;

  if (init_flag_ == true) {
    return;
  }

  if (settings.data_mode_ > SPI_MODE3 ||
      (settings.bit_order_ != SPI_MSBFIRST && settings.bit_order_ != SPI_LSBFIRST)) {
    return;
  }
  const uint32_t flags =
      settings.bit_order_ == SPI_LSBFIRST ? SPI_DEVICE_BIT_LSBFIRST : 0;
  if (shared_bus_ != nullptr) {
    bus_ = std::make_shared<cpp_bus_driver::HardwareSpi>(
        shared_bus_, settings.data_mode_, flags);
  } else {
    bus_ = std::make_shared<cpp_bus_driver::HardwareSpi>(mosi_, sclk_, miso_,
        static_cast<spi_host_device_t>(spi_num_), settings.data_mode_, flags);
    bus_->set_bus_init_flag(bus_init_flag_);
  }

  if (!bus_->Init(settings.clock_, cs_)) {
    bus_->LogMessage(cpp_bus_driver::Logger::LogLevel::kError, __FILE__, __LINE__,
        "Init failed\n");
    return;
  }

  init_flag_ = true;
}

void SPIClass::endTransaction() {
  std::lock_guard<std::mutex> lock(transaction_mutex_, std::adopt_lock);
  if (!init_flag_ || bus_ == nullptr || tx_buffer_.empty()) {
    tx_buffer_.clear();
    rx_buffer_ = nullptr;
    rx_length_ = 0;
    return;
  }
  size_t buffertx_buffer__length = tx_buffer_.size();
  size_t bufferrx_buffer__length = buffertx_buffer__length - rx_length_;

  if (buffertx_buffer__length == 0) {
    bus_->LogMessage(cpp_bus_driver::Logger::LogLevel::kWarning, __FILE__, __LINE__,
        "Value out of range\n");
    return;
  }

  if (rx_buffer_ == nullptr || rx_length_ == 0) {
    if (!bus_->Write(tx_buffer_.data(), buffertx_buffer__length)) {
      bus_->LogMessage(cpp_bus_driver::Logger::LogLevel::kError, __FILE__, __LINE__,
          "Write failed\n");
    }

    // for (size_t i = 0; i < buffertx_buffer__length; i++)
    // {
    //     bus_->LogMessage(cpp_bus_driver::PlatformHal::Log_Level::DEBUG, __FILE__,
    //     __LINE__, "tx1[%d]: %#X\n", i, tx_buffer_[i]);
    // }
  } else {
    auto buffer_rx_data = std::make_unique<uint8_t[]>(buffertx_buffer__length);

    if (!bus_->WriteRead(
            tx_buffer_.data(), buffer_rx_data.get(), buffertx_buffer__length)) {
      bus_->LogMessage(cpp_bus_driver::Logger::LogLevel::kError, __FILE__, __LINE__,
          "WriteRead failed\n");
    }

    // for (size_t i = 0; i < buffertx_buffer__length; i++)
    // {
    //     bus_->LogMessage(cpp_bus_driver::PlatformHal::Log_Level::DEBUG, __FILE__,
    //     __LINE__, "tx2[%d]: %#X\n", i, tx_buffer_[i]);
    // }

    // for (size_t i = 0; i < buffertx_buffer__length; i++)
    // {
    //     bus_->LogMessage(cpp_bus_driver::PlatformHal::Log_Level::DEBUG, __FILE__,
    //     __LINE__, "rx[%d]: %#X\n", i, buffer_rx_data[i]);
    // }

    std::memcpy(
        rx_buffer_, buffer_rx_data.get() + bufferrx_buffer__length, rx_length_);

    // for (size_t i = 0; i < rx_length_; i++)
    // {
    //     bus_->LogMessage(cpp_bus_driver::PlatformHal::Log_Level::DEBUG, __FILE__,
    //     __LINE__, "rx_buffer_[%d]: %#X\n", i, rx_buffer_[i]);
    // }
  }

  tx_buffer_.clear();
  rx_buffer_ = nullptr;
  rx_length_ = 0;
}

// void SPIClass::write(uint8_t data)
// {
//     // if (_inTransaction)
//     // {
//     //     return spiWriteByteNL(_spi, data);
//     // }
//     // spiWriteByte(_spi, data);
// }

uint8_t SPIClass::transfer(uint8_t data) {
  // Command bytes are batched with the following payload under one hardware CS.
  transferBytes(&data, nullptr, 1);
  return 1;
}

// void SPIClass::write16(uint16_t data)
// {
//     // if (_inTransaction)
//     // {
//     //     return spiWriteShortNL(_spi, data);
//     // }
//     // spiWriteWord(_spi, data);
// }

// uint16_t SPIClass::transfer16(uint16_t data)
// {
//     // if (_inTransaction)
//     // {
//     //     return spiTransferShortNL(_spi, data);
//     // }
//     // return spiTransferWord(_spi, data);
//     return 0;
// }

// void SPIClass::write32(uint32_t data)
// {
//     // if (_inTransaction)
//     // {
//     //     return spiWriteLongNL(_spi, data);
//     // }
//     // spiWriteLong(_spi, data);
// }

// uint32_t SPIClass::transfer32(uint32_t data)
// {
//     // if (_inTransaction)
//     // {
//     //     return spiTransferLongNL(_spi, data);
//     // }
//     // return spiTransferLong(_spi, data);
//     return 0;
// }

// void SPIClass::transferBits(uint32_t data, uint32_t *out, uint8_t bits)
// {
//     // if (_inTransaction)
//     // {
//     //     return spiTransferBitsNL(_spi, data, out, bits);
//     // }
//     // spiTransferBits(_spi, data, out, bits);
// }

void SPIClass::writeBytes(const uint8_t* data, uint32_t size) {
  if (init_flag_ && bus_ != nullptr && !bus_->Write(data, size)) {
    bus_->LogMessage(cpp_bus_driver::Logger::LogLevel::kError, __FILE__, __LINE__,
        "Write failed\n");
  }
}

void SPIClass::transfer(void* data, uint32_t size) {
  transferBytes((const uint8_t*)data, (uint8_t*)data, size);
}

// /**
//  * @param data void *
//  * @param size uint32_t
//  */
// void SPIClass::writePixels(const void *data, uint32_t size)
// {
//     // if (_inTransaction)
//     // {
//     //     return spiWritePixelsNL(_spi, data, size);
//     // }
//     // spiSimpleTransaction(_spi);
//     // spiWritePixelsNL(_spi, data, size);
//     // spiEndTransaction(_spi);
// }

/**
 * @param data uint8_t * data buffer. can be NULL for Read Only operation
 * @param out  uint8_t * output buffer. can be NULL for Write Only operation
 * @param size uint32_t
 */
void SPIClass::transferBytes(const uint8_t* data, uint8_t* out, uint32_t size) {
  if (!init_flag_ || size == 0) {
    return;
  }
  if (data == nullptr) {
    tx_buffer_.insert(tx_buffer_.end(), size, 0);
  } else {
    tx_buffer_.insert(tx_buffer_.end(), data, data + size);
  }

  rx_buffer_ = out;
  rx_length_ = out == nullptr ? 0 : size;
}

// /**
//  * @param data uint8_t *
//  * @param size uint8_t  max for size is 64Byte
//  * @param repeat uint32_t
//  */
// void SPIClass::writePattern(const uint8_t *data, uint8_t size, uint32_t
// repeat)
// {
//     // if (size > 64)
//     // {
//     //     return; // max Hardware FIFO
//     // }

//     // uint32_t byte = (size * repeat);
//     // uint8_t r = (64 / size);
//     // const uint8_t max_bytes_FIFO = r * size; // Max number of whole
//     patterns (in bytes) that can fit into the hardware FIFO

//     // while (byte)
//     // {
//     //     if (byte > max_bytes_FIFO)
//     //     {
//     //         writePattern_(data, size, r);
//     //         byte -= max_bytes_FIFO;
//     //     }
//     //     else
//     //     {
//     //         writePattern_(data, size, (byte / size));
//     //         byte = 0;
//     //     }
//     // }
// }

// void SPIClass::writePattern_(const uint8_t *data, uint8_t size, uint8_t
// repeat)
// {
//     // uint8_t bytes = (size * repeat);
//     // uint8_t buffer[64];
//     // uint8_t *bufferPtr = &buffer[0];
//     // const uint8_t *dataPtr;
//     // uint8_t dataSize = bytes;
//     // for (uint8_t i = 0; i < repeat; i++)
//     // {
//     //     dataSize = size;
//     //     dataPtr = data;
//     //     while (dataSize--)
//     //     {
//     //         *bufferPtr = *dataPtr;
//     //         dataPtr++;
//     //         bufferPtr++;
//     //     }
//     // }

//     // writeBytes(&buffer[0], bytes);
// }

SPIClass SPI(SPI2_HOST);
