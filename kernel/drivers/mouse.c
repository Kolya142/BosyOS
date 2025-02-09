#include "mouse.h"
int MouseX = 0;
int MouseY = 0;
U8 MouseButtons = 0;

void PS2Write(U16 port, U8 data) {
    while (inb(0x64) & 2);
    outb(port, data);
}

U8 PS2Read(U16 port) {
    while (!inb(0x64) & 1);
    return inb(port);
}

void MouseInit() {
    PS2Write(0x64, 0xD4);
    PS2Write(0x60, 0xF4);
    usleep(100);
    U8 ack = PS2Read(0x60);
    if (ack == 0xFA) {
        U8 t = TTYCurrentColor;
        TTYSetColor(Black, Green);
        PS2Write(0x60, 0xF2);
        usleep(100);
        TTYPrint("Mouse Driver initialized\nMouse id: ");
        TTYCurrentColor = t;
        PrintHex(PS2Read(0x60));
        TTYPrintChar('\n');

        PS2Write(0x60, 0xE8);
        PS2Write(0x60, 3);
    }
    else {
        U8 t = TTYCurrentColor;
        TTYSetColor(Black, Red);
        TTYPrint("Mouse Driver initialization failed\n");
        TTYCurrentColor = t;
    }
}

void MouseUpd() {
    static volatile U8 packet[3];
    static volatile U8 packeti = 0;
    if (inb(0x64) & 0x01) {
        packet[packeti++] = inb(0x60);
        if (packeti == 3) {
            if (!(packet[0] & 0x08)) {
                packeti = 0;
                return;
            }
            int packetx = (packet[0] & (1<<4)) ? (~(int)(packet[1]-1)) : (packet[1]);
            int packety = (packet[0] & (1<<5)) ? (~(int)(packet[2]-1)) : (packet[2]);
            packety = -packety;
            
            if (packetx > -3 && packetx < 3) {
                packetx = 0;
            }
            if (packety > -3 && packety < 3) {
                packety = 0;
            }
            // if (packetx < -10) {packetx = -10;}
            // if (packety < -10) {packety = -10;}

            // if (packetx > 10) {packetx = 10;}
            // if (packety > 10) {packety = 10;}
            MouseButtons = packet[0] & 7;
            MouseX += packetx;
            MouseY += packety;
            if (MouseX < 0) {MouseX = 0;}
            if (MouseY < 0) {MouseY = 0;}
            packeti = 0;
        }
    }
}
