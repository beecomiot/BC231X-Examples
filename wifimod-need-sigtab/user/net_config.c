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

#include <bpclient_config.h>
#include <bp_public.h>
#include <bp_make_pack.h>
#include <bp_pack_type.h>
#include <bp_vrb_flags.h>
#include <bp_crc32.h>
#include <bp_init.h>
#include <bp_connect.h>
#include <bp_disconn.h>
#include <bp_parse.h>
#include <bp_report.h>
#include <bp_getack.h>
#include <bp_specack.h>
#include <bp_postack.h>

#include <bp_ping.h>
#include <bp_sig_table.h>
#include <bp_sig_table_tools.h>

#include <net_config.h>
#include <bc_comm.h>
#include <user_config.h>
#include <file_config.h>
#include <bps_task.h>
#include <bps_public.h>
#include <bps_parse.h>
#include <bps_sig_ret_code.h>
#include <bps_comm_type.h>
#include <bps_net_mode.h>
#include <bps_net_state.h>
#include <bps_memcpy.h>
#include <led_indicator.h>

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
#include <bp_custom_sig_table.h>
#include <bp_strlen.h>
#include <state_mng.h>


xTaskHandle net_config_task_handler = NULL;
static EnNetConfigLogicMode g_NetConfigLogic = EN_LOGIC_1;
static uint8_t g_intrTriggered = 0;
static uint8_t g_smartConfigDoing = 0;
static uint16_t g_runCount = 0; 
static uint8_t g_netConfigOk = 0;
static  BP_SpecackStr str_specack;

char RECV_BUF[RECV_BUF_SIZE];

int ICACHE_FLASH_ATTR handleBpPacket(int sockfd, BP_UINT8 * recvBuf, BP_WORD size, BP_UINT8 * type_and_flags) 
{
    int n;
    BP_UINT16 left_len;
    BP_UINT32 crc = 0;
    int i;
    int len;

    if(sockfd < 0) {
        return -1;
    }
    if(BP_NULL == recvBuf) {
        return -1;
    }
    if(BP_NULL == type_and_flags) {
        return -1;
    }
    if(size < FIX_HEAD_SIZE) {
        return -1;
    }
    n=recv(sockfd,recvBuf,FIX_HEAD_SIZE, MSG_WAITALL);
    if(FIX_HEAD_SIZE != n) {
        perror("Recv error 1\r\n");
        return -2;
    }
    if(0 != BP_ParseFixHead(recvBuf, type_and_flags, &left_len)) {
        perror("Recv error 1\r\n");
        return -4;
    }

    len = left_len;
    bc_printf("* len=%d\r\n", len);

    if(size < len + FIX_HEAD_SIZE) {
        return -1;
    }
    n += recv(sockfd,recvBuf+FIX_HEAD_SIZE,len, MSG_WAITALL);
    len += FIX_HEAD_SIZE;

    if(n != len) {
        bc_printf("* Recv error 2\r\n");
        return -2;
    }
    bc_printf("* recv: ");
    for(i = 0; i < len; i++) {
        bc_printf("%02x ", recvBuf[i]);
    }
    bc_printf("\n");

    if(0 != BP_CheckCRC(*type_and_flags, recvBuf, len)) {
        bc_printf("* CRC Check error\n");
        return -3;
    }

    return len;
}

LOCAL void startSmartConfig() {
    if(!g_smartConfigDoing) {
        printf("start smart config\n");
        g_smartConfigDoing = 1;
        smartconfig_start(smartconfig_done);
    }
}

LOCAL void stopSmartConfig() {
    if(g_smartConfigDoing) {
        g_smartConfigDoing = 0;
        smartconfig_stop();
    }
}

LOCAL void gpio_intr_handler(void *pvParameters) 
{
    uint32_t gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    uint8_t level = 0;
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status);

    if (gpio_status & GPIO_Pin_13)
    {
        updateModuleStateFromISR(EN_MS_NET_CONFIG);
        updateIndicator(EN_INDICATOR_NET_CONFIG);
        g_intrTriggered = 1;
    }
}

LOCAL EnNetConfigLogicMode logic35() {
    EnNetConfigLogicMode logicMode = EN_LOGIC_3;
    WIFI_MODE wm = wifi_get_opmode();

    if(g_netConfigOk) {
        /* step 2: start local server to get admin ID */
        if(0 == startLocalServer()) {
            bc_printf("EN_LOGIC_1\n");
            logicMode = EN_LOGIC_1;
        } else {
            bc_printf("EN_LOGIC_8\n");
            logicMode = EN_LOGIC_8;
        }
    } else {
        /* step 1: start smart config */
        if(!g_smartConfigDoing) {
            if(STATION_MODE != wm) {
                wifi_set_mode(STATION_MODE);
                printf("smart config:reset mode\n");
                return logicMode;
            }

            if(STATION_IDLE != wifi_station_get_connect_status()) {
                wifi_station_disconnect();
                printf("disconnect\n");
                return logicMode;
            }
            startSmartConfig();
        }
    }

    if(g_runCount++ > SMART_CONFIG_CHECK_COUNT) {
        logicMode = EN_LOGIC_7;
    }

    return logicMode;

}

LOCAL EnNetConfigLogicMode logic24() {
    return EN_LOGIC_2;
}

LOCAL EnNetConfigLogicMode logic1() {
    stopSmartConfig();
    g_bcWifiInfo.isWifiInfoSet = 1;
    wifi_station_get_config_default(&(g_bcWifiInfo.stationConfig));
    return EN_LOGIC_1;
}

LOCAL EnNetConfigLogicMode logic678() {
    stopSmartConfig();
    return EN_LOGIC_8;
}

LOCAL void net_config_task(void* p)
{
    int tmp;
    // uint8_t startConfigFlag = 0;

    while(1) {
        vTaskDelay(500/portTICK_RATE_MS);
        if(EN_MS_NET_CONFIG != getModuleState()) {
            // startConfigFlag = 0;
            continue;
        }
        bc_printf("net_config stack left: %d\n", uxTaskGetStackHighWaterMark(NULL));
        // startConfigFlag = 1;

        if(g_intrTriggered) {
            g_intrTriggered = 0;
            tmp = AP_SET_GPIO_CHECK_COUNT;
            while(tmp--) {
                if(GPIO_INPUT_GET(13) != AP_SET_GPIO_STATUS_PRESSED) {
                    break;
                }
                vTaskDelay(AP_SET_GPIO_CHECK_UNIT/portTICK_RATE_MS);
            }
            if(tmp <= 0) {
                g_NetConfigLogic = EN_LOGIC_2;
            } else {
                g_NetConfigLogic = EN_LOGIC_3;
            }
        }

        g_runCount = 0;
        g_netConfigOk = 0;

        while(1) {
            uint8_t withdrawConfig = 0;
            switch(g_NetConfigLogic) {
                case EN_LOGIC_1:
                    g_NetConfigLogic = logic1();
                    withdrawConfig = 1;
                    break;
                case EN_LOGIC_6:
                case EN_LOGIC_7:
                case EN_LOGIC_8:
                    g_NetConfigLogic = logic678();
                    withdrawConfig = 1;
                    break;
                case EN_LOGIC_2:
                case EN_LOGIC_4:
                    g_NetConfigLogic = logic24();
                    break;
                case EN_LOGIC_3:
                case EN_LOGIC_5:
                    bc_printf("logic35: \n");
                    g_NetConfigLogic = logic35();
                    break;
            }
            if(withdrawConfig) {
                break;
            }
            vTaskDelay(SMART_CONFIG_CHECK_UNIT/portTICK_RATE_MS);
        }
        updateModuleState(EN_MS_CONNECT_SERVER);
        updateIndicator(EN_INDICATOR_SERV_CONNECTING);
    }

    vTaskDelete(NULL);
}

void net_config_task_init()
{
    int ret;

    bc_printf("task %s creating\n", NET_CONFIG_TASK_NAME);

    ETS_GPIO_INTR_DISABLE();
    GPIO_ConfigTypeDef io_in_conf;
    io_in_conf.GPIO_IntrType = GPIO_PIN_INTR_NEGEDGE;
    io_in_conf.GPIO_Mode = GPIO_Mode_Input;
    io_in_conf.GPIO_Pin = GPIO_Pin_13;
    io_in_conf.GPIO_Pullup = GPIO_PullUp_EN;
    gpio_config(&io_in_conf);
    gpio_intr_handler_register((void *)gpio_intr_handler, NULL);
    ETS_GPIO_INTR_ENABLE();

    ret = xTaskCreate(net_config_task,
                      NET_CONFIG_TASK_NAME,
                      NET_CONFIG_TASK_STACK_WORDS,
                      NULL,
                      NET_CONFIG_TASK_PRIORITY,
                      &net_config_task_handler);

    if (ret != pdPASS)  {
        bc_printf("create task %s failed\n", NET_CONFIG_TASK_NAME);
    }
}

void updateNetConfigLogic(EnNetConfigLogicMode logic_mode)
{
    g_NetConfigLogic = logic_mode;
}

void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata)
{
    switch(status) {
        case SC_STATUS_WAIT:
            printf("SC_STATUS_WAIT\n");
            break; 
        case SC_STATUS_FIND_CHANNEL:
            printf("SC_STATUS_FIND_CHANNEL\n");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            printf("SC_STATUS_GETTING_SSID_PSWD\n");
            sc_type *type = pdata;
            if (*type == SC_TYPE_ESPTOUCH) {
                printf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
            } else {
                printf("SC_TYPE:SC_TYPE_AIRKISS\n");
            }
            break;
        case SC_STATUS_LINK:
            printf("SC_STATUS_LINK\n");
            struct station_config *sta_conf = pdata;

            wifi_station_set_config(sta_conf);
            wifi_station_disconnect();
            wifi_station_connect();
            break;
        case SC_STATUS_LINK_OVER:
            printf("SC_STATUS_LINK_OVER\n");
            if (pdata != NULL) {
                //SC_TYPE_ESPTOUCH
                uint8 phone_ip[4] = {0};

                memcpy(phone_ip, (uint8*)pdata, 4);
                printf("Phone ip: %d.%d.%d.%d\n",phone_ip[0],phone_ip[1],phone_ip[2],phone_ip[3]);
            } else {
                //SC_TYPE_AIRKISS - support airkiss v2.0
                // airkiss_start_discover();
            }
            g_netConfigOk = 1;
            stopSmartConfig();
            break;
    }

}

int startLocalServer()
{
    int ret;
	int len;
    int wait_times = 0;
    int local_server_sockfd;
    int local_client_sockfd;
    struct sockaddr_in sock_addr;
    int n;
	char user[MAX_BC_USER_NAME_LEN];
	char ssid[MAX_WIFI_SSID_LEN];
	char pass[MAX_WIFI_PASSWORD_LEN];
    PackBuf * p_pack_buf = BP_NULL;
    BP_UINT8 type_and_flags;
    BP_SpecsetStr str_specset;
    fd_set rfds;
    struct timeval tv;
    socklen_t addr_len;
    static BPSCmdPostField postField;
    static BPSPacketData bpsPackData;

    /* create the socket */
    local_server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (local_server_sockfd < 0) {
        bc_printf("failed: local_server_sockfd < 0\n");
        return -1;
    }
    /* initialize the server parameters  */
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port = htons(LOCAL_SERVER_TCP_PORT);

    /* bind the socket with the server parameters */
    ret = bind(local_server_sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret) {
        bc_printf("failed: bind != 0\n");
        close(local_server_sockfd);
        return -2;
    }

    /* the local server starts listening */
    ret = listen(local_server_sockfd, LOCAL_SERVER_LISTEN_BACKLOG);

    if (ret) {
        bc_printf("failed: listen != 0\n");
        close(local_server_sockfd);
        return -3;
    }

    /* first set the polling period if there is no client connect to the local server*/
    tv.tv_sec = 0;
    tv.tv_usec = SMART_CONFIG_CHECK_UNIT * 1000;

    /* main task cycle */
    while(1) {
        /* initialize the fd set */
        FD_ZERO(&rfds);
        FD_SET(local_server_sockfd, &rfds);

        /* wait message from the fd set */
        ret = select(local_server_sockfd+1, &rfds, NULL, NULL, &tv);
        
        if(ret < 0) {
            /* error occurred */
            bc_printf("failed: select != 0\n");
            close(local_server_sockfd);
            return -4;
        } else if(0 == ret) {
            /* timeout occurred */
            tv.tv_sec = 0;
            tv.tv_usec = SMART_CONFIG_CHECK_UNIT * 1000;
            if(g_runCount++ > SMART_CONFIG_CHECK_COUNT) {
                close(local_server_sockfd);
                bc_printf("failed: g_runCount > SMART_CONFIG_CHECK_COUNT\n");
                return -5;
            }
        } else if(FD_ISSET(local_server_sockfd, &rfds)) {
            /* client connection occured */
            FD_CLR(local_server_sockfd, &rfds);
            local_client_sockfd = accept(local_server_sockfd, (struct sockaddr*)&sock_addr, &addr_len);
            if (local_client_sockfd < 0) {
                close(local_server_sockfd);
                return -6;
            }
            len = handleBpPacket(local_client_sockfd, RECV_BUF, RECV_BUF_SIZE, &type_and_flags);
            if(len <= 0) {
                close(local_client_sockfd);
                continue;
            }

            /* check if the BPPacket of SPECSET */
            if(BP_PACK_TYPE_SPECSET != ((type_and_flags >> 4) & 0x0F)) {
                close(local_client_sockfd);
                continue;
            }

            bzero(&ssid, MAX_WIFI_SSID_LEN);
            bzero(&pass, MAX_WIFI_PASSWORD_LEN);
            bzero(&user, MAX_BC_USER_NAME_LEN);
            str_specset.Ssid = ssid;
            str_specset.Password = pass;
            str_specset.UserName = user;

            n = BP_ParseSpecset(&BPContextEmbeded, &str_specset, RECV_BUF, len);
            bc_printf("* ParseSpecset err: %x\n", n);
            if(0 != n) {
                str_specack.Type = str_specset.Type;
                str_specack.RetCode = 0x06;
                p_pack_buf = BP_PackSpecack(&BPContextEmbeded, &str_specack);
                n=send(local_client_sockfd,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
            } else {
                /* 0 -> OK */
                if(0 != ConfigWrite(EN_CONFIG_ID_ADMIN, (const char *)user)) {
                    str_specack.RetCode = 0xFF;
                } else {
                    str_specack.RetCode = 0;
                    BPContextEmbeded.AdminNameLen = strlen(user);
                    memcpy_bps(BPContextEmbeded.AdminName, user, BPContextEmbeded.AdminNameLen);
                    BPContextEmbeded.AdminName[BPContextEmbeded.AdminNameLen] = '\0';
                }
                str_specack.Type = str_specset.Type;
                bc_printf("* :%s,%s,%s: \n", ssid, pass, user);
                p_pack_buf = BP_PackSpecack(&BPContextEmbeded, &str_specack);
                n=send(local_client_sockfd,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
            }
            close(local_client_sockfd);
            close(local_server_sockfd);
            if(0 != str_specack.RetCode) {
                bc_printf("failed: 0 != str_specack.RetCode\n");
                return -7;
            }
            break;
        }
    }

    bc_printf("success: return 0\n");
    return 0;
}
