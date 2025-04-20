#pragma once

#include "common.h"

typedef struct {
    bool paused;
    bool _running;
    //bool die;
    u64 ticks;
} emu_context;

int emu_run(int argc, const char **argv);

emu_context *emu_get_context();

void emu_cycles(uint8_t cycles);
