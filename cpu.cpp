#include "cpu.h"
#include "bus.h"
#include "emu.h"
#include "interrupts.h"
//#include <dbg.h"
#include "timer.h"
#include "cekej.h"
#include "ppu.h"
#include "lcd.h"
//#include "my_queue.h"


cpu_context ctx;

cpu_context* cpu_get_context(){
  return &ctx;
}

void cpu_init() {

    bus_init();
    ctx.regs.pc = 0x100;
    ctx.regs.sp = 0xFFFE;
    *((short *)&ctx.regs.a) = 0xB001;
    *((short *)&ctx.regs.b) = 0x1300;
    *((short *)&ctx.regs.d) = 0xD800;
    *((short *)&ctx.regs.h) = 0x4D01;
    ctx.ie_register = 0;
    ctx.int_flags = 0;
    ctx.int_master_enabled = false;
    ctx.enabling_ime = false;
    
    memset(ctx.dbg_flags, 0, sizeof(ctx.dbg_flags));
    
    timer_get_context()->_div = 0xABCC;
}

static void fetch_instruction() {
    ctx.cur_opcode = bus_read(ctx.regs.pc++);
    ctx.cur_inst = instruction_by_opcode(ctx.cur_opcode);
}

void fetch_data();

static void execute() {
    IN_PROC proc = inst_get_processor(ctx.cur_inst->type);

    if (!proc) {
        NO_IMPL
    }

    proc(&ctx);
}

bool cpu_step(char *vypis){

    char *ptr = vypis;
    if (!ctx.halted) {
        u16 pc = ctx.regs.pc;              
        fetch_instruction();
        emu_cycles(1);       
        fetch_data();
        
        if(ctx.dbg_flags[1]){
            char flags[16];
            sprintf(flags, "%c%c%c%c", 
                ctx.regs.f & (1 << 7) ? 'Z' : '-',
                ctx.regs.f & (1 << 6) ? 'N' : '-',
                ctx.regs.f & (1 << 5) ? 'H' : '-',
                ctx.regs.f & (1 << 4) ? 'C' : '-'
            );

            char inst[16];
            inst_to_str(&ctx, inst);
              
            sprintf(ptr,"%08llX - %04X: %-12s (%02X %02X %02X) A: %02X F: %s BC: %02X%02X DE: %02X%02X HL: %02X%02X IF: %02X IE: %02X  IME: %d  TIMA: %d  DIV: %d \n",
                emu_get_context()->ticks,
                pc, inst, ctx.cur_opcode,
                bus_read(pc + 1), bus_read(pc + 2), ctx.regs.a, flags, ctx.regs.b, ctx.regs.c,
                ctx.regs.d, ctx.regs.e, ctx.regs.h, ctx.regs.l, ctx.int_flags, ctx.ie_register, ctx.int_master_enabled, timer_get_context()->tima, timer_get_context()->_div);
            
            /*if(pc == 0x40 || ctx.dbg_flags[1]){
              sprintf(ptr,"%08llX - IF: %02X IE: %02X  IME: %d   PPU_Tick: %d  LY: %d  Mode: %d\n",
                  emu_get_context()->ticks, ctx.int_flags, ctx.ie_register, ctx.int_master_enabled, ppu_get_context()->line_ticks, lcd_get_context()->ly, ppu_get_context()->_mode);
                  ctx.dbg_flags[1] = 0;
            }else sprintf(ptr, " ");*/
        }
            
        if (ctx.cur_inst == NULL) {
            //printf("Unknown Instruction! %02X\n", ctx.cur_opcode);
            //exit(-7);
            return false;
        }
       
        execute();
        
    } else {
        //is halted...                          
         emu_cycles(1);        
        /*sprintf(ptr," %08llX - IF: %02X IE: %02X  IME: %d  PPU_Tick: %d  LY: %d   MODE: %d\n",
            emu_get_context()->ticks, ctx.int_flags, ctx.ie_register, ctx.int_master_enabled, ppu_get_context()->line_ticks, lcd_get_context()->ly, ppu_get_context()->_mode);
          */
        if (ctx.int_flags) {
            ctx.halted = false;           
            //ctx.dbg_flags[1] = 1;                        
        }
    }

    if (ctx.int_master_enabled) {
        cpu_handle_interrupts(&ctx, ptr);
        ctx.enabling_ime = false;
    }

    if (ctx.enabling_ime) {
        ctx.int_master_enabled = true;
    }

    return true;
}

u8 cpu_get_ie_register() {
    return ctx.ie_register;
}

void cpu_set_ie_register(u8 n) {
    ctx.ie_register = n;
}

void cpu_request_interrupt(interrupt_type t) {
    ctx.int_flags |= t;
}
