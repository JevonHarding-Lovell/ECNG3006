// Name: Jevon Harding-Lovell
// ID Number: 400008802

/*Amend the C code in William Ch 4, Section 4.7 "Demonstration Cyclic Executive Code" as needed for your platform.
Key items to look out for:

you will need a serial print function - locate an appropriate library for your compiler (4 marks) 
the "sleep" and "time" function(s) will need to be written - based on a hardware timer that uses an isr to increment a time structure (8 marks)
Your program should adhere to coding standards, and include your name/ID number as a comment. (6 marks)*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/hw_timer.h"


#define SLOTX   4
#define CYCLEX  5
#define SLOT_T  5000   // 5000 ms (5 second slots)

volatile uint32_t ms_ticks = 0;  // Millisecond counter updated by ISR
int cycle = 0, slot = 0;
uint32_t now = 0, then = 0;


void IRAM_ATTR timer_isr(void *arg) 
{
    ms_ticks++;  // Increment every 1ms
}

uint32_t get_time_ms(void) 
{
    return ms_ticks;  // Return current millisecond count
}

void sleep_seconds(int seconds) 
{
    vTaskDelay(seconds * 1000 / portTICK_PERIOD_MS);
}

void one(void) 
{
    printf("task 1 running\n");
    sleep_seconds(1);
}

void two(void) 
{
    printf("task 2 running\n");
    sleep_seconds(2);
}

void three(void) 
{
    printf("task 3 running\n");
    sleep_seconds(3);
}

void four(void) 
{
    printf("task 4 running\n");
    sleep_seconds(4);
}

void five(void) 
{
    printf("task 5 running\n");
    sleep_seconds(5);
}

void burn(void) 
{
    uint32_t bstart = get_time_ms();

    // Burn time until slot time elapsed
    while (((now = get_time_ms()) - then) < SLOT_T) 
    {
        // burn CPU time here 
    }
    
    printf("burn time = %dms\n\n", (get_time_ms() - bstart));
    then = now;
    cycle = CYCLEX;
}

void (*ttable[SLOTX][CYCLEX])(void) = 
{
    {one, two, burn, burn, burn},
    {one, three, burn, burn, burn},
    {one, four, burn, burn, burn},
    {burn, burn, burn, burn, burn}
};

void setup_uart(void) // UART for serial output
{
    uart_config_t uart_config = {.baud_rate = 115200, .data_bits = UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits = UART_STOP_BITS_1, .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
}

void setup_timer(void) 
{
    hw_timer_init(timer_isr, NULL);
    hw_timer_set_divider(80); // 80MHz / 80 = 1MHz (1us per tick)
    hw_timer_set_reload(true);
    hw_timer_set_alarm_value(1000); // Set interrupt to trigger every 1000us (1ms)
    hw_timer_intr_enable();
    hw_timer_enable();
}

void app_main(void) 
{
    setup_uart();
    setup_timer();

    printf("ESP8266 Cyclic Executive Starting...\n");
    printf("Clock ticks ready\n\n");
    
    then = get_time_ms();  // Initialize timing reference

    // Main cyclic executive loop
    for (;;) 
    {
        for (slot = 0; slot < SLOTX; slot++) 
        {
            for (cycle = 0; cycle < CYCLEX; cycle++) 
            {
                (*ttable[slot][cycle])(); 
            }
        }
    }
}