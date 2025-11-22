#ifndef RTC_PERIPHERAL_H
#define RTC_PERIPHERAL_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <stddef.h>

bool init_rtc(void);
bool start_rtc_alarm(uint32_t alarm_seconds);
bool stop_rtc_alarm(void);
bool is_done_rtc_alarm(void);
time_t singleread_rtc_time(void);
bool singlewrite_rtc_time(time_t new_time);
int burstread_rtc_data(uint8_t *buffer, int max_bytes);
bool burstwrite_rtc_data(const uint8_t *data, int data_len);

#endif