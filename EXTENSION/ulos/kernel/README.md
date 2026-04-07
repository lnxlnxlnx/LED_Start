# kernel

这个目录包含了 ulOS 的核心内核代码。

## 作用

- **inc/**: 内核相关的头文件
  - **ul_heap.h**: 堆内存管理功能的声明
  - **ul_libc.h**: C标准库功能的声明
  - **ul_object.h**: 对象管理功能的声明
  - **ulOS_idle.h**: 空闲线程相关的声明
  - **ulOS_ipc.h**: 进程间通信（IPC）机制的声明
  - **ulOS_mutex.h**: 互斥锁功能的声明
  - **ulOS_thread.h**: 线程管理功能的声明
  - **ulOS_timer.h**: 定时器功能的声明

- **src/**: 内核相关的源文件
  - **ul_heap.c**: 堆内存管理功能的实现
  - **ul_libc.c**: C标准库功能的实现
  - **ul_object.c**: 对象管理功能的实现
  - **ulOS_idle.c**: 空闲线程的实现
  - **ulOS_ipc.c**: 进程间通信（IPC）机制的实现
  - **ulOS_mutex.c**: 互斥锁功能的实现
  - **ulOS_thread.c**: 线程管理功能的实现
  - **ulOS_timer.c**: 定时器功能的实现

这个目录是 ulOS 的核心，提供了操作系统的基本功能，包括线程管理、进程间通信、内存管理、定时器等。