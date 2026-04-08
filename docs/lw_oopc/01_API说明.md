# lw_oopc API 说明

一句话理解：`lw_oopc` 是一套“用 C 宏模拟 OOP（对象/继承/多态/接口/析构）”的轻量框架。

## 🎯 1. 头文件与配置宏

头文件：`EXTENSION/lw_oopc/lw_oopc.h`

### 1.1 偏移量配置（二选一）

- `#define LW_OOPC_USE_STDDEF_OFFSETOF`
- `#define LW_OOPC_USE_USER_DEFINED_OFFSETOF`

默认已启用 `LW_OOPC_USE_STDDEF_OFFSETOF`。

### 1.2 内存泄漏检测相关（可选）

- `LW_OOPC_SUPPORT_MEMORY_LEAK_DETECTOR`
- `LW_OOPC_PRINT_DEBUG_INFO`

默认未启用泄漏检测。未启用时：

- `lw_oopc_file_line` 展开为空
- `lw_oopc_free` 映射到 `free`
- `*_new()` 走普通 `malloc`

## ✨ 2. 类型与基础函数

### 2.1 布尔类型

- `typedef int lw_oopc_bool`
- `lw_oopc_true`
- `lw_oopc_false`

### 2.2 公共函数

- `void lw_oopc_report(void);`
  - 未开启泄漏检测时为空实现
  - 开启泄漏检测时用于输出泄漏报告

### 2.3 内存函数（仅泄漏检测开启时有效）

- `void* lw_oopc_malloc(size_t size, const char* type, const char* file, int line);`
- `void lw_oopc_free(void* memblock);`

## 🔍 3. 类/接口声明宏

### 3.1 `INTERFACE(type)`

用于声明接口，本质是只包含函数指针的结构体。

```c
INTERFACE(IMoveable)
{
    void (*move)(IMoveable* t);
};
```

### 3.2 `ABS_CLASS(type)`

用于声明抽象类：有构造/析构声明，但通常不直接实例化。

```c
ABS_CLASS(Animal)
{
    void (*eat)(Animal* t);
};
```

### 3.3 `CLASS(type)`

用于声明可实例化类，自动声明：

- `type* type_new(...)`
- `void type_ctor(type* t)`
- `int type_dtor(type* t)`
- `void type_delete(type* t)`

## ✅ 4. 构造/析构实现宏

### 4.1 `CTOR(type)` + `END_CTOR`

定义类构造函数体，并自动生成 `type_new()`。

### 4.2 `DTOR(type)` + `END_DTOR`

定义析构逻辑。`type_delete()` 会调用 `type_dtor()`，当 `type_dtor()` 返回 `lw_oopc_true` 时释放内存。

### 4.3 `ABS_CTOR(type)` + `END_ABS_CTOR`

用于抽象类构造函数实现。

## 💡 5. 继承/接口/转型宏

### 5.1 成员声明

- `EXTENDS(FatherType)`：声明“继承父类”的嵌入成员
- `IMPLEMENTS(InterfaceType)`：声明“实现接口”的嵌入成员

### 5.2 父类构造调用

- `SUPER_CTOR(FatherType)`：在子类构造函数里调用父类构造

### 5.3 向上/向下转型

- `SUPER_PTR(cthis, father)`
- `SUPER_PTR_2(...)` / `SUPER_PTR_3(...)`
- `SUB_PTR(selfptr, self, child)`
- `SUB_PTR_2(...)` / `SUB_PTR_3(...)`

### 5.4 函数绑定

- `FUNCTION_SETTING(field, func)`：给函数指针字段赋实现函数

### 5.5 访问父类字段

- `INHERIT_FROM(father, cthis, field)`

## 🔥 6. 典型使用模板

```c
CLASS(Dog)
{
    EXTENDS(Animal);
    IMPLEMENTS(IMoveable);
};

static void Dog_move(IMoveable* t) { (void)t; }

CTOR(Dog)
SUPER_CTOR(Animal);
FUNCTION_SETTING(IMoveable.move, Dog_move);
END_CTOR

DTOR(Dog)
return Animal_dtor(SUPER_PTR(cthis, Animal));
END_DTOR
```

## ⚠️ 7. 使用约束（必须注意）

1. `*_new()` 创建的对象，必须走 `*_delete()` 回收，不要直接 `free`。
2. 析构链要手动维护，子类 `dtor` 应明确调用父类 `dtor`。
3. 如果对象内部持有其他对象指针，必须在 `finalize/dtor` 中释放。
4. 向下转型时务必保证真实类型正确，`SUB_PTR` 不做运行时类型检查。
5. 在本仓库里请包含 `lw_oopc.h`，不要包含标准 `<malloc.h>`（已改为 `<stdlib.h>` 方案）。
