#include <user/bsf.h>
#include <misc/meml.h>

Bool BsfExec(BsfApp *app)
{
    BsfHeader *head = &app->header;
    if (head->Magic != 'BOSY') {
        return False;
    }
    Ptr *bind = (Ptr)0x5000;
    MemCpy(bind, app->data, head->CodeS);

    U0 (*entry)() = (U0(*)())bind;
    asmv("pusha\npushf");
    entry();
    asmv("popf\npopa");
    return True;
}