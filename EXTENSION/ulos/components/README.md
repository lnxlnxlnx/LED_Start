# component

这个目录包含了 ulOS 的各种硬件组件驱动和通用功能模块。

## 作用

- **ul_at24cxx.h/.c**: AT24CXX 系列 I2C 接口的EEPROM存储器驱动
- **ul_button.h/.c**: 按钮组件驱动，处理按键的检测和防抖
- **ul_buzzer.h/.c**: 蜂鸣器组件驱动
- **ul_crc.h/.c**: CRC校验算法实现
- **ul_filter.h/.c**: 各种滤波算法（如移动平均、卡尔曼滤波等）
- **ul_ina226.h/.c**: INA226 电源监控芯片驱动
- **ul_max6675.h/.c**: MAX6675 热电偶温度传感器驱动
- **ul_pid.h/.c**: PID控制器算法实现
- **ul_pin.h/.c**: 引脚操作相关的通用功能
- **ul_softwarei2c.h/.c**: 软件模拟I2C通信协议
- **ul_softwarespi.h/.c**: 软件模拟SPI通信协议
- **ul_swtimer.h/.c**: 软件定时器功能实现
- **ul_topic.h/.c**: 主题/事件发布订阅系统
- **ul_w25qxx.h/.c**: W25QXX 系列SPI Flash存储器驱动

这些组件为基于 ulOS 的嵌入式项目提供了常用的硬件外设驱动和算法模块，方便开发者快速集成各种硬件功能。