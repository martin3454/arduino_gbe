#include "ui.h"
//#include "emu.h"
#include "bus.h"
#include "ppu.h"
#include "gamepad.h"

#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI(); 


void ui_init(){
  SPI.begin(14, 12, 13, 15);
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_GREENYELLOW);
  //u16 *video_buffer = ppu_get_context()->video_buffer;
  //tft.pushImage(0, 0, 160, 144, video_buffer);   
  //tft.setTextSize(1);
  //tft.setTextColor(TFT_WHITE);
  //tft.setCursor(0, 0);
  //tft.println(F("Arduino"));
  //tft.fillRect(0, 0, 160, 144, 0xffffffff);
}

//static uint16_t video_buff2[144*160];
static uint16_t tile_colors[4] = {0xFFFF, 0xD69A, 0x7BEF, 0x0000}; 

void display_tile(char *buf, u16 startLocation, u16 tileNum, int16_t x, int16_t y) {

    for (u8 tileY = 0; tileY < 16; tileY += 2) {
        u8 b1 = bus_read(startLocation + (tileNum * 16) + tileY);
        u8 b2 = bus_read(startLocation + (tileNum * 16) + tileY + 1);
     
        //sprintf(buf, "0x%02X%02X\n", b1, b2);
        
        for (int8_t bit = 7; bit >= 0; bit--) {
            u8 hi = !!(b1 & (1 << bit)) << 1;
            u8 lo = !!(b2 & (1 << bit));            
            u8 color = hi | lo;

            int16_t _x = x + (7 - bit);
            int16_t _y = y + (tileY / 2);                       
            
            //tft.fillRect(_x, _y, w, h, barva);         
            tft.drawPixel(_x + 164, _y, tile_colors[color]);                                     
        }
    }
}


void update_dbg_window(char *buf) {
    int16_t xDraw = 0;
    int16_t yDraw = 0;
    int16_t tileNum = 0;
    uint8_t x, y;
    x = y = 0;
    
    u16 addr = 0x8000;

    //oam ram tiles
    for(uint8_t i = 0; i < 40; i++){
        tileNum = ppu_get_context()->oam_ram[i].tile;
        display_tile(buf, addr, tileNum, xDraw + x, yDraw + y);
        xDraw += 8;
        x++;
        if(i % 10 == 0 && i != 0){
            yDraw += 8;
            xDraw = 0;
            y++;
            x = 0;
        }
    }



    
    //384 tiles, 24 x 16
    /*for (uint8_t y =0 ; y < 24; y++) {
        for (uint8_t x = 0; x < 16; x++) {
            display_tile(buf, addr, tileNum, x + xDraw, y + yDraw);
            xDraw += 8;
            tileNum++;
        }
        yDraw += 8;
        xDraw = 0;
    }*/
}


void ui_update(char *buf) {
   //tft.fillScreen(TFT_LIGHTGREY);   
   //u16 *video_buffer2 = ppu_get_context()->video_buffer2;
   u16 *video_buffer = ppu_get_context()->video_buffer;
   //memcpy(video_buff2, video_buffer, 144*160*sizeof(uint16_t));
   //memset(video_buffer, ppu_get_context()->pfc.line_x, YRES * XRES * sizeof(u16));
   tft.pushImage(0, 0, 160, 144, video_buffer);
   //tft.pushImage(0, 0, 10, 10, pole);
   //memset(pole, ppu_get_context()->pfc.line_x, 100 * sizeof(u16));   
   //update_dbg_window(buf);  
}

void ui_on_key(bool down, char key) {
    switch(key) {
        case 'B': gamepad_get_state()->b = down; break;
        case 'A': gamepad_get_state()->a = down; break;
        case '*': gamepad_get_state()->start = down; break;
        case '#': gamepad_get_state()->select = down; break;
        case '2': gamepad_get_state()->up = down; break;
        case '5': gamepad_get_state()->down = down; break;
        case '4': gamepad_get_state()->left = down; break;
        case '6': gamepad_get_state()->right = down; break;
    }
}


void ui_handle_events() {
    //SDL_Event e;
    while (1){
        //TODO SDL_UpdateWindowSurface(sdlWindow);
        //TODO SDL_UpdateWindowSurface(sdlTraceWindow);
        //TODO SDL_UpdateWindowSurface(sdlDebugWindow);

        /*if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            emu_get_context()->die = true;
        }*/
        
    }
}
