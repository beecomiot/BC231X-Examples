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
/// @file   bps_cwords.h
/// @brief  macros of command words
/// 
/// @version    0.1
/// @author     Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_CWORDS_H
#define __BPS_CWORDS_H

/** 
  basic command words
 */
#define CMD_COMM_TEST_REQ               0x00
#define CMD_COMM_TEST_RSP               (CMD_COMM_TEST_REQ+1)

#define CMD_HD_INFO_REQ                 0x02
#define CMD_HD_INFO_RSP                 (CMD_HD_INFO_REQ+1)

#define CMD_ALIVE_INTERVAL_REQ          0x04
#define CMD_ALIVE_INTERVAL_RSP          (CMD_ALIVE_INTERVAL_REQ+1)

#define CMD_NET_MODE_OPEN_REQ           0x06
#define CMD_NET_MODE_OPEN_RSP           (CMD_NET_MODE_OPEN_REQ+1)

#define CMD_NET_MODE_SET_REQ            0x08
#define CMD_NET_MODE_SET_RSP            (CMD_NET_MODE_SET_REQ+1)

#define CMD_NET_STATE_REQ               0x0A
#define CMD_NET_STATE_RSP               (CMD_NET_STATE_REQ+1)

/**
  signal command words
  */

#define CMD_GET_SIG_TAB_REQ             0x80
#define CMD_GET_SIG_TAB_RSP             (CMD_GET_SIG_TAB_REQ+1)

#define CMD_REPORT_SIG_REQ              0x82
#define CMD_REPORT_SIG_RSP              (CMD_REPORT_SIG_REQ+1)

#define CMD_RESPOND_CMD_REQ             0x84
#define CMD_RESPOND_CMD_RSP             (CMD_RESPOND_CMD_REQ+1)

#define CMD_RESPOND_VALUE_REQ           0x86
#define CMD_RESPOND_VALUE_RSP           (CMD_RESPOND_VALUE_REQ+1)

#endif

