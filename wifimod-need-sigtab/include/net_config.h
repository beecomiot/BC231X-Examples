#ifndef __NET_CONFIG_H__
#define __NET_CONFIG_H__

#include "esp_common.h"
#include "bc_comm.h"

#define MAX_BC_USER_NAME_LEN 128
#define MAX_WIFI_SSID_LEN 32
#define MAX_WIFI_PASSWORD_LEN 64

#define RECV_BUF_SIZE 1024

#define AP_SET_GPIO_CHECK_UNIT  100 // 100ms
#define AP_SET_GPIO_CHECK_COUNT  (2000/AP_SET_GPIO_CHECK_UNIT)

#define AP_SET_GPIO_STATUS_PRESSED          0
#define AP_SET_GPIO_STATUS_SUSPEND          1

#define SMART_CONFIG_CHECK_UNIT_S    0 // 0s
#define SMART_CONFIG_CHECK_UNIT    100 // 100ms
#define SMART_CONFIG_CHECK_COUNT    (45000/SMART_CONFIG_CHECK_UNIT) // 45s

#define AP_CONFIG_CHECK_UNIT_S 1 // 1s
#define AP_CONFIG_CHECK_UNIT    0 // 0ms
#define AP_CONFIG_CHECK_COUNT    (300/AP_CONFIG_CHECK_UNIT_S) // 300s

#define LOCAL_SERVER_LISTEN_BACKLOG 1 // listen for only one client
#define LOCAL_SERVER_TCP_PORT 8024

#define SOFTAP_SSID     "beecomIoT"
#define SOFTAP_PASS     "beecomIoT"

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
EnNetConfigLogicMode getNetConfigLogic();
int startLocalServer(unsigned int sec, unsigned int usec, unsigned int count);
void setAliveTime(uint16_t tmp);
const uint16_t getAliveTime();
void setNetMode(uint8_t tmp);
const uint8_t getNetMode();

#endif

