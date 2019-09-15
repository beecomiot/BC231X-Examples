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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "espressif/c_types.h"
#include "espressif/esp_misc.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "bc_comm.h"
#include "user_config.h"
#include "wifi_control.h"
#include "espressif/pwm.h"
#include "lwip/netdb.h"
#include "gpio.h"

#define BC_THREAD_NAME "bc"
#define BC_THREAD_STACK_WORDS 2048 // 1024 x 4 bytes(4096bytes)
#define BC_THREAD_PRORIOTY 6 // thread priority

#define LOCAL_SERVER_THREAD_NAME "local_server"
#define LOCAL_SERVER_THREAD_STACK_WORDS 2024 // 1024 x 4 bytes(4096bytes)
#define LOCAL_SERVER_THREAD_PRORIOTY 6 // thread priority
#define BC_ERROR_DELAY 5000     // 5000ms

#define LOCAL_SERVER_THREAD_POLL_PERIOD 5 // 5 second
#define LOCAL_SERVER_LISTEN_BACKLOG 1 // listen for only one client
#define LOCAL_SERVER_THREAD_BINDING_POLL_PERIOD 1 // 1 second
#define LOCAL_SERVER_TCP_PORT 8024

int gWifiApClientConnected = 0;

typedef void (* wifi_state_cb_t)();

wifi_state_cb_t on_station_first_connect = NULL;
wifi_state_cb_t on_station_connect = NULL;
wifi_disco_cb_t on_station_disconnect = NULL;

wifi_state_cb_t on_client_connect = NULL;
wifi_state_cb_t on_client_disconnect = NULL;

volatile bool wifi_station_static_ip = false;
volatile bool wifi_station_is_connected = false;

void ICACHE_FLASH_ATTR wifi_event_handler_cb(System_Event_t *event)
{
    struct sockaddr_in sock_addr;
    static bool station_was_connected = false;
    if (event == NULL) {
        return;
    }

    // bc_printf("[WiFi] event %u\n", event->event_id);
    // local_server_thread_init();

    switch (event->event_id) {
        case EVENT_STAMODE_DISCONNECTED:
			bc_printf("EVENT_STAMODE_DISCONNECTED\n");
            wifi_station_is_connected = false;
            Event_StaMode_Disconnected_t *ev = (Event_StaMode_Disconnected_t *)&event->event_info;
            if(on_station_disconnect){
                on_station_disconnect(ev->reason);
            }
            break;
        case EVENT_STAMODE_CONNECTED:
			bc_printf("EVENT_STAMODE_CONNECTED\n");
            if(wifi_station_static_ip){
                wifi_station_is_connected = true;
                if(!station_was_connected){
                    station_was_connected = true;
                    if(on_station_first_connect){
                        on_station_first_connect();
                    }
                }
                if(on_station_connect){
                    on_station_connect();
                }
            }
            break;
        case EVENT_STAMODE_DHCP_TIMEOUT:
			bc_printf("EVENT_STAMODE_DHCP_TIMEOUT\n");
            if(wifi_station_is_connected){
                wifi_station_is_connected = false;
                if(on_station_disconnect){
                    on_station_disconnect(REASON_UNSPECIFIED);
                }
            }
            break;
        case EVENT_STAMODE_GOT_IP:
			bc_printf("EVENT_STAMODE_GOT_IP\n");
			// bc_thread_init();
            wifi_station_is_connected = true;
            if(!station_was_connected){
                station_was_connected = true;
                if(on_station_first_connect){
                    on_station_first_connect();
                }
            }
            if(on_station_connect){
                on_station_connect();
            }
            break;

        case EVENT_SOFTAPMODE_STACONNECTED:
            gWifiApClientConnected = 1;
			bc_printf("EVENT_SOFTAPMODE_STACONNECTED: %d\n", gWifiApClientConnected);
            if(on_client_connect){
                on_client_connect();
            }
            break;
        case EVENT_SOFTAPMODE_STADISCONNECTED:
			bc_printf("EVENT_SOFTAPMODE_STADISCONNECTED\n");
            gWifiApClientConnected = 0;
            if(on_client_disconnect){
                on_client_disconnect();
            }
            break;
        default:
            break;
    }
}

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/

void ICACHE_FLASH_ATTR set_on_station_first_connect(wifi_state_cb_t cb){
    on_station_first_connect = cb;
}

void ICACHE_FLASH_ATTR set_on_station_connect(wifi_state_cb_t cb){
    on_station_connect = cb;
}

void ICACHE_FLASH_ATTR set_on_station_disconnect(wifi_disco_cb_t cb){
    on_station_disconnect = cb;
}

void ICACHE_FLASH_ATTR set_on_client_connect(wifi_state_cb_t cb){
    on_client_connect = cb;
}

void ICACHE_FLASH_ATTR set_on_client_disconnect(wifi_state_cb_t cb){
    on_client_disconnect = cb;
}
bool ICACHE_FLASH_ATTR wifi_set_mode(WIFI_MODE mode){
    if(!mode){
        bool s = wifi_set_opmode(mode);
        wifi_fpm_open();
        wifi_fpm_set_sleep_type(MODEM_SLEEP_T);
        wifi_fpm_do_sleep(0xFFFFFFFF);
        return s;
    }
    wifi_fpm_close();
    return wifi_set_opmode(mode);
}

WIFI_MODE ICACHE_FLASH_ATTR init_esp_wifi(){
    wifi_set_event_handler_cb(wifi_event_handler_cb);
    WIFI_MODE mode = wifi_get_opmode_default();
    wifi_set_mode(mode);
    return mode;
}

bool ICACHE_FLASH_ATTR start_wifi_ap(const char * ssid, const char * pass){
    WIFI_MODE mode = wifi_get_opmode();
    if((mode & SOFTAP_MODE) == 0){
        mode |= SOFTAP_MODE;
        if(!wifi_set_mode(mode)){
            bc_printf("Failed to enable AP mode!\n");
            return false;
        }
    }
    if(!ssid){
        bc_printf("No SSID Given. Will start the AP saved in flash\n");
        return true;
    }
    struct softap_config config;
    bzero(&config, sizeof(struct softap_config));
    config.ssid_len = 0;
    sprintf(config.ssid, ssid);
    if(pass){
        sprintf(config.password, pass);
    }
    config.max_connection = 1;
    config.beacon_interval = 100;
    config.channel = 6;
    config.authmode = AUTH_WPA_WPA2_PSK;
    config.ssid_hidden = 0;
    return wifi_softap_set_config(&config);
}

bool ICACHE_FLASH_ATTR start_wifi_station(const char * ssid, const char * pass){
    WIFI_MODE mode = wifi_get_opmode();
    if((mode & STATION_MODE) == 0){
        mode |= STATION_MODE;
        if(!wifi_set_mode(mode)){
            bc_printf("Failed to enable Station mode!\n");
            return false;
        }
    }
    if(!ssid){
        bc_printf("No SSID Given. Will connect to the station saved in flash\n");
        return true;
    }
    struct station_config config;
    memset(&config, 0, sizeof(struct station_config));
    strcpy(config.ssid, ssid);
    if(pass){
        strcpy(config.password, pass);
    }
    if(!wifi_station_set_config(&config)){
        bc_printf("Failed to set Station config!\n");
        return false;
    }

    if(!wifi_station_dhcpc_status()){
        bc_printf("DHCP is not started. Starting it...\n");
        if(!wifi_station_dhcpc_start()){
            bc_printf("DHCP start failed!\n");
            return false;
        }
    }
    return wifi_station_connect();
}

bool ICACHE_FLASH_ATTR wifi_ap_enabled(){
    return !!(wifi_get_opmode() & SOFTAP_MODE);
}

bool ICACHE_FLASH_ATTR wifi_station_connected(){
    if(!wifi_station_is_connected){
        return false;
    }
    WIFI_MODE mode = wifi_get_opmode();
    if((mode & STATION_MODE) == 0){
        return false;
    }
    STATION_STATUS wifistate = wifi_station_get_connect_status();
    wifi_station_is_connected = (wifistate == STATION_GOT_IP || (wifi_station_static_ip && wifistate == STATION_CONNECTING));
    return wifi_station_is_connected;
}

bool ICACHE_FLASH_ATTR stop_wifi_station(){
    WIFI_MODE mode = wifi_get_opmode();
    mode &= ~STATION_MODE;
    if(!wifi_set_mode(mode)){
        bc_printf("Failed to disable Station mode!\n");
        return false;
    }
    return true;
}

bool ICACHE_FLASH_ATTR stop_wifi_ap(){
    WIFI_MODE mode = wifi_get_opmode();
    mode &= ~SOFTAP_MODE;
    if(!wifi_set_mode(mode)){
        os_printf("Failed to disable AP mode!\n");
        return false;
    }   
    return true;
}

