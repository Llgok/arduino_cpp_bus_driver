/*
 * @Description: None
 * @Author: LILYGO_L
 * @Date: 2025-08-05 11:44:23
 * @LastEditTime: 2025-08-05 13:40:31
 * @License: GPL 3.0
 */
#include "arduino_cpp_bus_driver_library.h"

void delay(uint32_t ms)
{
    usleep(ms * 1000);
}

int64_t millis(void)
{
    return esp_timer_get_time() / 1000;
}

void pinMode(uint8_t pin, uint8_t mode)
{

}

void digitalWrite(uint8_t pin, uint8_t val)
{
    
}

int digitalRead(uint8_t pin)
{
    return 0;
}

void attachInterrupt(uint8_t pin, std::function<void(void)> intRoutine, int mode)
{

}

void detachInterrupt(uint8_t pin)
{

}

