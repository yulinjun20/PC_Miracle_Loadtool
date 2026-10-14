#ifndef _LOAD_TOOLS_DEFINE_H
#define _LOAD_TOOLS_DEFINE_H
#include <COMMCTRL.h>
/*
#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif
*/
#define LANGE_CHINESE   0
#define LANGE_ENGLISH   1


// 文件类型
#define FILE_TYPE_BOOT           0
#define FILE_TYPE_VOS            1
#define FILE_TYPE_APPLICATION    2
#define FILE_TYPE_PARAM          3
#define FILE_TYPE_DATA           4
#define FILE_TYPE_FONT           5

// 打包任务类型
#define BALE_TASK_APPLICATION    0x01
#define BALE_TASK_PARAM          0x02
#define BALE_TASK_DATAM          0x03
#define BALE_TASK_FONT           0x04
#define BALE_TASK_VOS            0x05
#define BALE_TASK_PARA           0x06
#define BALE_TASK_DATA           0x07
#define BALE_TASK_DEL_APP        0x81
#define BALE_TASK_DEL_ALL_APP    0x82
#define BALE_TASK_CREATE_FILE    0x8f


#define DOWNLOAD_STEP_LEN     4096
#define DOWNLOAD_LEN_FIRM     3064

// 命令码
#define CMD_CHANGE_BAUD_REQUEST         0xb000
#define CMD_CHANGE_BAUD_RESPONSE        0xb001

#define CMD_AUTHEN1_REQUEST             0xb100
#define CMD_AUTHEN1_RESPONSE            0xb101
#define CMD_AUTHEN2_REQUEST             0xb102
#define CMD_AUTHEN2_RESPONSE            0xb103

#define CMD_LOAD_FILE_REQUEST           0xb200
#define CMD_LOAD_FILE_RESPONSE          0xb201
#define CMD_LOAD_FILE_DATA_REQUEST      0xb202
#define CMD_LOAD_FILE_DATA_RESPONSE     0xb203
#define CMD_SAVE_FILE_REQUEST           0xb204
#define CMD_SAVE_FILE_RESPONSE          0xb205

#define CMD_GET_APP_LIST_REQUEST        0xb300
#define CMD_GET_APP_LIST_RESPONSE       0xb301
#define CMD_GET_APP_APPEND_REQUEST      0xb302
#define CMD_GET_APP_APPEND_RESPONSE     0xb303
#define CMD_GET_APP_INFO_REQUEST        0xb304
#define CMD_GET_APP_INFO_RESPONSE       0xb305
#define CMD_GET_FONT_INFO_REQUEST       0xb306
#define CMD_GET_FONT_INFO_RESPONSE      0xb307
#define CMD_DEL_FILE_REQUEST            0xb308
#define CMD_DEL_FILE_RESPONSE           0xb309
#define CMD_SET_TIME_REQUEST            0xb30a
#define CMD_SET_TIME_RESPONSE           0xb30b
#define CMD_CREATE_FILE_SYSTEM_REQUEST  0xb30c
#define CMD_CREATE_FILE_SYSTEM_RESPONSE 0xb30d
#define CMD_GET_SERIAL_NUMBER_REQUEST   0xb30e
#define CMD_GET_SERIAL_NUMBER_RESPONSE  0xb30f
#define CMD_SET_SERIAL_NUMBER_REQUEST   0xb310
#define CMD_SET_SERIAL_NUMBER_RESPONSE  0xb311
#define CMD_DELETE_FILE_REQUEST         0xb312
#define CMD_DELETE_FILE_RESPONSE        0xb313

#define CMD_FINSH_ALL                   0xb400



#define  MAX_APPLICATION_NUMBER  25


#define CREATE_FILE_SYSTEM_TIME   25000
#define DELETE_FILE_TIME          10000

struct APPLICATION_INFO
{
    BYTE m_abyAppName[32];
    BYTE m_abyAID[16];
    BYTE m_abyAppVer[16];
    BYTE m_abyAppProvider[32];
    BYTE m_abyDescript[64];
    BYTE m_abyDownloadTime[14];
    BYTE m_abyMainAddress[4];
    BYTE m_abyEventAddress[4];
    BYTE m_byAppNum;
    BYTE m_abyRFU[73];
};   // 256 bytes

struct APPLICATION_LIST
{
    BYTE m_byValidityFlag;   // 1：有效，0：无效
    BYTE m_byAppStatus;
    BYTE m_byAppNum;
    BYTE m_abyAppFileSize[4];
    HTREEITEM m_hTree;
    APPLICATION_INFO m_sAppInfo;
};


#define COMM_RETRY_TIMES  3

// 错误消息编号

#define MSG_CREATE_THREAD_FAILED          0
#define MSG_OPEN_SERIAL_PORT_FAILED       1
#define MSG_CONNECTING                    2
#define MSG_USER_STOP_DOWNLOAD            3
#define MSG_HANDSHACK_FAILED              4
#define MSG_CHANGE_BAUD_FAILED            5
#define MSG_SEND_FAILED                   6
#define MSG_RECEIVE_FAILED                7
#define MSG_CHANGE_BAUD                   8
#define MSG_AUTHENTICATION                9
#define MSG_AUTHENTICATION_ERROR         10
#define MSG_OPEN_USB_PORT_FAILED         11

#define MSG_CHN_CREATE_THREAD_FAILED         "创建线程失败"
#define MSG_CHN_OPEN_SERIAL_PORT_FAILED      "打开串口失败"
#define MSG_CHN_CONNECTING                   "正在连接..."
#define MSG_CHN_USER_STOP_DOWNLOAD           "用户终止下载!"
#define MSG_CHN_HANDSHACK_FAILED             "握手失败"
#define MSG_CHN_CHANGE_BAUD_FAILED           "修改波特率失败"
#define MSG_CHN_SEND_FAILED                  "发送错误"
#define MSG_CHN_RECEIVE_FAILED               "接收错误"
#define MSG_CHN_CHANGE_BAUD                  "修改波特率"
#define MSG_CHN_AUTHENTICATION               "认证..."
#define MSG_CHN_AUTHENTICATION_ERROR         "认证失败."
#define MSG_CHN_OPEN_USB_PORT_FAILED         "打开USB口失败"


#define MSG_ENG_CREATE_THREAD_FAILED         "Create thread failed."
#define MSG_ENG_OPEN_SERIAL_PORT_FAILED      "Open serial port failed."
#define MSG_ENG_CONNECTING                   "Connecting..."
#define MSG_ENG_USER_STOP_DOWNLOAD           "User stop download."
#define MSG_ENG_HANDSHACK_FAILED             "Handshack failed."
#define MSG_ENG_CHANGE_BAUD_FAILED           "Change baud failed."
#define MSG_ENG_SEND_FAILED                  "Send failed"
#define MSG_ENG_RECEIVE_FAILED               "Receive failed"
#define MSG_ENG_CHANGE_BAUD                  "Change baud..."
#define MSG_ENG_AUTHENTICATION               "Authentication."
#define MSG_ENG_AUTHENTICATION_ERROR         "Authentication failed."
#define MSG_ENG_OPEN_USB_PORT_FAILED         "Open USB port failed."



#endif


