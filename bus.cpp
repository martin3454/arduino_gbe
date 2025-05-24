#include "bus.h"
#include "cart.h"
#include "ram.h"
#include "cpu.h"
#include "io.h"
#include "ppu.h"
#include "dma.h"


// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers
// 0xFF80 - 0xFFFE : Zero Page

typedef uint8_t (*ReadHandler)(uint16_t address);
typedef void (*WriteHandler)(uint16_t address, uint8_t value);

static ReadHandler read_map[16];
static WriteHandler write_map[16];


void bus_init(){
  for(u8 i = 0; i < 16; i++){
    read_map[i] = read_unmapped;
    write_map[i] = write_unmapped;
  }
  read_map[0x00] = read_map[0x01] = read_map[0x02] = read_map[0x03] = read_map[0x04] = read_map[0x05] = read_map[0x06] = read_map[0x07] =  cart_read;
  read_map[0x08] = read_map[0x09] = ppu_vram_read;
  read_map[0x0A] = read_map[0x0B] = cart_read;
  read_map[0x0C] = read_map[0x0D] = wram_read;
  read_map[0x0F] = bus_read_region_F; 

  write_map[0x00] = write_map[0x01] = write_map[0x02] = write_map[0x03] = write_map[0x04] = write_map[0x05] = write_map[0x06] = write_map[0x07] =  cart_write;
  write_map[0x08] = write_map[0x09] = ppu_vram_write;
  write_map[0x0A] = write_map[0x0B] = cart_write;
  write_map[0x0C] = write_map[0x0D] = wram_write;
  write_map[0x0F] = bus_write_region_F; 
    
}


// Default unmapped read/write
u8 read_unmapped(u16 addr) {
    return 0;
}

void write_unmapped(u16 addr, u8 val) {
    // No-op
}

u8 bus_read_region_F(u16 address){
    
    if (address < 0xFE00) {
        //reserved echo ram...
        return 0;
    } else if (address < 0xFEA0) {
        //OAM
        if (dma_transferring()) {
            return 0xFF;
        }

        return ppu_oam_read(address);
    } else if (address < 0xFF00) {
        //reserved unusable...
        return 0;
    } else if (address < 0xFF80) {
        //IO Registers...
        return io_read(address);
    } else if (address == 0xFFFF) {
        //CPU ENABLE REGISTER...
        return cpu_get_ie_register();
    }
    
    return hram_read(address);
}
void bus_write_region_F(u16 address, u8 value){
    
    if (address < 0xFE00) {
        //reserved echo ram
    } else if (address < 0xFEA0) {
        //OAM
        if (dma_transferring()) {
            return;
        }
        
        ppu_oam_write(address, value);
    } else if (address < 0xFF00) {
        //unusable reserved
    } else if (address < 0xFF80) {
        //IO Registers...
        io_write(address, value);
    } else if (address == 0xFFFF) {
        //CPU SET ENABLE REGISTER
        
        cpu_set_ie_register(value);
    } else {
        hram_write(address, value);
    }
}


u8 bus_read(u16 address) {
    return read_map[address >> 12](address);
    /*if (address < 0x8000) {
        //ROM Data
        return cart_read(address);
    } else if (address < 0xA000) {
        //Char/Map Data
        return ppu_vram_read(address);
    } else if (address < 0xC000) {
        //Cartridge RAM
        return cart_read(address);
    } else if (address < 0xE000) {
        //WRAM (Working RAM)
        return wram_read(address);
    } else if (address < 0xFE00) {
        //reserved echo ram...
        return 0;
    } else if (address < 0xFEA0) {
        //OAM
        if (dma_transferring()) {
            return 0xFF;
        }

        return ppu_oam_read(address);
    } else if (address < 0xFF00) {
        //reserved unusable...
        return 0;
    } else if (address < 0xFF80) {
        //IO Registers...
        return io_read(address);
    } else if (address == 0xFFFF) {
        //CPU ENABLE REGISTER...
        return cpu_get_ie_register();
    }

    //NO_IMPL
    return hram_read(address);*/
}

void bus_write(u16 address, u8 value) {
    write_map[address >> 12](address, value);
    /*if (address < 0x8000) {
        //ROM Data
        cart_write(address, value);
    } else if (address < 0xA000) {
        //Char/Map Data
        ppu_vram_write(address, value);
    } else if (address < 0xC000) {
        //EXT-RAM
        cart_write(address, value);
    } else if (address < 0xE000) {
        //WRAM
        wram_write(address, value);
    } else if (address < 0xFE00) {
        //reserved echo ram
    } else if (address < 0xFEA0) {
        //OAM
        if (dma_transferring()) {
            return;
        }
        
        ppu_oam_write(address, value);
    } else if (address < 0xFF00) {
        //unusable reserved
    } else if (address < 0xFF80) {
        //IO Registers...
        io_write(address, value);
    } else if (address == 0xFFFF) {
        //CPU SET ENABLE REGISTER
        
        cpu_set_ie_register(value);
    } else {
        hram_write(address, value);
    }*/
}

u16 bus_read16(u16 address) {
    u16 lo = bus_read(address);
    u16 hi = bus_read(address + 1);

    return lo | (hi << 8);
}

void bus_write16(u16 address, u16 value) {
    bus_write(address + 1, (value >> 8) & 0xFF);
    bus_write(address, value & 0xFF);
}
