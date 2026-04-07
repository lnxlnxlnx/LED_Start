#include "XDemo.h"
#include "XMemory.h"
#include <string.h>

/* virtual functions */
static void VXClass_copy(XDemo* object, const XDemo* src);
static void VXClass_move(XDemo* object, XDemo* src);
static void VXClass_deinit(XDemo* object);
static bool VXDemo_start(XDemo* object);
static bool VXDemo_stop(XDemo* object);

XVtable* XDemo_class_init(void)
{
    XVTABLE_CREAT_DEFAULT

#if VTABLE_ISSTACK
    XVTABLE_STACK_INIT_DEFAULT(XCLASS_VTABLE_GET_SIZE(XDemo))
#else
    XVTABLE_HEAP_INIT_DEFAULT
#endif

    /* inherit XClass slots first */
    XVTABLE_INHERIT_DEFAULT(XClass_class_init());

    /* append XDemo own virtual functions in enum order */
    void* table[] = { VXDemo_start, VXDemo_stop };
    XVTABLE_ADD_FUNC_LIST_DEFAULT(table);

    /* override base class lifecycle functions */
    XVTABLE_OVERLOAD_DEFAULT(EXClass_Copy, VXClass_copy);
    XVTABLE_OVERLOAD_DEFAULT(EXClass_Move, VXClass_move);
    XVTABLE_OVERLOAD_DEFAULT(EXClass_Deinit, VXClass_deinit);

    return XVTABLE_DEFAULT;
}

void XDemo_init(XDemo* object)
{
    if (ISNULL(object, ""))
        return;
    memset(object, 0, sizeof(XDemo));
    XClass_init((XClass*)object);
    XClassGetVtable(object) = XDemo_class_init();
    object->mode = 0;
}

XDemo* XDemo_create(void)
{
    XDemo* object = (XDemo*)XMemory_malloc(sizeof(XDemo));
    if (object == NULL)
        return NULL;
    XDemo_init(object);
    return object;
}

static void VXClass_copy(XDemo* object, const XDemo* src)
{
    if (ISNULL(object, "") || ISNULL(src, ""))
        return;
    object->mode = src->mode;
    XClassGetVtable(object) = XDemo_class_init();
}

static void VXClass_move(XDemo* object, XDemo* src)
{
    if (ISNULL(object, "") || ISNULL(src, ""))
        return;
    object->mode = src->mode;
    src->mode = 0;
    XClassGetVtable(object) = XDemo_class_init();
}

static void VXClass_deinit(XDemo* object)
{
    if (ISNULL(object, ""))
        return;
    object->mode = 0;
}

static bool VXDemo_start(XDemo* object)
{
    if (ISNULL(object, ""))
        return false;
    object->mode = 1;
    return true;
}

static bool VXDemo_stop(XDemo* object)
{
    if (ISNULL(object, ""))
        return false;
    object->mode = 0;
    return true;
}

