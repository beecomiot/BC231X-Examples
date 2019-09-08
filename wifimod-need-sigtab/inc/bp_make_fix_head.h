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
/// @file 	bp_make_fix_head.h
/// @brief 	functions to make fixed header for BP packet header file
/// 
/// @version 	0.1
/// @author 	Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BP_MAKE_FIX_HEAD_H
#define __BP_MAKE_FIX_HEAD_H

#include <bpclient_config.h>

BP_UINT8 * BP_make_fix_head(BP_UINT8 * pack, BP_UINT8 type_and_flag, BP_UINT8 len);

#endif

