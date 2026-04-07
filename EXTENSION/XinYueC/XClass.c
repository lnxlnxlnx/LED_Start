#include"XClass.h"
#include"XVtable.h"
#include"XMemory.h"
#include<stdio.h>
#if defined(STM32F10X_LD) || defined(STM32F10X_MD) || defined(STM32F10X_HD) || defined(STM32F10X_XL) || defined(STM32F10X_CL)
#include "stm32f10x.h"
#define XASSERT_TARGET_STM32 1
#else
#define XASSERT_TARGET_STM32 0
#endif
bool ArgIsNULL(const void* args/*参数数值*/, const char* argsName/*参数名字*/, const char* str/*附加参数*/, const char* funcName/*函数名字*/, const char* filePath/*所在文件路径*/, int line/*所在行号*/)
{
	if (args == NULL)
	{
		printf("%s\n参数:%s是NULL\t函数名:%s\n文件路径:%s\n正在编译文件的行号:%d\n", str, argsName, funcName, filePath, line);
		return true;
	}
	return false;
}

void xassert_failed(const char* expr, const char* str, const char* filePath, const char* funcName, int line)
{
	printf("XAssert触发: expr=%s info=%s func=%s file=%s line=%d\n", expr, str, funcName, filePath, line);

#if XASSERT_TARGET_STM32
	/* NOTE: 替代策略（STM32）：
	 * 1) 调试阶段：停机并触发 BKPT，便于定位调用栈；
	 * 2) 发布阶段：执行系统复位，避免卡死在异常状态。 */
#if ((defined(DEBUG_ON) && (DEBUG_ON)) || defined(_DEBUG) || defined(DEBUG))
	__disable_irq();
	__BKPT(0);
	while (1)
	{
	}
#else
	NVIC_SystemReset();
	while (1)
	{
	}
#endif
#else
	/* NOTE: 非 STM32 平台兜底策略：保留死循环，后续可替换为日志上报或平台专用异常处理。 */
	while (1)
	{
	}
#endif
}

void XClass_deinit_base(XClass* object)
{
	if (ISNULL(object, "") || ISNULL(XClassGetVtable(object), ""))
		return;
	XClassGetVirtualFunc(object, EXClass_Deinit, void(*)(XClass*))(object);//底层是一个void**数组，所以需要强转成对应的函数指针类型(所以这里要传入Type)
}
void XClass_copy_base(XClass* object, const XClass* src)
{
	if (ISNULL(src, "") || ISNULL(XClassGetVtable(src), ""))
		return;
	XClassGetVirtualFunc(src, EXClass_Copy, void(*)(XClass*,const XClass*))(object,src);
}
void XClass_move_base(XClass* object, XClass* src)
{
	if (ISNULL(src, "") || ISNULL(XClassGetVtable(src), ""))
		return;
	XClassGetVirtualFunc(src, EXClass_Move, void(*)(XClass*, XClass*))(object,src);
}
void XClass_delete_base(XClass* object)
{
	XClass_deinit_base(object);
	XMemory_free(object);
}
