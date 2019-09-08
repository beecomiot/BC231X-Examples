#ifndef __FILE_CONFIG_H__
#define __FILE_CONFIG_H__

#include "esp_common.h"
#include <sys/types.h>   
#include <sys/stat.h>   
#include <fcntl.h>
#include <unistd.h>

/**
  */

#define CONFIG_FILE_SN          "/sn.cfg"
#define CONFIG_FILE_KEY         "/key.cfg"
#define CONFIG_FILE_ADMIN       "/admin.cfg"

typedef enum EnConfigId {
    EN_CONFIG_ID_RESERVED=0,
    EN_CONFIG_ID_SN,
    EN_CONFIG_ID_KEY,
    EN_CONFIG_ID_ADMIN,
} EnConfigId;

/******************************************************************************
 * FunctionName : ConfigRead
 * Description  : read the configuration value 
 * Parameters   : config_id, please refer to <enum EnConfigId>
 *              : buf[out], store the value. the size must be more than 256bytes
 *              : size, the size of buf
 * Returns      : 0, success; other, failed
*******************************************************************************/
int ConfigRead(uint8_t config_id, char * buf, uint32 size);

/******************************************************************************
 * FunctionName : ConfigWrite
 * Description  : write the configuration value
 * Parameters   : config_id, please refer to <enum EnConfigId>
 *              : buf, the value string
 * Returns      : 0, success; other, failed
*******************************************************************************/
int ConfigWrite(uint8_t config_id, const char * buf);

const char * GetConfigFileName(uint8_t config_id);

#endif

