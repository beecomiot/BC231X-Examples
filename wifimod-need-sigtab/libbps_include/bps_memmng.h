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
/// @file   bps_memmng.h
/// @brief  memory management
/// 
/// @version    0.1
/// @author     Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_MEMMNG_H
#define __BPS_MEMMNG_H

#ifdef BPS_MEM_DYN

/** 
  * @Brief memory management APIs, you can customize this based on different OS
 */
#include <stdlib.h>
#define malloc_bps  malloc
#define free_bps    free

#endif

#endif

