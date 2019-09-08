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
/// @file   bps_cmd_comm_test.h
/// @brief  definations of functions and variable types for command 'communication test'
/// 
/// @version    0.1
/// @author     Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_CMD_COMM_TEST_H
#define __BPS_CMD_COMM_TEST_H

#include <bps_public.h>
#include <bps_cwords.h>

#define CMD_COMM_TEST_WORD_REQ  0x00
#define CMD_COMM_TEST_WORD_RSP  0x01

typedef struct BPSCmdCommTestReq {
    BPS_UINT8 recv;
} BPSCmdCommTestReq;

typedef struct BPSCmdCommTestRsp {
    BPS_UINT8 recv;
} BPSCmdCommTestRsp;

/** 
  * @Brief BPSPackCommTestReq construct packet of 'communication test' request
  * @Param req the request parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackCommTestReq(BPSCmdCommTestReq * req, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSPackCommTestRsp construct packet of 'communication test' response
  * @Param rsp the response parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackCommTestRsp(BPSCmdCommTestRsp * rsp, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParseCommTestReq parse packet of 'communication test' request
  * @Param req the request data struct to store the message
  * @Param buf the buffer stored the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParseCommTestReq(BPSCmdCommTestReq * req, const BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParseCommTestRsp parse packet of 'communication test' response
  * @Param rsp the response data struct to store the message
  * @Param buf the buffer to store the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParseCommTestRsp(BPSCmdCommTestRsp * rsp, const BPS_UINT8 * buf, BPS_WORD size);

#ifdef BPS_MEM_DYN
    #define ParseCommTestReqDyn     ParseCommTestReq
    #define ParseCommTestRspDyn     ParseCommTestRsp
    #define BPSFreeMemCommTestReq(x)     
    #define BPSFreeMemCommTestRsp(x)     
#endif

#endif

