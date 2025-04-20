#include "cpu.h"
#include "stack.h"
#include "interrupts.h"


const char* isr_to_str(u16 adr){
  switch(adr){
    case 0x40: return "ISR:0x40";
    case 0x48: return "ISR:0x48";
    case 0x50: return "ISR:0x50";
    case 0x58: return "ISR:0x58";
    default: return "xxx";
    
  }
}

void int_handle(cpu_context *ctx, u16 address) {
    stack_push16(ctx->regs.pc);
    ctx->regs.pc = address;
}

bool int_check(cpu_context *ctx, u16 address, interrupt_type it, char* buff) {
    //char *ptr = buff;
    
    if (ctx->int_flags & it && ctx->ie_register & it) {
        //const char* str = isr_to_str(address);
        //strcat(ptr, str);
        int_handle(ctx, address);
        ctx->int_flags &= ~it;
        ctx->halted = false;
        ctx->int_master_enabled = false;

        return true;
    }

    return false;
}

void cpu_handle_interrupts(cpu_context *ctx, char* buff) {
  
    if (int_check(ctx, 0x40, IT_VBLANK, buff)) {

    } else if (int_check(ctx, 0x48, IT_LCD_STAT, buff)) {

    } else if (int_check(ctx, 0x50, IT_TIMER, buff)) {

    }  else if (int_check(ctx, 0x58, IT_SERIAL, buff)) {

    }  else if (int_check(ctx, 0x60, IT_JOYPAD, buff)) {

    } 
}
