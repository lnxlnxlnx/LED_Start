#include "XDemo.h"
#include "XMemory.h"

void XDemo_copy_base(XDemo* object, const XDemo* src)
{
    if (ISNULL(object, "") || ISNULL(src, "") || ISNULL(XClassGetVtable(src), ""))
        return;
    XClassGetVirtualFunc(src, EXClass_Copy, void(*)(XDemo*, const XDemo*))(object, src);
}

void XDemo_move_base(XDemo* object, XDemo* src)
{
    if (ISNULL(object, "") || ISNULL(src, "") || ISNULL(XClassGetVtable(src), ""))
        return;
    XClassGetVirtualFunc(src, EXClass_Move, void(*)(XDemo*, XDemo*))(object, src);
}

void XDemo_deinit_base(XDemo* object)
{
    if (ISNULL(object, "") || ISNULL(XClassGetVtable(object), ""))
        return;
    XClassGetVirtualFunc(object, EXClass_Deinit, void(*)(XDemo*))(object);
}

void XDemo_delete_base(XDemo* object)
{
    if (ISNULL(object, ""))
        return;
    XDemo_deinit_base(object);
    XMemory_free(object);
}

bool XDemo_start_base(XDemo* object)
{
    if (ISNULL(object, "") || ISNULL(XClassGetVtable(object), ""))
        return false;
    return XClassGetVirtualFunc(object, EXDemo_Start, bool(*)(XDemo*))(object);
}

bool XDemo_stop_base(XDemo* object)
{
    if (ISNULL(object, "") || ISNULL(XClassGetVtable(object), ""))
        return false;
    return XClassGetVirtualFunc(object, EXDemo_Stop, bool(*)(XDemo*))(object);
}

