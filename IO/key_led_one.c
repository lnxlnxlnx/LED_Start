#include "key_led_one.h"

uint16_t LED_DELAY_TIME = 200;
STATE_MACHINE state_machine = {FALLING, KNONE};
EVENT_TYPE curent_event = KNONE;

static void key_led_one_init(void)
{
    /* hardware init is moved to main() through device_factory */
}

static void _led_loop_control(uint8_t direct)
{
    int i;
    if (!direct)
    {
        for (i = 0; i < 8; i++)
        {
            LED(i, 0);
            delay_ms(LED_DELAY_TIME);
            LED(i, 1);
        }
    }
    else
    {
        for (i = 0; i < 8; i++)
        {
            LED(7 - i, 0);
            delay_ms(LED_DELAY_TIME);
            LED(7 - i, 1);
        }
    }
}

static void change_state(void);

static void led_blink(void)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        LED0 = 0;
        delay_ms(200);
        LED0 = 1;
        delay_ms(200);
    }
}

static void key_led_fsm_disptch(void)
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
    default:
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
    default:
        state_machine.state = FALLING;
        break;
    }
}

void key_led_one_loop(void)
{
    int i;
    key_led_one_init();
    while (1)
    {
        key_led_fsm_disptch();
        switch (state_machine.state)
        {
        case FALLING:
            _led_loop_control(0);
            break;
        case RISING:
            _led_loop_control(1);
            break;
        case CIRCLE:
            for (i = 0; i < 8; i += 2)
            {
                LED(i, 0);
                delay_ms(LED_DELAY_TIME);
                LED(i, 1);
            }
            for (i = 1; i < 8; i += 2)
            {
                LED(8 - i, 0);
                delay_ms(LED_DELAY_TIME);
                LED(8 - i, 1);
            }
            break;
        case STOP:
            break;
        default:
            state_machine.state = FALLING;
            break;
        }
        delay_ms(100);
    }
}
