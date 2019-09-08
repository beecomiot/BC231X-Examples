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

/* user headers */
#include <user_config.h>
#include <file_config.h>
#include <bps_task.h>
#include <bps_public.h>
#include <bps_parse.h>
#include <bps_sig_ret_code.h>
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

#define HEADER_CLEAR() (g_RecvBuffer[0] = 0)

char SN[65] = "01234567890123456789012345678901"; // 65 bytes(65 ASCII + 1 '\0')
char KEY[33] = "01234567890123456789012345678901"; // 33 bytes(32 ASCII + 1 '\0')
xQueueHandle g_ComQueue;

BPSCmdHDInfoField g_HDInfoFieldArray[] = 
{
    // {SN_RST_HD_INFO,      sizeof("ABCDEFGHIJKLMNOP") - 1,     "ABCDEFGHIJKLMNOP"},
    {SN_RST_HD_INFO,      sizeof(SN) - 1,                            SN},
    {HARD_V_RST_HD_INFO,  sizeof(HARDWARE_VERSION) - 1,              HARDWARE_VERSION},
    {SOFT_V_RST_HD_INFO,  sizeof(SOFTWARE_VERSION) - 1,              SOFTWARE_VERSION},
};
const BPS_WORD g_HDInfoFieldArraySize = sizeof(g_HDInfoFieldArray)/sizeof(BPSCmdHDInfoField);

xTaskHandle bps_task_handler = NULL;
// xQueueHandle g_ComQueue;
// static int gBpsRecvBufferHandlePosition = 0;
// static int gBpsRecvBufferFillPosition = 0;
static uint8 g_RecvBuffer[BPS_TRANS_BUF_SIZE];
static uint8 g_SendBuffer[BPS_TRANS_BUF_SIZE];
// static char gBpsSendBuffer[BPS_TRANS_BUF_SIZE];
static BPSPacketData bpsPackData;;
static uint8 g_SNConfiged=0;

LOCAL void bps_task(void* p)
{
    BPSPacketData parseData;
    BPS_UINT8 * sendBufTmp;
    BPS_UINT16 tmpLen;
    const BPS_WORD packRmnSize = sizeof(g_SendBuffer) - BPS_HEADER_SIZE - BPS_VERSION_SIZE - BPS_ADDR_SIZE - BPS_REMAIN_LEN_SIZE;
    int i;

    // if(0 == ConfigRead(EN_CONFIG_ID_SN, SN, sizeof(SN)) && 0 == ConfigRead(EN_CONFIG_ID_KEY, KEY, sizeof(KEY))) {
    //     g_SNConfiged = 1;
    //     bc_printf("SN=%s, KEY=%s\n", SN, KEY);
    // } else {
    //     g_SNConfiged = 0;
    //     bc_printf("error: SN=%s, KEY=%s\n", SN, KEY);
    // }

    sendBufTmp = g_SendBuffer;
    sendBufTmp = PackBPSHeader(g_SendBuffer);
    sendBufTmp = PackBPSVersion(sendBufTmp);
    while(1) {
        bc_printf("bps_task stack left: %d\n", uxTaskGetStackHighWaterMark(NULL));
        if(pdPASS == xQueueReceive(g_ComQueue, &parseData, 1000/portTICK_RATE_MS)) {
            sendBufTmp = g_SendBuffer + BPS_ADDR_POSITION;
            sendBufTmp = PackBPSAddr(sendBufTmp, BPS_MODULE_SENDER, BPS_MCU_RECEIVER);
            sendBufTmp += BPS_REMAIN_LEN_SIZE;
            tmpLen = 0;
            bc_printf("task cmdWord=%02x\r\n", parseData.cmdWord);
            switch(parseData.cmdWord) {
                case CMD_COMM_TEST_WORD_REQ:
                    bc_printf("CMD_COMM_TEST_WORD_REQ bps_task\r\n");
                    tmpLen = BPSPackCommTestRsp(&(bpsPackData.pu.commTestRsp), sendBufTmp, packRmnSize);
                    break;
                case CMD_COMM_TEST_WORD_RSP:
                    break;
                case CMD_HD_INFO_WORD_REQ:
                    bc_printf("CMD_HD_INFO_WORD_REQ bps_task\r\n");
                    bpsPackData.pu.hdInfoRsp.fieldArray = g_HDInfoFieldArray;
                    bpsPackData.pu.hdInfoRsp.fieldNum = g_HDInfoFieldArraySize;
                    tmpLen = BPSPackHDInfoRsp(&(bpsPackData.pu.hdInfoRsp), sendBufTmp, packRmnSize);
                case CMD_HD_INFO_WORD_RSP:
                    break;
                case CMD_PING_WORD_REQ:
                    bc_printf("CMD_PING_WORD_REQ bps_task\r\n");
                    /* TODO: parse and do ping packet */
                    bpsPackData.pu.pingRsp.interval = 0x5A5A; // TODO: test data
                    tmpLen = BPSPackPingRsp(&(bpsPackData.pu.pingRsp), sendBufTmp, packRmnSize);
                    break;
                case CMD_PING_WORD_RSP:
                    break;
                case CMD_OPEN_NETSET_WORD_REQ:
                    bc_printf("CMD_OPEN_NETSET_WORD_REQ bps_task\r\n");
                    /* TODO: parse and do ping packet */
                    bpsPackData.pu.openNetsetRsp.retCode = 0; // TODO: test data
                    tmpLen = BPSPackOpenNetsetRsp(&(bpsPackData.pu.openNetsetRsp), sendBufTmp, packRmnSize);
                    break;
                case CMD_OPEN_NETSET_WORD_RSP:
                    break;
                case CMD_CONFIG_NETSET_WORD_REQ:
                    bc_printf("CMD_CONFIG_NETSET_WORD_REQ bps_task\r\n");
                    /* TODO: parse and do ping packet */
                    bpsPackData.pu.configNetsetRsp.retCode = BPS_RET_CODE_OK; // TODO: test data
                    bpsPackData.pu.configNetsetRsp.commType = CT_WIFI;
                    bpsPackData.pu.configNetsetRsp.mode = NM_WIFI_AP;
                    tmpLen = BPSPackConfigNetsetRsp(&(bpsPackData.pu.configNetsetRsp), sendBufTmp, packRmnSize);
                    break;
                case CMD_CONFIG_NETSET_WORD_RSP:
                case CMD_NETSTATE_QUERY_WORD_REQ:
                    bc_printf("CMD_NETSTATE_QUERY_WORD_REQ bps_task\r\n");
                    bpsPackData.pu.netstateQueryRsp.state = NS_WIFI_CONNECTED; // TODO: test data
                    tmpLen = BPSPackNetstateQueryRsp(&(bpsPackData.pu.netstateQueryRsp), sendBufTmp, packRmnSize);
                    break;
                case CMD_NETSTATE_QUERY_WORD_RSP:
                    break;
                case CMD_GET_SIGTAB_WORD_REQ:
                    {
                        BPS_UINT16 sigNum =g_CusSigNum + g_SysSigNum;
                        BPSCmdGetSigtabField fields[sigNum];
                        bc_printf("CMD_GET_SIGTAB_WORD_REQ bps_task\r\n");
                        for(i = 0; i < g_CusSigNum; i++) {
                            fields[i].signalId = g_CusSigTable[i].SigId;
                            fields[i].signalType = g_CusSigTable[i].SigType;
                            fields[i].accuracy = g_CusSigTable[i].Accuracy;
                        }
                        for(i = 0; i < g_SysSigNum; i++) {
                            fields[i+g_CusSigNum].signalId = g_SysSigTable[i].SigId;
                            fields[i+g_CusSigNum].signalType = g_SysSigTable[i].SigType;
                            fields[i+g_CusSigNum].accuracy = g_SysSigTable[i].Accuracy;
                        }
                        bpsPackData.pu.getSigtabRsp.fieldNum = sigNum;
                        bpsPackData.pu.getSigtabRsp.fieldArray = fields;
                        tmpLen = BPSPackGetSigtabRsp(&(bpsPackData.pu.getSigtabRsp), sendBufTmp, packRmnSize);
                        break;
                    }
                    break;
                case CMD_GET_SIGTAB_WORD_RSP:
                    break;
                case CMD_REPORT_SIG_WORD_REQ: 
                    bc_printf("CMD_REPORT_SIG_WORD_REQ bps_task\r\n");
                    /* TODO: parse and do report signal packet, then response it after the net server responded */
                    bpsPackData.pu.reportSigRsp.retCode = BPS_RET_CODE_OK;
                    tmpLen = BPSPackReportSigRsp(&(bpsPackData.pu.reportSigRsp), sendBufTmp, packRmnSize);
                case CMD_REPORT_SIG_WORD_RSP:
                    break;
                case CMD_POST_WORD_REQ:
                    /* packet from internet */
                    bc_printf("CMD_POST_WORD_REQ bps_task\r\n");
                    tmpLen = BPSPackPostReq(&(parseData.pu.postReq), sendBufTmp, packRmnSize);
                    break;
                case CMD_POST_WORD_RSP:
                    bc_printf("CMD_POST_WORD_RSP bps_task\r\n");
                    // tmpLen = BPSParsePostRsp(&parseData, sendBufTmp, packRmnSize);
                    break;
                case CMD_GET_SIG_WORD_REQ:
                case CMD_GET_SIG_WORD_RSP:
                    break;
                default:
                    bc_printf("default bps_task\r\n");
                    break;
            }
            if(tmpLen > 0) {
                PackBPSRemainLen(g_SendBuffer + BPS_REMAIN_LEN_POSITION, tmpLen);
                if(PackBPSChecksum(g_SendBuffer, sizeof(g_SendBuffer))) {
                    tmpLen += BPS_HEADER_SIZE + BPS_VERSION_SIZE + BPS_ADDR_SIZE + BPS_REMAIN_LEN_SIZE + BPS_CHECKSUM_SIZE;
                    for(i = 0; i < tmpLen; i++) {
                        uart_tx_one_char(UART0, g_SendBuffer[i]);
                    }
                }
            }

        }
    }

    vTaskDelete(NULL);
}

void bps_task_init(void)
{
    int ret;

    if(bps_task_handler) {
        bc_printf("task %s created already\n", BPS_TASK_NAME);
        return;
    }

    bc_printf("task %s creating\n", BPS_TASK_NAME);

    ret = xTaskCreate(bps_task,
                      BPS_TASK_NAME,
                      BPS_TASK_STACK_WORDS,
                      NULL,
                      BPS_TASK_PRORIOTY,
                      &bps_task_handler);

    if (ret != pdPASS)  {
        bc_printf("create task %s failed\n", BPS_TASK_NAME);
    }
}

void comHandler(uint8 * buf, uint32 size)
{
    static int recvSize = 0;
    static EnBPSParseStep step = EN_BPS_PARSE_HEADER;
    static uint16 rmnLen;
    static uint8 dataTmp;
    static uint8 cmdWord;;
    static portBASE_TYPE xHigherPriorityTaskWoken;
    static BPSPacketData parseData;
    static uint8 toAllocMemory = BPS_FALSE;;

    /* memory for parse data, string signal will j */
    static BPSCmdReportSigField reportSigField;
    static BPS_UINT8 sigStringBuf[BPS_MAX_STRING_LEN + 1];

	if(!buf || 0 == size) {
		return;
	}

    while(size--) {
        dataTmp = *buf++;
        switch(step) {
            case EN_BPS_PARSE_HEADER:
                if(IsBPSHeader2(g_RecvBuffer[0], dataTmp)) {
                    recvSize = 1;
                    g_RecvBuffer[recvSize++] = dataTmp;
                    toAllocMemory = BPS_TRUE;
                    step = EN_BPS_PARSE_VERSION;
                } else {
                    g_RecvBuffer[0] = dataTmp;
                }
                break;
            case EN_BPS_PARSE_VERSION:
                if(IsBPSVersionCompat(dataTmp)) {
                    g_RecvBuffer[recvSize++] = dataTmp;
                    step = EN_BPS_PARSE_ADDR;
                } else {
                    HEADER_CLEAR();
                    step = EN_BPS_PARSE_HEADER;
                }
                break;
            case EN_BPS_PARSE_ADDR:
                g_RecvBuffer[recvSize++] = dataTmp;
                step = EN_BPS_PARSE_RMN_LEN1;
                break;
            case EN_BPS_PARSE_RMN_LEN1:
                g_RecvBuffer[recvSize++] = dataTmp;
                step = EN_BPS_PARSE_RMN_LEN2;
                break;
            case EN_BPS_PARSE_RMN_LEN2:
                g_RecvBuffer[recvSize++] = dataTmp;
                rmnLen = GetBPSRemainLen(g_RecvBuffer+recvSize-BPS_REMAIN_LEN_SIZE);
                if(rmnLen + recvSize > sizeof(g_RecvBuffer) || rmnLen < BPS_CMD_WORD_SIZE) {
                    HEADER_CLEAR();
                    step = EN_BPS_PARSE_HEADER;
                } else {
                    step = EN_BPS_PARSE_DATA;
                }
                break;
            case EN_BPS_PARSE_DATA:
                if(toAllocMemory) {
                    /* the first byte of data is the command. judge it whether to alloc memory */
                    toAllocMemory = BPS_FALSE;
                    switch(dataTmp) {
                        case CMD_REPORT_SIG_WORD_REQ:
                            parseData.pu.reportSigReq.fieldArray = &reportSigField;
                            reportSigField.value.t_str = sigStringBuf;
                            parseData.pu.reportSigReq.maxFieldNum = 1;
                            break;
                        case CMD_SYSTEM_PARA_WORD_REQ:
                            parseData.pu.sysParaReq.data = sigStringBuf;
                            break;
                    }
                }
                g_RecvBuffer[recvSize++] = dataTmp;
                if(0 == --rmnLen) {
                    step = EN_BPS_PARSE_CHKSUM;
                }
                break;
            case EN_BPS_PARSE_CHKSUM:
                g_RecvBuffer[recvSize++] = dataTmp;
                for(rmnLen = 0; rmnLen < recvSize; rmnLen++) {
                    bc_printf("%02x ", g_RecvBuffer[rmnLen]);
                }
                bc_printf("\r\n");
                if(IsBPSChksumOK(g_RecvBuffer)) {
                    bc_printf("checksum ok\r\n");
                    if(BPSParseNoCheck(g_RecvBuffer, &parseData)) {
                        bc_printf("parse ok %02x, %02x\r\n", parseData.cmdWord, g_RecvBuffer[recvSize-1]);
                        xHigherPriorityTaskWoken = pdFALSE;
                        xQueueSendToBackFromISR(g_ComQueue, &parseData, &xHigherPriorityTaskWoken);
                        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
                    }
                }
                HEADER_CLEAR();
                step = EN_BPS_PARSE_HEADER;
                break;
            default:
                HEADER_CLEAR();
                step = EN_BPS_PARSE_HEADER;
                break;
        }
    }

    // buf[size] = '\0';
    // bc_printf("comHandler do: %s\r\n", buf);
    // send(sockfd_bcserver,buf,size,0);
}
