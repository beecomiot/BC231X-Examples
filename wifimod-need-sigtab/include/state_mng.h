#ifndef __STATE_MNG_H__
#define __STATE_MNG_H__

#include "esp_common.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define STATE_MNG_QUEUE_SIZE 5

typedef enum EnModuleState {
    EN_MS_NET_CONFIG,
    EN_MS_CONNECT_SERVER,
    EN_MS_NET_SIGNAL,
} EnModuleState;

extern xTaskHandle state_mng_task_handler;
extern xQueueHandle g_StateQueue;

EnModuleState getModuleState();
portBASE_TYPE updateModuleState(EnModuleState st);
void updateModuleStateFromISR(EnModuleState st);
void state_mng_task_init();

#endif

