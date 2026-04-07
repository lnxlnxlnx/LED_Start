# platform

这个目录包含了 ulOS 针对不同硬件平台的移植代码。

## 作用（例）

- **cortex-m3/**: ARM Cortex-M3 架构的移植代码
  - **ulOS_cpuport_asm.s**: Cortex-M3 架构相关的汇编代码，包含上下文切换等底层操作
  - **ulOS_cpuport.c**: Cortex-M3 架构相关的C语言代码，包含栈初始化、中断处理等

这个目录包含了针对特定CPU架构的移植代码，实现了与硬件相关的功能，如上下文切换、中断处理等，使得 ulOS 能够在不同的硬件平台上运行。