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

#include <sys_config.h>
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

xTaskHandle sys_config_task_handler = NULL;

LOCAL void sys_config_task(void* p)
{
    BPSPacketData parseData;
    BPSPacketData bpsPackData;;
    uint8 sendBuffer[BPS_TRANS_BUF_SIZE];
    const BPS_WORD packRmnSize = sizeof(sendBuffer) - BPS_HEADER_SIZE - BPS_VERSION_SIZE - BPS_ADDR_SIZE - BPS_REMAIN_LEN_SIZE;
    BPS_UINT8 * sendBufTmp;
    BPS_UINT16 tmpLen;
    BPS_UINT16 i;

    sendBufTmp = sendBuffer;
    sendBufTmp = PackBPSHeader(sendBufTmp);
    sendBufTmp = PackBPSVersion(sendBufTmp);

    while(1) {
        bc_printf("sys_config stack left: %d\n", uxTaskGetStackHighWaterMark(NULL));
        if(pdPASS == xQueueReceive(g_ComQueue, &parseData, 1000/portTICK_RATE_MS)) {
            sendBufTmp = sendBuffer + BPS_ADDR_POSITION;
            sendBufTmp = PackBPSAddr(sendBufTmp, BPS_MODULE_SENDER, BPS_MCU_RECEIVER);
            sendBufTmp += BPS_REMAIN_LEN_SIZE;
            tmpLen = 0;
            bc_printf("task cmdWord=%02x\r\n", parseData.cmdWord);
            switch(parseData.cmdWord) {
                case CMD_COMM_TEST_WORD_REQ:
                    bc_printf("CMD_COMM_TEST_WORD_REQ sys_config_task\r\n");
                    tmpLen = BPSPackCommTestRsp(&(bpsPackData.pu.commTestRsp), sendBufTmp, packRmnSize);
                    break;
                case CMD_COMM_TEST_WORD_RSP:
                    break;
                case CMD_SYSTEM_PARA_WORD_REQ: 
                    {
                        char * para = NULL;
                        BPS_UINT8 retCode = BPS_RET_CODE_OK;
                        bc_printf("CMD_SYSTEM_PARA_WORD_REQ sys_config_task\r\n");
                        /* write para */
                        if(WRITE_SYS_PARA == parseData.pu.sysParaReq.configType) {
                            parseData.pu.sysParaReq.data[parseData.pu.sysParaReq.len] = '\0';
                            bc_printf("sys config=%d->%d->%s\n", parseData.pu.sysParaReq.len, parseData.pu.sysParaReq.paraType, parseData.pu.sysParaReq.data);
                            if(0 != ConfigWrite(parseData.pu.sysParaReq.paraType, parseData.pu.sysParaReq.data)) {
                                retCode = BPS_RET_CODE_FAILED;
                            }
                        }

                        /* construct response */
                        bpsPackData.pu.sysParaRsp.configType = parseData.pu.sysParaReq.configType;
                        bpsPackData.pu.sysParaRsp.paraType = parseData.pu.sysParaReq.paraType;
                        if(bpsPackData.pu.sysParaRsp.configType >= NUM_SYS_PARA) {
                           retCode = BPS_RET_CODE_CMD_TYPE_INVALID;
                        }
                        if(bpsPackData.pu.sysParaRsp.paraType >= NUM_SYS_PARA_TYPE) {
                            retCode = BPS_RET_CODE_CMD_PARA_INVALID;;
                        }
                        if(SN_SYS_PARA_TYPE == bpsPackData.pu.sysParaRsp.paraType) {
                            if(WRITE_SYS_PARA == parseData.pu.sysParaRsp.configType) {
                                strncpy(SN, parseData.pu.sysParaReq.data, parseData.pu.sysParaReq.len);
                                SN[parseData.pu.sysParaReq.len] = '\0';
                            } else {
                                para = SN;
                            }
                        } else if(KEY_SYS_PARA_TYPE == bpsPackData.pu.sysParaRsp.paraType) {
                            if(WRITE_SYS_PARA == parseData.pu.sysParaRsp.configType) {
                                strncpy(KEY, parseData.pu.sysParaReq.data, parseData.pu.sysParaReq.len);
                                KEY[parseData.pu.sysParaReq.len] = '\0';
                            } else {
                                para = KEY;
                            }
                        }

                        if(NULL != para) {
                            bpsPackData.pu.sysParaRsp.len = strlen_bp(para);
                            bpsPackData.pu.sysParaRsp.data = para;
                        }
                        bpsPackData.pu.sysParaRsp.retCode = retCode;
                        bc_printf("retCode=%d\n", retCode);
                        tmpLen = BPSPackSystemParaRsp(&(bpsPackData.pu.sysParaRsp), sendBufTmp, packRmnSize);
                        break;
                    }
                default:
                    bc_printf("default sys_config_task\r\n");
                    break;
            }
            if(tmpLen > 0) {
                PackBPSRemainLen(sendBuffer + BPS_REMAIN_LEN_POSITION, tmpLen);
                if(PackBPSChecksum(sendBuffer, sizeof(sendBuffer))) {
                    tmpLen += BPS_HEADER_SIZE + BPS_VERSION_SIZE + BPS_ADDR_SIZE + BPS_REMAIN_LEN_SIZE + BPS_CHECKSUM_SIZE;
                    for(i = 0; i < tmpLen; i++) {
                        uart_tx_one_char(UART0, sendBuffer[i]);
                    }
                }
            }

        }
    }

    vTaskDelete(NULL);
}

void sys_config_task_init()
{
    int ret;

    bc_printf("task %s creating\n", SYS_CONFIG_TASK_NAME);

    ret = xTaskCreate(sys_config_task,
                      SYS_CONFIG_TASK_NAME,
                      SYS_CONFIG_TASK_STACK_WORDS,
                      NULL,
                      SYS_CONFIG_TASK_PRIORITY,
                      &sys_config_task_handler);

    if (ret != pdPASS)  {
        bc_printf("create task %s failed\n", SYS_CONFIG_TASK_NAME);
    }
}

