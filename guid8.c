// IS a Linux executable
#include <stdio.h>
#include <stdlib.h>

typedef struct GUIDM {
    unsigned short dd;
    unsigned short dm;
    unsigned short de;
    unsigned short df;
} GUIDM;

int randomR() {
    int buffer;
    FILE *f = fopen("/dev/urandom", "r");
    fread(&buffer, sizeof(buffer), 1, f);
    fclose(f);
    return buffer;
}

int main() {
    GUIDM guid;
    guid.dd = 0x1234 ^ (unsigned short)(randomR());
    guid.dm = 0x5678 ^ (unsigned short)(randomR());
    guid.de = 0x9abc ^ (unsigned short)(randomR());
    guid.df = 0xdef0 ^ (unsigned short)(randomR());

    printf("GUID-: %04x%04x-%04x%04x\n", guid.dd, guid.dm, guid.de, guid.df);
    return 0;
}