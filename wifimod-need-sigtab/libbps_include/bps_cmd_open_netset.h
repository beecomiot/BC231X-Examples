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
/// @file   bps_cmd_open_netset.h
/// @brief  definations of functions and variable types for command 'open net setting'
/// 
/// @version    0.1
/// @author     Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_CMD_OPEN_NETSET_H
#define __BPS_CMD_OPEN_NETSET_H

#include <bps_public.h>
#include <bps_cwords.h>

#define CMD_OPEN_NETSET_WORD_REQ  0x06
#define CMD_OPEN_NETSET_WORD_RSP  0x07

typedef enum ReqTypeOpenNetset {
    TURN_OFF_OPEN_NETSET = 0,
    TURN_ON_OPEN_NETSET,
} ReqTypeOpenNetset;

typedef struct BPSCmdOpenNetsetReq {
    BPS_UINT8 type;
} BPSCmdOpenNetsetReq;

typedef struct BPSCmdOpenNetsetRsp {
    BPS_UINT8 retCode;
} BPSCmdOpenNetsetRsp;

/** 
  * @Brief BPSPackOpenNetsetReq construct packet of 'open net setting' request
  * @Param req the request parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackOpenNetsetReq(BPSCmdOpenNetsetReq * req, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSPackOpenNetsetRsp construct packet of 'open net setting' response
  * @Param rsp the response parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackOpenNetsetRsp(BPSCmdOpenNetsetRsp * rsp, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParseOpenNetsetReq parse packet of 'open net setting' request
  * @Param req the request data struct to store the message
  * @Param buf the buffer stored the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParseOpenNetsetReq(BPSCmdOpenNetsetReq * req, const BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParseOpenNetsetRsp parse packet of 'open net setting' response
  * @Param rsp the response data struct to store the message
  * @Param buf the buffer to store the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParseOpenNetsetRsp(BPSCmdOpenNetsetRsp * rsp, const BPS_UINT8 * buf, BPS_WORD size);

#ifdef BPS_MEM_DYN
    #define ParseOpenNetsetReqDyn     ParseOpenNetsetReq
    #define ParseOpenNetsetRspDyn     ParseOpenNetsetRsp
    #define BPSFreeMemOpenNetsetReq(x)     
    #define BPSFreeMemOpenNetsetRsp(x)     
#endif

#endif

