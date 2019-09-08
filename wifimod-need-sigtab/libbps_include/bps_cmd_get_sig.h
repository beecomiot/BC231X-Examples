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
/// @file   bps_cmd_get_sig.h
/// @brief  definations of functions and variable types for command 'get signal values'
/// 
/// @version    0.1
/// @author     Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_CMD_GET_SIG_H
#define __BPS_CMD_GET_SIG_H

#include <bps_public.h>
#include <bps_cwords.h>

#define CMD_GET_SIG_WORD_REQ  0x86
#define CMD_GET_SIG_WORD_RSP  0x87

typedef struct BPSCmdGetSigField {
    BPS_UINT16 signalId;
    BPS_UINT8 signalType;
    BPSSigTypeU value;
} BPSCmdGetSigField;

typedef struct BPSCmdGetSigReq {
    BPS_WORD sigNum;
    BPS_UINT16 * sigArray;
} BPSCmdGetSigReq;

typedef struct BPSCmdGetSigRsp {
    BPS_UINT8 retCode;
    BPS_WORD fieldNum;
    BPSCmdGetSigField * fieldArray;
    void * extension;
} BPSCmdGetSigRsp;

/** 
  * @Brief BPSPackGetSigReq construct packet of 'get signal values' request
  * @Param req the request parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackGetSigReq(BPSCmdGetSigReq * req, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSPackGetSigRsp construct packet of 'get signal values' response
  * @Param rsp the response parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackGetSigRsp(BPSCmdGetSigRsp * rsp, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParseGetSigReq parse packet of 'get signal values' request
  * @Param req the request data struct to store the message
  * @Param buf the buffer stored the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParseGetSigReq(BPSCmdGetSigReq * req, const BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParseGetSigRsp parse packet of 'get signal values' response
  * @Param rsp the response data struct to store the message. 
  *     Make sure that there are enough memory to store the message, or to use BPSParseGetSigRspDyn
  *     which is more efficient for memory usage.
  * @Param buf the buffer to store the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParseGetSigRsp(BPSCmdGetSigRsp * rsp, const BPS_UINT8 * buf, BPS_WORD size);

#ifdef BPS_MEM_DYN
/** 
  * @Brief BPSParseGetSigReqDyn parse packet of 'get signal values' request
  * @Param req the request data struct to store the message.
  *         YOU HAVE TO CLEAR THE MEMORY OF ALL THE POINTERS RECURSIVELY IN 'req' 
  *         when the address of 'req' is unreachable(if the function return 0, you need do nothing about clearing memory)
  * @Param buf the buffer stored the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */

BPS_UINT16 BPSParseGetSigReqDyn(BPSCmdGetSigReq * req, const BPS_UINT8 * buf, BPS_WORD size);
/** 
  * @Brief BPSParseGetSigRsp parse packet of 'get signal values' response
  * @Param rsp the response data struct to store the message.
  *         YOU HAVE TO CLEAR THE MEMORY OF ALL THE POINTERS RECURSIVELY IN 'rsp' 
  *         when the address of 'rsp' is unreachable(if the function return 0, you need do nothing about clearing memory)
  * @Param buf the buffer to store the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
  *         rsp 
 */
EXPORT_API BPS_UINT16 BPSParseGetSigRspDyn(BPSCmdGetSigRsp * rsp, const BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParseGetSigRsp parse packet of 'get signal values' response
  * @Param rsp the response data struct to store the message.
  *         YOU HAVE TO CLEAR THE MEMORY OF ALL THE POINTERS RECURSIVELY IN 'rsp' 
  *         when the address of 'rsp' is unreachable(if the function return 0, you need do nothing about clearing memory)
  * @Param buf the buffer to store the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
  *         rsp 
 */

/** 
  * @Brief BPSFreeMemGetSigReq free the memory of the request struct
  * @Param req the request data struct.
 */
EXPORT_API void BPSFreeMemGetSigReq(BPSCmdGetSigReq * req);

/** 
  * @Brief BPSFreeMemGetSigRsp free the memory of the response struct
  * @Param rsp the response data struct.
 */
EXPORT_API void BPSFreeMemGetSigRsp(BPSCmdGetSigRsp * rsp);
#endif

#endif

