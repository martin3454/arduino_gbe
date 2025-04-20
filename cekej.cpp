#include "cekej.h"



void myDelay(uint16_t delayMs) {
    vTaskDelay(pdMS_TO_TICKS(delayMs));  
}

TickType_t get_tick(){
  return xTaskGetTickCount();
}
