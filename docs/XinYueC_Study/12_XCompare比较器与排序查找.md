# 12. XCompare 比较器与排序查找

这一份讲 `XCompare` 在 XinYueC 里的作用。  
一句话：**比较器是排序、查找、优先队列行为的核心契约**。

## 1. 比较器签名和返回值约定

签名：

```c
typedef int32_t(*XCompare)(const void* lhs, const void* rhs);
```

返回值约定：

- `XCompare_Less`（-1）
- `XCompare_Equality`（0）
- `XCompare_Greater`（1）

你自己写 compare 时，尽量只返回这三种值，避免语义漂移。

## 2. 库里已有的比较函数

`XCompare.c` 提供了基础类型 compare，例如：

- `XCompare_int`
- `XCompare_uint32_t`
- `XCompare_float`

这些由宏批量生成，先用它们能减少错误。

## 3. compare 在哪些地方会用到

1. 原生数组排序：`XQuickSort(..., compare, order)`
2. 容器排序：`XVector_sort_base`
3. 容器查找：`XVector_find_base`、`indexOf/lastIndexOf`
4. 优先队列：`XPriorityQueue`

## 4. 原生数组 vs 容器排序

### 原生数组

compare 作为参数传入：

```c
XQuickSort(arr, n, sizeof(int), (XCompare)XCompare_int, XSORT_ASC);
```

### 容器

compare 存在对象内部字段 `m_compare`，需先设置：

```c
XContainerSetCompare(vec, (XCompare)XCompare_int);
XVector_sort_base(vec, XSORT_ASC);
```

这一点是最容易忘的。

## 5. 自定义结构体 compare 模板

```c
typedef struct
{
    int id;
    int score;
} Student;

int32_t Student_compare(const void* lhs, const void* rhs)
{
    const Student* a = (const Student*)lhs;
    const Student* b = (const Student*)rhs;

    if (a->score < b->score) return XCompare_Less;
    if (a->score > b->score) return XCompare_Greater;
    return XCompare_Equality;
}
```

然后：

```c
XVector* vec = XVector_Create(Student);
XContainerSetCompare(vec, Student_compare);
```

## 6. `XCompareRuleOne/Two` 是什么

`XFunctionCallback` 里还有规则函数（`RuleOne/RuleTwo`），本质是对 compare 再包一层策略。  
可以用于不同数据结构的判定规则统一（如 map/set/binary tree）。

如果你现在主要在学 `vector/sort/find`，先掌握基础 `XCompare` 即可，规则函数可后看。

## 7. 比较器设计的三个规则

1. 自反性：`cmp(a, a) == Equality`
2. 对称性：`cmp(a, b)` 与 `cmp(b, a)` 符号相反
3. 传递性：`a<b 且 b<c` 时应有 `a<c`

不满足这些规则会导致：

- 排序结果不稳定
- 查找行为异常
- 优先队列顺序错乱

## 8. 常见坑

1. compare 里直接返回 `a-b`（可能溢出）
2. 结构体 compare 忽略关键字段
3. 容器排序前忘了 `XContainerSetCompare`
4. compare 与业务“相等定义”不一致

## 9. 建议练习

1. 写 `Student_compare_by_score`
2. 写 `Student_compare_by_id`
3. 同一批数据分别排序，观察行为差异

通过这个练习你会真正理解：compare 决定的是“顺序语义”，不只是技术细节。
