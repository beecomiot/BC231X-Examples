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
/// @file 	config.h
/// @brief 	configurations for different platforms
/// 
/// @version 	0.1
/// @author 	Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __CONFIG_H
#define __CONFIG_H


#define BPS_NULL 	0
#define BPS_INLINE 	extern inline

#ifdef WIN32
#define EXPORT_API __declspec(dllexport) 
#else
#define EXPORT_API
#endif

/***************************************************************************************************************
*** #define BPS_CPU64/BPS_CPU32/BPS_CPU16/BPS_CPU8, MUST BE DEFINED BY YOUR OWN
*** #define BPS_MEM_DYN, for system that can malloc dynamically
***************************************************************************************************************/


/** 
  * @Brief: BPCPUXX define determine the cpu type
  * @Note: Must define one of "BPS_CPU64/BPS_CPU32/BPS_CPU16/BPS_CPU8" to match your cpu, NO DEFAULT VALUE
 */

#ifdef BPS_CPU64
	#define BPS_UINT8 	unsigned char
	#define BPS_INT8 	char
	#define BPS_UINT16 	unsigned short
	#define BPS_INT16 	short
	#define BPS_UINT32 	unsigned int
	#define BPS_INT32 	int
	#define BPS_UINT64 	unsigned long long
	#define BPS_INT64 	long long
	#define BPS_FLOAT 	float
	#define BPS_WORD 	BPS_UINT64
#elif defined BPS_CPU32
	#define BPS_UINT8 	unsigned char
	#define BPS_INT8 	char
	#define BPS_UINT16 	unsigned short
	#define BPS_INT16 	short
	#define BPS_UINT32 	unsigned int
	#define BPS_INT32 	int
	#define BPS_FLOAT 	float
	#define BPS_WORD 	BPS_UINT32

#elif defined BPS_CPU16
	#define BPS_UINT8 	unsigned char
	#define BPS_INT8 	char
	#define BPS_UINT16 	unsigned short
	#define BPS_INT16 	short
	#define BPS_UINT32 	unsigned long
	#define BPS_INT32 	long
	#define BPS_FLOAT 	float
	#define BPS_WORD 	BPS_UINT16

#elif defined BPS_CPU8
	#define BPS_UINT8 	unsigned char
	#define BPS_INT8 	char
	#define BPS_UINT16 	unsigned short
	#define BPS_INT16 	short
	#define BPS_UINT32 	unsigned long
	#define BPS_INT32 	long
	#define BPS_FLOAT 	float
	#define BPS_WORD 	BPS_UINT8

#else
#error Please define your cpu macro first: BPS_CPU64/BPS_CPU32/BPS_CPU16/BPS_CPU8

#endif

#define BPS_TRUE        1
#define BPS_FALSE       0

/** 
  * @Brief: BPS_MEM_DYN for system that can alloc memory dynamically(malloc/free)
  * it will enable to compile functions 'ParseXXXReqDyn/ParseXXXRspDyn', 
  * which are more efficient for memory usage.
 */
// #define BPS_MEM_DYN

#endif

