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
/// @file 	bps_public.h
/// @brief 	public tool functions header file
/// 
/// @version 	0.1
/// @author 	Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_PUBLIC_H
#define __BPS_PUBLIC_H

#include <config.h>

#define BPS_HEADER 		0xBBCC
#define BPS_HEADER_SIZE 		2
#define BPS_HEADER_POSITION 		0

#define BPS_VERSION 	0
#define BPS_VERSION_SIZE 	1
#define BPS_VERSION_POSITION 	(BPS_HEADER_POSITION + BPS_HEADER_SIZE)

#define BPS_ADDR_MCU_DEFAULT 	0
#define BPS_ADDR_SIZE 	1
#define BPS_ADDR_POSITION 	(BPS_VERSION_POSITION + BPS_VERSION_SIZE)

#define BPS_REMAIN_LEN_SIZE 	2
#define BPS_REMAIN_LEN_POSITION 	(BPS_ADDR_POSITION + BPS_ADDR_SIZE)


#define BPS_CMD_WORD_SIZE 	1
#define BPS_CMD_WORD_POSITION 	    (BPS_REMAIN_LEN_POSITION + BPS_REMAIN_LEN_SIZE)

#define BPS_CHECKSUM_SIZE 	1

#define BPS_SetBig16 	BPS_SetNet16
#define BPS_SetBig32 	BPS_SetNet32

#define BPS_GetBig16 	BPS_GetNet16
#define BPS_GetBig32 	BPS_GetNet32

#define BPS_MAX_STRING_LEN  255

typedef enum BPSSigType {
	BPS_SIG_TYPE_U32 = 0, 
	BPS_SIG_TYPE_U16,
	BPS_SIG_TYPE_I32, 
	BPS_SIG_TYPE_I16, 
	BPS_SIG_TYPE_ENM, 
	BPS_SIG_TYPE_FLT, 
	BPS_SIG_TYPE_STR, 
	BPS_SIG_TYPE_BOOLEAN, 
	BPS_SIG_TYPE_TIME, 
	BPS_SIG_TYPE_DATE, 
	BPS_SIG_TYPE_UNKNOWN = 15,
} BPSSigType;

typedef union BPSSigTypeU {
	BPS_UINT32 	t_u32;
	BPS_UINT16 	t_u16;
	BPS_INT32 	t_i32;
	BPS_INT16 	t_i16;
	BPS_UINT16 	t_enm;
	BPS_FLOAT 	t_flt;
	BPS_UINT8* 	t_str;
	BPS_UINT8 	t_bool;
	BPS_UINT32 	t_time;
	BPS_UINT32 	t_date;
} BPSSigTypeU;

EXPORT_API BPS_UINT8 * BPS_SetNet16(BPS_UINT8 * dst, BPS_UINT16 val);
EXPORT_API BPS_UINT8 * BPS_SetNet32(BPS_UINT8 * dst, BPS_UINT32 val);
EXPORT_API BPS_UINT8 * BPS_SetNBytes(BPS_UINT8 * dst, BPS_UINT8 * src, BPS_WORD num);

EXPORT_API BPS_UINT8 * BPS_Set2ByteField(BPS_UINT8 * pack, BPS_UINT8 * field, BPS_UINT16 field_len);
EXPORT_API BPS_UINT8 * BPS_Set1ByteField(BPS_UINT8 * pack, const BPS_UINT8 * field, BPS_UINT8 field_len);

EXPORT_API const BPS_UINT8 * BPS_GetNet16(const BPS_UINT8 * src, BPS_UINT16 * val);
EXPORT_API const BPS_UINT8 * BPS_GetNet32(const BPS_UINT8 * src, BPS_UINT32 * val);

EXPORT_API const BPS_UINT8 * BPS_Get2ByteField(BPS_UINT8 * pack, BPS_UINT8 * field_buf, BPS_UINT16 * field_len);
EXPORT_API BPS_WORD BPS_GetSigValueLen(BPSSigType type, BPSSigTypeU value);
/* except BPS_SIG_TYPE_STR */
EXPORT_API BPS_WORD BPS_GetSigValueLen2(BPSSigType type);
EXPORT_API BPS_UINT8 * BPS_SetSigValue(BPS_UINT8 * pack, BPSSigType type, BPSSigTypeU value);
EXPORT_API const BPS_UINT8 * BPS_GetSigValue(const BPS_UINT8 * pack, BPSSigType type, BPSSigTypeU * value, BPS_WORD len);

/** 
  * @Brief IsBPSHeader2 check b1==0xBB and b2==0xCC
  * @Param b1   first byte
  * @Param b2 	second byte
  * @return Success     somegthing not 0
  *         Failed      0
 */
EXPORT_API BPS_UINT8 IsBPSHeader2(BPS_UINT8 b1, BPS_UINT8 b2);

/** 
  * @Brief IsBPSHeader check buf[0]==0xBB and buf[1]==0xCC
  * @Param buf   the message buffer
  * @return Success     somegthing not 0
  *         Failed      0
 */
EXPORT_API BPS_UINT8 IsBPSHeader(BPS_UINT8 * buf);

/** 
  * @Brief IsBPSVersionCompat check the BP-S version compatible
  * @Param buf   the version
  * @return Success     somegthing not 0 when the 'version' is smaller than BPS_VERSION
  *         Failed      0
 */
EXPORT_API BPS_UINT8 IsBPSVersionCompat(BPS_UINT8 version);

/** 
  * @Brief GetBPSSenderAddr get the sender address
  * @Param buf   the address
  * @return the address of sender(from 0-15)
 */
EXPORT_API BPS_UINT8 GetBPSSenderAddr(BPS_UINT8 addr);

/** 
  * @Brief GetBPSRecverAddr get the receiver address
  * @Param buf   the address
  * @return the address of receiver(from 0-15)
 */
EXPORT_API BPS_UINT8 GetBPSRecverAddr(BPS_UINT8 addr);

/** 
  * @Brief GetBPSRemainLen get the remain length
  * @Param buf   the message buffer
  * @return the length, or '0' when something wrong
 */
EXPORT_API BPS_UINT16 GetBPSRemainLen(BPS_UINT8 * buf);

/** 
  * @Brief GetBPSRemainLen2 get the remain length(safely)
  * @Param buf   the message buffer
  * @Param size  the buffer size
  * @return the length, or '0' when something wrong
 */
EXPORT_API BPS_UINT16 GetBPSRemainLen2(BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief IsCmdWordValid check whether the command is valid
  * @Param cmd_word   the command word
  * @return Success     somegthing not 0
  *         Failed      0
 */
EXPORT_API BPS_UINT8 IsCmdWordValid(BPS_UINT8 cmd_word);

/** 
  * @Brief IsBPSChksumOK check the checksum of the message
  * @Param buf   the message buffer
  * @return Success     somegthing not 0 when
  *         Failed      0
 */
EXPORT_API BPS_UINT8 IsBPSChksumOK(BPS_UINT8 * buf);

/** 
  * @Brief IsBPSChksumOK check the checksum of the message(safely)
  * @Param buf   the message buffer
  * @Param size  the buffer size
  * @return Success     somegthing not 0 when
  *         Failed      0
 */
EXPORT_API BPS_UINT8 IsBPSChksumOK2(BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief PackBPSHeader to pack the BPS_HEADER into message buffer
  * @Param buf   the message buffer
  * @return Success     the buffer with the offset that the function made
  *         Failed      BPS_NULL
 */
EXPORT_API BPS_UINT8 * PackBPSHeader(BPS_UINT8 * buf);

/** 
  * @Brief PackBPSHeader2 to pack the BPS_HEADER into message buffer(safely)
  * @Param buf   the message buffer
  * @Param size  the buffer size
  * @return Success     the buffer with the offset that the function made
  *         Failed      BPS_NULL
 */
EXPORT_API BPS_UINT8 * PackBPSHeader2(BPS_UINT8 * buf, BPS_WORD size);

/** 
  * @Brief PackBPSVersion to pack the BPS_VERSION into message buffer
  * @Param buf   the message buffer
  * @return Success     the buffer with the offset that the function made
  *         Failed      BPS_NULL
 */
EXPORT_API BPS_UINT8 * PackBPSVersion(BPS_UINT8 * buf);

/** 
  * @Brief PackBPSAddr to pack BP-S address
  * @Param buf   the message buffer
  * @Param sender   the sender address(from 0-15)
  * @Param recver  the receiver address(from 0-15)
  * @return Success     the buffer with the offset that the function made
  *         Failed      BPS_NULL
 */
EXPORT_API BPS_UINT8 * PackBPSAddr(BPS_UINT8 * buf, BPS_UINT8 sender, BPS_UINT8 recver);

/** 
  * @Brief PackBPSRemainLen to pack remaining length into message buffer
  * @Param buf   the message buffer
  * @Param len   the remaining length
  * @return Success     the buffer with the offset that the function made
  *         Failed      BPS_NULL
 */
EXPORT_API BPS_UINT8 * PackBPSRemainLen(BPS_UINT8 * buf, BPS_UINT16 len);

/** 
  * @Brief PackBPSRemainLen to pack remaining length into message buffer(safely)
  * @Param buf   the message buffer
  * @Param len   the remaining length
  * @Param size  the buffer size
  * @return Success     the buffer with the offset that the function made
  *         Failed      BPS_NULL
 */
EXPORT_API BPS_UINT8 * PackBPSRemainLen2(BPS_UINT8 * buf, BPS_UINT16 len, BPS_WORD size);

/** 
  * @Brief PackBPSChecksum to pack checksum into message buffer
  * @Param buf   the message buffer with offset from the start
  * @Param len   the buffer size
  * @Param size  the buffer size
  * @return Success     the message buffer from start
  *         Failed      BPS_NULL
 */
EXPORT_API BPS_UINT8 * PackBPSChecksum(BPS_UINT8 * buf, BPS_WORD size);

#endif

