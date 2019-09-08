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
/// @file   bps_cmd_system_para.h
/// @brief  definations of functions and variable types for command 'configure system parameters'
/// 
/// @version    0.1
/// @author     Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_CMD_SYSTEM_PARA_H
#define __BPS_CMD_SYSTEM_PARA_H

#include <bps_public.h>
#include <bps_cwords.h>

#define CMD_SYSTEM_PARA_WORD_REQ  0xEE
#define CMD_SYSTEM_PARA_WORD_RSP  0xEF

typedef enum ConfigTypeSystemPara {
    READ_SYS_PARA=0,
    WRITE_SYS_PARA,
    NUM_SYS_PARA,
} ConfigTypeSystemPara;

typedef enum ParaTypeSystemPara {
    RESERVED_SYS_PARA_TYPE=0,
    SN_SYS_PARA_TYPE,
    KEY_SYS_PARA_TYPE,
    NUM_SYS_PARA_TYPE,
} ParaTypeSystemPara;

typedef struct BPSCmdSystemParaReq {
    BPS_UINT8 configType;
    BPS_UINT8 paraType;
    BPS_UINT8 len;
    BPS_UINT8 * data;
} BPSCmdSystemParaReq;

typedef struct BPSCmdSystemParaRsp {
    BPS_UINT8 configType;
    BPS_UINT8 paraType;
    BPS_UINT8 retCode;
    BPS_UINT8 len;
    BPS_UINT8 * data;
} BPSCmdSystemParaRsp;

/** 
  * @Brief BPSPackSystemParaReq construct packet of 'configure system parameters' request
  * @Param req the request parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackSystemParaReq(BPSCmdSystemParaReq * req, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSPackSystemParaRsp construct packet of 'configure system parameters' response
  * @Param rsp the response parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackSystemParaRsp(BPSCmdSystemParaRsp * rsp, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParseSystemParaReq parse packet of 'configure system parameters' request
  * @Param req the request data struct to store the message
  * @Param buf the buffer stored the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParseSystemParaReq(BPSCmdSystemParaReq * req, const BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParseSystemParaRsp parse packet of 'configure system parameters' response
  * @Param rsp the response data struct to store the message. 
  *     Make sure that there are enough memory to store the message, or to use BPSParseSystemParaRspDyn
  *     which is more efficient for memory usage.
  * @Param buf the buffer to store the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParseSystemParaRsp(BPSCmdSystemParaRsp * rsp, const BPS_UINT8 * buf, BPS_WORD size);

// TODO:
// #ifdef BPS_MEM_DYN
//     #define ParseSystemParaReqDyn     ParseSystemParaReq
// /** 
//   * @Brief BPSParseSystemParaRsp parse packet of 'configure system parameters' response
//   * @Param rsp the response data struct to store the message.
//   *         YOU HAVE TO CLEAR THE MEMORY OF ALL THE POINTERS RECURSIVELY IN 'rsp' 
//   *         when the address of 'rsp' is unreachable(if the function return 0, you need do nothing about clearing memory)
//   * @Param buf the buffer to store the message(point at the commond word position+1)
//   * @Param size the buffer size
//   * @return the number of bytes which the function handled, 0 means parsing failed/none
//   *         rsp 
//  */
// EXPORT_API BPS_UINT16 BPSParseSystemParaRspDyn(BPSCmdSystemParaRsp * rsp, const BPS_UINT8 * buf, BPS_WORD size);
// 
//     #define BPSFreeMemSystemParaReq(x)  
// /** 
//   * @Brief BPSFreeMemSystemParaRsp free the memory of the response struct
//   * @Param rsp the response data struct.
//  */
// EXPORT_API void BPSFreeMemSystemParaRsp(BPSCmdSystemParaRsp * rsp);
// #endif

#endif

