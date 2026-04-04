#ifndef D69D8D26_2026_4C14_98C8_3C2D83C24A06
#define D69D8D26_2026_4C14_98C8_3C2D83C24A06

#include "XVector.h"

void test_xvector(void)
{
    // ======================
    // 1. 创建 vector（存 int）
    // ======================
    printf("Testing XVector...\r\n");
    XVector* vec = XVector_create(sizeof(int));
    printf("XVector_Create(sizeof(int)):\r\n");
    // ======================
    // 2. 尾部添加数据
    // ======================
    int a = 10;
    int b = 20;
    int c = 30;

    XVector_push_back_base(vec, &a);
    XVector_push_back_base(vec, &b);
    XVector_push_back_base(vec, &c);

    // ======================
    // 3. 遍历
    // ======================
    int i;
    size_t count = XVector_size_base(vec);  // 你源码里是 size_base

    for (i = 0; i < count; i++)
    {
        int* p = (int*)XVector_at_base(vec, i);
        printf("vec[%d] = %d\r\n", i, *p);
    }

    // ======================
    // 4. 删除最后一个
    // ======================
    XVector_pop_back_base(vec);

    // ======================
    // 5. 释放
    // ======================
    XVector_delete_base(vec);
}
#endif /* D69D8D26_2026_4C14_98C8_3C2D83C24A06 */
