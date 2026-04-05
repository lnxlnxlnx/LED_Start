# 11. XMemory 内存抽象与配置

这一份讲 XinYueC 的 `XMemory`。它是全库的内存统一入口，不只是 `malloc/free` 的别名。

## 1. XMemory 解决什么问题

目标是把内存策略从业务代码里抽离：

- 不同平台用不同分配器
- 库代码只调用 `XMemory_*`
- 方便替换为单片机内存池或 RTOS 分配器

## 2. 核心结构

`XMemory` 结构里有四个函数指针：

- `allocate`
- `deallocate`
- `reallocate`
- `callocZero`

所以它本质是“可注入的分配策略表”。

## 3. 常用 API

- `XMemory_setMethod`
- `XMemory_setMallocMethod`
- `XMemory_setDeleteMethod`
- `XMemory_setReallocMethod`
- `XMemory_setCallocMethod`
- `XMemory_malloc/free/realloc/calloc`

用法示例：

```c
XMemory mem = {my_malloc, my_free, my_realloc, my_calloc};
XMemory_setMethod(&mem);
```

## 4. 当前工程默认行为

`XMemory.c` 里根据平台宏选择默认实现：

- PC 平台一般是 `malloc/free/realloc/calloc`
- 嵌入式分支可走 `mymalloc/myfree/myrealloc`

这意味着你在 STM32 工程里可以无侵入切换底层内存实现。

## 5. `realloc` 不可用时的兜底

库里有 `XMemory_realloc_isNULL()` 检查。  
很多容器（例如 `XVector`）会在 `realloc` 不可用时走“`malloc + memcpy + free`”兜底路径。

意义：

- 保持兼容性
- 让缺失 `realloc` 的环境也能工作

## 6. 与容器的关系

XinYueC 绝大多数容器都通过 `XMemory_*` 管理内存，例如：

- `XVector` 扩容
- `XListSLinked` 节点创建/销毁
- `XVtable` 扩容

因此你一旦替换了 `XMemory`，等于替换了整套容器的内存后端。

## 7. 字节序读写接口

`XMemory_read_data` 和 `XMemory_write_data` 用于跨字节序读写：

- 读：从指定字节序流读到本机内存
- 写：把本机数据按指定字节序写出

在协议解析、寄存器序列化里很实用。

## 8. 常见风险点

1. 自定义 `free` 与 `malloc` 不匹配
2. 自定义 `realloc` 语义不标准
3. 容器中元素有动态成员，但未设置 `deinit` 回调
4. 多模块混用不同分配器导致跨模块释放错误

## 9. 最小实践建议

先做一个小实验：

1. 包装一层统计分配次数的 `my_malloc/my_free`
2. 用 `XMemory_setMethod` 注入
3. 跑 `XVector` 的 push/pop 测试

你会直观看到容器真实分配行为和扩容节奏。
