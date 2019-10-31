/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"
#include <sys/types.h>   
#include <sys/stat.h>   
#include <fcntl.h>
#include <unistd.h>

/* user headers */
#include "user_config.h"
#include "wifi_control.h"
#include "gpio.h"
#include "uart.h"
#include "bc_comm.h"
#include "bps_task.h"
#include "led_indicator.h"
#include <file_config.h>
#include <bp_strlen.h>
#include <bps_memset.h>
#include <bps_strlen.h>
#include <bps_parse.h>
#include <bc_comm.h>
#include <state_mng.h>
#include <net_config.h>
#include <bps_net_mode.h>

#define FS1_FLASH_SIZE      (512*1024)

#define FS1_FLASH_ADDR      (0x100000)

#define SECTOR_SIZE         (4 * 1024)
#define LOG_BLOCK           (SECTOR_SIZE)
#define LOG_PAGE            (128)

#define FD_BUF_SIZE         32*4
#define CACHE_BUF_SIZE      (LOG_PAGE + 32)*8

static os_timer_t timer;

char admin[MAX_BC_USER_NAME_LEN];

uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;
        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}
/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR wait_for_connection_ready(uint8 flag)
{
    os_timer_disarm(&timer);
    if(wifi_station_connected()){
        bc_printf("connected\n");
    } else {
        bc_printf("reconnect after 2s\n");
        os_timer_setfn(&timer, (os_timer_func_t *)wait_for_connection_ready, NULL);
        os_timer_arm(&timer, 2000, 0);
    }
}

LOCAL void ICACHE_FLASH_ATTR on_wifi_connect(){
    os_timer_disarm(&timer);
    os_timer_setfn(&timer, (os_timer_func_t *)wait_for_connection_ready, NULL);
    os_timer_arm(&timer, 100, 0);
}

LOCAL void ICACHE_FLASH_ATTR on_wifi_disconnect(uint8_t reason){
    bc_printf("disconnect %d\n", reason);
}
void spiffs_fs1_init(void)
{
    struct esp_spiffs_config config;

    config.phys_size = FS1_FLASH_SIZE;
    config.phys_addr = FS1_FLASH_ADDR;
    config.phys_erase_block = SECTOR_SIZE;
    config.log_block_size = LOG_BLOCK;
    config.log_page_size = LOG_PAGE;
    config.fd_buf_size = FD_BUF_SIZE * 2;
    config.cache_buf_size = CACHE_BUF_SIZE;

    esp_spiffs_init(&config);
}

LOCAL void init_task(void* p)
{
    EnIndicatorState st = EN_INDICATOR_SERV_CONNECTING;
    int specModeCheckCount = 30;
    uint16_t aliveTime;
    uint8_t netMode;
    char tmpString[8];

    /* read SN and KEY */
    // Read file from generated image with mkspiffs tool 
    if(0 == ConfigRead(EN_CONFIG_ID_SN, SN, sizeof(SN)) && 0 == ConfigRead(EN_CONFIG_ID_KEY, KEY, sizeof(KEY))) {
        bc_printf("SN=%s\n", SN);
        bc_printf("KEY=%s\n", KEY);
    } else {
        bc_printf("error SN=%s\n", SN);
        bc_printf("error KEY=%s\n", KEY);
        st = EN_INDICATOR_UNCONFIGURE;
    }
    bc_printf("***********************\n");
    g_HDInfoFieldArray[0].len = strlen_bp(SN);

    if(0 == ConfigRead(EN_CONFIG_ID_ADMIN, admin, sizeof(admin))) {
        bc_printf("admin=%s\n", admin);
        BPContextEmbeded.AdminNameLen = strlen(admin);
        if(BPContextEmbeded.AdminNameLen == 0) {
            /* if the name length is 0, we also think it unconfigured */
            st = EN_INDICATOR_UNCONFIGURE;
        }
    } else {
        bc_printf("error admin=\n");
        st = EN_INDICATOR_UNCONFIGURE;
    }

    /* read AliveTime and NetMode */
    if(0 == ConfigRead(EN_CONFIG_ID_ALIVE_TIME, tmpString, sizeof(tmpString)-1)) {
        aliveTime = atoi(tmpString);
        bc_printf("aliveTime=%d\n", aliveTime);
    } else {
        aliveTime = BPContextEmbeded.BPAlivePeroid;
        sprintf(tmpString, "%d", aliveTime);
        ConfigWrite(EN_CONFIG_ID_ALIVE_TIME, tmpString);
        bc_printf("aliveTime(default)=%s\n", tmpString);
    }
    setAliveTime(aliveTime);

    if(0 == ConfigRead(EN_CONFIG_ID_NET_MODE, tmpString, sizeof(tmpString)-1)) {
        netMode = atoi(tmpString);
        bc_printf("netMode=%d\n", aliveTime);
    } else {
        netMode = NM_WIFI_SMARTCONFIG;
        sprintf(tmpString, "%d", netMode);
        ConfigWrite(EN_CONFIG_ID_NET_MODE, tmpString);
        bc_printf("netMode(default)=%s\n", tmpString);
    }
    setNetMode(netMode);

    /* create queue for serial */
    g_ComQueue = xQueueCreate(BPS_COM_QUEUE_SIZE, sizeof(BPSPacketData));
    if(NULL == g_ComQueue) {
        bc_printf("com queue created failed\n");
        st=EN_INDICATOR_CRASH;
    }

    /* create queue for serial */
    g_BcCommQueue = xQueueCreate(BC_COM_QUEUE_SIZE, sizeof(BPSPacketData));
    if(NULL == g_BcCommQueue) {
        bc_printf("bc com queue created failed\n");
        st=EN_INDICATOR_CRASH;
    }

    /* create queue for state manage */
    g_StateQueue = xQueueCreate(STATE_MNG_QUEUE_SIZE, sizeof(EnModuleState));
    if(NULL == g_StateQueue) {
        bc_printf("state queue created failed\n");
        st=EN_INDICATOR_CRASH;
    }

    memset_bps(&g_bcWifiInfo, 0, sizeof(g_bcWifiInfo));
    if(!wifi_station_get_config_default(&(g_bcWifiInfo.stationConfig))) {
        bc_printf("get wifi station info failed\n");
        g_bcWifiInfo.isWifiInfoSet = BPS_FALSE;
    } else {
        if(strlen_bps(g_bcWifiInfo.stationConfig.ssid) > 0 && strlen_bps(g_bcWifiInfo.stationConfig.password) > 0) {
            g_bcWifiInfo.isWifiInfoSet = BPS_TRUE;
        }
    }
    bc_printf("ssid=%s\n", g_bcWifiInfo.stationConfig.ssid);
    bc_printf("wifi_pwd=%s\n", g_bcWifiInfo.stationConfig.password);

    /* check whether to go to special mode */
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
    GPIO_DIS_OUTPUT(GPIO_ID_PIN(13));

    while(specModeCheckCount-- > 0) {
        if(GPIO_INPUT_GET(GPIO_ID_PIN(13))) {
            break;
        }
        vTaskDelay(100/portTICK_RATE_MS);
    }
    if(specModeCheckCount <= 0) {
        st = EN_INDICATOR_SPEC_MODE;
    }

    led_indicator_task_init(st);
    if(EN_INDICATOR_SPEC_MODE == st) {
        sys_config_task_init();
    } else if(EN_INDICATOR_CRASH != st) {
        bps_task_init();
        state_mng_task_init();
        bc_comm_init();
        net_config_task_init();
    }
    bc_printf("init stack left: %d\n", uxTaskGetStackHighWaterMark(NULL));
    vTaskDelete(NULL);
}

void user_init(void)
{
    /* initialize uart baud rate to 115200 */
    uart_init_new(BIT_RATE_9600);
    /* initialize spiffs */
    spiffs_fs1_init();
    /* set the uart handler */
	setBcRxHandler(comHandler);
    /* initialize BP context */
    BP_InitEmbededContext();
    memset_bps(&admin, 0, sizeof(admin));
    BPContextEmbeded.AdminName = admin;
    BPContextEmbeded.AdminNameLen = 0;

    bc_printf("SDK version:%s\n", system_get_sdk_version());
    // bc_printf("beecom->12345678\n");
    // set_on_station_connect(on_wifi_connect);
    // set_on_station_disconnect(on_wifi_disconnect);
	init_esp_wifi();
    // start_wifi_ap("beecom", "12345678");
    // GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 1);

    /* create the initialization task */
    xTaskCreate(init_task,
                      "init task",
                      256,
                      NULL,
                      15,
                      NULL);
}

