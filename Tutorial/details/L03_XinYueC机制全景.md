# L03 XinYueC 机制全景（第 3 周）

## 1. 学习目标

这节课结束后，你应该能做到：

1. 解释“宏枚举 -> 虚表 -> 派发”完整链路。
2. 说清 `XVTABLE_INHERIT_DEFAULT` 为什么必须先写。
3. 追踪一次 `XVector_push_back_base` 到真实函数实现。

## 2. 知识点讲解（初学者版）

## 2.1 先理解一句话

XinYueC 是在 C 里手工模拟“类继承 + 虚函数多态”。

它靠的是三样东西：

1. 宏枚举：给虚函数槽位编号
2. 虚函数表：函数指针数组
3. 派发宏：按编号调用真实函数

## 2.2 宏枚举在做什么

你会看到：

```c
XCLASS_DEFINE_BEGING(XVector)
XCLASS_DEFINE_ENUM(XVector, Push_Back)
...
XCLASS_DEFINE_END(XVector)
```

本质是生成：

```c
enum XVectorVtableEnum { EXVector_Push_Back, ... };
```

这个 `EXVector_Push_Back` 就是“虚表索引”。

## 2.3 虚表初始化三步

在 `XVector_class_init` 里，核心顺序是：

1. 创建虚表（栈上或堆上）
2. 继承父类槽位：`XVTABLE_INHERIT_DEFAULT(XContainerObject_class_init())`
3. 追加子类函数：`XVTABLE_ADD_FUNC_LIST_DEFAULT(table)`
4. 覆盖父类行为（可选）：`XVTABLE_OVERLOAD_DEFAULT(...)`

注意：第 2 步不能省。省了父类槽位就断了。

## 2.4 多态派发怎么发生

`XVector_push_back_base` 不直接写死调用某函数，而是：

1. 拿对象里的 `m_vtable`
2. 用 `EXVector_Push_Back` 取槽位
3. 强转成目标函数签名并调用

这就是运行时多态。

## 3. 本仓库动手步骤

目标：写出一份 `push_back` 调用链笔记。

### Step 1：从 API 入口开始

看 `XVector.c` 的 `XVector_push_back_base`。

### Step 2：定位索引

看 `XVector.h` 的 `EXVector_Push_Back` 枚举位置。

### Step 3：定位虚表填充

看 `XVector_virtual.c` 的 `table[]`，确认 `VXVector_push_back` 的顺序。

### Step 4：写 5 行调用链

模板：

1. `XVector_init` 绑定 `m_vtable`
2. `XVector_class_init` 继承并填表
3. `XVector_push_back_base` 发起派发
4. `XClassGetVirtualFunc(...EXVector_Push_Back...)` 取函数
5. `VXVector_push_back` 真正执行

## 4. 常见坑

1. `enum` 顺序和 `table[]` 顺序不一致，调用错位。
2. 只追加不继承，父类槽位缺失。
3. 函数签名强转不匹配，运行时异常。

## 5. 课后作业与思考题

## 作业

再追踪一次 `XVector_sort_base`，写出同样的 5 步调用链。

## 思考题

1. 如果把 `XVTABLE_OVERLOAD_DEFAULT(EXClass_Copy, ...)` 去掉，会影响什么？
2. 为什么这套机制在嵌入式里有价值，而不是直接写大量 `if/else`？

## 6. 验收标准

- 能口头解释 `XVTABLE_INHERIT_DEFAULT` 的作用。
- 能画出一条完整派发链（函数名级别）。
- 能指出至少一个“宏错位导致的风险”。

