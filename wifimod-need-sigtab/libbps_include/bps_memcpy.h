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
/// @file 	bps_memcpy.h
/// @brief 	function "bp_memcpy" header file
/// 
/// @version 	0.1
/// @author 	Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_MEMCPY_H
#define __BPS_MEMCPY_H

#include <config.h>

#ifndef BPS_USE_STD
/** memory copy function
 *  
 * 	copy "count" bytes from "src" to "dst"  
 * 	@param dst destination address
 * 	@param src source address  
 * 	@param count size copied
 * 	@return  BPS_NULL/dst BPS_NULL: dst/src equals BPS_NULL; dst: copy OK
 * 	@note  there will be something when the memories of dst and src are overlapped
 * 	
 */ 
void * memcpy_bps(void * dst, const void * src, BPS_WORD count);
#else
    #include <string.h>
    #define memcpy_bps memcpy
#endif

#endif
