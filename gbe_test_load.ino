
#include <Keypad.h>
#define ROWS 4
#define COLS 4

//#include "LittleFS.h"
#include "emu.h"
#include "cart.h"
#include "cpu.h"
#include "timer.h"
#include "dma.h"
#include "ui.h"
#include "ppu.h"
#include "lcd.h"
#include "gamepad.h"

#include "my_queue.h"


const char kp4x4Keys[ROWS][COLS]= {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
  };
  
byte rowKp4x4Pin [4] = {18, 5, 17, 16};       //{27, 14, 12, 13};
byte colKp4x4Pin [4] = {32, 33, 25, 26};       //{32, 33, 25, 26};
const char tlacitka[8] = {'A', 'B', '*', '#', '2', '5', '4', '6'}; //a,b,start,select,up,down,left,right 
bool stisk_tlacitka[8] = {false, false, false, false, false, false, false, false};
Keypad klaves = Keypad(makeKeymap(kp4x4Keys), rowKp4x4Pin, colKp4x4Pin, ROWS, COLS);

static emu_context ctx;

//QueueHandle_t queue_h;

/*void queue_push(uint8_t item){
 // uint8_t value = item;
  //xQueueSend(queue_h, &value, portMAX_DELAY);
  //Serial.println("push ");
}*/

void button_check();

emu_context *emu_get_context() {
    return &ctx;
}

//uint32_t cas1, cas2;

void cpu_run() {
          
    char debug_vypis[100] = {'\0'};
    char tiles_data[60];
    uint32_t frame = 0;
    ctx._running = true;
    ctx.paused = false;
    ctx.ticks = 0;
    
    
    while(ctx._running) {
        //cas2 = millis();
        if (ctx.paused) {
            delay(100);
            continue;
        }
        if(cpu_step(debug_vypis)) {
          
        }else break;

       if(frame != ppu_get_context()->current_frame){
          button_check();
          //cas2 = millis();
          //uint32_t dif = cas2 - cas1;
          //Serial.println(dif);
          //cas1 = cas2; 
          button_check();         
          ui_update(tiles_data);
          frame = ppu_get_context()->current_frame;      
       }
       
       /*if(cas1 + 800 < cas2){
          cas1 = cas2;
          button_check();
       }*/         
    }    
}
 


void setup() {
  
  Serial.begin(115200);
  //cas1 = cas2 = 0;
  
  /*if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  File file = LittleFS.open("/tetris.gb");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }*/ 

  /*Serial.println(" velikost souboru v bytech: ");
  int velikost = file.size();
  Serial.println(velikost);
  uint8_t* rom_buff = (uint8_t*)malloc(velikost * sizeof(uint8_t));
  int n_bytes = file.read(rom_buff, velikost);
  file.close();
  Serial.println(n_bytes);*/
  
//************************************************************
  const char *files[] = {
        "rezerva",
        "Contra.gb"
  };

  char buff[10];
  
  Serial.println(cart_load(files[1], buff));
  Serial.println(buff);

  ui_init();
  timer_init();
  //bus_init();
  cpu_init();
  ppu_init();
  cpu_run();

  //queue_h = xQueueCreate(10, sizeof(uint8_t));
  //xTaskCreate(cpu_run,"cpu_run",2048,NULL ,1, NULL);
  //xTaskCreate(emu_cycles,"emu_cycles",2048,NULL ,8, NULL);    
}

void loop() {
  // put your main code here, to run repeatedly:

}


void button_check(){
  klaves.getKeys();
  for(uint8_t i = 0; i < 8; i++){
    int8_t a = klaves.findInList(tlacitka[i]);
    if(a != -1){
      stisk_tlacitka[i] = true;
      ui_on_key(true, tlacitka[i]);
    }else if(stisk_tlacitka[i]){
      stisk_tlacitka[i] = false;
      ui_on_key(false, tlacitka[i]);
    }      
  }  
}


//NOT USED
int emu_run(int argc, const char **argv) {
    /*
     * if (argc < 2) {
        //printf("Usage: emu <rom_file>\n");
        return -1;
    }
    if (!cart_load(argv[1])) {
        //printf("Failed to load ROM file: %s\n", argv[1]);
        return -2;
    }

    ui_init();
    cpu_run(NULL);
*/
    return 0;
    
    
}

void emu_cycles(uint8_t cpu_cycles) {    
    for (uint8_t i=0; i<cpu_cycles; i++) {
     for (int8_t n=0; n<4; n++) {        
          ctx.ticks++;
          timer_tick();
          ppu_tick();
      }
     dma_tick();
    }
}
