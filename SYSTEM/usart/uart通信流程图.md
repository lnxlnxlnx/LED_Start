::: mermaid
graph TD
A[开始] --> B[系统及定时器初始化]
B --> C[while循环]
C--> D[打印STM32F103RCT6
LED翻转]
D --> E[延时]
E --> C
:::