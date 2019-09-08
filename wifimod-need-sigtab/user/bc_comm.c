#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "espressif/c_types.h"
#include "espressif/esp_misc.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "espressif/pwm.h"
#include "lwip/netdb.h"
#include "gpio.h"

#include "bc_comm.h"
#include "user_config.h"
#include "wifi_control.h"
#include <led_indicator.h>

#include <bpclient_config.h>
#include <bp_public.h>
#include <bp_make_pack.h>
#include <bp_pack_type.h>
#include <bp_vrb_flags.h>
#include <bp_crc32.h>
#include <bp_init.h>
#include <bp_connect.h>
#include <bp_disconn.h>
#include <bp_parse.h>
#include <bp_report.h>
#include <bp_getack.h>
#include <bp_specack.h>
#include <bp_postack.h>
#include <bp_ping.h>
#include <bp_sig_table.h>
#include <bp_sig_table_tools.h>

#include <bps_parse.h>
#include <bps_cmd_post.h>
#include <bps_sig_type.h>
#include <bps_task.h>
#include <state_mng.h>

#define RECV_BUF_SIZE 1024
#define SEND_BUF_SIZE 10

#define MAX_BC_USER_NAME_LEN 256
#define MAX_WIFI_SSID_LEN 32
#define MAX_WIFI_PASSWORD_LEN 64

#define BC_COMM_NAME "bc"
#define BC_COMM_STACK_WORDS 512 // 400 x 4 bytes(1600bytes) -- about 200 left
#define BC_COMM_PRORIOTY 6 // thread priority

#define LOCAL_SERVER_THREAD_NAME "local_server"
#define LOCAL_SERVER_THREAD_STACK_WORDS 512 // 400 x 4 bytes(1600bytes) -- about 100 left
#define LOCAL_SERVER_THREAD_PRORIOTY 6 // thread priority
#define BC_ERROR_DELAY 5000     // 5000ms

#define LOCAL_SERVER_THREAD_POLL_PERIOD 10 // 5 second
#define LOCAL_SERVER_LISTEN_BACKLOG 1 // listen for only one client
#define LOCAL_SERVER_THREAD_BINDING_POLL_PERIOD 1 // 1 second
#define LOCAL_SERVER_THREAD_BINDING_POLL_TIMES 8 // polling times when in binding mode
#define LOCAL_SERVER_TCP_PORT 8024

#define BC_COMM_POLL_PERIOD 1 // 1 second
#define BC_COMM_POLL_PERIOD_US 10000 // 10 millisecond


BcWifiInfo g_bcWifiInfo;

LOCAL xTaskHandle local_server_hander = NULL;
LOCAL xTaskHandle bc_comm_handler = NULL;
char buf[64];
int gBpsRecvBufferHandlePosition = 0;
int gBpsRecvBufferFillPosition = 0;
char gBpsRecvBuffer[10];
char gBpsSendBuffer[10];
char RECV_BUF[RECV_BUF_SIZE];
// char * Sn = "BEECOM_DEVELOP15554061246550";
// char * Password = "ghO7pL0622xUza7A";
int socketClient = -1;

int sockfd_bcserver = -1;
BP_SpecackStr str_specack;
BP_WORD aliveTime;

const char * g_bcServer = "root.bcserver.site";

// int ICACHE_FLASH_ATTR handleBpPacket(int sockfd, BP_UINT8 * recvBuf, BP_WORD size, BP_UINT8 * type_and_flags) 
// {
//     int n;
//     BP_UINT16 left_len;
//     BP_UINT32 crc = 0;
//     int i;
//     int len;
// 
//     if(sockfd < 0) {
//         return -1;
//     }
//     if(BP_NULL == recvBuf) {
//         return -1;
//     }
//     if(BP_NULL == type_and_flags) {
//         return -1;
//     }
//     if(size < FIX_HEAD_SIZE) {
//         return -1;
//     }
//     n=recv(sockfd,recvBuf,FIX_HEAD_SIZE, MSG_WAITALL);
//     if(FIX_HEAD_SIZE != n) {
//         perror("Recv error 1\r\n");
//         return -2;
//     }
//     if(0 != BP_ParseFixHead(recvBuf, type_and_flags, &left_len)) {
//         perror("Recv error 1\r\n");
//         return -4;
//     }
// 
//     len = left_len;
//     bc_printf("* len=%d\r\n", len);
// 
//     if(size < len + FIX_HEAD_SIZE) {
//         return -1;
//     }
//     n += recv(sockfd,recvBuf+FIX_HEAD_SIZE,len, MSG_WAITALL);
//     len += FIX_HEAD_SIZE;
// 
//     if(n != len) {
//         bc_printf("* Recv error 2\r\n");
//         return -2;
//     }
//     bc_printf("* recv: ");
//     for(i = 0; i < len; i++) {
//         bc_printf("%02x ", recvBuf[i]);
//     }
//     bc_printf("\n");
// 
//     if(0 != BP_CheckCRC(*type_and_flags, recvBuf, len)) {
//         bc_printf("* CRC Check error\n");
//         return -3;
//     }
// 
//     return len;
// }

LOCAL void local_server_thread(void* p)
{
    int ret;
	int len;
    int wait_times = 0;
    int local_server_sockfd;
    int local_client_sockfd;
    struct sockaddr_in sock_addr;
    int n;
	char user[MAX_BC_USER_NAME_LEN];
	char ssid[MAX_WIFI_SSID_LEN];
	char pass[MAX_WIFI_PASSWORD_LEN];
    PackBuf * p_pack_buf = BP_NULL;
    BP_UINT8 type_and_flags;
    BP_SpecsetStr str_specset;
    fd_set rfds;
    struct timeval tv;
    socklen_t addr_len;
    static BPSCmdPostField postField;
    static BPSPacketData bpsPackData;

    bc_printf("server loading\n");
    /* create the socket */
    local_server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (local_server_sockfd < 0) {
        bc_printf("failed: local_server_sockfd < 0\n");
        local_server_hander = NULL;
        vTaskDelete(NULL);
        return;
    }

    /* initialize the server parameters  */
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port = htons(LOCAL_SERVER_TCP_PORT);

    /* bind the socket with the server parameters */
    ret = bind(local_server_sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret) {
        bc_printf("failed bind\n");
        local_server_hander = NULL;
        vTaskDelete(NULL);
        return;
    }

    /* the local server starts listening */
    bc_printf("server socket listen ......");
    ret = listen(local_server_sockfd, LOCAL_SERVER_LISTEN_BACKLOG);

    if (ret) {
        bc_printf("failed listen\n");
        local_server_hander = NULL;
        vTaskDelete(NULL);
        return;
    }

    /* first set the polling period if there is no client connect to the local server*/
    tv.tv_sec = LOCAL_SERVER_THREAD_POLL_PERIOD;
    tv.tv_usec = 0;

    /* main task cycle */
    while(1) {
        /* initialize the fd set */
        FD_ZERO(&rfds);
        FD_SET(local_server_sockfd, &rfds);

        /* wait message from the fd set */
        ret = select(local_server_sockfd+1, &rfds, NULL, NULL, &tv);
        
        if(ret < 0) {
            /* error occurred */
            bc_printf("server: select faied\n");
            local_server_hander = NULL;
            vTaskDelete(NULL);
            return;
        }
        if(0 == ret) {
            /* timeout occurred */
            bc_printf("localserver stack left: %d\n", uxTaskGetStackHighWaterMark(NULL));
            if(wait_times-- > 0) {
                bc_printf("server: select timeout(binding): %d\n", wait_times);
                tv.tv_sec = LOCAL_SERVER_THREAD_BINDING_POLL_PERIOD;
                tv.tv_usec = 0;
                /* TODO: do something about ap stop/start */
            } else {
                wait_times = 0;
                tv.tv_sec = LOCAL_SERVER_THREAD_POLL_PERIOD;
                tv.tv_usec = 0;
                postField.signalId = 0x0001;
                postField.signalType = BPS_SIG_TYPE_ENM;
                postField.value.t_enm = 0x0002;
                bpsPackData.pu.postReq.fieldArray = &postField;
                bpsPackData.pu.postReq.fieldNum = 1;
                bpsPackData.cmdWord = CMD_POST_WORD_REQ;
                xQueueSendToBack(g_ComQueue, &bpsPackData, 0);
                bc_printf("server: select timeout\n");
            }
        } 

        if(FD_ISSET(local_server_sockfd, &rfds)) {
            /* client connection occured */
            FD_CLR(local_server_sockfd, &rfds);
            local_client_sockfd = accept(local_server_sockfd, (struct sockaddr*)&sock_addr, &addr_len);
            if (local_client_sockfd < 0) {
                bc_printf("failed accept");
                continue;
            }
            len = handleBpPacket(local_client_sockfd, RECV_BUF, RECV_BUF_SIZE, &type_and_flags);
            len = 0;
            if(len <= 0) {
                bc_close(&local_client_sockfd);
                continue;
            }

            /* check if the BPPacket of SPECSET */
            if(BP_PACK_TYPE_SPECSET != ((type_and_flags >> 4) & 0x0F)) {
                bc_close(&local_client_sockfd);
                continue;
            }

            bzero(&ssid, MAX_WIFI_SSID_LEN);
            bzero(&pass, MAX_WIFI_PASSWORD_LEN);
            bzero(&user, MAX_BC_USER_NAME_LEN);
            str_specset.Ssid = ssid;
            str_specset.Password = pass;
            str_specset.UserName = user;

            n = BP_ParseSpecset(&BPContextEmbeded, &str_specset, RECV_BUF, len);
            bc_printf("* ParseSpecset err: %x\n", n);
            if(0 != n) {
                str_specack.Type = str_specset.Type;
                str_specack.RetCode = 0x06;
                p_pack_buf = BP_PackSpecack(&BPContextEmbeded, &str_specack);
                n=send(local_client_sockfd,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
                bc_close(&local_client_sockfd);
            } else {
                /* 0 -> OK */
                str_specack.Type = str_specset.Type;
                str_specack.RetCode = 0;
                BPContextEmbeded.AdminName = user;
                BPContextEmbeded.AdminNameLen = strlen(user);
                bc_printf("* :%s,%s,%s: \n", ssid, pass, user);
                p_pack_buf = BP_PackSpecack(&BPContextEmbeded, &str_specack);
                n=send(local_client_sockfd,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
                bc_close(&local_client_sockfd);
                wifi_station_disconnect();
                start_wifi_station(ssid, pass);
                wait_times = LOCAL_SERVER_THREAD_BINDING_POLL_TIMES;
                tv.tv_sec = LOCAL_SERVER_THREAD_BINDING_POLL_PERIOD;
                tv.tv_usec = 0;
            }
        }
    }
    bc_close(&local_client_sockfd);
    bc_close(&local_server_sockfd);
    vTaskDelete(NULL);
}

LOCAL void bc_comm(void* p)
{
    int ret;
	int len, i;
	int n;
    struct sockaddr_in sock_addr;
	struct in_addr **addr_list = NULL;
	struct hostent *host = NULL;
    fd_set rfds;
    struct timeval tv;
	BP_UINT8 type_and_flags;
	BP_UINT16 left_len;
    BP_ConnackStr str_connack;
    BP_RprtackStr str_rprtack;
    PackBuf * p_pack_buf = BP_NULL;

     while(1) {
         vTaskDelay(1000/portTICK_RATE_MS);
         if(EN_MS_CONNECT_SERVER != getModuleState()) {
             bc_close(&sockfd_bcserver);
             continue;
         }
         bc_printf("bc_comm stack left: %d\n", uxTaskGetStackHighWaterMark(NULL));
         if(!g_bcWifiInfo.isWifiInfoSet) {
             updateIndicator(EN_INDICATOR_NO_WIFI_INFO);
             continue;
         }

         /* create socket connection to BcServer */
         host = gethostbyname(g_bcServer);
         if(NULL == host) {
             bc_printf("gethostbyname err\n");
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         }
         sockfd_bcserver = socket(AF_INET, SOCK_STREAM, 0);
         if(sockfd_bcserver < 0) {
             bc_printf("socket err\n");
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         } 
         addr_list = (struct in_addr **)host->h_addr_list;
         if(NULL == addr_list) {
             bc_printf("* NULL == addr_list\n");
             bc_close(&sockfd_bcserver);
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         }
         memset(&sock_addr, 0, sizeof(sock_addr));
         sock_addr.sin_family = AF_INET;
         sock_addr.sin_addr = *addr_list[0];
         // sock_addr.sin_addr.s_addr = inet_addr("192.168.2.196");
         sock_addr.sin_port = htons(8025);
         bc_printf("connecting\r\n");
         gBpsRecvBufferFillPosition = 0;
         gBpsRecvBufferHandlePosition = 0;
         if(connect(sockfd_bcserver,(struct sockaddr*)&sock_addr,sizeof(sock_addr)) < 0) {
             bc_printf("* Connect Error\n");
             bc_close(&sockfd_bcserver);
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         } 
         /* send BPPacket CONNECT to BcServer */
         bc_printf("BP_PackConnect\r\n");
         p_pack_buf = BP_PackConnect(&BPContextEmbeded,SN, KEY);
         if(BP_NULL == p_pack_buf) {
             bc_printf("BP_PackConnect err\n");
             bc_close(&sockfd_bcserver);
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         } 
         bc_printf("send\r\n");
         n=send(sockfd_bcserver,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
         if(n != p_pack_buf->MsgSize) {
             bc_printf("* Send connect error");
             bc_close(&sockfd_bcserver);
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         }

         /* receive and parse BP Packet CONNACK */
         bc_printf("handleBpPacket\r\n");
         len = handleBpPacket(sockfd_bcserver, RECV_BUF, RECV_BUF_SIZE, &type_and_flags);
         if(len <= 0) {
             bc_close(&sockfd_bcserver);
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         }
         bc_printf("BP_ParseConnack\r\n");
         if(0 != BP_ParseConnack(&BPContextEmbeded, &str_connack, RECV_BUF, len)) {
             bc_printf("* parse connack err:\n");
             bc_close(&sockfd_bcserver);
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         }
         if(0 != str_connack.RetCode) {
             bc_printf("* CONNACK ERROR: %d\n", str_connack.RetCode);
             bc_close(&sockfd_bcserver);
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         }

         /* send BPPacket REPORT to check the signal map checksum */
         bc_printf("BP_PackReportSigTabChksum\r\n");
         p_pack_buf = BP_PackReportSigTabChksum(&BPContextEmbeded);
         if(BP_NULL == p_pack_buf) {
             bc_printf("BP_PackReportSigTabChksum err\n");
             bc_close(&sockfd_bcserver);
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         } 
         bc_printf("send\r\n");
         for(i =0; i < p_pack_buf->MsgSize; i++) {
             bc_printf("%02x ", p_pack_buf->PackStart[i]);
         }
         bc_printf("\r\n");
         n=send(sockfd_bcserver,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
         if(n != p_pack_buf->MsgSize) {
             bc_printf("* Send error");
             bc_close(&sockfd_bcserver);
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         }
         /* receive and parse BP Packet RPRTACK */
         bc_printf("handleBpPacket\r\n");
         len = handleBpPacket(sockfd_bcserver, RECV_BUF, RECV_BUF_SIZE, &type_and_flags);
         if(len <= 0) {
             bc_printf("handleBpPacket error\r\n");
             bc_close(&sockfd_bcserver);
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         }
         bc_printf("BP_ParseRprtack\r\n");
         if(0 != BP_ParseRprtack(&BPContextEmbeded, &str_rprtack, RECV_BUF, len)) {
             bc_printf("BP_ParseRprtack error\n");
             bc_close(&sockfd_bcserver);
             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
             continue;
         }

         if(str_rprtack.RetCode != 0) {
             /* send BPPacket REPORT to update the signal map if the checksum error */
             bc_printf("* RPRTACK ERROR: %d\n", str_rprtack.RetCode);
             p_pack_buf = BP_PackReportSigTable(&BPContextEmbeded);
             if(BP_NULL == p_pack_buf) {
                 bc_printf("BP_PackReportSigTable err\n");
                 bc_close(&sockfd_bcserver);
                 vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
                 continue;
             } 
             bc_printf("send\r\n");
             for(i =0; i < p_pack_buf->MsgSize; i++) {
                 bc_printf("%02x ", p_pack_buf->PackStart[i]);
             }
             bc_printf("\r\n");
             n=send(sockfd_bcserver,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
             if(n != p_pack_buf->MsgSize) {
                 bc_printf("* Send error");
                 bc_close(&sockfd_bcserver);
                 vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
                 continue;
             }
             /* receive and parse BP Packet RPRTACK */
             len = handleBpPacket(sockfd_bcserver, RECV_BUF, RECV_BUF_SIZE, &type_and_flags);
             bc_printf("parse rprtack handle\r\n");
             if(len <= 0) {
                 bc_close(&sockfd_bcserver);
                 vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
                 continue;
             }
             if(0 != BP_ParseRprtack(&BPContextEmbeded, &str_rprtack, RECV_BUF, len)) {
                 bc_printf("BP_ParseRprtack error\n");
                 bc_close(&sockfd_bcserver);
                 vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
                 continue;
             }
             if(str_rprtack.RetCode != 0) {
                 bc_printf("* RPRTACK ERROR: %d\n", str_rprtack.RetCode);
                 bc_close(&sockfd_bcserver);
                 vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
                 continue;
             }
         }
         updateIndicator(EN_INDICATOR_COMM_OK);

         tv.tv_sec = BC_COMM_POLL_PERIOD;
         tv.tv_usec = 0;
         aliveTime = BP_ALIVE_PERIOD - BP_TIMEOUT;

         while(1) {
             /* initialize the fd set */
             FD_ZERO(&rfds);
             FD_SET(sockfd_bcserver, &rfds);

             /* wait message from the fd set */
             ret = select(sockfd_bcserver+1, &rfds, NULL, NULL, &tv);

             // if(gWifiApClientConnected > 0) {
             //     /* ap is connected, close old socket and wait */
             //     bc_printf("select: %d,%d\n", sockfd_bcserver, gWifiApClientConnected);
             //     if(sockfd_bcserver > 0) {
             //         bc_printf("close old socket\n");
             //         p_pack_buf = BP_PackDisconn(&BPContextEmbeded);
             //         n=send(sockfd_bcserver,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
             //         bc_close(&sockfd_bcserver);
             //     }
             //     break;
             // }

             if(ret < 0) {
                 /* error occurred */
                 bc_close(&sockfd_bcserver);
                 bc_printf("select error\r\n");
                 vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
                 break;
             } 
             if(0 == ret) {
                 bc_printf("bcserver stack left: %d\n", uxTaskGetStackHighWaterMark(NULL));
                 /* timeout occurred */
                 tv.tv_sec = BC_COMM_POLL_PERIOD;
                 tv.tv_usec = 0;
                 if(aliveTime-- > 0) {
                     continue;
                 }
                 aliveTime = BP_ALIVE_PERIOD - BP_TIMEOUT;
                 /* send BPPacket PING to notify BcServer that the device is alive */
                 bc_printf("start ping\n");
                 p_pack_buf = BP_PackPing(&BPContextEmbeded);
                 if(BP_NULL == p_pack_buf) {
                     bc_printf("BP_PackPing err\n");
                     bc_close(&sockfd_bcserver);
                     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
                     break;
                 } 
                 n=send(sockfd_bcserver,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
                 if(n != p_pack_buf->MsgSize) {
                     bc_printf("* Send ping error");
                     bc_close(&sockfd_bcserver);
                     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
                     break;
                 }

             } 
             if(FD_ISSET(sockfd_bcserver, &rfds)) {
                 /* got message from BcServer */
                 FD_CLR(sockfd_bcserver, &rfds);
                 len = handleBpPacket(sockfd_bcserver, RECV_BUF, RECV_BUF_SIZE, &type_and_flags);
                 if(len <= 0) {
                     bc_close(&sockfd_bcserver);
                     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
                     break;
                 }

                 if(((type_and_flags >> 4) & 0xF) == BP_PACK_TYPE_POST) {
                     BP_PostStr str_post;
                     /* parse BP Packet POST*/
                     if(0 != BP_ParsePost(&str_post, RECV_BUF, len)) {
                         bc_printf("* parse post err:\n");
                         bc_close(&sockfd_bcserver);
                         vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
                         break;
                     }
                     for(i = 0; i < str_post.SigNum; i++) {
                         BP_SigId2Val * tmp = BP_GetSigId2Val(str_post.SigValArray[i].SigIdx, str_post.SigValArray[i].SigId);
                         bc_printf("value: %x\n", tmp->SigVal.t_u16);
                         if(tmp->SigVal.t_u16 > 0) {
                             GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 1);
                         } else {
                             GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 0);
                         }
                     }
                     bc_printf("post ok\n");
                 } else if(((type_and_flags >> 4) & 0xF) == BP_PACK_TYPE_PINGACK) {
                     /* parse BP Packet PINGACK*/
                     BP_PingackStr str_pingack;
                     if(0 != BP_ParsePingack(&BPContextEmbeded, &str_pingack, RECV_BUF, len)) {
                         bc_printf("* parse pingack err:\n");
                         bc_close(&sockfd_bcserver);
                         vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
                         break;
                     }
                     bc_printf("* PINGACK:\n");
                 } else {
                     bc_printf("* UNKNOWN Packet\n");
                 }
             }
         }

         updateIndicator(EN_INDICATOR_SERV_CONNECTING);

     }
}

// LOCAL void bc_comm(void* p)
// {
//     int ret;
// 	int len, i;
// 	int n;
//     struct sockaddr_in sock_addr;
// 	struct in_addr **addr_list = NULL;
// 	struct hostent *host = NULL;
//     fd_set rfds;
//     struct timeval tv;
// 	BP_UINT8 type_and_flags;
// 	BP_UINT16 left_len;
//     BP_ConnackStr str_connack;
//     BP_RprtackStr str_rprtack;
//     PackBuf * p_pack_buf = BP_NULL;
// 
//     while(1) {
//         /* if there is a client connected to local ap to configure the device , 
//            and the device will be ready to connect to / change a new Wifi router. 
//            So bc_comm need to nothing but to wait the configurration completed */
//         if(gWifiApClientConnected > 0) {
//             bc_printf("gWifiApClientConnected > 0\n");
//             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//             continue;
//         }
// 
//         /* create socket connection to BcServer */
//         host = gethostbyname("root.bcserver.site");
//         if(NULL == host) {
//             bc_printf("gethostbyname err\n");
//             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//             continue;
//         }
//         sockfd_bcserver = socket(AF_INET, SOCK_STREAM, 0);
//         if(sockfd_bcserver < 0) {
//             bc_printf("socket err\n");
//             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//             continue;
//         } 
//         addr_list = (struct in_addr **)host->h_addr_list;
//         if(NULL == addr_list) {
//             bc_printf("* NULL == addr_list\n");
//             bc_close(&sockfd_bcserver);
//             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//             continue;
//         }
//         memset(&sock_addr, 0, sizeof(sock_addr));
//         sock_addr.sin_family = AF_INET;
//         sock_addr.sin_addr = *addr_list[0];
//         // sock_addr.sin_addr.s_addr = inet_addr("192.168.1.89");
//         sock_addr.sin_port = htons(8025);
//         bc_printf("connecting\r\n");
//         gBpsRecvBufferFillPosition = 0;
//         gBpsRecvBufferHandlePosition = 0;
//         if(connect(sockfd_bcserver,(struct sockaddr*)&sock_addr,sizeof(sock_addr)) < 0) {
//             bc_printf("* Connect Error\n");
//             bc_close(&sockfd_bcserver);
//             vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//             continue;
//         } 
// 
//         tv.tv_sec = BC_COMM_POLL_PERIOD;
//         tv.tv_usec = BC_COMM_POLL_PERIOD_US;
//         aliveTime = BP_ALIVE_PERIOD - BP_TIMEOUT;
// 
//         while(1) {
//             /* initialize the fd set */
//             FD_ZERO(&rfds);
//             FD_SET(sockfd_bcserver, &rfds);
// 
//             /* wait message from the fd set */
//             ret = select(sockfd_bcserver+1, &rfds, NULL, NULL, &tv);
// 
//             if(gWifiApClientConnected > 0) {
//                 /* ap is connected, close old socket and wait */
//                 bc_printf("select: %d,%d\n", sockfd_bcserver, gWifiApClientConnected);
//                 if(sockfd_bcserver > 0) {
//                     bc_printf("close old socket\n");
//                     p_pack_buf = BP_PackDisconn(&BPContextEmbeded);
//                     n=send(sockfd_bcserver,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
//                     bc_close(&sockfd_bcserver);
//                 }
//                 break;
//             }
// 
//             if(ret < 0) {
//                 /* error occurred */
//                 bc_close(&sockfd_bcserver);
//                 vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//                 break;
//             } 
//             if(0 == ret) {
//                 // bc_printf("bcserver stack left: %d\n", uxTaskGetStackHighWaterMark(NULL));
//                 /* timeout occurred */
//                 tv.tv_sec = BC_COMM_POLL_PERIOD;
//                 tv.tv_usec = BC_COMM_POLL_PERIOD_US;
// 
//                 len = gBpsRecvBufferFillPosition - gBpsRecvBufferHandlePosition;
//                 if(len > 0) {
//                     bc_printf("+send,fill=%d,%d", gBpsRecvBufferHandlePosition, gBpsRecvBufferFillPosition);
//                     n=send(sockfd_bcserver,gBpsRecvBuffer+gBpsRecvBufferHandlePosition,len,0);
//                     if(n != len) {
//                         bc_close(&sockfd_bcserver);
//                         vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//                         break;
//                     }
//                     gBpsRecvBufferHandlePosition += len;
//                 } else if(len < 0) {
//                     bc_printf("-send,fill=%d,%d", gBpsRecvBufferHandlePosition, gBpsRecvBufferFillPosition);
//                     len = RECV_BUF_SIZE - gBpsRecvBufferHandlePosition;
//                     n=send(sockfd_bcserver,gBpsRecvBuffer+gBpsRecvBufferHandlePosition,len,0);
//                     if(n != len) {
//                         bc_close(&sockfd_bcserver);
//                         vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//                         break;
//                     }
//                     gBpsRecvBufferHandlePosition = 0;
//                     len = gBpsRecvBufferFillPosition - gBpsRecvBufferHandlePosition;
//                     n=send(sockfd_bcserver,gBpsRecvBuffer+gBpsRecvBufferHandlePosition, len, 0);
//                     if(n != len) {
//                         bc_close(&sockfd_bcserver);
//                         vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//                         break;
//                     }
//                     gBpsRecvBufferHandlePosition = gBpsRecvBufferFillPosition;
//                 }
// 
//             } 
//             if(FD_ISSET(sockfd_bcserver, &rfds)) {
//                 /* got message from BcServer */
//                 FD_CLR(sockfd_bcserver, &rfds);
//                 len = handleBpPacket(sockfd_bcserver, RECV_BUF, RECV_BUF_SIZE, &type_and_flags);
//                 if(len <= 0) {
//                     bc_close(&sockfd_bcserver);
//                     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//                     break;
//                 }
// 				for(i = 0; i < len; i++) {
//                 	// uart_tx_one_char(UART0, RECV_BUF[i]);
// 				}
// 
//                 // if(((type_and_flags >> 4) & 0xF) == BP_PACK_TYPE_POST) {
//                 //     BP_PostStr str_post;
//                 //     /* parse BP Packet POST*/
//                 //     if(0 != BP_ParsePost(&str_post, RECV_BUF, len)) {
//                 //         bc_printf("* parse post err:\n");
//                 //         bc_close(&sockfd_bcserver);
//                 //         vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//                 //         break;
//                 //     }
//                 //     for(i = 0; i < str_post.SigNum; i++) {
//                 //         BP_SigId2Val * tmp = BP_GetSigId2Val(str_post.SigValArray[i].SigIdx, str_post.SigValArray[i].SigId);
//                 //         bc_printf("value: %x\n", tmp->SigVal.t_u16);
//                 //         if(tmp->SigVal.t_u16 > 0) {
//                 //             GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 1);
//                 //         } else {
//                 //             GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 0);
//                 //         }
//                 //     }
//                 //     bc_printf("post ok\n");
//                 // } else if(((type_and_flags >> 4) & 0xF) == BP_PACK_TYPE_PINGACK) {
//                 //     /* parse BP Packet PINGACK*/
//                 //     BP_PingackStr str_pingack;
//                 //     if(0 != BP_ParsePingack(&BPContextEmbeded, &str_pingack, RECV_BUF, len)) {
//                 //         bc_printf("* parse pingack err:\n");
//                 //         bc_close(&sockfd_bcserver);
//                 //         vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//                 //         break;
//                 //     }
//                 //     bc_printf("* PINGACK:\n");
//                 // }
//             }
// 		}
// 
//         // /* send BPPacket CONNECT to BcServer */
//         // p_pack_buf = BP_PackConnect(&BPContextEmbeded, Sn, Password);
//         // if(BP_NULL == p_pack_buf) {
//         //     bc_printf("BP_PackConnect err\n");
//         //     bc_close(&sockfd_bcserver);
//         //     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //     continue;
//         // } 
//         // n=send(sockfd_bcserver,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
//         // if(n != p_pack_buf->MsgSize) {
//         //     bc_printf("* Send connect error");
//         //     bc_close(&sockfd_bcserver);
//         //     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //     continue;
//         // }
// 
//         // /* receive and parse BP Packet CONNACK */
//         // len = handleBpPacket(sockfd_bcserver, RECV_BUF, RECV_BUF_SIZE, &type_and_flags);
//         // if(len <= 0) {
//         //     bc_close(&sockfd_bcserver);
//         //     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //     continue;
//         // }
//         // if(0 != BP_ParseConnack(&BPContextEmbeded, &str_connack, RECV_BUF, len)) {
//         //     bc_printf("* parse connack err:\n");
//         //     bc_close(&sockfd_bcserver);
//         //     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //     continue;
//         // }
//         // if(0 != str_connack.RetCode) {
//         //     bc_printf("* CONNACK ERROR: %d\n", str_connack.RetCode);
//         //     bc_close(&sockfd_bcserver);
//         //     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //     continue;
//         // }
// 
//         // /* send BPPacket REPORT to check the signal map checksum */
//         // p_pack_buf = BP_PackReportSigTabChksum(&BPContextEmbeded);
//         // if(BP_NULL == p_pack_buf) {
//         //     bc_printf("BP_PackReportSigTabChksum err\n");
//         //     bc_close(&sockfd_bcserver);
//         //     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //     continue;
//         // } 
//         // n=send(sockfd_bcserver,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
//         // if(n != p_pack_buf->MsgSize) {
//         //     bc_printf("* Send error");
//         //     bc_close(&sockfd_bcserver);
//         //     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //     continue;
//         // }
//         // /* receive and parse BP Packet RPRTACK */
//         // len = handleBpPacket(sockfd_bcserver, RECV_BUF, RECV_BUF_SIZE, &type_and_flags);
//         // if(len <= 0) {
//         //     bc_close(&sockfd_bcserver);
//         //     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //     continue;
//         // }
//         // if(0 != BP_ParseRprtack(&BPContextEmbeded, &str_rprtack, RECV_BUF, len)) {
//         //     bc_printf("BP_ParseRprtack error\n");
//         //     bc_close(&sockfd_bcserver);
//         //     vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //     continue;
//         // }
// 
//         // if(str_rprtack.RetCode != 0) {
//         //     /* send BPPacket REPORT to update the signal map if the checksum error */
//         //     bc_printf("* RPRTACK ERROR: %d\n", str_rprtack.RetCode);
//         //     p_pack_buf = BP_PackReportSigTable(&BPContextEmbeded);
//         //     if(BP_NULL == p_pack_buf) {
//         //         bc_printf("BP_PackReportSigTable err\n");
//         //         bc_close(&sockfd_bcserver);
//         //         vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //         continue;
//         //     } 
//         //     n=send(sockfd_bcserver,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
//         //     if(n != p_pack_buf->MsgSize) {
//         //         bc_printf("* Send error");
//         //         bc_close(&sockfd_bcserver);
//         //         vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //         continue;
//         //     }
//         //     /* receive and parse BP Packet RPRTACK */
//         //     len = handleBpPacket(sockfd_bcserver, RECV_BUF, RECV_BUF_SIZE, &type_and_flags);
//         //     if(len <= 0) {
//         //         bc_close(&sockfd_bcserver);
//         //         vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //         continue;
//         //     }
//         //     if(0 != BP_ParseRprtack(&BPContextEmbeded, &str_rprtack, RECV_BUF, len)) {
//         //         bc_printf("BP_ParseRprtack error\n");
//         //         bc_close(&sockfd_bcserver);
//         //         vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //         continue;
//         //     }
//         //     if(str_rprtack.RetCode != 0) {
//         //         bc_printf("* RPRTACK ERROR: %d\n", str_rprtack.RetCode);
//         //         bc_close(&sockfd_bcserver);
//         //         vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
//         //         continue;
//         //     }
//         // }
// 
//         // tv.tv_sec = BC_COMM_POLL_PERIOD;
//         // tv.tv_usec = 0;
//         // aliveTime = BP_ALIVE_PERIOD - BP_TIMEOUT;
// 
//         
// //        while(1) {
// //            /* initialize the fd set */
// //            FD_ZERO(&rfds);
// //            FD_SET(sockfd_bcserver, &rfds);
// //
// //            /* wait message from the fd set */
// //            ret = select(sockfd_bcserver+1, &rfds, NULL, NULL, &tv);
// //
// //            if(gWifiApClientConnected > 0) {
// //                /* ap is connected, close old socket and wait */
// //                bc_printf("select: %d,%d\n", sockfd_bcserver, gWifiApClientConnected);
// //                if(sockfd_bcserver > 0) {
// //                    bc_printf("close old socket\n");
// //                    p_pack_buf = BP_PackDisconn(&BPContextEmbeded);
// //                    n=send(sockfd_bcserver,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
// //                    bc_close(&sockfd_bcserver);
// //                }
// //                break;
// //            }
// //
// //            if(ret < 0) {
// //                /* error occurred */
// //                bc_close(&sockfd_bcserver);
// //                vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
// //                break;
// //            } 
// //            if(0 == ret) {
// //                bc_printf("bcserver stack left: %d\n", uxTaskGetStackHighWaterMark(NULL));
// //                /* timeout occurred */
// //                tv.tv_sec = BC_COMM_POLL_PERIOD;
// //                tv.tv_usec = 0;
// //                if(aliveTime-- > 0) {
// //                    continue;
// //                }
// //                aliveTime = BP_ALIVE_PERIOD - BP_TIMEOUT;
// //                /* send BPPacket PING to notify BcServer that the device is alive */
// //                bc_printf("start ping\n");
// //                p_pack_buf = BP_PackPing(&BPContextEmbeded);
// //                if(BP_NULL == p_pack_buf) {
// //                    bc_printf("BP_PackPing err\n");
// //                    bc_close(&sockfd_bcserver);
// //                    vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
// //                    break;
// //                } 
// //                n=send(sockfd_bcserver,p_pack_buf->PackStart,p_pack_buf->MsgSize,0);
// //                if(n != p_pack_buf->MsgSize) {
// //                    bc_printf("* Send ping error");
// //                    bc_close(&sockfd_bcserver);
// //                    vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
// //                    break;
// //                }
// //
// //            } 
// //            if(FD_ISSET(sockfd_bcserver, &rfds)) {
// //                /* got message from BcServer */
// //                FD_CLR(sockfd_bcserver, &rfds);
// //                len = handleBpPacket(sockfd_bcserver, RECV_BUF, RECV_BUF_SIZE, &type_and_flags);
// //                if(len <= 0) {
// //                    bc_close(&sockfd_bcserver);
// //                    vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
// //                    break;
// //                }
// //
// //                if(((type_and_flags >> 4) & 0xF) == BP_PACK_TYPE_POST) {
// //                    BP_PostStr str_post;
// //                    /* parse BP Packet POST*/
// //                    if(0 != BP_ParsePost(&str_post, RECV_BUF, len)) {
// //                        bc_printf("* parse post err:\n");
// //                        bc_close(&sockfd_bcserver);
// //                        vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
// //                        break;
// //                    }
// //                    for(i = 0; i < str_post.SigNum; i++) {
// //                        BP_SigId2Val * tmp = BP_GetSigId2Val(str_post.SigValArray[i].SigIdx, str_post.SigValArray[i].SigId);
// //                        bc_printf("value: %x\n", tmp->SigVal.t_u16);
// //                        if(tmp->SigVal.t_u16 > 0) {
// //                            GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 1);
// //                        } else {
// //                            GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 0);
// //                        }
// //                    }
// //                    bc_printf("post ok\n");
// //                } else if(((type_and_flags >> 4) & 0xF) == BP_PACK_TYPE_PINGACK) {
// //                    /* parse BP Packet PINGACK*/
// //                    BP_PingackStr str_pingack;
// //                    if(0 != BP_ParsePingack(&BPContextEmbeded, &str_pingack, RECV_BUF, len)) {
// //                        bc_printf("* parse pingack err:\n");
// //                        bc_close(&sockfd_bcserver);
// //                        vTaskDelay(BC_ERROR_DELAY / portTICK_RATE_MS);
// //                        break;
// //                    }
// //                    bc_printf("* PINGACK:\n");
// //                }
// //            }
// //        }
//     }
// 
//     bc_close(&sockfd_bcserver);
//     vTaskDelete(NULL);
// }

void local_server_thread_init(void)
{
    int ret;

    if(local_server_hander) {
        // bc_printf("%s created already\n", LOCAL_SERVER_THREAD_NAME);
        return;
    }
    /* initialize the embeded BP context */
    BP_InitEmbededContext();

    /* create the thread of local server */
    bc_printf("thread creating\n");
    ret = xTaskCreate(local_server_thread,
                      LOCAL_SERVER_THREAD_NAME,
                      LOCAL_SERVER_THREAD_STACK_WORDS,
                      NULL,
                      LOCAL_SERVER_THREAD_PRORIOTY,
                      &local_server_hander);

    if (ret != pdPASS)  {
        bc_printf("create thread %s failed\n", LOCAL_SERVER_THREAD_NAME);
        return ;
    }
}

void bc_comm_init(void)
{
    int ret;

    if(bc_comm_handler) {
        bc_printf("bc_comm created already\n");
        return;
    }
    bc_printf("task %s creating\n", BC_COMM_NAME);

    /* create the thread of Beecom */
    ret = xTaskCreate(bc_comm,
                      BC_COMM_NAME,
                      BC_COMM_STACK_WORDS,
                      NULL,
                      BC_COMM_PRORIOTY,
                      &bc_comm_handler);

    if (ret != pdPASS)  {
        bc_printf("create thread %s failed\n", BC_COMM_NAME);
        return ;
    }
}

void bc_close(int * fd) 
{
    if(fd != NULL && *fd > 0) {
        close(*fd);

        *fd = -1;
    }
}

