#ifndef XDEMO_H
#define XDEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "XClass.h"

/*
 * A minimal "class" template based on XinYue style:
 * - Inherit from XClass
 * - Define vtable enum slots
 * - Provide create/init + base dispatch APIs
 */
XCLASS_DEFINE_BEGING(XDemo)
XCLASS_DEFINE_ENUM(XDemo, Start) = XCLASS_VTABLE_GET_SIZE(XClass),
XCLASS_DEFINE_ENUM(XDemo, Stop),
XCLASS_DEFINE_END(XDemo)

typedef struct XDemo
{
    XClass m_class; /* must be the first member */
    int mode;
} XDemo;

XVtable* XDemo_class_init(void);

XDemo* XDemo_create(void);
void XDemo_init(XDemo* object);

void XDemo_copy_base(XDemo* object, const XDemo* src);
void XDemo_move_base(XDemo* object, XDemo* src);
void XDemo_deinit_base(XDemo* object);
void XDemo_delete_base(XDemo* object);

bool XDemo_start_base(XDemo* object);
bool XDemo_stop_base(XDemo* object);

#ifdef __cplusplus
}
#endif

#endif /* XDEMO_H */

