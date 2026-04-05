# 03. 设计模式与 XinYueC 映射

这份是“模式 -> 代码机制 -> 你项目中的落点”映射表。

## Strategy（策略模式）

- XinYue 机制：`XCompare`、`XMemory_setMethod`、`XCDataCopyMethod`
- 你的落点：排序规则切换、内存后端切换、数据处理规则切换
- 典型收益：去掉 `if/else` 选择分支，改为运行时注入

## Iterator（迭代器模式）

- XinYue 机制：`XVector_begin/end`、`for_each_iterator`
- 你的落点：容器遍历、过滤、统计
- 典型收益：遍历逻辑统一，减少裸指针重复代码

## Factory（工厂模式）

- XinYue 机制：`create/init` 二段式生命周期
- 你的落点：传感器/设备实例创建、模块初始化入口统一
- 典型收益：减少创建逻辑分散与重复初始化错误

## State（状态模式）

- XinYue 机制：已有状态机写法可抽象（如按键/LED流程）
- 你的落点：按键流程、通信流程、任务流程
- 典型收益：复杂分支可视化，新增状态成本降低

## Adapter（适配器模式）

- XinYue 机制：统一接口 + 函数指针表
- 你的落点：不同驱动层（HAL/寄存器）兼容
- 典型收益：上层逻辑不依赖底层实现细节

## Facade（外观模式）

- XinYue 机制：模块封装接口
- 你的落点：对外暴露 `BoardService_xxx` 统一入口
- 典型收益：业务层调用更简单，耦合面变小

## Command（命令模式）

- XinYue 机制：`XQueue` / `XPriorityQueue`
- 你的落点：任务队列、指令分发、串口命令执行
- 典型收益：请求排队、重试、日志回放更容易

## “先学哪个”建议

按项目收益排序建议：

1. Strategy
2. State
3. Adapter
4. Factory
5. Iterator
6. Command
7. Facade

