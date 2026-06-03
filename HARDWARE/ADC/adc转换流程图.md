::: mermaid
graph TD
    A[开始] --> B[系统及定时器初始化]
    B --> C[while循环]
    subgraph 定时器中断服务函数
        TIM[定时器中断] --> SMG[数码管扫描]
        TIM --> D1{adc_t++ > 200?}
        D1 -- 是 --> ADC[ADC采集] --> DISP[数码管显示] --> END[退出中断]
        D1 -- 否 --> D2{led_t++ > 500?}
        D2 -- 是 --> LED[led翻转] --> END
        D2 -- 否 --> END
    end
    C -.循环等待中断触发.-> TIM
:::