#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void myDelay(uint16_t delayMs);
TickType_t get_tick();
