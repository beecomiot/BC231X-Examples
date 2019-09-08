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
/// @brief 	function "bps_memcpy" header file
/// 
/// @version 	0.1
/// @author 	Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_STRLEN_H
#define __BPS_STRLEN_H

#include <config.h>

#ifndef BPS_USE_STD
/** string length function
 *  
 * 	get the string length
 * 	@param string
 * 	@return  string length
 * 	
 */ 
EXPORT_API BPS_WORD strlen_bps(const char * str);
#else
    #include <string.h>
    #define strlen_bps strlen
#endif

#endif
