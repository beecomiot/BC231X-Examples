#ifndef __BC_COMM_H__
#define __BC_COMM_H__

#include <bp_public.h>

typedef struct BcWifiInfo {
    uint8_t isWifiInfoSet;
    struct station_config stationConfig;
} BcWifiInfo;

extern BcWifiInfo g_bcWifiInfo;

int ICACHE_FLASH_ATTR handleBpPacket(int sockfd, BP_UINT8 * recvBuf, BP_WORD size, BP_UINT8 * type_and_flags);

/******************************************************************************
 * FunctionName : local_server_thread_init
 * Description  : initialize the embeded BP context and create the local server
                  task
 * Parameters   : void
 * Returns      : void
*******************************************************************************/
void local_server_thread_init(void);

/******************************************************************************
 * FunctionName : bc_comm_init
 * Description  : create the Beecom thread task
 * Parameters   : void
 * Returns      : void
*******************************************************************************/
void bc_comm_init(void);

/******************************************************************************
 * FunctionName : bc_close
 * Description  : close the socket and set it -1
 * Parameters   : fd, the pointer to the socket file descriptor
 * Returns      : void
*******************************************************************************/
void bc_close(int * fd);

// /******************************************************************************
//  * FunctionName : comHandler
//  * Description  : serial port data received handler
//  * Parameters   : fd, data received; size, data size
//  * Returns      : void
// *******************************************************************************/
// void comHandler(uint8 * buf, uint32 size);

#endif

