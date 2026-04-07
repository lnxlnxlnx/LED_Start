#include "key_led_one.h"
#include "exti.h"
#include "beep.h"
#include "multi_button.h"

// 全局变量定义
uint16_t LED_DELAY_TIME = 200;
STATE_MACHINE state_machine = { FALLING, KNONE };
EVENT_TYPE curent_event = KNONE;
LED_BEEP_MACHINE led_beep_machine = { NORMAL, KNONE };
static Button btn1;

uint8_t read_button_gpio(uint8_t button_id)
{
    switch (button_id) {
        case 1:
            return WK_UP; // 假设KEY0对应按钮1
        default:
            return 0;
    }
}

void btn1_single_click_handler(Button* btn, void* user_data)
{
    printf("Button 1: Single Click\n");
}

// 静态初始化函数
static void key_led_one_init(void)
{
    LED_Init();
    //KEY_Init();
    EXTIX_Init();
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
// LED闪烁
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

static void led_beep_fsm_dispatch() {
    led_beep_machine.event = curent_event;
    switch (led_beep_machine.event)
    {
        case K0:
            LED0 = !LED0; // 切换LED状态
            curent_event = KNONE;       //TODO:也可以用标志位来判断是否已经处理过这个事件
            break;
        case K1:
            LED1 = !LED1; // 切换LED状态
            curent_event = KNONE;
            break;
        case K2:
            LED2 = !LED2; // 切换LED状态
            curent_event = KNONE;
            break;
        case kUP:
            curent_event = KNONE;
            break;
        default:
            break;
    }
    //printf("OK\n");
    delay_ms(10);
    //curent_event = KNONE;
}

void led_beep_loop(void) { 
    key_led_one_init();
    BEEP_Init();
    button_init(&btn1,  read_button_gpio, 0, 1); 
    button_attach(&btn1, BTN_SINGLE_CLICK, btn1_single_click_handler, NULL);
    button_start(&btn1);
    printf("LED Beep FSM Started\n");
    while(1){
        led_beep_fsm_dispatch();
    }
}

// 状态机分发（原代码逻辑不变）
static void key_led_fsm_disptch()
{
    state_machine.event = curent_event;
    switch (state_machine.event)
    {
        case K0:
            led_blink();
            state_machine.state = STOP;
            break;
        case K1:
            LED_DELAY_TIME += 100;
            // 加个边界限制，防止延时过大/负数
            if (LED_DELAY_TIME > 1000) LED_DELAY_TIME = 1000;
            break;
        case K2:
            LED_DELAY_TIME -= 100;
            if (LED_DELAY_TIME < 100) LED_DELAY_TIME = 100;
            break;
        case kUP:
            change_state();
            break;
        case KNONE:
            break;
    }
    curent_event = KNONE;
}

static void change_state(void)
{
    switch (state_machine.state)
    {
        case FALLING:
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
        //key_led_fsm_disptch((EVENT_TYPE)KEY_Scan(0));
        key_led_fsm_disptch();
        switch (state_machine.state)
        {
            case FALLING:
                _led_loop_control(0);
                break;
            case RISING:
                _led_loop_control(1);
                break;
                //TODO: 添加循环模式
            case CIRCLE:
                for (int i = 0; i < 8; i += 2)
                {
                    led_funcs[i](0);
                    delay_ms(LED_DELAY_TIME);
                    led_funcs[i](1);
                }
                for (int i = 1; i < 8; i += 2)
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

