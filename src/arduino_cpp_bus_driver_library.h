/*
 * @Description: None
 * @Author: LILYGO_L
 * @Date: 2024-12-16 17:14:18
 * @LastEditTime: 2025-08-05 13:40:18
 * @License: GPL 3.0
 */

#pragma once

#include "cpp_bus_driver_library.h"

constexpr uint8_t INPUT = 0x01;
constexpr uint8_t OUTPUT = 0x03;
constexpr uint8_t PULLUP = 0x04;

constexpr uint8_t RISING = 0x01;
constexpr uint8_t FALLING = 0x02;

constexpr uint8_t LOW = 0x0;
constexpr uint8_t HIGH = 0x1;

constexpr uint8_t LSBFIRST = 0;
constexpr uint8_t MSBFIRST = 1;

constexpr uint8_t SPI_MODE0 = 0;
constexpr uint8_t SPI_MODE1 = 1;
constexpr uint8_t SPI_MODE2 = 2;
constexpr uint8_t SPI_MODE3 = 3;

void delay(uint32_t ms);
int64_t millis(void);

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);

void (attachInterrupt)(uint8_t pin, std::function<void(void)> intRoutine, int mode);
void detachInterrupt(uint8_t pin);