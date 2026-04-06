#ifndef A7B5D5BB_D091_4E04_8CB0_2109DD8D3222
#define A7B5D5BB_D091_4E04_8CB0_2109DD8D3222
#include "XSort.h"
#include "XVector.h"
#include <stdio.h>

// 1. 写比较函数
int Compare_Int(const void* a, const void* b)
{
    int num1 = *(int*)a;
    int num2 = *(int*)b;

    if (num1 < num2) return -1;
    if (num1 > num2) return 1;
    return 0;
}

void test_arr_quicksort(void)
{
    // 2. 定义数组
    int arr[] = { 9,5,1,7,3,8,2,4,6,0 };
    int n = sizeof(arr) / sizeof(arr[0]);

    // 3. 排序！
    XQuickSort(
        arr,                // 数组
        n,                  // 个数
        sizeof(int),        // 元素大小
        Compare_Int,        // 比较函数
        XSORT_ASC           // 升序
    );

    // 4. 打印看结果
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

// ==============================
// 测试：Vector + 快速排序 结合使用
// ==============================
void test_vector_quicksort(void)
{
    // 1. 创建一个存 int 的动态数组
    XVector* vec = XVector_create(sizeof(int));

    // 2. 添加乱序数据
    int data[] = {9,5,1,7,3,8,2,4,6,0};
    int count = sizeof(data)/sizeof(int);

    for(int i=0; i<count; i++)
    {
        XVector_push_back_base(vec, &data[i]);
    }

    // 3. 对 vector 内部数据进行快速排序
    // XQuickSort(
    //     vec,   // vector 内部数组地址
    //     XVector_size_base(vec),   // 元素个数
    //     sizeof(int),              // 元素大小
    //     Compare_Int,              // 比较函数
    //     XSORT_ASC                 // 升序
    // );
    printf("Before sorting XVector:\r\n");
    for(int i=0; i<XVector_size_base(vec); i++)
    {
        int* val = (int*)XVector_at_base(vec, i);
        printf("%d ", *val);
    }
    XVector_sort_base(vec, XSORT_ASC);
    printf("After sorting XVector:\r\n");
    // 4. 遍历打印排序结果
    for(int i=0; i<XVector_size_base(vec); i++)
    {
        int* val = (int*)XVector_at_base(vec, i);
        printf("%d ", *val);
    }
    printf("\r\n");

    // 5. 释放
    XVector_delete_base(vec);
}

#endif /* A7B5D5BB_D091_4E04_8CB0_2109DD8D3222 */
