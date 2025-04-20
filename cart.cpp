#include "cart.h"

typedef struct {
    char filename[1024];
    u32 rom_size;
    u8 *rom_data;
    rom_header *header;
} cart_context;

static cart_context ctx;

static const char* ROM_TYPES[] = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "0x04 ???",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM 1",
    "ROM+RAM+BATTERY 1",
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY 2",
    "MBC3",
    "MBC3+RAM 2",
    "MBC3+RAM+BATTERY 2",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};


static const char* LIC_CODE[0xA5] = {
    [0x00] = "None",
[0x01] = "Nintendo R&D1",
[0x02] = "UNKNOWN",
[0x03] = "UNKNOWN",
[0x04] = "UNKNOWN",
[0x05] = "UNKNOWN",
[0x06] = "UNKNOWN",
[0x07] = "UNKNOWN",
[0x08] = "Capcom",
[0x09] = "UNKNOWN",
[0x0a] = "UNKNOWN",
[0x0b] = "UNKNOWN",
[0x0c] = "UNKNOWN",
[0x0d] = "UNKNOWN",
[0x0e] = "UNKNOWN",
[0x0f] = "UNKNOWN",
[0x10] = "UNKNOWN",
[0x11] = "UNKNOWN",
[0x12] = "UNKNOWN",
[0x13] = "Electronic Arts",
[0x14] = "UNKNOWN",
[0x15] = "UNKNOWN",
[0x16] = "UNKNOWN",
[0x17] = "UNKNOWN",
[0x18] = "Hudson Soft",
[0x19] = "b-ai",
[0x1a] = "UNKNOWN",
[0x1b] = "UNKNOWN",
[0x1c] = "UNKNOWN",
[0x1d] = "UNKNOWN",
[0x1e] = "UNKNOWN",
[0x1f] = "UNKNOWN",
[0x20] = "kss",
[0x21] = "UNKNOWN",
[0x22] = "pow",
[0x23] = "UNKNOWN",
[0x24] = "PCM Complete",
[0x25] = "san-x",
[0x26] = "UNKNOWN",
[0x27] = "UNKNOWN",
[0x28] = "Kemco Japan",
[0x29] = "seta",
[0x2a] = "UNKNOWN",
[0x2b] = "UNKNOWN",
[0x2c] = "UNKNOWN",
[0x2d] = "UNKNOWN",
[0x2e] = "UNKNOWN",
[0x2f] = "UNKNOWN",
[0x30] = "Viacom",
[0x31] = "Nintendo",
[0x32] = "Bandai",
[0x33] = "Ocean/Acclaim",
[0x34] = "Konami",
[0x35] = "Hector",
[0x36] = "UNKNOWN",
[0x37] = "Taito",
[0x38] = "Hudson",
[0x39] = "Banpresto",
[0x3a] = "UNKNOWN",
[0x3b] = "UNKNOWN",
[0x3c] = "UNKNOWN",
[0x3d] = "UNKNOWN",
[0x3e] = "UNKNOWN",
[0x3f] = "UNKNOWN",
[0x40] = "UNKNOWN",
[0x41] = "Ubi Soft",
[0x42] = "Atlus",
[0x43] = "UNKNOWN",
[0x44] = "Malibu",
[0x45] = "UNKNOWN",
[0x46] = "angel",
[0x47] = "Bullet-Proof",
[0x48] = "UNKNOWN",
[0x49] = "irem",
[0x4a] = "UNKNOWN",
[0x4b] = "UNKNOWN",
[0x4c] = "UNKNOWN",
[0x4d] = "UNKNOWN",
[0x4e] = "UNKNOWN",
[0x4f] = "UNKNOWN",
[0x50] = "Absolute",
[0x51] = "Acclaim",
[0x52] = "Activision",
[0x53] = "American sammy",
[0x54] = "Konami",
[0x55] = "Hi tech entertainment",
[0x56] = "LJN",
[0x57] = "Matchbox",
[0x58] = "Mattel",
[0x59] = "Milton Bradley",
[0x5a] = "UNKNOWN",
[0x5b] = "UNKNOWN",
[0x5c] = "UNKNOWN",
[0x5d] = "UNKNOWN",
[0x5e] = "UNKNOWN",
[0x5f] = "UNKNOWN",
[0x60] = "Titus",
[0x61] = "Virgin",
[0x62] = "UNKNOWN",
[0x63] = "UNKNOWN",
[0x64] = "LucasArts",
[0x65] = "UNKNOWN",
[0x66] = "UNKNOWN",
[0x67] = "Ocean",
[0x68] = "UNKNOWN",
[0x69] = "Electronic Arts",
[0x6a] = "UNKNOWN",
[0x6b] = "UNKNOWN",
[0x6c] = "UNKNOWN",
[0x6d] = "UNKNOWN",
[0x6e] = "UNKNOWN",
[0x6f] = "UNKNOWN",
[0x70] = "Infogrames",
[0x71] = "Interplay",
[0x72] = "Broderbund",
[0x73] = "sculptured",
[0x74] = "UNKNOWN",
[0x75] = "sci",
[0x76] = "UNKNOWN",
[0x77] = "UNKNOWN",
[0x78] = "THQ",
[0x79] = "Accolade",
[0x7a] = "UNKNOWN",
[0x7b] = "UNKNOWN",
[0x7c] = "UNKNOWN",
[0x7d] = "UNKNOWN",
[0x7e] = "UNKNOWN",
[0x7f] = "UNKNOWN",
[0x80] = "misawa",
[0x81] = "UNKNOWN",
[0x82] = "UNKNOWN",
[0x83] = "lozc",
[0x84] = "UNKNOWN",
[0x85] = "UNKNOWN",
[0x86] = "Tokuma Shoten Intermedia",
[0x87] = "Tsukuda Original",
[0x88] = "UNKNOWN",
[0x89] = "UNKNOWN",
[0x8a] = "UNKNOWN",
[0x8b] = "UNKNOWN",
[0x8c] = "UNKNOWN",
[0x8d] = "UNKNOWN",
[0x8e] = "UNKNOWN",
[0x8f] = "UNKNOWN",
[0x90] = "UNKNOWN",
[0x91] = "Chunsoft",
[0x92] = "Video system",
[0x93] = "Ocean/Acclaim",
[0x94] = "UNKNOWN",
[0x95] = "Varie",
[0x96] = "Yonezawa/s’pal",
[0x97] = "Kaneko",
[0x98] = "UNKNOWN",
[0x99] = "Pack in soft",
[0x9a] = "UNKNOWN",
[0x9b] = "UNKNOWN",
[0x9c] = "UNKNOWN",
[0x9d] = "UNKNOWN",
[0x9e] = "UNKNOWN",
[0x9f] = "UNKNOWN",
[0xa0] = "UNKNOWN",
[0xa1] = "UNKNOWN",
[0xa2] = "UNKNOWN",
[0xa3] = "UNKNOWN",
[0xa4] = "Konami (Yu-Gi-Oh!)"  
};

const char* cart_lic_name() {
    if (ctx.header->new_lic_code <= 0xA4) {
        return LIC_CODE[ctx.header->lic_code];
    }
    return "UNKNOWN";
}

const char* cart_type_name() {
    if (ctx.header->type <= 0x22) {
        return ROM_TYPES[ctx.header->type];
    }
    return "UNKNOWN";
}

bool cart_load(const char *cart, uint8_t *rom_buff, int n_bytes) {
    
    snprintf(ctx.filename, sizeof(ctx.filename), "%s", cart);               
    ctx.rom_data = (u8*)malloc(n_bytes * sizeof(u8));
    memcpy(ctx.rom_data, rom_buff, n_bytes);
    free(rom_buff);
   
    ctx.header = (rom_header *)(ctx.rom_data + 0x100);
    ctx.header->title[15] = 0;

    u16 x = 0;
    for (u16 i=0x0134; i<=0x014C; i++) {
        x = x - ctx.rom_data[i] - 1;
    }
    return true;
}

u8 cart_read(u16 address) {
    //for now just ROM ONLY type supported...

    return ctx.rom_data[address];
}

void cart_write(u16 address, u8 value) {
    //for now, ROM ONLY...

    //NO_IMPL
}
