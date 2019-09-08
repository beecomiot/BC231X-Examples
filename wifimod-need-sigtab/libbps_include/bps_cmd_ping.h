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
/// @file   bps_cmd_ping.h
/// @brief  definations of functions and variable types for command 'ping'
/// 
/// @version    0.1
/// @author     Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_CMD_PING_H
#define __BPS_CMD_PING_H

#include <bps_public.h>
#include <bps_cwords.h>

#define CMD_PING_WORD_REQ  0x04
#define CMD_PING_WORD_RSP  0x05

typedef enum ReqTypePing {
    QUERY_RT_PING = 0,
    SET_RT_PING,
} ReqTypePing;

typedef struct BPSCmdPingReq {
    BPS_UINT8 type;
    BPS_UINT16 interval;
} BPSCmdPingReq;

typedef struct BPSCmdPingRsp {
    BPS_UINT16 interval;
} BPSCmdPingRsp;

/** 
  * @Brief BPSPackPingReq construct packet of 'ping' request
  * @Param req the request parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackPingReq(BPSCmdPingReq * req, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSPackPingRsp construct packet of 'ping' response
  * @Param rsp the response parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackPingRsp(BPSCmdPingRsp * rsp, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParsePingReq parse packet of 'ping' request
  * @Param req the request data struct to store the message
  * @Param buf the buffer stored the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParsePingReq(BPSCmdPingReq * req, const BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParsePingRsp parse packet of 'ping' response
  * @Param rsp the response data struct to store the message
  * @Param buf the buffer to store the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParsePingRsp(BPSCmdPingRsp * rsp, const BPS_UINT8 * buf, BPS_WORD size);

#ifdef BPS_MEM_DYN
    #define ParsePingReqDyn     ParsePingReq
    #define ParsePingRspDyn     ParsePingRsp
    #define BPSFreeMemPingReq(x)     
    #define BPSFreeMemPingRsp(x)     
#endif

#endif

