# LED_Start — STM32F103RC 裸机项目

## 平台

- **MCU**: STM32F103RC (Cortex-M3, 72MHz, 256KB Flash, 48KB SRAM)
- **开发板**: 正点原子 NANO STM32F103
- **工具链**: Keil uVision (uvprojx) + ARMCC 5
- **标准外设库**: STM32F10x Standard Peripherals Library v3.5.0
- **预定义宏**: `STM32F10X_HD`, `USE_STDPERIPH_DRIVER`

## 目录结构

```
LED_Start/
├── USER/              # 应用入口(main.c)、中断(stm32f10x_it.c)、Keil项目文件
├── HARDWARE/          # 板级外设驱动
│   ├── LED/           #   PC0~PC7 LED 驱动
│   ├── KEY/           #   按键 GPIO + KEY_Scan 轮询
│   ├── SMG/           #   数码管 74HC138(位选) + 74HC595(段选) 驱动
│   ├── TIMER/         #   定时器配置 + 软件 tick 基座 (TIM3 1ms 主调度)
│   ├── EXTI/          #   外部中断 (被秒表项目禁用)
│   ├── PWM/           #   TIM3 PWM 输出 (PC6/PC7)
│   ├── ADC/           #   ADC1 CH9 (PB1) 电位器
│   ├── DAC/           #   PA4 DAC 输出
│   ├── BEEP/          #   PB8 蜂鸣器
│   ├── REMOTE/        #   PB0 红外接收 (NEC 协议)
│   ├── RTC/           #   LSE 32.768kHz 日历
│   ├── DMA/           #   USART1 TX DMA
│   ├── DHT11/         #   PB9 温湿度传感器
│   ├── DS18B20/       #   PB9 温度传感器 (与DHT11冲突)
│   ├── TPAD/          #   PB1 触摸按键
│   └── WKUP/          #   PA0 待机唤醒
├── SYSTEM/            # 平台基础代码
│   ├── delay/         #   SysTick 轮询延时 (us/ms/s)
│   ├── sys/           #   时钟配置、NVIC、位带操作宏
│   └── usart/         #   USART1 (PA9/PA10) printf 重定向
├── EXTENSION/         # 第三方组件
│   ├── MultiButton/   #   轻量级按键状态机
│   ├── EasyLogger/    #   日志库 (按模块级别过滤)
│   └── ulos/          #   微型 RTOS 内核
├── CORE/              # CMSIS (core_cm3.h, 启动文件)
├── STM32F10x_FWLib/   # STM32F10x 标准外设库
├── IO/                # 特性级演示代码 + 状态机框架
├── CONFIG/            # 日志级别配置
├── OBJ/               # 构建产物
├── doc/               # 文档
└── 引脚分配表.md       # GPIO 引脚功能速查
```

## 当前唯一的应用: 多功能秒表系统

`main.c` 初始化的外设按顺序：
```
Stm32_Clock_Init(9) → uart_init(115200) → LED_Init()
→ LED_SMG_Init() → My_KEY_Init() → My_TIM3_Init(999, 72-1) → Stopwatch_Init()
```

### 秒表核心文件

| 文件 | 关键点 |
|------|--------|
| `HARDWARE/TIMER/stopwatch.c` | 全部秒表逻辑（ISR 驱动） |
| `HARDWARE/TIMER/stopwatch.h` | 秒表 API + 状态/时间类型 |
| `USER/main.c` | 入口，初始化后空循环 |
| `HARDWARE/TIMER/timer.c` | TIM3 1ms 中断 → 调用 `LED_SMG_Scan()` + `Stopwatch_TimerTick()` |
| `HARDWARE/SMG/smg.c` | 74HC138 + 74HC595 动态扫描，定时器中断中调用 `LED_SMG_Scan()` |

### 按键

KEY1 (PC9), KEY2 (PD2), WK_UP (PA0) — 在 TIM3 ISR 内用 **软件消抖**（20ms 连续采样），不需要 `KEY_Scan()`。

### 时间基准自动适配

`My_TIM3_Init(arr, psc)` 计算 `g_tim3.tick_us`，秒表用 `TIMER_MS(&g_tim3, ms)` 宏把所有超时值转为 tick 数：
- `g_tick_10ms` = 厘秒间隔
- `g_freeze_limit` = 冻结 3 秒
- `g_flow_interval` = 流水灯 500ms
- `g_key_debounce` = 按键消抖 20ms

改 arr/psc 后这些值自动缩放。

## 构建

- 打开 `USER/F103RC.uvprojx` → 编译下载
- 包含路径：`.\HARDWARE\LED;.\HARDWARE\TIMER;.\HARDWARE\SMG;.\HARDWARE\KEY;.\SYSTEM\delay;.\SYSTEM\sys;.\SYSTEM\usart;.\USER;.\CORE;.\STM32F10x_FWLib\inc`

## 增加新模块的步骤

1. 在 `HARDWARE/` 下建目录，写 `.c/.h`
2. 确认 `stm32f10x_conf.h` 里启用了所需标准库头文件
3. 在 Keil 里 Add Files 到对应组
4. 在 `main.c` 中 `#include` 并初始化
5. 如果功能需要周期性执行 → 在 `TIM3_IRQHandler` 中调用

## 重要警告

- **EXTIX_Init() 被禁用**：秒表用软件消抖处理按键，EXTI 中断冲突
- **USE_LED / USE_IC_UPDATE 宏为 0**：旧版测试代码已禁用
- 不要在主循环里用 `delay_ms()` 超过 100ms 的阻塞延时，否则按键响应会迟钝（按键在 ISR 中已有消抖）
