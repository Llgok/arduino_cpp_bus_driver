/*
 * @Description: None
 * @Author: LILYGO_L
 * @Date: 2025-08-05 11:44:23
 * @LastEditTime: 2026-07-01 14:35:00
 * @License: GPL 3.0
 */
#include "arduino_cpp_bus_driver.h"

struct InterruptArg {
  std::function<void(void)> interrupt_function;
};

static std::unordered_map<uint8_t, std::unique_ptr<InterruptArg>> Interrupt_Map;

auto g_arduino_cpp_bus_driver_tool = std::make_unique<cpp_bus_driver::Tool>();

static void IRAM_ATTR InterruptCallbackTemplate(void* arg) {
  auto* local_arg = static_cast<InterruptArg*>(arg);
  if (local_arg->interrupt_function) {
    local_arg->interrupt_function();
  }
}

void delay(uint32_t ms) { g_arduino_cpp_bus_driver_tool->DelayMs(ms); }

void delayMicroseconds(uint32_t us) {
  g_arduino_cpp_bus_driver_tool->DelayUs(us);
}

int64_t millis(void) {
  return g_arduino_cpp_bus_driver_tool->GetSystemTimeMs();
}

void pinMode(uint8_t pin, uint8_t mode) {
  if (pin == static_cast<uint8_t>(-1)) {
    g_arduino_cpp_bus_driver_tool->LogMessage(
        cpp_bus_driver::Tool::LogLevel::kWarning, __FILE__, __LINE__,
        "Value out of range\n");
    return;
  }

  switch (mode) {
    case INPUT:
      g_arduino_cpp_bus_driver_tool->SetGpioMode(
          pin, cpp_bus_driver::Tool::GpioMode::kInput);
      break;
    case OUTPUT:
      g_arduino_cpp_bus_driver_tool->SetGpioMode(
          pin, cpp_bus_driver::Tool::GpioMode::kOutput);
      break;
    case PULLUP:
      g_arduino_cpp_bus_driver_tool->SetGpioMode(pin,
          cpp_bus_driver::Tool::GpioMode::kInputOutput,
          cpp_bus_driver::Tool::GpioStatus::kPullup);
      break;
    case INPUT_PULLUP:
      g_arduino_cpp_bus_driver_tool->SetGpioMode(pin,
          cpp_bus_driver::Tool::GpioMode::kInput,
          cpp_bus_driver::Tool::GpioStatus::kPullup);
      break;
    case PULLDOWN:
      g_arduino_cpp_bus_driver_tool->SetGpioMode(pin,
          cpp_bus_driver::Tool::GpioMode::kInputOutput,
          cpp_bus_driver::Tool::GpioStatus::kPulldown);
      break;
    case INPUT_PULLDOWN:
      g_arduino_cpp_bus_driver_tool->SetGpioMode(pin,
          cpp_bus_driver::Tool::GpioMode::kInput,
          cpp_bus_driver::Tool::GpioStatus::kPulldown);
      break;
    case OPEN_DRAIN:
      g_arduino_cpp_bus_driver_tool->SetGpioMode(
          pin, cpp_bus_driver::Tool::GpioMode::kInputOutputOd);
      break;
    case OUTPUT_OPEN_DRAIN:
      g_arduino_cpp_bus_driver_tool->SetGpioMode(
          pin, cpp_bus_driver::Tool::GpioMode::kOutputOd);
      break;
    case ANALOG:
      g_arduino_cpp_bus_driver_tool->SetGpioMode(
          pin, cpp_bus_driver::Tool::GpioMode::kDisable);
      break;

    default:
      g_arduino_cpp_bus_driver_tool->LogMessage(
          cpp_bus_driver::Tool::LogLevel::kWarning, __FILE__, __LINE__,
          "Value out of range\n");
      break;
  }
}

void digitalWrite(uint8_t pin, uint8_t val) {
  if (pin == static_cast<uint8_t>(-1)) {
    // g_arduino_cpp_bus_driver_tool->LogMessage(cpp_bus_driver::Tool::LogLevel::kWarning,
    // __FILE__, __LINE__, "Value out of range\n");
    return;
  }

  g_arduino_cpp_bus_driver_tool->GpioWrite(pin, val);
}

int digitalRead(uint8_t pin) {
  if (pin == static_cast<uint8_t>(-1)) {
    // g_arduino_cpp_bus_driver_tool->LogMessage(cpp_bus_driver::Tool::LogLevel::kWarning,
    // __FILE__, __LINE__, "Value out of range\n");
    return 0;
  }

  return g_arduino_cpp_bus_driver_tool->GpioRead(pin);
}

void attachInterrupt(
    uint8_t pin, std::function<void(void)> intRoutine, int mode) {
  if (pin == static_cast<uint8_t>(-1)) {
    g_arduino_cpp_bus_driver_tool->LogMessage(
        cpp_bus_driver::Tool::LogLevel::kWarning, __FILE__, __LINE__,
        "Value out of range\n");
    return;
  }

  cpp_bus_driver::Tool::InterruptMode buffer_mode =
      cpp_bus_driver::Tool::InterruptMode::kDisable;
  switch (mode) {
    case DISABLED:
      break;
    case RISING:
      buffer_mode = cpp_bus_driver::Tool::InterruptMode::kRising;
      break;
    case FALLING:
      buffer_mode = cpp_bus_driver::Tool::InterruptMode::kFalling;
      break;
    case CHANGE:
      buffer_mode = cpp_bus_driver::Tool::InterruptMode::kChange;
      break;
    case ONLOW:
      buffer_mode = cpp_bus_driver::Tool::InterruptMode::kOnLow;
      break;
    case ONHIGH:
      buffer_mode = cpp_bus_driver::Tool::InterruptMode::kOnHigh;
      break;
    case ONLOW_WE:
      buffer_mode = cpp_bus_driver::Tool::InterruptMode::kOnLow;
      break;
    case ONHIGH_WE:
      buffer_mode = cpp_bus_driver::Tool::InterruptMode::kOnHigh;
      break;

    default:
      g_arduino_cpp_bus_driver_tool->LogMessage(
          cpp_bus_driver::Tool::LogLevel::kWarning, __FILE__, __LINE__,
          "Value out of range\n");
      break;
  }

  auto arg = std::make_unique<InterruptArg>(intRoutine);
  Interrupt_Map[pin] = std::move(arg);

  if (!g_arduino_cpp_bus_driver_tool->InitGpioInterrupt(pin, buffer_mode,
          InterruptCallbackTemplate, Interrupt_Map[pin].get())) {
    g_arduino_cpp_bus_driver_tool->LogMessage(
        cpp_bus_driver::Tool::LogLevel::kError, __FILE__, __LINE__,
        "create_gpio_interrupt fail\n");
  }
}

void detachInterrupt(uint8_t pin) {
  if (pin == static_cast<uint8_t>(-1)) {
    g_arduino_cpp_bus_driver_tool->LogMessage(
        cpp_bus_driver::Tool::LogLevel::kWarning, __FILE__, __LINE__,
        "Value out of range\n");
    return;
  }

  if (!g_arduino_cpp_bus_driver_tool->DeinitGpioInterrupt(pin)) {
    g_arduino_cpp_bus_driver_tool->LogMessage(
        cpp_bus_driver::Tool::LogLevel::kError, __FILE__, __LINE__,
        "DeleteGpioInterrupt fail\n");
  }
}
