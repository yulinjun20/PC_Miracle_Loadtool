// ShowAppInfo.cpp : implementation file
//

#include "stdafx.h"
#include "LoadTools.h"
#include "ShowAppInfo.h"
#include "LoadToolsDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

APPLICATION_INFO g_sMyAppInfo;  // 在使用CShowAppInfo对象时，请先初始化该对象

/////////////////////////////////////////////////////////////////////////////
// CShowAppInfo dialog


CShowAppInfo::CShowAppInfo(CWnd* pParent /*=NULL*/)
    : CDialog(CShowAppInfo::IDD, pParent)
{
    //{{AFX_DATA_INIT(CShowAppInfo)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void CShowAppInfo::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CShowAppInfo)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowAppInfo, CDialog)
    //{{AFX_MSG_MAP(CShowAppInfo)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowAppInfo message handlers

BOOL CShowAppInfo::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    SetAppInfo(&g_sMyAppInfo);
    return TRUE;  // return TRUE  unless you set the focus to a control
}



// 设置对话框上的显示信息
void CShowAppInfo::SetAppInfo(APPLICATION_INFO *pAppInfo)
{
    char strTemp[200];
    DWORD dwTemp;

    // 显示应用程序名
    memset(strTemp, 0, sizeof(strTemp));
    memcpy(strTemp, pAppInfo->m_abyAppName, 32);
    strTemp[32] = 0;
    SetDlgItemText(IDC_STATIC_SHOW_APP_NAME, strTemp);
    
    // 显示应用程序ID
    memset(strTemp, 0, sizeof(strTemp));
    memcpy(strTemp, pAppInfo->m_abyAID, 16);
    strTemp[16] = 0;
    SetDlgItemText(IDC_STATIC_SHOW_APP_ID, strTemp);

    // 显示应用程序供应商信息
    memset(strTemp, 0, sizeof(strTemp));
    memcpy(strTemp, pAppInfo->m_abyAppProvider, 32);
    strTemp[32] = 0;
    SetDlgItemText(IDC_STATIC_SHOW_PROVIDER, strTemp);

    // 显示应用程序描述信息
    memset(strTemp, 0, sizeof(strTemp));
    memcpy(strTemp, pAppInfo->m_abyDescript, 64);
    strTemp[64] = 0;
    SetDlgItemText(IDC_STATIC_SHOW_DESCRIPT, strTemp);

    // 显示应用程序下载时间
    memset(strTemp, 0, sizeof(strTemp));
    memcpy(strTemp, pAppInfo->m_abyDownloadTime, 4);   // 年
    strTemp[4] = '-';
    memcpy(&strTemp[5], &(pAppInfo->m_abyDownloadTime[4]), 2);  // 月
    strTemp[7] = '-';
    memcpy(&strTemp[8], &(pAppInfo->m_abyDownloadTime[6]), 2);  // 日
    strTemp[10] = ' ';
    memcpy(&strTemp[11], &(pAppInfo->m_abyDownloadTime[8]), 2);  // 时
    strTemp[13] = ':';
    memcpy(&strTemp[14], &(pAppInfo->m_abyDownloadTime[10]), 2);  // 分
    strTemp[16] = ':';
    memcpy(&strTemp[17], &(pAppInfo->m_abyDownloadTime[12]), 2);  // 秒
    strTemp[19] = 0;
    SetDlgItemText(IDC_STATIC_SHOW_DOWNLOAD_TIME, strTemp);

    // 显示应用程序版本号信息
    memset(strTemp, 0, sizeof(strTemp));
    memcpy(strTemp, pAppInfo->m_abyAppVer, 16);
    strTemp[16] = 0;
    SetDlgItemText(IDC_STATIC_SHOW_APP_VERSION, strTemp);

    // 显示应用程序编号
    memset(strTemp, 0, sizeof(strTemp));
    sprintf(strTemp, "%d", pAppInfo->m_byAppNum);
    SetDlgItemText(IDC_STATIC_SHOW_APP_NUMBER, strTemp);

    // 显示应用程序主函数入口地址
    memset(strTemp, 0, sizeof(strTemp));
    ByteToSting(pAppInfo->m_abyMainAddress, 4, strTemp);
    SetDlgItemText(IDC_STATIC_SHOW_MAIN_ADDR, strTemp);

    // 显示应用程序时间函数入口地址
    memset(strTemp, 0, sizeof(strTemp));
    ByteToSting(pAppInfo->m_abyEventAddress, 4, strTemp);
    SetDlgItemText(IDC_STATIC_SHOW_EVENT_ADDR, strTemp);

    // 显示应用程序文件大小信息
    memset(strTemp, 0, sizeof(strTemp));
    dwTemp = pAppInfo->m_abyRFU[0]*65536*256 + pAppInfo->m_abyRFU[1]*65536
        + pAppInfo->m_abyRFU[2]*256 + pAppInfo->m_abyRFU[3];
    sprintf(strTemp, "%d bytes", dwTemp);
    SetDlgItemText(IDC_STATIC_SHOW_APP_SIZE, strTemp);
}


// 将字节转换成字符串
void CShowAppInfo::ByteToSting(BYTE *pbyInput, BYTE byInputLen, char *strOut)
{
    int i;
    BYTE byTemp;

    for (i=0; i<byInputLen; i++)
    {
        byTemp = pbyInput[i];
        byTemp = ((byTemp>>4)&0x0f);
        if (byTemp > 9)
        {
            strOut[2*i] = byTemp + 'A' - 10;
        }
        else
        {
            strOut[2*i] = byTemp + '0';
        }
        byTemp = pbyInput[i];
        byTemp = (byTemp&0x0f);
        if (byTemp > 9)
        {
            strOut[2*i+1] = byTemp + 'A' - 10;
        }
        else
        {
            strOut[2*i+1] = byTemp + '0';
        }
    }
}
