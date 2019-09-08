#ifndef __SYS_CONFIG_H__
#define __SYS_CONFIG_H__

#include "esp_common.h"

#define SYS_CONFIG_COM_QUEUE_SIZE   5

extern xTaskHandle sys_config_task_handler;;

void sys_config_task_init();

#endif

