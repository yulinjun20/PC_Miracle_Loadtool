#ifndef _BASE_H_
#define _BASE_H_

#include <windows.h>
#include "LoadToolsDefine.h"


// 不要大于65536
#define INCISE_SIZE    20480

#define IS_COMPRESS   1
#define NO_COMPRESS   0

#define LOCAL_DOWNLOAD    0
#define REMOTE_DOWNLOAD   1



#ifndef NULL
#define NULL 0
#endif

#define ERROR_INFO_MAX_LENGTH 512

#define MAX_APP_PATH_NUMBER 5

struct CONFIG_INFO
{
    int m_iSerialPortNumber;           // combo index 0..98 -> COM1..COM99
    int m_iSerialPortBaudsSelect;      // baud index 0..9

    char m_strBootFilePath[256];       // boot文件存放路径
    char m_strVOSFilePath[256];        // VOS文件存放路径
    char m_strAppFilePath[MAX_APP_PATH_NUMBER][256];   // 应用程序路径名，最多保存5个
    char m_strFontFilePath[256];       // 字库文件存放路径
    char m_strBaleFilePath[256];       // 打包文件存放路径
    char m_strParamFilePath[256];      // 参数文件存放路径
    char m_strDataFilePath[256];       // 数据文件存放路径
    char m_strParamEditFilepath[256];  // 参数编辑文件存放路径

    char m_strSerialNumber[36];        // 保存序列号

    BYTE m_byAppPathCurrentSelect;     // 当前选择的路径编号，取值0到4
    BYTE m_byAppFilePathNumber;        // 应用程序路径个数，取值0到5

    BYTE m_byCommType;     // 通讯方式选择，0表示选择串口，1表示选择USB口

    BYTE m_byBootSelect;               // boot下载选择
    BYTE m_byVOSSelect;                // VOS下载选择
    BYTE m_byApplicationSelect;        // 应用文件下载选择
    BYTE m_byFontSelect;               // 字库文件下载选择
    BYTE m_byDataFileSelect;           // 数据文件下载选择
    BYTE m_byParamSelect;              // 参数文件下载选择
    BYTE m_byBaleFileSelect;           // 打包文件下载选择
    BYTE m_byChangeSNSelect;           // 修改序列号选择
    BYTE m_byCreateFileSystemSelect;   // 创建文件系统选择

    BYTE m_bySetTimeSelect;            // 设置时间选择。1表示被选中，0表示未被选中

    BYTE m_abySeed[8];                 // 获取随机数的随机因子

    BYTE m_byLoadType;            // 下载方式：0 local，1 remote
    BYTE m_byPrinterType;              // 打印机类型选择
    BYTE m_byPosType;                  // 机型选择
    BYTE m_abyVerFlag[2];

    char m_strScAppFilePath[256];     // SC App firmware path (placeholder)
    char m_strWifiFilePath[256];      // WIFI firmware path (placeholder)
    char m_strLogoFilePath[256];      // Logo file path (placeholder)
    BYTE m_byScAppSelect;             // SC App download select
    BYTE m_byWifiSelect;              // WIFI download select
    BYTE m_byLogoSelect;              // Logo download select

    BYTE m_abyRev[240];                // 保留字段
};


void DwordToByte(DWORD dwIn, BYTE *pbyOut); 

int GetConfigureInfo(void);
int SaveConfigInfo(void);
void GetRandom(BYTE *pbyRandomOut);


int Compress(BYTE *srcbuf, BYTE *dstbuf, DWORD srclen, DWORD *dstlen);
int Decompress(BYTE *srcbuf, BYTE *dstbuf, DWORD srclen, DWORD *dstlen);



#define ENCRYPT              0x01
#define DECRYPT              0x02
// mode=1:encrypt; mode=0:decrypt
void des(BYTE *DES_DATA, BYTE *DES_RESULT, BYTE *DesKey, char cryp_decrypt);


int G_MyGetMessage(int iIndex, char *strOut);




int RSACompute(BYTE *pbyModule, DWORD dwModuleLen, BYTE *pbyExp,
                DWORD dwExpLen, BYTE *pbyDataIn, BYTE *pbyDataOut);



// DataOut[20]
void Hash(BYTE* DataIn,unsigned int DataInLen,BYTE* DataOut);

#endif

