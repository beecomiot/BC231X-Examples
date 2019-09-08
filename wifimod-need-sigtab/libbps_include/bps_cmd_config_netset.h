///////////////////////////////////////////////////////////////////////////////
/// Copyright 2019 Ansersion
/// 
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
/// 
///     http://www.apache.org/licenses/LICENSE-2.0
/// 
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// @file   bps_cmd_config_netset.h
/// @brief  definations of functions and variable types for command 'configure net setting'
/// 
/// @version    0.1
/// @author     Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_CMD_CONFIG_NETSET_H
#define __BPS_CMD_CONFIG_NETSET_H

#include <bps_public.h>
#include <bps_cwords.h>

#define CMD_CONFIG_NETSET_WORD_REQ  0x08
#define CMD_CONFIG_NETSET_WORD_RSP  0x09

typedef enum ReqTypeConfigNet {
    QUERY_RT_CONFIG_NET = 0,
    SET_RT_CONFIG_NET,
} ReqTypeConfigNet;

typedef struct BPSCmdConfigNetsetReq {
    BPS_UINT8 type;
    BPS_UINT8 mode;
} BPSCmdConfigNetsetReq;

typedef struct BPSCmdConfigNetsetRsp {
    BPS_UINT8 retCode;
    BPS_UINT8 commType;
    BPS_UINT8 mode;
} BPSCmdConfigNetsetRsp;

/** 
  * @Brief BPSPackConfigNetsetReq construct packet of 'configure net setting' request
  * @Param req the request parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackConfigNetsetReq(BPSCmdConfigNetsetReq * req, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSPackConfigNetsetRsp construct packet of 'configure net setting' response
  * @Param rsp the response parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackConfigNetsetRsp(BPSCmdConfigNetsetRsp * rsp, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParseConfigNetsetReq parse packet of 'configure net setting' request
  * @Param req the request data struct to store the message
  * @Param buf the buffer stored the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParseConfigNetsetReq(BPSCmdConfigNetsetReq * req, const BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParseConfigNetsetRsp parse packet of 'configure net setting' response
  * @Param rsp the response data struct to store the message
  * @Param buf the buffer to store the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParseConfigNetsetRsp(BPSCmdConfigNetsetRsp * rsp, const BPS_UINT8 * buf, BPS_WORD size);

#ifdef BPS_MEM_DYN
    #define ParseConfigNetsetReqDyn     ParseConfigNetsetReq
    #define ParseConfigNetsetRspDyn     ParseConfigNetsetRsp
    #define BPSFreeMemConfigNetsetReq(x)     
    #define BPSFreeMemConfigNetsetRsp(x)     
#endif

#endif

