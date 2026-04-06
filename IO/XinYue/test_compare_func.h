#ifndef D26599CA_77AE_4736_8473_5AB8BB35175C
#define D26599CA_77AE_4736_8473_5AB8BB35175C
#include "XCompare.h"
#include "XVector.h"
#include "stdlib.h"
#include <elog.h>

typedef struct
{
    int id;
    int score;
} Student;

int32_t Student_compare(const void* a, const void* b)
{
    const Student* lhs = (const Student*)a;
    const Student* rhs = (const Student*)b;
    if (lhs->score < rhs->score) return XCompare_Less;
    if (lhs->score > rhs->score) return XCompare_Greater;
    return lhs->id - rhs->id; // 如果分数相同，按ID升序排序
    //NOTE:这里两个qsort结果是不同的，stdlib中的是升序，XCompare是降序
}

void test_xcompare(void)
{
    Student students[] = {
        {1, 85},
        {2, 92},
        {3, 78},
        {4, 90},
        {5, 88},
        {6, 92},
        {7, 85}
    };
    size_t n = sizeof(students) / sizeof(students[0]);

    // 使用XCompare进行排序
    XQuickSort(
        students,                // 数组
        n,                       // 个数
        sizeof(Student),         // 元素大小
        Student_compare, // 比较函数
        XSORT_ASC                // 升序
    );

    //使用stdlib的qsort函数，传入自定义的比较函数
    //qsort(students, n, sizeof(Student), Student_compare);


    // 输出排序结果
    log_i("Sorted Students:");
    for (size_t i = 0; i < n; i++)
    {
        printf("ID: %d, Score: %d\n", students[i].id, students[i].score);
    }
    log_i("XCompare test completed.");
}

#endif /* D26599CA_77AE_4736_8473_5AB8BB35175C */
