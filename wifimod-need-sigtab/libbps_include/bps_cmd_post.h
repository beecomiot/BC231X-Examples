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
/// @file   bps_cmd_post.h
/// @brief  definations of functions and variable types for command 'post signal'
/// 
/// @version    0.1
/// @author     Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_CMD_POST_H
#define __BPS_CMD_POST_H

#include <bps_public.h>
#include <bps_cwords.h>

#define CMD_POST_WORD_REQ  0x84
#define CMD_POST_WORD_RSP  0x85

typedef struct BPSCmdPostField {
    BPS_UINT16 signalId;
    BPS_UINT8 signalType;
    BPSSigTypeU value;
} BPSCmdPostField;

typedef struct BPSCmdPostReq {
    BPSCmdPostField * fieldArray;
    BPS_WORD fieldNum;
    /** maxFiledNum is set to be safe only for parsing that without dynamical memory allocation */
    BPS_WORD maxFieldNum;
} BPSCmdPostReq;

typedef struct BPSCmdPostRsp {
    BPS_UINT8 retCode;
    void * extension;
} BPSCmdPostRsp;

/** 
  * @Brief BPSPackPostReq construct packet of 'post signal' request
  * @Param req the request parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackPostReq(BPSCmdPostReq * req, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSPackPostRsp construct packet of 'post signal' response
  * @Param rsp the response parameter
  * @Param buf the buffer to store the message(point at the commond word position)
  * @Param size the buffer size
  * @return the number of bytes which the function handled
 */
EXPORT_API BPS_UINT16 BPSPackPostRsp(BPSCmdPostRsp * rsp, BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParsePostReq parse packet of 'post signal' request
  * @Param req the request data struct to store the message
  * @Param buf the buffer stored the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParsePostReq(BPSCmdPostReq * req, const BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParsePostRsp parse packet of 'post signal' response
  * @Param rsp the response data struct to store the message. 
  *     Make sure that there are enough memory to store the message, or to use BPSParsePostRspDyn
  *     which is more efficient for memory usage.
  * @Param buf the buffer to store the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */
EXPORT_API BPS_UINT16 BPSParsePostRsp(BPSCmdPostRsp * rsp, const BPS_UINT8 * buf, BPS_WORD size);

#ifdef BPS_MEM_DYN
/** 
  * @Brief BPSParsePostReqDyn parse packet of 'post signal' request
  * @Param req the request data struct to store the message.
  *         YOU HAVE TO CLEAR THE MEMORY OF ALL THE POINTERS RECURSIVELY IN 'req' 
  *         when the address of 'req' is unreachable(if the function return 0, you need do nothing about clearing memory)
  * @Param buf the buffer stored the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
 */

BPS_UINT16 BPSParsePostReqDyn(BPSCmdPostReq * req, const BPS_UINT8 * buf, BPS_WORD size);
/** 
  * @Brief BPSParsePostRsp parse packet of 'post signal' response
  * @Param rsp the response data struct to store the message.
  *         YOU HAVE TO CLEAR THE MEMORY OF ALL THE POINTERS RECURSIVELY IN 'rsp' 
  *         when the address of 'rsp' is unreachable(if the function return 0, you need do nothing about clearing memory)
  * @Param buf the buffer to store the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
  *         rsp 
 */
EXPORT_API BPS_UINT16 BPSParsePostRspDyn(BPSCmdPostRsp * rsp, const BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief BPSParsePostRsp parse packet of 'post signal' response
  * @Param rsp the response data struct to store the message.
  *         YOU HAVE TO CLEAR THE MEMORY OF ALL THE POINTERS RECURSIVELY IN 'rsp' 
  *         when the address of 'rsp' is unreachable(if the function return 0, you need do nothing about clearing memory)
  * @Param buf the buffer to store the message(point at the commond word position+1)
  * @Param size the buffer size
  * @return the number of bytes which the function handled, 0 means parsing failed/none
  *         rsp 
 */

/** 
  * @Brief BPSFreeMemPostReq free the memory of the request struct
  * @Param req the request data struct.
 */
EXPORT_API void BPSFreeMemPostReq(BPSCmdPostReq * req);

/** 
  * @Brief BPSFreeMemPostRsp free the memory of the response struct
  * @Param rsp the response data struct.
 */
EXPORT_API void BPSFreeMemPostRsp(BPSCmdPostRsp * rsp);
#endif

#endif

