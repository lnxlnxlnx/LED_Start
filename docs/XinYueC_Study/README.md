# XinYueC 学习索引

这套资料是按你当前仓库里的移植结果写的，不是脱离工程的通用教程。

## 先看哪里

建议按这个顺序阅读：

1. `01_快速上手.md`
2. `02_常用容器与使用套路.md`
3. `03_源码设计与分层.md`
4. `04_C语言写法与语法拆解.md`
5. `05_当前工程中的接入与注意事项.md`

## 这个库在当前工程里的位置

- 源码目录：`XinYueC/`
- 头文件目录：`XinYueC/include/`
- 当前测试入口：`IO/XinYue/test_vector.h`、`IO/XinYue/test_quickSort.h`
- 当前调用位置：`USER/main.c`

## 先建立一个整体认识

XinYueC 不是单纯的“算法函数合集”，它更像一套用 C 写出来的“泛型容器 + 伪面向对象”框架。

它的核心思路是：

- 用 `void* + typeSize` 处理任意类型数据
- 用函数指针回调处理比较、拷贝、移动、析构
- 用虚函数表 `XVtable` 模拟 C++ 的多态
- 用“结构体首成员继承”模拟类继承
- 用宏包装出“更像模板”的调用方式

所以学习它时，最好分两层看：

- 第一层：先把 `XVector / XStack / XQueue / XPriorityQueue` 用起来
- 第二层：再理解 `XClass / XVtable / XContainerObject` 是怎么把这些容器统一起来的

## 一个很重要的提醒

这个库的头文件注释很多，但“注释描述”和“当前源码真实行为”并不总是完全一致。学习时要以实际实现为准，尤其要多看：

- `XVector.c`
- `XVector_virtual.c`
- `XQueue.c`
- `XPriorityQueue.c`
- `XContainerObject.h`
- `XClass.h`

最后一份文档里我专门整理了当前版本的几个使用注意点和容易踩坑的地方。
