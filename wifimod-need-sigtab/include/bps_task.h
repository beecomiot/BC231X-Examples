#ifndef __BPS_TASK_H__
#define __BPS_TASK_H__

#include "freertos/task.h"
#include "freertos/queue.h"

#include <bps_public.h>
#include <bps_cmd_hd_info.h>

#define BPS_TRANS_BUF_SIZE 512
#define BPS_COM_QUEUE_SIZE 5
#define BC_COM_QUEUE_SIZE 5

#define BPS_MODULE_SENDER   0x01
#define BPS_MCU_RECEIVER    0x00

extern xTaskHandle bps_task_handler;
extern xQueueHandle g_ComQueue;
extern BPSCmdHDInfoField g_HDInfoFieldArray[];
extern char SN[65];
extern char KEY[33];

typedef enum EnBPSParseStep {
    EN_BPS_PARSE_HEADER,
    EN_BPS_PARSE_VERSION,
    EN_BPS_PARSE_ADDR,
    EN_BPS_PARSE_RMN_LEN1,
    EN_BPS_PARSE_RMN_LEN2,
    EN_BPS_PARSE_DATA,
    EN_BPS_PARSE_CHKSUM,
} EnBPSParseStep;

/******************************************************************************
 * FunctionName : bps_task_init
 * Description  : create the BPS task
 * Parameters   : void
 * Returns      : void
*******************************************************************************/
void bps_task_init(void);

/******************************************************************************
 * FunctionName : comHandler
 * Description  : serial port data received handler
 * Parameters   : fd, data received; size, data size
 * Returns      : void
*******************************************************************************/
void comHandler(uint8 * buf, uint32 size);

#endif

