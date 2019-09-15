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

#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define TASK_PERIOD_MS                      100     // 100ms

/* task stack size */
#define BPS_TASK_STACK_WORDS                512     // 256 x 4 bytes(1024 bytes)
#define LED_INDICATOR_TASK_STACK_WORDS      176
#define NET_CONFIG_TASK_STACK_WORDS         512
#define STATE_MNG_TASK_STACK_WORDS          176
#define BC_COMM_TASK_STACK_WORDS            512

#define SYS_CONFIG_TASK_STACK_WORDS         512

/* task name */
#define BPS_TASK_NAME                       "bps"
#define LED_INDICATOR_TASK_NAME             "indicator"
#define NET_CONFIG_TASK_NAME                "net_config"
#define STATE_MNG_TASK_NAME                 "state_mng"
#define BC_COMM_TASK_NAME                   "bc_comm"

#define SYS_CONFIG_TASK_NAME                "sys_config"

/* task priority */
#define LED_INDICATOR_TASK_PRIORITY         1       
#define BPS_TASK_PRORIOTY                   9       
#define NET_CONFIG_TASK_PRIORITY            7
#define STATE_MNG_TASK_PRIORITY             10
#define BC_COMM_TASK_PRIORITY               8

#define SYS_CONFIG_TASK_PRIORITY            7

#define HARDWARE_VERSION                    "1.0.0.0"
#define SOFTWARE_VERSION                    "2.0.0.0"


    #ifndef bc_printf
        #ifdef DEBUG
            #define bc_printf(fmt, ...) do {    \
                    static const char flash_str[] ICACHE_RODATA_ATTR STORE_ATTR = fmt;  \
                    printf(flash_str, ##__VA_ARGS__);   \
                } while(0)
        #else
            #define bc_printf(fmt, ...) 
        #endif
    #endif

#endif

