

#pragma once

#include "common.h"




void bus_init();
u8 read_unmapped(u16 addr);  
void write_unmapped(u16 addr, u8 val);

u8 bus_read_region_F(u16 address);
void bus_write_region_F(u16 address, u8 value);


u8 bus_read(u16 address);
void bus_write(u16 address, u8 value);


u16 bus_read16(u16 address);
void bus_write16(u16 address, u16 value);
