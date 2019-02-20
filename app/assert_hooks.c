#include "assert_hooks.h"

#include "stm32f7xx_hal.h"
#include "usart.h"
#include "led_gpio_hal.h"

void debug_send_char(const char c)
{
    // wait for UART ready
    while (HAL_UART_GetState(stdio_uart) != HAL_UART_STATE_READY) {}
    // use HAL_MAX_DELAY for CPU polling mode
    HAL_UART_Transmit(stdio_uart, (uint8_t *) &c, 1, HAL_MAX_DELAY);
}

void debug_print(const char *ptr, int len)
{
    static int n;
    static const char r = '\r';
    for (n = 0; n < len; n++) {
        if (*ptr == '\n') {
            debug_send_char(r);
        }
        debug_send_char(*ptr++);
    }
}

static const int LED_BLINK_CPUDELAY = 200000;

//void led_blink(DeviceLeds led, int duration_on, int duration_off)
//{
//    for (int i=0; i < duration_on * LED_BLINK_CPUDELAY; i++)
//        led_set_state(led, LED_ON);
//    for (int i=0; i < duration_off * LED_BLINK_CPUDELAY; i++)
//        led_set_state(led, LED_OFF);
//}

void led_show_error(void)
{
    led_set_state(LED_GREEN, LED_OFF);
    led_set_state(LED_YELLOW, LED_OFF);
    led_set_state(LED_RED, LED_ON);
}

void led_blink_morse(DeviceLeds led, const int buf[], unsigned int size)
{
    static unsigned int n;
    for (n=0; n<size; n++) {
        switch(buf[n]) {
        case 0:
            for (int i=0; i < LED_BLINK_CPUDELAY; i++)
                led_set_state(led, LED_OFF);
            break;
        case 1:
            for (int i=0; i < LED_BLINK_CPUDELAY; i++)
                led_set_state(led, LED_ON);
            break;
        default:
            for (int i=0; i < 3 * LED_BLINK_CPUDELAY; i++)
                led_set_state(led, LED_ON);
            break;
        }
    }
}

void led_blink_error(void)
{
    // dot=1, dash=3, space=3, wordspace=7
    static const int buf[] = {
        0,0,0,0,0,0,0,   // wordspace
        1,0,1,0,1, 0,0,0, // S
        3,0,3,0,3, 0,0,0, // O
        1,0,1,0,1, 0,0,0  // S
    };
    enum { bufsize = sizeof(buf)/sizeof(buf[0])};
    led_blink_morse(LED_YELLOW, buf, bufsize);
}

void vAssertCalled( uint32_t ulLine, const char *pcFile )
{
    volatile unsigned long ul = 0;

    ( void ) pcFile;
    ( void ) ulLine;

    taskDISABLE_INTERRUPTS(); // game over
    led_show_error();

    static const char str[] = "assert called\n";
    while (HAL_UART_GetState(stdio_uart) != HAL_UART_STATE_READY) {}
    HAL_UART_Transmit_IT(stdio_uart, (uint8_t *)str, sizeof(*str));

//    printf("\nError: assert called at %s:%ld\n", pcFile, ulLine);

    taskENTER_CRITICAL();
    {
        /* Set ul to a non-zero value using the debugger to step out of this
        function. */
        while( ul == 0 )
        {
            led_blink_error();
            __NOP();
        }
    }
    taskEXIT_CRITICAL();
}

void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS(); // game over
    led_show_error();
    static const char str[] = "Error: malloc failed\n";
    debug_print(str, sizeof(str));
    while(1) {
        led_blink_error();
    }
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    (void) pxTask;
    (void) pcTaskName;
    taskDISABLE_INTERRUPTS(); // game over
    led_show_error();
    static const char str[] = "Error: stack overflow\n";
    debug_print(str, sizeof(str));
    while(1) {
        led_blink_error();
    }
}
