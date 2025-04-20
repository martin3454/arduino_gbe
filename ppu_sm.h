#pragma once

#include "common.h"

void ppu_mode_oam();
void ppu_mode_xfer();
void ppu_mode_vblank();
void ppu_mode_hblank();

void pipeline_fifo_reset();
void pipeline_process();
bool window_visible();
