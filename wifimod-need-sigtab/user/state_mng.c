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

#include <state_mng.h>
#include <user_config.h>
#include <file_config.h>
#include <bps_task.h>
#include <bps_public.h>
#include <bps_parse.h>
#include <bps_ret_code.h>
#include <bps_comm_type.h>
#include <bps_net_mode.h>
#include <bps_net_state.h>

#include <bps_cmd_comm_test.h>
#include <bps_cmd_hd_info.h>
#include <bps_cmd_ping.h>
#include <bps_cmd_open_netset.h>
#include <bps_cmd_config_netset.h>
#include <bps_cmd_netstate_query.h>
#include <bps_cmd_get_sigtab.h>
#include <bps_cmd_report_sig.h>
#include <bps_cmd_post.h>
#include <bps_cmd_get_sig.h>
#include <bps_cmd_system_para.h>
#include <bps_ret_code.h>
#include <bp_sig_table.h>
#include <bp_custom_sig_table.h>
#include <bp_strlen.h>

xTaskHandle state_mng_task_handler = NULL;
xQueueHandle g_StateQueue;
static EnModuleState ms;

LOCAL void state_mng_task(void* p)
{
    ms = EN_MS_CONNECT_SERVER;
    EnModuleState msTmp;
    while(1) {
        if(pdPASS == xQueueReceive(g_StateQueue, &msTmp, 1000/portTICK_RATE_MS)) {
            switch(ms) {
                case EN_MS_NET_CONFIG:
                    if(EN_MS_CONNECT_SERVER == msTmp) ms = msTmp;
                    break;
                case EN_MS_CONNECT_SERVER:
                    if(EN_MS_NET_CONFIG == msTmp) ms = msTmp;
                    if(EN_MS_NET_SIGNAL == msTmp) ms = msTmp;
                    break;
                case EN_MS_NET_SIGNAL:
                    if(EN_MS_NET_CONFIG == msTmp) ms = msTmp;
                    if(EN_MS_CONNECT_SERVER == msTmp) ms = msTmp;
                    break;
            }
        }
    }

    vTaskDelete(NULL);
}

EnModuleState getModuleState() 
{
    return ms;
}

portBASE_TYPE updateModuleState(EnModuleState st)
{
    return xQueueSendToBack(g_StateQueue, &st, 0);
}

void updateModuleStateFromISR(EnModuleState st)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendToBackFromISR(g_StateQueue, &st, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void state_mng_task_init()
{
    int ret;

    bc_printf("task %s creating\n", STATE_MNG_TASK_NAME);

    ret = xTaskCreate(state_mng_task,
                      STATE_MNG_TASK_NAME,
                      STATE_MNG_TASK_STACK_WORDS,
                      NULL,
                      STATE_MNG_TASK_PRIORITY,
                      &state_mng_task_handler);

    if (ret != pdPASS)  {
        bc_printf("create task %s failed\n", STATE_MNG_TASK_NAME);
    }
}

