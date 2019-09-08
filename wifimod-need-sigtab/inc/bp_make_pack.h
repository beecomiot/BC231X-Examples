///////////////////////////////////////////////////////////////////////////////
/// Copyright 2017 Ansersion
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
/// @file 	bp_make_pack.h
/// @brief 	APIs to make BP packet
/// 
/// @version 	0.1
/// @author 	Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BP_MAKE_PACK_H
#define __BP_MAKE_PACK_H

#include <bpclient_config.h>
#include <bp_public.h>

/** 
  * @Brief BP_InitPack Initialize the PackBuf and set its BP Packet type
  * @Param pack_buf    The PackBuf of BP context
  * @Param type_mask   The mask of BPPacket type 
  * @return Success    Not BP_NULL
  *         Failed     BP_NULL
 */
EXPORT_API PackBuf * BP_InitPack(PackBuf * pack_buf, BP_UINT8 type_msk);

#ifdef BP_MEM_MNG
// EXPORT_API PackBuf * BP_InitPack2(PackBuf * pack_buf, BP_WORD size);
#endif
EXPORT_API PackBuf * BP_ReinitPack(PackBuf * pack_buf, BP_UINT8 type_msk);

/** 
  * @Brief BP_ToPack   Add fixed head and checksum to BP packet
  * @Param pack_buf    PackBuf which is constructed with variable head and payload
  * @return Success    The pointer which points the start of the BP packet
  *         Failed     BP_NULL
 */
EXPORT_API BP_UINT8 * BP_ToPack(PackBuf * pack_buf);

#endif

