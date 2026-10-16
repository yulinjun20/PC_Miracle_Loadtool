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


// �ļ�����
#define FILE_TYPE_BOOT           0  /* Boot abandoned */
#define FILE_TYPE_VOS            1  /* OS; legacy 0xB0 transit */
#define FILE_TYPE_APPLICATION    2
#define FILE_TYPE_PARAM          3
#define FILE_TYPE_DATA           4
#define FILE_TYPE_FONT           5
#define FILE_TYPE_SC_APP         6
#define FILE_TYPE_WIFI           7
#define FILE_TYPE_LOGO           8

// �����������?
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
#define DOWNLOAD_LEN_FIRM     4096

// ������
#define CMD_CHANGE_BAUD_REQUEST         0xb000
#define CMD_CHANGE_BAUD_RESPONSE        0xb001

#define CMD_AUTHEN1_REQUEST             0xb100
#define CMD_AUTHEN1_RESPONSE            0xb101
#define CMD_AUTHEN2_REQUEST             0xb102
#define CMD_AUTHEN2_RESPONSE            0xb103


/* Unified file download session (DownloadProtocol_v1) for non-OS */
#define CMD_FILE_DL_START_REQ           0xb210
#define CMD_FILE_DL_START_RSP           0xb211
#define CMD_FILE_DL_DATA_REQ            0xb212
#define CMD_FILE_DL_DATA_RSP            0xb213
#define CMD_FILE_DL_END_REQ             0xb214
#define CMD_FILE_DL_END_RSP             0xb215
#define CMD_FILE_DL_ABORT_REQ           0xb216
#define CMD_FILE_DL_ABORT_RSP           0xb217

#define FILE_DL_PROTO_VER               1
#define FILE_DL_CHUNK_MAX               4096
#define FILE_DL_NAME_MAX                32

/* End/Start status (aligned with device usb_file_dl ERR_*) */
#define FILE_DL_ERR_OK                  0
#define FILE_DL_ERR_UNSUPPORTED_PROTO   1
#define FILE_DL_ERR_UNSUPPORTED_TYPE    2
#define FILE_DL_ERR_NAME_INVALID        3
#define FILE_DL_ERR_SIZE_EXCEED         4
#define FILE_DL_ERR_NO_SPACE            5
#define FILE_DL_ERR_BUSY                6
#define FILE_DL_ERR_OFFSET              7
#define FILE_DL_ERR_CHUNK_CRC           8
#define FILE_DL_ERR_FILE_HASH           9
#define FILE_DL_ERR_WRITE               10
#define FILE_DL_ERR_ABORT               11
#define FILE_DL_ERR_VERIFY_FAIL         12
#define FACTORY_MAX_SLOTS               12
#define OS_REBOOT_WAIT_MS               20000

#define DL_MODE_DEVELOPER               0
#define DL_MODE_FACTORY                 1

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
#define CMD_FINISH_ALL                  CMD_FINSH_ALL  /* D11 */



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
    BYTE m_byValidityFlag;   // 1����Ч��0����Ч
    BYTE m_byAppStatus;
    BYTE m_byAppNum;
    BYTE m_abyAppFileSize[4];
    HTREEITEM m_hTree;
    APPLICATION_INFO m_sAppInfo;
};


#define COMM_RETRY_TIMES  3

// ������Ϣ���?

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

#define MSG_CHN_CREATE_THREAD_FAILED         "�����߳�ʧ��"
#define MSG_CHN_OPEN_SERIAL_PORT_FAILED      "�򿪴���ʧ��"
#define MSG_CHN_CONNECTING                   "��������..."
#define MSG_CHN_USER_STOP_DOWNLOAD           "�û���ֹ����!"
#define MSG_CHN_HANDSHACK_FAILED             "����ʧ��"
#define MSG_CHN_CHANGE_BAUD_FAILED           "�޸Ĳ�����ʧ��"
#define MSG_CHN_SEND_FAILED                  "���ʹ���"
#define MSG_CHN_RECEIVE_FAILED               "���մ���"
#define MSG_CHN_CHANGE_BAUD                  "�޸Ĳ�����"
#define MSG_CHN_AUTHENTICATION               "��֤..."
#define MSG_CHN_AUTHENTICATION_ERROR         "��֤ʧ��."
#define MSG_CHN_OPEN_USB_PORT_FAILED         "��USB��ʧ��"


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


