#include <stdint.h>

// pit: 0x09ca4b4d, 0x5fa68611:
/*0*/ pit_uptime(uint32_t *t);
/*1*/ pit_time(uint32_t *t);

// beep: 0x46ef3f2c, 0x27e134cd:
/*0*/ beep(*value & 0xFFFF);
/*1*/ beephz((*value >> 16) & 0xFFFF, *value & 0xFFFF);
/*2*/ beepspc((*value >> 16) & 0xFFFF, *value & 0xFFFF);

// ide: 0x6aa73a10, 0xa8b55cb4:
/*0*/ ide_read(IDEAsk *ask);
/*1*/ ide_write(IDEAsk *ask);

// mouse: 0x58f8d530, 0xfffe784f:
/*0*/ mouse_read(uint8_t *state); // state - {x, y, btn}

// keyboard: 0x0bb5676c, 0xff3ae302:
/*0*/ keyboard_key(uint8_t *key);
/*1*/ keyboard_sc(uint8_t *scancode);
/*2*/ keyboard_mod(uint8_t *mods); // Bits: {2 - ctrl, 1 - shift, 2 - super}
/*3*/ keyboard_keys(uint8_t **keys); // returns keys bytemap

// serial: 0xa3f13d05, 0x2eb0f0f:
/*0*/ serial_write(uint8_t *byte);
/*1*/ serial_read(uint8_t *byte);
/*2*/ serial_write(uint8_t *byte);
/*3*/ serial_setspeed(uint8_t *bod);

// network: 0xbb149088, 0xc3442e1e:
/*0*/ net_write(NetPackage* package);
/*1*/ net_read(NetPackage* package);
/*2*/ net_macget(NetMac* package);
/*3*/ net_status(uint32_t *stat);

// random: 0x8e6d1237, 0xc02dd685:
/*0*/ rand_dword(uint32_t *dw);
/*1*/ rand_word(uint16_t *w);
/*2*/ rand_byte(uint8_t *b);

// tty: 0xc0d05943, 0x8be5d999:
/*0*/ tty_uprint(const unsigned char *string);
// ...
// vga: 0xebbfd78c, 0x59687d45:
