#include "ppu.h"
#include "lcd.h"
//#include <string.h>
#include "ppu_sm.h"


//void pipeline_fifo_reset();
//void pipeline_process();

static ppu_context ctx;

ppu_context *ppu_get_context() {
    return &ctx;
}

void ppu_init() {
    ctx.current_frame = 0;
    ctx.line_ticks = 0;
    ctx._mode = 0;
    //ctx.video_buffer2 = (u16*)malloc(YRES * XRES * sizeof(u16));
    ctx.pfc.line_x = 0;
    ctx.pfc.pushed_x = 0;
    ctx.pfc.fetch_x = 0;
    ctx.pfc.pixel_fifo._size = 0;
    ctx.pfc.pixel_fifo.zac = 0;
    ctx.pfc.pixel_fifo.kon = -1;    
    //ctx.pfc.pixel_fifo.head = ctx.pfc.pixel_fifo.tail = NULL;
    ctx.pfc.cur_fetch_state = FS_TILE;

    ctx.line_sprites = 0;
    ctx.fetched_entry_count = 0;
    ctx.window_line = 0;
    
    lcd_init();
    LCDS_MODE_SET(MODE_OAM);

    memset(ctx.oam_ram, 0, sizeof(ctx.oam_ram));
    memset(ctx.video_buffer, 0, YRES * XRES * sizeof(u16));
}

void ppu_tick() {
    ctx.line_ticks++;

    switch(LCDS_MODE) {
    case MODE_OAM:
        ppu_mode_oam();
        ctx._mode = 0;
        break;
    case MODE_XFER:
        ppu_mode_xfer();
        ctx._mode = 1;
        break;
    case MODE_VBLANK:
        ppu_mode_vblank();
        ctx._mode = 2;
        break;
    case MODE_HBLANK:
        ppu_mode_hblank();
        ctx._mode = 3;
        break;
    }
}


void ppu_oam_write(u16 address, u8 value) {
    if (address >= 0xFE00) {
        address -= 0xFE00;
    }

    u8 *p = (u8 *)ctx.oam_ram;
    p[address] = value;
}

u8 ppu_oam_read(u16 address) {
    if (address >= 0xFE00) {
        address -= 0xFE00;
    }

    u8 *p = (u8 *)ctx.oam_ram;
    return p[address];
}

void ppu_vram_write(u16 address, u8 value) {
    ctx.vram[address - 0x8000] = value;
}

u8 ppu_vram_read(u16 address) {
    return ctx.vram[address - 0x8000];
}
