#ifndef __NET_CONFIG_H__
#define __NET_CONFIG_H__

#include "esp_common.h"

#define MAX_BC_USER_NAME_LEN 128
#define MAX_WIFI_SSID_LEN 32
#define MAX_WIFI_PASSWORD_LEN 64

#define RECV_BUF_SIZE 1024

#define AP_SET_GPIO_CHECK_UNIT  100 // 100ms
#define AP_SET_GPIO_CHECK_COUNT  (2000/AP_SET_GPIO_CHECK_UNIT)

#define AP_SET_GPIO_STATUS_PRESSED          0
#define AP_SET_GPIO_STATUS_SUSPEND          1

#define SMART_CONFIG_CHECK_UNIT    100 // 100ms
#define SMART_CONFIG_CHECK_COUNT    (45000/SMART_CONFIG_CHECK_UNIT) // 45s

#define LOCAL_SERVER_LISTEN_BACKLOG 1 // listen for only one client
#define LOCAL_SERVER_TCP_PORT 8024

typedef enum EnNetConfigLogicMode {
    EN_LOGIC_1, /* net configure OK -> stop net configure */
    EN_LOGIC_2, /* long pressed -> AP net configure */
    EN_LOGIC_3, /* clicked -> smart configure */
    EN_LOGIC_4, /* BPS -> AP net configure */
    EN_LOGIC_5, /* BPS -> smart configure */
    EN_LOGIC_6, /* BPS -> stop net configure */
    EN_LOGIC_7, /* net configure timeout -> stop net configure */
    EN_LOGIC_8, /* net configure failed -> stop net configure */
} EnNetConfigLogicMode;

extern xTaskHandle net_config_task_handler;;

int ICACHE_FLASH_ATTR handleBpPacket(int sockfd, BP_UINT8 * recvBuf, BP_WORD size, BP_UINT8 * type_and_flags);
void ICACHE_FLASH_ATTR smartconfig_done(sc_status status, void *pdata);
void net_config_task_init();
void updateNetConfigLogic(EnNetConfigLogicMode logic_mode);
int startLocalServer();

#endif

