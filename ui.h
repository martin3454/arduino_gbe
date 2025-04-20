#pragma once

#include "common.h"

static const int SCREEN_WIDTH = 160;
static const int SCREEN_HEIGHT = 144;

//uint16_t buff[160 * 144];

void ui_init();
void ui_handle_events();
void ui_update(char *buff);
void ui_on_key(bool down, char key);
