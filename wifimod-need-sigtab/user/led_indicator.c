#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "espressif/c_types.h"
#include "espressif/esp_misc.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "espressif/pwm.h"
#include "lwip/netdb.h"
#include "gpio.h"
#include "uart.h"

#include <led_indicator.h>
#include <user_config.h>

xTaskHandle led_indicator_task_handler = NULL;
static EnIndicatorState state;

/* light forever */
static IndicateUnit iuCommOk[] = { 
    {INDICATOR_DURING_SLOW, 1},
};

/* quick blink */
static IndicateUnit iuNetConfig[] = {
    {INDICATOR_DURING_QUICK, 1},
    {INDICATOR_DURING_QUICK, 0},
};

/* slow blink */
static IndicateUnit iuServConnecting[] = {
    {INDICATOR_DURING_SLOW, 1}, 
    {INDICATOR_DURING_SLOW, 0},
};

/* quick, quick */
static IndicateUnit iuNoWifiInfo[] = {
    {INDICATOR_DURING_QUICK, 1}, 
    {INDICATOR_DURING_QUICK, 0}, 
    {INDICATOR_DURING_QUICK, 1}, 
    {INDICATOR_DURING_SLOW, 0}, 
};

/* quick, quick, slow */
static IndicateUnit iuUnconfigure[] = {
    {INDICATOR_DURING_QUICK, 1}, 
    {INDICATOR_DURING_QUICK, 0}, 
    {INDICATOR_DURING_QUICK, 1}, 
    {INDICATOR_DURING_QUICK, 0}, 
    {INDICATOR_DURING_SLOW, 1}, 
    {INDICATOR_DURING_QUICK, 0}, 
};

/* quick, quick, quick */
static IndicateUnit iuSpecMode[] = {
    {INDICATOR_DURING_QUICK, 1}, 
    {INDICATOR_DURING_QUICK, 0}, 
    {INDICATOR_DURING_QUICK, 1}, 
    {INDICATOR_DURING_QUICK, 0}, 
    {INDICATOR_DURING_QUICK, 1}, 
    {INDICATOR_DURING_SLOW, 0}, 
};

/* quick, quick, slow, slow */
static IndicateUnit iuCrash[] = {
    {INDICATOR_DURING_QUICK, 1}, 
    {INDICATOR_DURING_QUICK, 0}, 
    {INDICATOR_DURING_QUICK, 1}, 
    {INDICATOR_DURING_QUICK, 0}, 
    {INDICATOR_DURING_SLOW, 1}, 
    {INDICATOR_DURING_QUICK, 0}, 
    {INDICATOR_DURING_SLOW, 1}, 
    {INDICATOR_DURING_QUICK, 0}, 
};

LOCAL void blink(IndicateUnit * iu, uint8_t num, portTickType * last_wake_time) {
    int i; 
    for(i = 0; i < num; i++) {
        GPIO_OUTPUT_SET(GPIO_ID_PIN(5), iu[i].isLight);
        vTaskDelayUntil(last_wake_time, iu[i].duringMs / portTICK_RATE_MS);
    }
}

LOCAL void led_indicator_task(void* p)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1) {
        switch(state) {
            case EN_INDICATOR_COMM_OK:
                blink(iuCommOk, sizeof(iuCommOk)/sizeof(iuCommOk[0]), &xLastWakeTime);
                break;
            case EN_INDICATOR_NET_CONFIG:
                blink(iuNetConfig, sizeof(iuNetConfig)/sizeof(iuNetConfig[0]), &xLastWakeTime);
                break;
            case EN_INDICATOR_SERV_CONNECTING:
                blink(iuServConnecting, sizeof(iuServConnecting)/sizeof(iuServConnecting[0]), &xLastWakeTime);
                break;
            case EN_INDICATOR_NO_WIFI_INFO:
                blink(iuNoWifiInfo, sizeof(iuNoWifiInfo)/sizeof(iuNoWifiInfo[0]), &xLastWakeTime);
                break;
            case EN_INDICATOR_UNCONFIGURE:
                blink(iuUnconfigure, sizeof(iuUnconfigure)/sizeof(iuUnconfigure[0]), &xLastWakeTime);
                break;
            case EN_INDICATOR_SPEC_MODE:
                blink(iuSpecMode, sizeof(iuSpecMode)/sizeof(iuSpecMode[0]), &xLastWakeTime);
                break;
            case EN_INDICATOR_CRASH:
                blink(iuCrash, sizeof(iuCrash)/sizeof(iuCrash[0]), &xLastWakeTime);
                break;
            default:
                state = EN_INDICATOR_SERV_CONNECTING;
                break;
        }
    }

    vTaskDelete(NULL);
}

void led_indicator_task_init(EnIndicatorState st)
{
    int ret;

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO5);
    state = st;
    bc_printf("task %s creating\n", LED_INDICATOR_TASK_NAME);

    ret = xTaskCreate(led_indicator_task,
                      LED_INDICATOR_TASK_NAME,
                      LED_INDICATOR_TASK_STACK_WORDS,
                      NULL,
                      LED_INDICATOR_TASK_PRIORITY,
                      &led_indicator_task_handler);

    if (ret != pdPASS)  {
        bc_printf("create task %s failed\n", LED_INDICATOR_TASK_NAME);
    }
}


void updateIndicator(EnIndicatorState st)
{
    state = st;
}

EnIndicatorState getIndicator()
{
    return state;
}
