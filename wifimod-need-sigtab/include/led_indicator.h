#ifndef __LED_INDICATOR_H__
#define __LED_INDICATOR_H__

#include "esp_common.h"

#define INDICATOR_DURING_QUICK      100
#define INDICATOR_DURING_SLOW       500

extern xTaskHandle led_indicator_task_handler;

typedef enum EnIndicatorState {
    /* for user */
    EN_INDICATOR_COMM_OK,
    EN_INDICATOR_NET_CONFIG,
    EN_INDICATOR_SERV_CONNECTING,
    EN_INDICATOR_NO_WIFI_INFO,
    /* for manufacturer */
    EN_INDICATOR_UNCONFIGURE,
    EN_INDICATOR_SPEC_MODE,
    EN_INDICATOR_CRASH,
} EnIndicatorState;

typedef struct IndicateUnit {
    uint16_t duringMs;
    uint8_t  isLight;
} IndicateUnit;;

void led_indicator_task_init(EnIndicatorState st);
void updateIndicator(EnIndicatorState st);

#endif

