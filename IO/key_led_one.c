#include "key_led_one.h"

// 全局变量定义
uint16_t LED_DELAY_TIME = 200;
STATE_MACHINE state_machine = {FALLING, KNONE};

// 静态初始化函数
static void key_led_one_init(void)
{
    LED_Init();
    KEY_Init();
}

// LED循环控制（修复for循环C99语法）
static void _led_loop_control(uint8_t direct)
{
    int i; // 移到循环外定义，兼容C89
    if (!direct)
    {
        //for (;;)
        //{
            for (i = 0; i < 8; i++)
            {
                led_funcs[i](0);
                delay_ms(LED_DELAY_TIME);
                led_funcs[i](1);
            }
        //}
    }
    else
    {
        //for (;;)
        //{
            for (i = 0; i < 8; i++)
            {
                led_funcs[7 - i](0);
                delay_ms(LED_DELAY_TIME);
                led_funcs[7 - i](1);
            }
        //}
    }
}

static void change_state(void);
// LED闪烁（修复for循环C99语法）
static void led_blink(void)
{
    int i; // 移到循环外定义，兼容C89
    for (i = 0; i < 3; i++)
    {
        LED0 = 0;
        delay_ms(200);
        LED0 = 1;
        delay_ms(200); // 补个延时，否则闪烁太快看不到
    }
}

// 状态机分发（原代码逻辑不变）
static void key_led_fsm_disptch(EVENT_TYPE event)
{
    state_machine.event = event;
    switch (state_machine.event)
    {
    case K0:
        led_blink();
        state_machine.state = STOP;
        break;
    case K1:
        LED_DELAY_TIME += 100;
        // 加个边界限制，防止延时过大/负数
        if(LED_DELAY_TIME > 1000) LED_DELAY_TIME = 1000;
        break;
    case K2:
        LED_DELAY_TIME -= 100;
        if(LED_DELAY_TIME < 100) LED_DELAY_TIME = 100;
        break;
    case kUP:
        // if (state_machine.state == CIRCLE)
        //     state_machine.state = FALLING;
        // state_machine.state = (STATE_TYPE)(state_machine.state + 1);
        // // 状态越界保护，防止超出枚举范围
        // if(state_machine.state > STOP) state_machine.state = FALLING;
        change_state();
        break;
    case KNONE:
        break;
    }
}

static void change_state(void)
{
    switch (state_machine.state)
    {    case FALLING:
        state_machine.state = RISING;
        break;
        case RISING:
        state_machine.state = CIRCLE;
        break;
        case CIRCLE:
        state_machine.state = STOP;
        break;
        case STOP:
        state_machine.state = FALLING;
        break;
    }
}

// 主循环函数（外部调用）
void key_led_one_loop(void)
{
    key_led_one_init();
    while (1)
    {
        key_led_fsm_disptch((EVENT_TYPE)KEY_Scan(0));

        switch (state_machine.state)
        {
        case FALLING:
            _led_loop_control(0);
            break;
        case RISING:
            _led_loop_control(1);
            break;
        case CIRCLE:
            for(int i = 0; i < 8; i += 2)
            {
                led_funcs[i](0);
                delay_ms(LED_DELAY_TIME);
                led_funcs[i](1);
            }
            for(int i = 1; i < 8; i += 2)
            {
                led_funcs[8 - i](0);
                delay_ms(LED_DELAY_TIME);
                led_funcs[8 - i](1);
            }
            break;
        case STOP:
            //delay_ms(LED_DELAY_TIME);// 这里不需要延时，直接进入下一轮循环即可，不然按键可能识别不到
            break;
        }
        //change_state();
        delay_ms(100);
    }
}

