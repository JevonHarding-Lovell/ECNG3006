// Name: Jevon Harding-Lovell
// ID Number: 400008802
// Switch-Case

/*you will need a serial receive function that lets you know if a character has arrived - locate an appropriate 
library for your compiler (2 marks)
the FSM transitions should only take place on a system tick - if the system tick is a factor of 500ms you will 
make your life easier ... hint use a counter and reset it once it hits 500 Ms or a character is received(3 marks)*/

#include <stdio.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freetos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
//#include <ncurses.h> //LINUX
//#include <sys/times.h> //LINUX
//#include <unistd.h> //LINUX

#define FOREVER 1
#define tick_interval 50 // 50ms 
#define debounce_time 500 // 500ms debounce time
#define ON 1
#define OFF 0
#define LED_PIN 2

unsigned int state, tick_cnt, interval_cnt;
char last_char, current_char;
int char_rec, key;
//clock_t tick; //LINUX
//struct tms n; //LINUX
//unsigned int tick_cnt, tps; //LINUX
//int key;

int uart_read_char()
{
    return -1; //Dummy UART read
}

void led_on()
{
    gpio_set_level(LED_PIN, 1); // Defined in driver/gpio.h
    printf("Led On - '%c' accepted\n", last_char);
}
  
void led_off()
{
    gpio_set_level(LED_PIN, 0);
    printf("Led Off\n\r");
}

void process_char()
{
    printf("'%c' recieved\n\r", current_char);
}

void app_main(void)
{
    gpio_config_t io_conf = {.pin_bit_mask = (1ULL << LED_PIN), . mode = GPIO_MODE_OUTPUT, .pull_up_en = GPIO_PULLUP_DISABLE, . pull_down_en = GPIO_PULLDOWN_DISABLE, .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);
        
    state = OFF;
    tick_cnt = 0;
    interval_cnt = 0;
    last_char = '\0';
    char_rec = 0;

    /*initscr(); //LINUX
    cbreak();
    nodelay(stdscr, TRUE);
    tps = sysconf(_SC_CLK_TCK);
    tick = times(&n) + (tick_interval * tps) / 1000;*/

    while (FOREVER)
    {
        vTaskDelay(pdMS_TO_TICKS(tick_interval));
        tick_cnt++;
        interval_cnt++;
        

        current_char = '\0';
        char_rec = 0;

        key = uart_read_char();
        if (key != -1)
        {
            current_char = (char)key;
            char_rec = 1;
            process_char();            
        }

        printf("State = %s, tick = %d, Interval %dms \r", (state == ON ? "ON " : "OFF"), tick_cnt, interval_cnt * tick_interval);


        switch (state)
        {
            case OFF:
                if (char_rec)
                {
                    last_char = current_char;
                    led_on();
                    state = ON;
                    interval_cnt = 0;
                }
                break;
                
            case ON:
                if (char_rec)
                {
                    if (current_char == last_char)
                    {
                        if (interval_cnt * tick_interval >= debounce_time)
                        {
                            led_off();
                            state = OFF;
                            interval_cnt = 0;
                        }
                        else
                        {
                            printf("Ignore duplicate '%c'\n\r", current_char);
                        }
                    }
                    else
                    {
                        last_char = current_char;
                        interval_cnt = 0;
                        printf("New character '%c' received\n\r", current_char);
                    }          
                }
            break;
        }

        if (interval_cnt * tick_interval >= debounce_time)
        {
            interval_cnt = debounce_time / tick_interval;
        }
    }
}

// Wsl and test the code*/