///////////////////////////////////////////////////////////////////////////////
/// Copyright 2017-2019 Ansersion
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
/// @file 	bp_disconn.h
/// @brief 	for BP DISCONN header file
/// 
/// @version 	0.1
/// @author 	Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BP_DISCONN_H
#define __BP_DISCONN_H

#include <bp_public.h>

/** 
  * @Brief BP_PackDisconn construct BPPacket: DISCONN
  * @Param bp_context   BP context
  * @return Success     PackBuf with packet BP PING
  *         Failed      BP_NULL
 */
EXPORT_API PackBuf * BP_PackDisconn(const BPContext * bp_context);
#endif

