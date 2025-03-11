#include <lib/Time.h>
#include <fs/vfs.h>

VFSNode *VFS;

U0 VFSInit() {
    VFS = MAlloc(sizeof(VFSNode) * 32);
}
U32 VFSRead(String name, Ptr buf, U32 count) {
    for (U32 i = 0; i < 32; ++i) {
        
    }
}
U32 VFSWrite(String name, Ptr buf, U32 count) {

}
U0 VFSReadDir(String name, U0(*reader)(String, VFSStat*)) {

}
U0 VFSMount(U0(*read)(String, Ptr, U32), U0(*write)(String, Ptr, U32), U0(*list)(String, U0(*reader)(String, VFSStat*))) {

}