# XinYueC Study Index

这套文档面向你当前仓库中的 XinYueC 移植版本，重点是“会用 + 看懂源码设计”。

## Recommended Order

1. `01_快速上手.md`
2. `02_常用容器与使用套路.md`
3. `03_源码设计与分层.md`
4. `04_C语言写法与语法拆解.md`
5. `05_当前工程中的接入与注意事项.md`
6. `06_C模块抽象与接口设计.md`
7. `07_C底层写法与内存模型.md`
8. `08_C实现面向对象实战.md`
9. `09_补充细节_宏枚举与虚表继承多态.md`
10. `10_迭代器机制与实践.md`
11. `11_XMemory内存抽象与配置.md`
12. `12_XCompare比较器与排序查找.md`
13. `13_思考题与实战训练.md`

## In This Repository

- Source: `XinYueC/`
- Public headers: `XinYueC/include/`
- Existing tests: `IO/XinYue/test_vector.h`, `IO/XinYue/test_quickSort.h`
- Current entry: `USER/main.c`

## How To Use This Set

- 如果你现在主要目标是“先写得出来”，优先看 `01 + 02 + 05`。
- 如果你主要目标是“看懂框架怎么搭”，优先看 `03 + 06 + 08`。
- 如果你主要目标是“补底层基础”，优先看 `04 + 07`。

建议每读完一份就配一个最小测试函数到 `IO/XinYue/`，再从 `USER/main.c` 临时调用验证。
