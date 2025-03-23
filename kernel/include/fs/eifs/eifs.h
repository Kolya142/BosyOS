#include <fs/eifs/direntry.h>
#include <fs/eifs/defines.h>
#include <fs/eifs/inode.h>

U32 EIFIAlloc();
U32 EIFBAlloc();

EIFINode EIFIGet(U32 i);
U0 EIFBGet(U32 i, U8 *buf);