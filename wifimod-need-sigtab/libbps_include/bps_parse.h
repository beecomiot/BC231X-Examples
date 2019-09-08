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
/// @file 	bps_parse.h
/// @brief  definations of functions and variable types for parsing BPS packets
/// 
/// @version 	0.1
/// @author 	Ansersion
/// 
/// 
///////////////////////////////////////////////////////////////////////////////

#ifndef __BPS_PARSE_H
#define __BPS_PARSE_H

#include <bps_public.h>
#include <bps_cmd_comm_test.h>
#include <bps_cmd_hd_info.h>
#include <bps_cmd_ping.h>
#include <bps_cmd_open_netset.h>
#include <bps_cmd_config_netset.h>
#include <bps_cmd_netstate_query.h>
#include <bps_cmd_get_sigtab.h>
#include <bps_cmd_report_sig.h>
#include <bps_cmd_post.h>
#include <bps_cmd_get_sig.h>
#include <bps_cmd_system_para.h>

typedef union BPSCmdPacketUnion {
    BPSCmdCommTestReq           commTestReq;
    BPSCmdCommTestRsp           commTestRsp;
    BPSCmdHDInfoReq             hdInfoReq;
    BPSCmdHDInfoRsp             hdInfoRsp;
    BPSCmdPingReq               pingReq;
    BPSCmdPingRsp               pingRsp;
    BPSCmdOpenNetsetReq         openNetsetReq;
    BPSCmdOpenNetsetRsp         openNetsetRsp;
    BPSCmdConfigNetsetReq       configNetsetReq;
    BPSCmdConfigNetsetRsp       configNetsetRsp;
    BPSCmdNetstateQueryReq      netstateQueryReq;
    BPSCmdNetstateQueryRsp      netstateQueryRsp;
    BPSCmdGetSigtabReq          getSigtabReq;
    BPSCmdGetSigtabRsp          getSigtabRsp;
    BPSCmdReportSigReq          reportSigReq;
    BPSCmdReportSigRsp          reportSigRsp;
    BPSCmdPostReq               postReq;
    BPSCmdPostRsp               postRsp;
    BPSCmdGetSigReq             getSigReq;
    BPSCmdGetSigRsp             getSigRsp;
    BPSCmdSystemParaReq         sysParaReq;
    BPSCmdSystemParaRsp         sysParaRsp;
} BPSCmdPacketUnion;

typedef struct BPSPacketData {
    BPS_UINT8 cmdWord;
    BPSCmdPacketUnion pu;
} BPSPacketData;


/** 
  * @Brief BPSParseNoCheck parse the BPS packet without checkout the header and checksum
  * @Param buf   the message buffer
  * @Param pd    the data struct to store the message
  * @return Success     the pointer to data passed
  *         Failed      BPS_NULL
 */
EXPORT_API BPSPacketData * BPSParseNoCheck(BPS_UINT8 * buf, BPSPacketData * pd);


#endif

