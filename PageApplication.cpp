// PageApplication.cpp : implementation file
//

#include "stdafx.h"
#include "LoadTools.h"
#include "PageApplication.h"
#include "ShowAppInfo.h"
#include "Base.h"
#include "LoadToolsDlg.h"
#include "Communication.h"
#include "LoadToolsDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bThreadRunFlag;

HANDLE g_lHandle = NULL;  // 线程句柄

extern CONFIG_INFO g_sConfigreInfo;

extern void UpdateConfigure(void);

extern void G_MyShowMessage(int iMessageIndex);
extern int CheckApplication(char *strFileName);

BYTE g_abyTempSend[200];

DWORD g_adwBauds[8] = {115200, 57600, 38400, 28800, 19200, 14400, 9600, 230400};

BYTE g_abyAuthenKey[8] = {0x33, 0x00, 0x22, 0xf1, 0xc4, 0x3d, 0x25, 0x5e};

extern APPLICATION_INFO g_sMyAppInfo;  // 在使用CShowAppInfo对象时，请先初始化该对象

// 指向下载工具LoadTools主界面的指针，只要程序运行，该指针就指向LoadTools主界面
extern CLoadToolsDlg *g_pCLoadToolsDlg;


/////////////////////////////////////////////////////////////////////////////
// CPageApplication property page

IMPLEMENT_DYNCREATE(CPageApplication, CPropertyPage)

CPageApplication::CPageApplication() : CPropertyPage(CPageApplication::IDD)
{
    m_bStopDownloadFlag = FALSE;
    m_iCurrentAppNo = 0;
    //{{AFX_DATA_INIT(CPageApplication)
    m_strBaleFilePath = _T("");
    m_strFontFilePath = _T("");
    m_strParamFilePath = _T("");
    m_strVposSN = _T("");
    m_bAppSelect = FALSE;
    m_bBaleFileSelect = FALSE;
    m_bCreateFileSystemSelect = FALSE;
    m_bFontFileSelect = FALSE;
    m_bParamFileSelect = FALSE;
    m_bDownloadSNSelect = FALSE;
    m_bSetTimeSelect = FALSE;
    m_strDataFilePath = _T("");
    m_bDataFileSelect = FALSE;
    //}}AFX_DATA_INIT
}

CPageApplication::~CPageApplication()
{
}

void CPageApplication::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPageApplication)
    DDX_Control(pDX, IDC_TREE_FILE_LIST, m_tcVposFileList);
    DDX_Control(pDX, IDC_COMBO_APP_FILE_PATH, m_cbAppFilePath);
    DDX_Text(pDX, IDC_EDIT_BALE_FILE_PATH, m_strBaleFilePath);
    DDX_Text(pDX, IDC_EDIT_FONT_FILE_PATH, m_strFontFilePath);
    DDX_Text(pDX, IDC_EDIT_PARAM_FILE_PATH, m_strParamFilePath);
    DDX_Text(pDX, IDC_EDIT_VPOS_SERIAL_NUMBER, m_strVposSN);
    DDV_MaxChars(pDX, m_strVposSN, 32);
    DDX_Check(pDX, IDC_CHECK_APP_SELECT, m_bAppSelect);
    DDX_Check(pDX, IDC_CHECK_BALE_FILE, m_bBaleFileSelect);
    DDX_Check(pDX, IDC_CHECK_CREATE_FILE_SYSTEM, m_bCreateFileSystemSelect);
    DDX_Check(pDX, IDC_CHECK_FONT_FILE, m_bFontFileSelect);
    DDX_Check(pDX, IDC_CHECK_PARAM_FILE, m_bParamFileSelect);
    DDX_Check(pDX, IDC_CHECK_SERIAL_NUMBER, m_bDownloadSNSelect);
    DDX_Check(pDX, IDC_CHECK_SET_TIME, m_bSetTimeSelect);
    DDX_Text(pDX, IDC_EDIT_DATA_FILE_PATH, m_strDataFilePath);
    DDX_Check(pDX, IDC_CHECK_DATA_FILE, m_bDataFileSelect);

    DDX_Control(pDX, IDC_BUTTON_BEGIN_DOWNLOAD_APP, m_btnDownload);
    DDX_Control(pDX, IDC_BUTTON_STOP_DOWNLOAD_APP, m_btnStop);

    DDX_Control(pDX, IDC_BUTTON_REFURBISH, m_btnReflush);
    DDX_Control(pDX, IDC_BUTTON_DELETE_APP, m_btnDeleteApp);
    DDX_Control(pDX, IDC_BUTTON_DELETE_APPEND_FILE, m_btnDeleteAppendFile);
    DDX_Control(pDX, IDC_BUTTON_SHOW_APP_INFO, m_btnShowAppInfo);

    DDX_Control(pDX, IDC_BUTTON_OPEN_APP_FILE, m_btnOpenAppFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_FONT_FILE, m_btnOpenFontFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_BALE_FILE, m_btnOpenBaleFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_PARAM_FILE, m_btnOpenParamFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_DATA_FILE, m_btnOpenDataFile);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageApplication, CPropertyPage)
    //{{AFX_MSG_MAP(CPageApplication)
    ON_BN_CLICKED(IDC_BUTTON_REFURBISH, OnButtonRefurbish)
    ON_BN_CLICKED(IDC_BUTTON_DELETE_APP, OnButtonDeleteApp)
    ON_BN_CLICKED(IDC_BUTTON_DELETE_APPEND_FILE, OnButtonDeleteAppendFile)
    ON_BN_CLICKED(IDC_BUTTON_SHOW_APP_INFO, OnButtonShowAppInfo)
    ON_BN_CLICKED(IDC_BUTTON_BEGIN_DOWNLOAD_APP, OnButtonBeginDownloadApp)
    ON_BN_CLICKED(IDC_BUTTON_STOP_DOWNLOAD_APP, OnButtonStopDownloadApp)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_APP_FILE, OnButtonOpenAppFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_FONT_FILE, OnButtonOpenFontFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_BALE_FILE, OnButtonOpenBaleFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_PARAM_FILE, OnButtonOpenParamFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_DATA_FILE, OnButtonOpenDataFile)
    ON_BN_CLICKED(IDC_CHECK_APP_SELECT, OnCheckAppSelect)
    ON_BN_CLICKED(IDC_CHECK_BALE_FILE, OnCheckBaleFile)
    ON_BN_CLICKED(IDC_CHECK_CREATE_FILE_SYSTEM, OnCheckCreateFileSystem)
    ON_BN_CLICKED(IDC_CHECK_FONT_FILE, OnCheckFontFile)
    ON_BN_CLICKED(IDC_CHECK_PARAM_FILE, OnCheckParamFile)
    ON_BN_CLICKED(IDC_CHECK_SERIAL_NUMBER, OnCheckSerialNumber)
    ON_BN_CLICKED(IDC_CHECK_SET_TIME, OnCheckSetTime)
    ON_BN_CLICKED(IDC_CHECK_DATA_FILE, OnCheckDataFile)
    ON_EN_CHANGE(IDC_EDIT_BALE_FILE_PATH, OnChangeEditBaleFilePath)
    ON_EN_CHANGE(IDC_EDIT_FONT_FILE_PATH, OnChangeEditFontFilePath)
    ON_EN_CHANGE(IDC_EDIT_PARAM_FILE_PATH, OnChangeEditParamFilePath)
    ON_EN_CHANGE(IDC_EDIT_VPOS_SERIAL_NUMBER, OnChangeEditVposSerialNumber)
    ON_CBN_EDITCHANGE(IDC_COMBO_APP_FILE_PATH, OnEditchangeComboAppFilePath)
    ON_CBN_SELCHANGE(IDC_COMBO_APP_FILE_PATH, OnSelchangeComboAppFilePath)
    ON_EN_CHANGE(IDC_EDIT_DATA_FILE_PATH, OnChangeEditDataFilePath)
    ON_NOTIFY(NM_DBLCLK, IDC_TREE_FILE_LIST, OnDblclkTreeFileList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageApplication message handlers

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
BOOL CPageApplication::OnInitDialog()
{
    int i;
    char strTempMsg[5];
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // TODO: Add extra initialization here
    if (0 != g_sConfigreInfo.m_byBaleFileSelect)
    {
        g_sConfigreInfo.m_byApplicationSelect = 0;
        g_sConfigreInfo.m_byFontSelect = 0;
        g_sConfigreInfo.m_byParamSelect = 0;
        g_sConfigreInfo.m_byCreateFileSystemSelect = 0;
        g_sConfigreInfo.m_byDataFileSelect = 0;
    }
    m_bAppSelect              = g_sConfigreInfo.m_byApplicationSelect;
    m_bFontFileSelect         = g_sConfigreInfo.m_byFontSelect;
    m_bParamFileSelect        = g_sConfigreInfo.m_byParamSelect;
    m_bDownloadSNSelect       = g_sConfigreInfo.m_byChangeSNSelect;
    m_bBaleFileSelect         = g_sConfigreInfo.m_byBaleFileSelect;
    m_bCreateFileSystemSelect = g_sConfigreInfo.m_byCreateFileSystemSelect;
    m_bSetTimeSelect          = g_sConfigreInfo.m_bySetTimeSelect;
    m_bDataFileSelect         = g_sConfigreInfo.m_byDataFileSelect;

    m_cbAppFilePath.ResetContent();  // 清除所有内容
    if (g_sConfigreInfo.m_byAppFilePathNumber > MAX_APP_PATH_NUMBER)
    {
        g_sConfigreInfo.m_byAppFilePathNumber = 0;
    }
    for (i=0; i<g_sConfigreInfo.m_byAppFilePathNumber; i++)
    {
        m_cbAppFilePath.AddString(&g_sConfigreInfo.m_strAppFilePath[i][0]);
    }
    if (0 != g_sConfigreInfo.m_byAppFilePathNumber)
    {
        m_strAppFilePath.Format("%s", 
            &g_sConfigreInfo.m_strAppFilePath[g_sConfigreInfo.m_byAppPathCurrentSelect][0]);
        m_cbAppFilePath.SetCurSel(g_sConfigreInfo.m_byAppPathCurrentSelect);
    }
    else
    {
        m_strAppFilePath.Format("");
    }

    m_strBaleFilePath.Format(g_sConfigreInfo.m_strBaleFilePath);
    m_strFontFilePath.Format(g_sConfigreInfo.m_strFontFilePath);
    m_strParamFilePath.Format(g_sConfigreInfo.m_strParamFilePath);
    m_strDataFilePath.Format(g_sConfigreInfo.m_strDataFilePath);

    g_sConfigreInfo.m_strSerialNumber[32] = 0;
    
    m_strVposSN.Format(g_sConfigreInfo.m_strSerialNumber);
    
    sprintf(strTempMsg, "%d", strlen(g_sConfigreInfo.m_strSerialNumber));
    SetDlgItemText(IDC_STATIC_SN_LENGTH, strTempMsg);

    m_bSetTimeSelect = TRUE;
    
    UpdateData(FALSE);

    GetDlgItem(IDC_BUTTON_STOP_DOWNLOAD_APP)->EnableWindow(FALSE);
    GetDlgItem(IDC_CHECK_SET_TIME)->EnableWindow(FALSE);

    
    m_btnDownload.SetIcon(IDI_ICON_RUN, NULL, 50, 32);
    m_btnDownload.SetShowText(TRUE);
    m_btnDownload.SetActiveBgColor(RGB(200,200,200), FALSE);
    
    m_btnStop.SetIcon(IDI_ICON_STOP, NULL, 50, 32);
    m_btnStop.SetShowText(TRUE);
    m_btnStop.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnDeleteApp.SetIcon(IDI_ICON_DELETE1, NULL, 40, 32);
    m_btnDeleteApp.SetShowText(TRUE);
    m_btnDeleteApp.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnReflush.SetIcon(IDI_ICON_SEARCH, NULL, 40, 32);
    m_btnReflush.SetShowText(TRUE);
    m_btnReflush.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnOpenAppFile.SetIcon(IDI_ICON_OPEN1, NULL, 32, 32);
    m_btnOpenAppFile.SetShowText(TRUE);
    m_btnOpenAppFile.SetActiveBgColor(RGB(200,200,200), FALSE);


    m_btnOpenFontFile.SetIcon(IDI_ICON_OPEN1, NULL, 32, 32);
    m_btnOpenFontFile.SetShowText(TRUE);
    m_btnOpenFontFile.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnOpenBaleFile.SetIcon(IDI_ICON_OPEN1, NULL, 32, 32);
    m_btnOpenBaleFile.SetShowText(TRUE);
    m_btnOpenBaleFile.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnOpenParamFile.SetIcon(IDI_ICON_OPEN1, NULL, 32, 32);
    m_btnOpenParamFile.SetShowText(TRUE);
    m_btnOpenParamFile.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnOpenDataFile.SetIcon(IDI_ICON_OPEN1, NULL, 32, 32);
    m_btnOpenDataFile.SetShowText(TRUE);
    m_btnOpenDataFile.SetActiveBgColor(RGB(200,200,200), FALSE);
    
    m_btnDeleteAppendFile.SetIcon(IDI_ICON_DELETE2, NULL, 35, 32);
    m_btnDeleteAppendFile.SetShowText(TRUE);
    m_btnDeleteAppendFile.SetActiveBgColor(RGB(200,200,200), FALSE);
    
    m_btnShowAppInfo.SetIcon(IDI_ICON_SHOW, NULL, 35, 32);
    m_btnShowAppInfo.SetShowText(TRUE);
    m_btnShowAppInfo.SetActiveBgColor(RGB(200,200,200), FALSE);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
BOOL CPageApplication::IsThreadRun()
{
    return g_bThreadRunFlag;
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
BOOL CPageApplication::OnKillActive() 
{
    // TODO: Add your specialized code here and/or call the base class
    
    if( IsThreadRun() )
    {
        return FALSE;  // 有线程在跑时，本界面不响应用户点击事件
    }

    return CPropertyPage::OnKillActive();
}

/****************************************************************************
  函数名     :  
  描述       :  刷新线程响应函数
  输入参数   :  1、void *pTemp：线程启动传入的话参数，在这里没有作用
  输出参数   :  无
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
int ThreadRefurbish(void *pTemp)
{
    int iRet;

    g_bThreadRunFlag = TRUE;
    g_pCLoadToolsDlg->m_PageApplication.MyEnableWindows(FALSE);
    iRet = g_pCLoadToolsDlg->m_PageApplication.DownloadStart();
    if (0 == iRet)
    {
        (void)g_pCLoadToolsDlg->m_PageApplication.UpdateTreeList();
    }
    if (2 != iRet)
    {
        g_pCLoadToolsDlg->m_PageApplication.DownloadEnd();
    }
    g_pCLoadToolsDlg->m_PageApplication.MyEnableWindows(TRUE);
    g_bThreadRunFlag = FALSE;
    return 0;
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnButtonRefurbish() 
{
    // TODO: Add your control notification handler code here
    DWORD ThreadId;

    UpdateConfigure();  // 将界面上的配置保存在配置缓冲中
    SaveConfigInfo();   // 将配置缓冲中的配置保存到配置文件中
    m_bStopDownloadFlag = FALSE; 
    if (!g_bThreadRunFlag)
    {
        g_lHandle = CreateThread(NULL, 0, 
            (LPTHREAD_START_ROUTINE)ThreadRefurbish, NULL, 0, &ThreadId);
        if (NULL == g_lHandle)
        {
            G_MyShowMessage(MSG_CREATE_THREAD_FAILED);
        }
    }
}

/****************************************************************************
  函数名     :  
  描述       :  握手认证
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
int CPageApplication::DownloadStart()
{
    int iRet;

    iRet = Handshack();
    if (0 != iRet)
    {
        return iRet;
    }
    return Authentication();
}

/****************************************************************************
  函数名     :  
  描述       :  发送结束通知数据包并关闭串口
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::DownloadEnd()
{
    BYTE abyTemp[2];

    (void)Crc16SendPacket(abyTemp, 0, CMD_FINSH_ALL);
    Sleep(50);
    SerClose();
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
int CPageApplication::UpdateTreeList()
{
    BYTE abyTemp[1200];
    WORD wLen = 0;
    int iRet, iAppNum, i, j, iAppendCount, k;

    char str[256];

    MyShowMessage("Refurbish infomation");

    // 获取VPOS序列号信息
    for (k=0; k<COMM_RETRY_TIMES; k++)
    {
        (void)Crc16SendPacket(abyTemp, 0, CMD_GET_SERIAL_NUMBER_REQUEST);
        iRet = Crc16RecvPacket(abyTemp, &wLen, 1000);
        if ((0==iRet) && (40==wLen))
        {
            k = 0;
            break;
        }
    }
    if (k >= COMM_RETRY_TIMES)
    {
        if (RECV_TIMEOUT == iRet)
        {
            MyShowMessage("Recive timeout when get serial number!");
        }
        else
        {
            MyShowMessage("Get serial number error!");
        }
        return 1;
    }
    memset(&abyTemp[100], ' ', 200);
    m_abyVposType[15] = 0;
    memcpy(&abyTemp[100], "POS type:", 9);
    memcpy(&abyTemp[109], m_abyVposType, strlen((char*)m_abyVposType));
    if (0 == abyTemp[5])
    {
        memcpy((char*)&abyTemp[124], "Serial number:", 14);
        memcpy((char*)&abyTemp[138], &abyTemp[6], 32);
    }
    abyTemp[163] = 0;

    m_tcVposFileList.DeleteAllItems();
    m_tcVposFileList.InsertItem((char*)&abyTemp[100]);

    // 获取应用程序列表
    for (k=0; k<COMM_RETRY_TIMES; k++)
    {
        (void)Crc16SendPacket(abyTemp, 0, CMD_GET_APP_LIST_REQUEST);
        iRet = Crc16RecvPacket(abyTemp, &wLen, 1000);
        if ((0==iRet) || (2==(wLen%6)))
        {
            k = 0;
            break;
        }
    }
    if (k >= COMM_RETRY_TIMES)
    {
        if (RECV_TIMEOUT == iRet)
        {
            MyShowMessage("Recive timeout when get application list!");
        }
        else
        {
            MyShowMessage("Get application list error");
        }
        return 1;
    }
    iAppNum = wLen/6 - 1;
    if (iAppNum > MAX_APPLICATION_NUMBER)
    {
        MyShowMessage("Get application list error, application number over load!");
        return 1;
    }

    // 0         1         2         3         4         5         6         7
    // 0123456789012345678901234567890123456789012345678901234567890123456789012
    // type                size                name    
    // Application         1000000 Bytes       多应用管理
    // Data File           200000 Bytes        data.tel
    // Param File          10000 Bytes         EnvFile
    // Font                960000 Bytes        FontFile

    for (i=0; i<MAX_APPLICATION_NUMBER; i++)
    {
        m_saAppInfo[i].m_byValidityFlag = 0;
    }

    // 初始化应用程序信息结构
    for (i=0; i<iAppNum; i++)
    {
        m_saAppInfo[i].m_byAppNum = abyTemp[i*6+6];
        m_saAppInfo[i].m_byAppStatus = abyTemp[i*6+7];
        memcpy(m_saAppInfo[i].m_abyAppFileSize, &abyTemp[i*6+8], 4);
    }

    // 获取应用程序信息
    for (i=0; i<iAppNum; i++)
    {
        abyTemp[0] = m_saAppInfo[i].m_byAppNum;
        for (k=0; k<COMM_RETRY_TIMES; k++)
        {
            (void)Crc16SendPacket(abyTemp, 1, CMD_GET_APP_INFO_REQUEST);
            iRet = Crc16RecvPacket(abyTemp, &wLen, 3000);
            if ((0==iRet) && (264==wLen))
            {
                k = 0;
                break;
            }
        }
        if (k >= COMM_RETRY_TIMES)
        {
            MyShowMessage("Get application infomation error!");
            return 1;
        }
        if (0 != abyTemp[5])
        {
            continue;
        }
        memcpy(&(m_saAppInfo[i].m_sAppInfo), &abyTemp[6], sizeof(APPLICATION_INFO));
        m_saAppInfo[i].m_byValidityFlag = 1;
    }

    // 显示应用程序信息
    for (i=0; i<iAppNum; i++)
    {
        memset(str, ' ', sizeof(str));
        // 应用程序信息显示
        sprintf(str, "%2d Application                   ", m_saAppInfo[i].m_byAppNum);
        iRet = m_saAppInfo[i].m_abyAppFileSize[0]*256*65536
            + m_saAppInfo[i].m_abyAppFileSize[1]*65536
            + m_saAppInfo[i].m_abyAppFileSize[2]*256
            + m_saAppInfo[i].m_abyAppFileSize[3];
        sprintf(&str[23], "%8d bytes", iRet);
        strcat(&str[29], "                ");
        m_saAppInfo[i].m_sAppInfo.m_abyAppName[31] = 0;
        sprintf(&str[43], "%s", m_saAppInfo[i].m_sAppInfo.m_abyAppName);

        // 将信息显示到界面上
        m_saAppInfo[i].m_hTree = m_tcVposFileList.InsertItem(str);
    }

    // 获取附属文件信息
    for (i=0; i<iAppNum; i++)
    {
        abyTemp[0] = m_saAppInfo[i].m_byAppNum;
        for (k=0; k<COMM_RETRY_TIMES; k++)
        {
            (void)Crc16SendPacket(abyTemp, 1, CMD_GET_APP_APPEND_REQUEST);
            iRet = Crc16RecvPacket(abyTemp, &wLen, 1000);
            if ((0==iRet) && (0==abyTemp[5]))
            {
                iAppendCount = wLen/37;
                for (j=0; j<iAppendCount; j++)
                {
                    iRet = (abyTemp[37*j+7]*256+abyTemp[37*j+8])*65536
                        +abyTemp[37*j+9]*256+abyTemp[37*j+10];
                    memset(str, ' ', sizeof(str));
                    if (4 == abyTemp[37*j+6])
                    {
                        sprintf(str, "%03d  Data file        ", j+1);
                    }
                    else
                    {
                        sprintf(str, "%03d Param file        ", j+1);
                    }
                    sprintf(&str[20], "%8d bytes", iRet);
                    strcat(&str[20], "              ");
                    abyTemp[37*j+27] = 0;
                    sprintf(&str[40], "%s", &abyTemp[37*j+11]);
                    m_tcVposFileList.InsertItem(str, m_saAppInfo[i].m_hTree, TVI_SORT);
                    m_tcVposFileList.Expand(m_saAppInfo[i].m_hTree, TVE_EXPAND);
                }
                break;
            }
            else if (0 == iRet)
            {
                break;
            }
        }
    }

    MyShowMessage("Success!");
    return 0;
}

/****************************************************************************
  函数名     :  
  描述       :  删除文件线程
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
int ThreadDeleteFile(void *pTemp)
{
    int iRet;
    g_bThreadRunFlag = TRUE;
    g_pCLoadToolsDlg->m_PageApplication.MyEnableWindows(FALSE);
    iRet = g_pCLoadToolsDlg->m_PageApplication.DownloadStart();
    if (0 == iRet)
    {
        iRet = g_pCLoadToolsDlg->m_PageApplication.DeleteFileProcess((BYTE*)pTemp);
        if (0 == iRet)
        {
            iRet = g_pCLoadToolsDlg->m_PageApplication.UpdateTreeList();
        }
    }
    if (2 != iRet)
    {
        g_pCLoadToolsDlg->m_PageApplication.DownloadEnd();
    }
    g_pCLoadToolsDlg->m_PageApplication.MyEnableWindows(TRUE);
    g_bThreadRunFlag = FALSE;
    return 0;
}

int CPageApplication::DeleteFileProcess(BYTE *pbyInData)
{
    BYTE abyTemp[100];
    WORD wLen = 0;
    int iRet, i;

    MyShowMessage("Delete file");
    // 删除文件
    for (i=0; i<COMM_RETRY_TIMES; i++)
    {
        (void)Crc16SendPacket(pbyInData, 2, CMD_DEL_FILE_REQUEST);
        iRet = Crc16RecvPacket(abyTemp, &wLen, 1000);
        if (0 == iRet)
        {
            i = 0;
            break;
        }
    }
    if (i >= COMM_RETRY_TIMES)
    {
        MessageBox("Receive delete file response packet failed!");
        return 1;
    }
    return 0;
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnButtonDeleteApp() 
{
    // TODO: Add your control notification handler code here
    HTREEITEM hSelItem;
    int i;
    DWORD ThreadId;

    hSelItem = m_tcVposFileList.GetSelectedItem();
    if (NULL == hSelItem)
    {
        MessageBox("No select application");
        return ;
    }

    for (i=0; i<MAX_APPLICATION_NUMBER; i++)
    {
        if (  (0!=m_saAppInfo[i].m_byValidityFlag)
            &&(hSelItem==m_saAppInfo[i].m_hTree))
        {
            m_bStopDownloadFlag = FALSE; 
            if (!g_bThreadRunFlag)
            {
                g_abyTempSend[0] = m_saAppInfo[i].m_byAppNum;
                g_abyTempSend[1] = 0;
                g_lHandle = CreateThread(NULL, 0, 
                    (LPTHREAD_START_ROUTINE)ThreadDeleteFile, g_abyTempSend, 
                    0, &ThreadId);
                if (NULL == g_lHandle)
                {
                    G_MyShowMessage(MSG_CREATE_THREAD_FAILED);
                }
            }
            else
            {
                MessageBox("There has a thread in running, can't delete the application, retry again after");
            }
            return ;
        }
    }
    MessageBox("You have not select the application!");
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnButtonShowAppInfo() 
{
    // TODO: Add your control notification handler code here
    HTREEITEM hSelItem;
    int i;

    hSelItem = m_tcVposFileList.GetSelectedItem();
    if (NULL == hSelItem)
    {
        MessageBox("You have not select the application!");
        return ;
    }

    for (i=0; i<MAX_APPLICATION_NUMBER; i++)
    {
        if (  (0!=m_saAppInfo[i].m_byValidityFlag)
            &&(hSelItem==m_saAppInfo[i].m_hTree))
        {
            memcpy(m_saAppInfo[i].m_sAppInfo.m_abyRFU, 
                m_saAppInfo[i].m_abyAppFileSize, 4);
            memcpy(&g_sMyAppInfo, &m_saAppInfo[i].m_sAppInfo, 
                sizeof(APPLICATION_INFO));
            CShowAppInfo cAppDlg;
            cAppDlg.DoModal();  // 调用应用程序信息显示对话框
            return ;
        }
    }
    MessageBox("You have not select the application!");
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
int ThreadDownload(void *pTemp)
{
    int iRet;
    g_bThreadRunFlag = TRUE;
    g_pCLoadToolsDlg->m_PageApplication.MyEnableWindows(FALSE);
    iRet = g_pCLoadToolsDlg->m_PageApplication.DownloadStart();
    if (0 == iRet)
    {
        iRet = g_pCLoadToolsDlg->m_PageApplication.DownloadProcess();
        if (0 == iRet)
        {
            iRet = g_pCLoadToolsDlg->m_PageApplication.UpdateTreeList();
        }
        g_pCLoadToolsDlg->m_PageApplication.DownloadEnd();
    }
    else if (2 != iRet)
    {
        g_pCLoadToolsDlg->m_PageApplication.DownloadEnd();
    }
    g_pCLoadToolsDlg->m_PageApplication.MyEnableWindows(TRUE);
    g_bThreadRunFlag = FALSE;
    return 0;
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnButtonBeginDownloadApp() 
{
    // TODO: Add your control notification handler code here
    DWORD ThreadId;
    int i;

    
    if (m_bAppSelect)
    {
        i = CheckApplication(m_strAppFilePath.GetBuffer(0));
        switch (i)
        {
        case -1:
            MessageBox("The application is not ViewAt application");
            return;
        case -2:
            MessageBox("Open application file error!");
            return;
        case -3:
            MessageBox("Read application file error!");
            return;
        case -4:
            MessageBox("Application error----main entry address error");
            return;
        case -5:
            MessageBox("Application error----Envent entry address error");
            return;
        case -6:
            MessageBox("Application error----Application infomation entry address error");
            return;
        default:
            break;
        }
    }
    UpdateConfigure();  // 将界面上的配置保存在配置缓冲中
    SaveConfigInfo();   // 将配置缓冲中的配置保存到配置文件中
    m_bStopDownloadFlag = FALSE; 
    if (!g_bThreadRunFlag)
    {
        g_lHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadDownload, NULL, 0, &ThreadId);
        if (NULL == g_lHandle)
        {
            G_MyShowMessage(MSG_CREATE_THREAD_FAILED);
        }
    }
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnButtonStopDownloadApp() 
{
    // TODO: Add your control notification handler code here
    m_bStopDownloadFlag = TRUE;
    MyEnableWindows(TRUE);
}

/****************************************************************************
  函数名     :  void CPageApplication::OnButtonOpenAppFile() 
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-03   V1.0        创建
****************************************************************************/
void CPageApplication::OnButtonOpenAppFile() 
{
    // TODO: Add your control notification handler code here
    static char BASED_CODE szMergeFilter[] = _T("Open File (*.bin)|*.bin||)");
    CString filter;
    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, m_strAppFilePath,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, filter, 0);
    if (dlg.DoModal() == IDOK)
    {
        m_strAppFilePath = dlg.GetPathName();
        UpdateData(FALSE);
        InsertOneAppFilePath();
    }
}

/****************************************************************************
  函数名     :  void CPageApplication::InsertOneAppFilePath()
  描述       :  向配置和控件中插入一条应用程序路径
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-03   V1.0        创建
****************************************************************************/
void CPageApplication::InsertOneAppFilePath()
{
    int i, iCount, iRet;

    iCount = g_sConfigreInfo.m_byAppFilePathNumber;
    if (iCount > MAX_APP_PATH_NUMBER)
    {
        g_sConfigreInfo.m_byAppFilePathNumber = 0;
    }

    // 检查是否有相同的路径，如果有则选中相同的路径即可
    for (i=0; i<iCount; i++)
    {
        iRet = strcmp(&g_sConfigreInfo.m_strAppFilePath[i][0], 
            m_strAppFilePath.GetBuffer(0));
        if (0 == iRet)
        {
            m_cbAppFilePath.SetCurSel(i);
            g_sConfigreInfo.m_byAppPathCurrentSelect = i;
            return ;  // 检查到有相同的路径
        }
    }

    // 
    if (MAX_APP_PATH_NUMBER == iCount)
    {
        iCount = MAX_APP_PATH_NUMBER - 1;
    }
    for (i=iCount; i>0; i--)
    {
        strcpy(&g_sConfigreInfo.m_strAppFilePath[i][0], 
            &g_sConfigreInfo.m_strAppFilePath[i-1][0]);
    }

    strcpy(&g_sConfigreInfo.m_strAppFilePath[0][0], 
        m_strAppFilePath.GetBuffer(0));
    iCount++;
    g_sConfigreInfo.m_byAppFilePathNumber = iCount;
    m_cbAppFilePath.ResetContent();
    for (i=0; i<iCount; i++)
    {
        m_cbAppFilePath.AddString(&g_sConfigreInfo.m_strAppFilePath[i][0]);
    }
    m_cbAppFilePath.SetCurSel(0);
    g_sConfigreInfo.m_byAppPathCurrentSelect = 0;
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnButtonOpenFontFile() 
{
    // TODO: Add your control notification handler code here
    static char BASED_CODE szMergeFilter[] = _T("Open File (*.bin)|*.bin||)");
    CString filter;
    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, m_strFontFilePath,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, filter, 0);
    if (dlg.DoModal() == IDOK)
    {
        m_strFontFilePath = dlg.GetPathName();
        UpdateData(FALSE);
    }
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnButtonOpenBaleFile() 
{
    // TODO: Add your control notification handler code here
    static char BASED_CODE szMergeFilter[] = _T("Open File (*.bin)|*.bin||)");
    CString filter;
    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, m_strBaleFilePath,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, filter, 0);
    if (dlg.DoModal() == IDOK)
    {
        m_strBaleFilePath = dlg.GetPathName();
        UpdateData(FALSE);
    }
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnButtonOpenParamFile() 
{
    // TODO: Add your control notification handler code here
    static char BASED_CODE szMergeFilter[] = _T("Open File (*.env)|*.env||)");
    CString filter;
    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, m_strParamFilePath,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, filter, 0);
    if (dlg.DoModal() == IDOK)
    {
        m_strParamFilePath = dlg.GetPathName();
        UpdateData(FALSE);
    }
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnCheckAppSelect() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if (m_bBaleFileSelect)
    {
        if (m_bAppSelect)
        {
            m_bAppSelect = FALSE;
            UpdateData(FALSE);
            MessageBox("If you want to download application, you can't select the bale file!");
        }
    }
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnCheckBaleFile() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if (m_bBaleFileSelect)
    {
        m_bAppSelect = FALSE;
        m_bCreateFileSystemSelect = FALSE;
        m_bFontFileSelect = FALSE;
        m_bParamFileSelect = FALSE;
        m_bDataFileSelect = FALSE;
        UpdateData(FALSE);
    }
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnCheckCreateFileSystem() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if (m_bBaleFileSelect)
    {
        if (m_bCreateFileSystemSelect)
        {
            m_bCreateFileSystemSelect = FALSE;
            UpdateData(FALSE);
            MessageBox("If you want to recreate file system, you can't select the bale file!");
        }
    }
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnCheckFontFile() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if (m_bBaleFileSelect)
    {
        if (m_bFontFileSelect)
        {
            m_bFontFileSelect = FALSE;
            UpdateData(FALSE);
            MessageBox("If you want to download font, you can't select the bale file!");
        }
    }
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnCheckParamFile() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if (m_bBaleFileSelect)
    {
        if (m_bParamFileSelect)
        {
            m_bParamFileSelect = FALSE;
            UpdateData(FALSE);
            MessageBox("If you want to download param, you can't select the bale file!");
        }
    }
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnCheckSerialNumber() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnChangeEditBaleFilePath() 
{
    // TODO: If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnChangeEditFontFilePath() 
{
    // TODO: If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnChangeEditParamFilePath() 
{
    // TODO: If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    
}

/****************************************************************************
  函数名     :  void CPageApplication::OnChangeEditVposSerialNumber() 
  描述       :  响应用户修改序列号事件，主要功能是去掉用户输入非数字的字符
  输入参数   :  无
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-03   V1.0        创建
****************************************************************************/
void CPageApplication::OnChangeEditVposSerialNumber() 
{
    // TODO: If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    
    // TODO: Add your control notification handler code here
    char strTemp[40], strTempMsg[5];
    int i, iLen;

    UpdateData(TRUE);
    iLen = m_strVposSN.GetLength();

    strcpy(strTemp, m_strVposSN.GetBuffer(0));

    for (i=0; i<iLen; i++)
    {
        if ((strTemp[i] < '0') || (strTemp[i] > '9'))
        {
            // 禁止用户输入非数字的字符
            strTemp[i] = 0;
            m_strVposSN.Format(strTemp);
            UpdateData(FALSE);
            ((CEdit*)GetDlgItem(IDC_EDIT_VPOS_SERIAL_NUMBER))->SetSel(i, i);  // 设置当前选中位置
            break;
        }
    }
    sprintf(strTempMsg, "%d", strlen(strTemp));
    SetDlgItemText(IDC_STATIC_SN_LENGTH, strTempMsg);
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::MyShowMessage(char *strMsg)
{
    SetDlgItemText(IDC_STATIC_PAGE_APP_MESSAGE, strMsg);
}

/****************************************************************************
  函数名     :  void CPageApplication::OnEditchangeComboAppFilePath() 
  描述       :  响应用户编辑组合框事件，防止用户编辑组合框，恢复用户编辑前状态
  输入参数   :  无
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-03   V1.0        创建
****************************************************************************/
void CPageApplication::OnEditchangeComboAppFilePath() 
{
    // TODO: Add your control notification handler code here
    int i;

    m_cbAppFilePath.ResetContent();  // 清除所有内容
    for (i=0; i<g_sConfigreInfo.m_byAppFilePathNumber; i++)
    {
        m_cbAppFilePath.AddString(&g_sConfigreInfo.m_strAppFilePath[i][0]);
    }
    if (0 != g_sConfigreInfo.m_byAppFilePathNumber)
    {
        m_cbAppFilePath.SetCurSel(g_sConfigreInfo.m_byAppPathCurrentSelect);
    }
    UpdateData(FALSE);  // 恢复用户编辑前状态
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::OnSelchangeComboAppFilePath() 
{
    // TODO: Add your control notification handler code here
    int i;
    FILE *fp;

    i = m_cbAppFilePath.GetCurSel();
    fp = fopen(&g_sConfigreInfo.m_strAppFilePath[i][0], "rb");
    if (NULL == fp)
    {
        MessageBox("The application file that you select is not exist!");
        m_cbAppFilePath.SetCurSel(g_sConfigreInfo.m_byAppPathCurrentSelect);
        return;
    }
    fclose(fp);
    g_sConfigreInfo.m_byAppPathCurrentSelect = i;
    m_strAppFilePath.Format(&g_sConfigreInfo.m_strAppFilePath[i][0]);
}

/****************************************************************************
  函数名     :  void CPageApplication::StringAddOne(BYTE *pbyInData, BYTE *pbyOutData)
  描述       :  将输入的由数字组成的字符串加1，如果溢出则将其清零
  输入参数   :  BYTE *pbyInData:被转换的字符串
  输出参数   :  BYTE *pbyOutData：转换后的结果
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-03   V1.0        创建
****************************************************************************/
void CPageApplication::StringAddOne(BYTE *pbyInData, BYTE *pbyOutData)
{
    int i, iLen;

    iLen = strlen((char*)pbyInData);

    strcpy((char*)pbyOutData, (char*)pbyInData);
    pbyOutData[iLen-1]++;

    for (i=iLen-1; i>0; i--)
    {
        if (pbyOutData[i]>'9')
        {
            pbyOutData[i-1]++;
            pbyOutData[i] = '0';
        }
        else
        {
            return;
        }
    }
    if (pbyOutData[0] > '9')
    {
        pbyOutData[0] = '0';
    }
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
void CPageApplication::MyEnableWindows(BOOL bFlag)
{
    GetDlgItem(IDC_BUTTON_BEGIN_DOWNLOAD_APP)->EnableWindow(bFlag);
    GetDlgItem(IDC_BUTTON_STOP_DOWNLOAD_APP)->EnableWindow(!bFlag);
}


/****************************************************************************
  函数名     :  int CPageApplication::Handshack()
  描述       :  握手函数
  输入参数   :  无
  输出参数   :  无
  返回值     :  0：成功，1：握手失败，2：打开串口失败
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
int CPageApplication::Handshack()
{
    if (0 == g_sConfigreInfo.m_byCommType)
    {
        return SerialHandshack();
    }
    else
    {
        return UsbHandshack();
    }
}


int CPageApplication::UsbHandshack()
{
    BYTE abyRecvBuff[200];
    int iRet, i;

    m_bStopDownloadFlag = FALSE;

    if (FALSE == OpenUsbDevice())
    {
        G_MyShowMessage(MSG_OPEN_USB_PORT_FAILED);
        return 2;
    }

    // 握手
    G_MyShowMessage(MSG_CONNECTING);
    i = 0;
    for(;;)
    {
        if (m_bStopDownloadFlag)
        {
            G_MyShowMessage(MSG_USER_STOP_DOWNLOAD);
            m_bStopDownloadFlag = FALSE;
            return 1;
        }
        WriteUsbDevice((BYTE *)"L" , 1);
        abyRecvBuff[0] = 0;
        iRet = ReadUsbDevice(abyRecvBuff, 1, 20);
        if ((0==iRet) && (abyRecvBuff[0]=='A'))
        {
            return 0;
        }
        i++;
        if (i > 10000)
        {
            G_MyShowMessage(MSG_HANDSHACK_FAILED);
            return 1;
        }
    }
}

int CPageApplication::SerialHandshack()
{
    BYTE abyRecvBuff[200];
    WORD wRecvLen;
    int iRet, i;
    DWORD dwTemp;

    m_bStopDownloadFlag = FALSE;

    if (SerOpen(g_sConfigreInfo.m_iSerialPortNumber+1, 115200))
    {
        G_MyShowMessage(MSG_OPEN_SERIAL_PORT_FAILED);
        return 2;
    }

    // 握手
    G_MyShowMessage(MSG_CONNECTING);
    i = 0;
    for(;;)
    {
        if (m_bStopDownloadFlag)
        {
            G_MyShowMessage(MSG_USER_STOP_DOWNLOAD);
            m_bStopDownloadFlag = FALSE;
            return 1;
        }
        SerWrite((BYTE *)"L" , 1);
        abyRecvBuff[0] = 0;
        iRet = SerRead(abyRecvBuff, 1, 20);
        if ((0==iRet) && (abyRecvBuff[0]=='A'))
        {
            break;
        }
        i++;
        if (i > 10000)
        {
            G_MyShowMessage(MSG_HANDSHACK_FAILED);
            return 1;
        }
    }
    if (0 == g_sConfigreInfo.m_iSerialPortBaudsSelect)
    {
        return 0;
    }

    // 如果用户设置的波特率不是115200，则修改波特率
    G_MyShowMessage(MSG_CHANGE_BAUD);
    dwTemp = g_adwBauds[g_sConfigreInfo.m_iSerialPortBaudsSelect];
    abyRecvBuff[0] = (BYTE)((dwTemp/65536)%256);
    abyRecvBuff[1] = (BYTE)((dwTemp%65536)/256);
    abyRecvBuff[2] = (BYTE)(dwTemp%256);
    iRet = Crc16SendPacket(abyRecvBuff, 3, CMD_CHANGE_BAUD_REQUEST);
    if (SUCCESS != iRet)
    {
        G_MyShowMessage(MSG_SEND_FAILED);
        return 1;
    }
    iRet = Crc16RecvPacket(abyRecvBuff, &wRecvLen, 1000);
    if (SUCCESS != iRet)
    {
        G_MyShowMessage(MSG_RECEIVE_FAILED);
        return 1;
    }
    if ((8==wRecvLen) && (0==abyRecvBuff[5]))
    {
        Sleep(30);
        SerClose();
        Sleep(60);
        if (SerOpen(g_sConfigreInfo.m_iSerialPortNumber+1,
            g_adwBauds[g_sConfigreInfo.m_iSerialPortBaudsSelect]))
        {
            G_MyShowMessage(MSG_OPEN_SERIAL_PORT_FAILED);
            return 2;
        }
        return SUCCESS;
    }
    else
    {
        G_MyShowMessage(MSG_CHANGE_BAUD_FAILED);
        return 1;
    }
}

/****************************************************************************
  函数名     :  int CPageApplication::Authentication()
  描述       :  认证函数
  输入参数   :  无
  输出参数   :  无
  返回值     :  0：成功， 1：失败
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
int CPageApplication::Authentication()
{
    int iRet, iCount = 0;
    WORD wRecvLen;
    BYTE abyLRand[8], abyRecvBuff[500], abyLRand_E[8], abySendBuff[10];

    G_MyShowMessage(MSG_AUTHENTICATION);
    GetRandom(abyLRand);
    iCount = 3;
    while (iCount > 0)
    {
        iRet = Crc16SendPacket(abyLRand, 8, CMD_AUTHEN1_REQUEST);
        if (0 != iRet)
        {
            G_MyShowMessage(MSG_SEND_FAILED);
            return 1;
        }
        iRet = Crc16RecvPacket(abyRecvBuff, &wRecvLen, 1000);
        if ((0!=iRet) || (39!=wRecvLen))
        {
            G_MyShowMessage(MSG_RECEIVE_FAILED);
        }
        else
        {
            break;
        }
        iCount--;
    }
    des(abyLRand, abyLRand_E, g_abyAuthenKey, ENCRYPT);
    if (0 != memcmp(abyLRand_E,&abyRecvBuff[5],8))
    {
        G_MyShowMessage(MSG_AUTHENTICATION_ERROR);   // 认证失败
        abySendBuff[0] = 1;
        iRet = Crc16SendPacket(abySendBuff, 9, CMD_AUTHEN2_REQUEST);
        return 1;
    }
    abySendBuff[0] = 0;
    memcpy(m_abyVposType, &abyRecvBuff[21], 16);
    des(&abyRecvBuff[13], &abySendBuff[1], g_abyAuthenKey, ENCRYPT);
    iRet = Crc16SendPacket(abySendBuff, 9, CMD_AUTHEN2_REQUEST);
    if (0 != iRet)
    {
        G_MyShowMessage(MSG_SEND_FAILED);
        return 1;
    }
    iRet = Crc16RecvPacket(abyRecvBuff, &wRecvLen, 1000);
    if ((0!=iRet) || (8!=wRecvLen))
    {
        G_MyShowMessage(MSG_RECEIVE_FAILED);
        return 1;
    }
    if (0 != abyRecvBuff[5])
    {
        G_MyShowMessage(MSG_AUTHENTICATION_ERROR);   // 认证失败
        return 1;
    }
    return 0;
}

/****************************************************************************
  函数名     :  
  描述       :  文件下载处理函数
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
int CPageApplication::SetVposTime()
{
    SYSTEMTIME sTime;
    BYTE abyTempBuff[200];
    BYTE byTemp;
    int iRet, i;
    WORD wRecvLen;

    GetLocalTime(&sTime);
    if ((sTime.wYear<2008) || (sTime.wYear > 2099))
    {
        MyShowMessage("Read system time error, please change system time!");
        return 1;
    }
    byTemp = (BYTE)(sTime.wYear - 2000);
    abyTempBuff[0] = (byTemp/10)*0x10 + (byTemp%10);

    byTemp = (BYTE)(sTime.wMonth);
    abyTempBuff[1] = (byTemp/10)*0x10 + (byTemp%10);

    byTemp = (BYTE)(sTime.wDay);
    abyTempBuff[2] = (byTemp/10)*0x10 + (byTemp%10);

    byTemp = (BYTE)(sTime.wHour);
    abyTempBuff[3] = (byTemp/10)*0x10 + (byTemp%10);

    byTemp = (BYTE)(sTime.wMinute);
    abyTempBuff[4] = (byTemp/10)*0x10 + (byTemp%10);

    byTemp = (BYTE)(sTime.wSecond);
    abyTempBuff[5] = (byTemp/10)*0x10 + (byTemp%10);

    for (i=0; i<COMM_RETRY_TIMES; i++)
    {
        Crc16SendPacket(abyTempBuff, 7, CMD_SET_TIME_REQUEST);
        iRet = Crc16RecvPacket(abyTempBuff, &wRecvLen, 1000);
        if (0 == iRet)
        {
            return 0;
        }
    }
    return iRet;
}

/****************************************************************************
  函数名     :  
  描述       :  文件下载处理函数
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
int CPageApplication::DownloadProcess()
{
    BYTE abyTemp[256];
    WORD wRecvLen;
    int i, iRet;
    HTREEITEM hTreeItem;

    if (   m_bBaleFileSelect   || m_bCreateFileSystemSelect
        || m_bDownloadSNSelect || m_bFontFileSelect
        || m_bParamFileSelect  || m_bAppSelect
        || m_bSetTimeSelect    || m_bDataFileSelect)
    {
        if (m_bSetTimeSelect)
        {
            iRet = SetVposTime();
            if (0 != iRet)
            {
                return iRet;
            }
        }
        if (m_bBaleFileSelect)
        {
            return DownloadBaleFile();
        }
        if (m_bCreateFileSystemSelect)
        {
            MyShowMessage("Recreate file system...");
            Crc16SendPacket(abyTemp, 0, CMD_CREATE_FILE_SYSTEM_REQUEST);
            iRet = Crc16RecvPacket(abyTemp, &wRecvLen, 60000);
            if ((0 != iRet) || (0 != abyTemp[5]))
            {
                MyShowMessage("Recreate file system failed!");
                return iRet;
            }
            DownloadEnd();
            i = DownloadStart();
            if (0 != i)
            {
                MyShowMessage("Handshack error atfer create file system!");
                return i;
            }
        }
        if (m_bDownloadSNSelect)
        {
            MyShowMessage("Change serial number...");
            strcpy((char*)abyTemp, m_strVposSN.GetBuffer(0));
            Crc16SendPacket(abyTemp, 32, CMD_SET_SERIAL_NUMBER_REQUEST);
            i = Crc16RecvPacket(abyTemp, &wRecvLen, 1000);
            if (   (0==i)
                && (0==abyTemp[5]))
            {
                StringAddOne((BYTE*)m_strVposSN.GetBuffer(0), abyTemp);
                m_strVposSN.Format((char*)abyTemp);
                UpdateData(FALSE);
            }
            else
            {
                ;
            }
        }
        if (m_bFontFileSelect)
        {
            i = DownloadFile(m_strFontFilePath.GetBuffer(0), FILE_TYPE_FONT, 0);
            if (0 != i)
            {
                return i;
            }
        }
        if (m_bAppSelect)
        {
            i = DownloadFile(m_strAppFilePath.GetBuffer(0), FILE_TYPE_APPLICATION, 0);
            if (0 != i)
            {
                return i;
            }
        }
        if ((!m_bAppSelect) && (m_bParamFileSelect || m_bDataFileSelect))
        {
            hTreeItem = m_tcVposFileList.GetSelectedItem();
            if (NULL != hTreeItem)
            {
                for (i=0; i<MAX_APPLICATION_NUMBER; i++)
                {
                    if ( (0 != m_saAppInfo[i].m_byValidityFlag)
                        && (hTreeItem == m_saAppInfo[i].m_hTree))
                    {
                        m_iCurrentAppNo = m_saAppInfo[i].m_byAppNum;
                        break;
                    }
                }
                if (i >= MAX_APPLICATION_NUMBER)
                {
                    MessageBox("The file you select is not application!");
                    return 1;
                }
            }
            else
            {
                MessageBox("You have not selected a application, can't download param file!");
                return 1;
            }
        }
        if (m_bParamFileSelect)
        {
            i = DownloadFile(m_strParamFilePath.GetBuffer(0), FILE_TYPE_PARAM, 
                m_iCurrentAppNo);
            if (0 != i)
            {
                return i;
            }
        }
        if (m_bDataFileSelect)
        {
            i = DownloadFile(m_strDataFilePath.GetBuffer(0), FILE_TYPE_DATA, 
                m_iCurrentAppNo);
            if (0 != i)
            {
                return i;
            }
        }
        MyShowMessage("Download success!");
    }
    return 0;
}


/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-08   V1.0        创建
****************************************************************************/
int CPageApplication::DownloadFile(char *strFileName, BYTE byFileType, 
                                   BYTE byAppNo)
{
    FILE *fp;
    int iRet, iFileLen, iErrorCount, iReadLen, iSendPacketLen;
    char *p, str[100];
    BYTE abyTemp[DOWNLOAD_STEP_LEN+10], abyCompress[DOWNLOAD_STEP_LEN*2+100];
    WORD wPacketLen;
    DWORD dwCurrentLen, dwCompressLen;

    fp = fopen(strFileName, "rb");
    if (NULL == fp)
    {
        MessageBox("Can't open file!");
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    iFileLen = ftell(fp);
    if (iFileLen <= 0)
    {
        MessageBox("File size error!");
        fclose(fp);
        return 1;
    }
    switch (byFileType)
    {
    case FILE_TYPE_APPLICATION:
        m_iCurrentAppNo = -1;
        if (iFileLen > (1024*1024))
        {
            MessageBox("File size must less then 1M bytes");
            fclose(fp);
            return 1;
        }
        if (iFileLen < (10*1024))
        {
            MessageBox("The file is not ViewAt application file!");
            fclose(fp);
            return 1;
        }
        strcpy(str, "Application...");
        break;
    case FILE_TYPE_PARAM:
        if (iFileLen > (128*1024))
        {
            MessageBox("The ENV param file size too big!");
            fclose(fp);
            return 1;
        }
        if (0 != (iFileLen%128))
        {
            MessageBox("The file is not ENV param file!");
            fclose(fp);
            return 1;
        }
        strcpy(str, "Param file....");
        break;
    case FILE_TYPE_DATA:
        if (iFileLen > (1560*1024))
        {
            MessageBox("The data file size too big!");
            fclose(fp);
            return 1;
        }
        strcpy(str, "Data file.....");
        break;
    case FILE_TYPE_FONT:
        if (iFileLen > (960*1024))
        {
            MessageBox("The file too big, is not ViewAt font file");
            fclose(fp);
            return 1;
        }
        if (iFileLen < (4*1024))
        {
            MessageBox("This file too small, is not ViewAt font file.");
            fclose(fp);
            return 1;
        }
        fseek(fp, 0, SEEK_SET);
        fread(abyTemp, 1, 20, fp);
        if (  (0!=memcmp(abyTemp,"ViewAt Font",11))
            && (0!=memcmp(abyTemp,"VIEWAT_COMPRESS_FONT",20)))
        {
            MessageBox("The file isn't ViewAt font file!");
            fclose(fp);
            return 3;
        }
        strcpy(str, "Font file.....");
        break;
    default:
        fclose(fp);
        MyShowMessage("File type error!");
        return 4;
    }
    memset(abyTemp, 0, 38);
    abyTemp[0] = byFileType;
    abyTemp[1] = 0;
    abyTemp[2] = (BYTE)((iFileLen/65536)%256);
    abyTemp[3] = (BYTE)((iFileLen%65536)/256);
    abyTemp[4] = (BYTE)(iFileLen%256);
    if (FILE_TYPE_DATA == byFileType)
    {
        abyTemp[5] = byAppNo;
        p = strrchr(strFileName, '\\');
        if (NULL==p)
        {
            strcpy((char*)&abyTemp[6], strFileName);
        }
        else
        {
            p++;
            strcpy((char*)&abyTemp[6], p);
        }
        abyTemp[22] = 0;
    }
    else if (FILE_TYPE_PARAM == byFileType)
    {
        abyTemp[5] = byAppNo;
        strcpy((char*)&abyTemp[6], "EnvFile");
        abyTemp[22] = 0;
    }
    iRet = Crc16SendPacket(abyTemp, 38, CMD_LOAD_FILE_REQUEST);
    if (0 != iRet)
    {
        fclose(fp);
        MyShowMessage("Send failed!");
        return 6;
    }
    iRet = Crc16RecvPacket(abyTemp, &wPacketLen, 1000);
    if ((0!=iRet) || (0!=abyTemp[5]))
    {
        fclose(fp);
        MyShowMessage("Receive failed!");
        return 5;
    }

    dwCurrentLen = 0;
    iErrorCount = 0;
    fseek(fp, 0, SEEK_SET);
    while (1)
    {
        if (dwCurrentLen >= (DWORD)iFileLen)
        {
            break;
        }
        sprintf(&str[14], "%d%%", (dwCurrentLen*100)/iFileLen);
        MyShowMessage(str);
        wPacketLen = DOWNLOAD_STEP_LEN;
        if ((dwCurrentLen+DOWNLOAD_STEP_LEN) > (DWORD)iFileLen)
        {
            wPacketLen = (WORD)(iFileLen - dwCurrentLen);
        }
        iReadLen = fread(&abyTemp[5], 1, wPacketLen, fp);
        if (iReadLen > 0)
        {
            abyTemp[0] = 0;
            abyTemp[1] = (BYTE)((dwCurrentLen/65536)%256);
            abyTemp[2] = (BYTE)((dwCurrentLen%65536)/256);
            abyTemp[3] = (BYTE)(dwCurrentLen%256);
            (void)Compress(&abyTemp[5], abyCompress, (DWORD)iReadLen, &dwCompressLen);
            if ((DWORD)iReadLen > dwCompressLen)
            {
                abyTemp[4] = 1;   // data zip
                abyTemp[5] = (BYTE)(iReadLen/256);
                abyTemp[6] = (BYTE)(iReadLen%256);
                iSendPacketLen = dwCompressLen+7;
                memcpy(&abyTemp[7], abyCompress, dwCompressLen);
            }
            else
            {
                abyTemp[4] = 0;  // data no zip
                iSendPacketLen = iReadLen+5;
            }
            iErrorCount = 0;
            while (1)
            {
                (void)Crc16SendPacket(abyTemp, (WORD)iSendPacketLen, 
                    CMD_LOAD_FILE_DATA_REQUEST);
                iRet = Crc16RecvPacket(abyCompress, &wPacketLen, 1000);
                if ((0==iRet) && (0==abyCompress[5]))
                {
                    break;
                }
                iErrorCount++;
                if (iErrorCount > 3)
                {
                    fclose(fp);
                    if (RECV_TIMEOUT == iRet)
                    {
                        MyShowMessage("Receive timeout!");
                    }
                    else
                    {
                        MyShowMessage("Receive failed!");
                    }
                    return 5;
                }
            }
            dwCurrentLen += iReadLen;
        }
        else
        {
            break;
        }
    }  // end while (1)
    MyShowMessage("Saving file...");
    Crc16SendPacket(abyTemp, 0, CMD_SAVE_FILE_REQUEST);
    iRet = Crc16RecvPacket(abyTemp, &wPacketLen, 30000);
    if ((0!=iRet) || (0!=abyTemp[5]))
    {
        fclose(fp);
        if (RECV_TIMEOUT == iRet)
        {
            MyShowMessage("Receive timeout!");
        }
        else
        {
            MyShowMessage("Receive failed!");
        }
        return 5;
    }
    if (FILE_TYPE_APPLICATION == byFileType)
    {
        m_iCurrentAppNo = abyTemp[6];
    }
    fclose(fp);
    MyShowMessage("Success!");
    return 0;
}

void CPageApplication::OnCheckSetTime() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-11   V1.0        创建
****************************************************************************/
void CPageApplication::OnButtonDeleteAppendFile() 
{
    // TODO: Add your control notification handler code here
    HTREEITEM hSelItem;
    int i;
    DWORD ThreadId;

    hSelItem = m_tcVposFileList.GetSelectedItem();
    if (NULL == hSelItem)
    {
        MessageBox("You have not selected a application!");
        return ;
    }

    for (i=0; i<MAX_APPLICATION_NUMBER; i++)
    {
        if (  (0!=m_saAppInfo[i].m_byValidityFlag)
            &&(hSelItem==m_saAppInfo[i].m_hTree))
        {
            m_bStopDownloadFlag = FALSE; 
            if (!g_bThreadRunFlag)
            {
                g_abyTempSend[0] = m_saAppInfo[i].m_sAppInfo.m_byAppNum;
                g_abyTempSend[1] = 1;
                g_lHandle = CreateThread(NULL, 0, 
                    (LPTHREAD_START_ROUTINE)ThreadDeleteFile, g_abyTempSend, 
                    0, &ThreadId);
                if (NULL == g_lHandle)
                {
                    G_MyShowMessage(MSG_CREATE_THREAD_FAILED);
                }
            }
            else
            {
                MessageBox("There is a thread in running, can't delete application, retry again after.");
            }
            return ;
        }
    }
    MessageBox("You have not selected a application!");
}


void CPageApplication::OnButtonOpenDataFile() 
{
    // TODO: Add your control notification handler code here
    static char BASED_CODE szMergeFilter[] = _T("Open File (*.*)|*.*||)");
    CString filter;
    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, m_strDataFilePath,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, filter, 0);
    if (dlg.DoModal() == IDOK)
    {
        m_strDataFilePath = dlg.GetPathName();
        UpdateData(FALSE);
    }
}

void CPageApplication::OnCheckDataFile() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if (m_bBaleFileSelect)
    {
        if (m_bDataFileSelect)
        {
            m_bDataFileSelect = FALSE;
            UpdateData(FALSE);
            MessageBox("If you want to download data file, you can't select bale file!");
        }
    }
}

void CPageApplication::OnChangeEditDataFilePath() 
{
    // TODO: If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
}

int CPageApplication::DownloadBaleFile()
{
    BYTE byTaskNum, 
        abyIndex[1200], 
        abyTempBuff[2048];
    int iRet, i, iCreateFileSystemFlag = 0;
    FILE *fp;
    DWORD dwTemp;
    WORD wRecvLen;
    
    fp = fopen(m_strBaleFilePath.GetBuffer(0), "rb");
    if (NULL == fp)
    {
        MessageBox("Can't open bale file!");
        return 1;
    }
    memset(abyIndex, 0, sizeof(abyIndex));
    fread(abyIndex, 1, 16, fp);
    if (0 != strcmp((char*)abyIndex, "ViewAt BaleFile"))
    {
        fclose(fp);
        MessageBox("The file isn't bale file!");
        return 1;
    }
    fseek(fp, -1, SEEK_END);
    fread(abyIndex, 1, 1, fp);
    byTaskNum = abyIndex[0];
    fseek(fp, -(byTaskNum*4+1), SEEK_END);
    iRet = fread(abyIndex, 1, byTaskNum*4, fp);
    if (iRet != (byTaskNum*4))
    {
        fclose(fp);
        MessageBox("Read bale file error!");
        return 1;
    }
    for (i=0; i<byTaskNum; i++)
    {
        dwTemp = (abyIndex[i*4]*256+abyIndex[i*4+1])*65536
            + abyIndex[i*4+2]*256 + abyIndex[i*4+3];
        fseek(fp, dwTemp, SEEK_SET);
        iRet = fread(abyTempBuff, 1, 1, fp);
        if (1 != iRet)
        {
            fclose(fp);
            MessageBox("Read bale file error!");
            return 1;
        }
        switch (abyTempBuff[0])
        {
        case BALE_TASK_CREATE_FILE:
            iRet = fread(abyTempBuff, 1, 8, fp);
            if (8 != iRet)
            {
                fclose(fp);
                MessageBox("Read bale file error!");
                return 1;
            }
            if (0 == memcmp(abyTempBuff,"81921170",8))
            {
                MyShowMessage("Recreate file system...");
                Crc16SendPacket(abyTempBuff, 0, CMD_CREATE_FILE_SYSTEM_REQUEST);
                (void)Crc16RecvPacket(abyTempBuff, &wRecvLen, 30000);
                DownloadEnd();
                iRet = DownloadStart();
                if (0 != iRet)
                {
                    fclose(fp);
                    MyShowMessage("Download success!");
                    return 0;
                }            
                iCreateFileSystemFlag = 1;
            }
            break;
        case BALE_TASK_DEL_APP:
            if (iCreateFileSystemFlag)
            {
                break;
            }
            MyShowMessage("Delete application...");
            memset(abyTempBuff, 0, 33);
            iRet = fread(abyTempBuff, 1, 32, fp);
            if (32 == iRet)
            {
                abyTempBuff[32] = 0;
                iRet = Crc16SendPacket(abyTempBuff, 33, CMD_DELETE_FILE_REQUEST);
                if (0 != iRet)
                {
                    MessageBox("Send file failed!");
                    return 1;
                }
                iRet = Crc16RecvPacket(abyTempBuff, &wRecvLen, 1000);
                if (0 != iRet)
                {
                    MessageBox("Receive response packet failed when delete application!");
                    return 1;
                }
            }
            else
            {
                fclose(fp);
                MessageBox("Read bale file error!");
                return 1;
            }
            break;
        case BALE_TASK_APPLICATION:
            iRet = ParseBaleFile(fp, dwTemp+1, FILE_TYPE_APPLICATION);
            if (0 != iRet)
            {
                fclose(fp);
                return 1;
            }
            break;
        case BALE_TASK_PARAM:
            iRet = ParseBaleFile(fp, dwTemp+1, FILE_TYPE_PARAM);
            if (0 != iRet)
            {
                fclose(fp);
                return 1;
            }
            break;
        case BALE_TASK_DATA:
            iRet = ParseBaleFile(fp, dwTemp+1, FILE_TYPE_DATA);
            if (0 != iRet)
            {
                fclose(fp);
                return 1;
            }
            break;
        case BALE_TASK_FONT:
            iRet = ParseBaleFile(fp, dwTemp+1, FILE_TYPE_FONT);
            if (0 != iRet)
            {
                fclose(fp);
                return 1;
            }
            break;
        default:
            fclose(fp);
            MessageBox("Task type error!");
            return 1;
        }
    }


    fclose(fp);
    return 0;
}

int CPageApplication::ParseBaleFile(FILE *fp, DWORD dwOffset, BYTE byFileType)
{
    BYTE abyTemp[2048];
    int iRet;
    DWORD dwUnzipLen, dwCurLen, dwReadLen;
    char strTempDir[300];
    FILE *fpTemp;

    GetCurrentDirectory(300, strTempDir);

    fseek(fp, dwOffset, SEEK_SET);
    switch (byFileType)
    {
    case FILE_TYPE_APPLICATION:
        iRet = fread(abyTemp, 1, 6, fp);
        if (6 != iRet)
        {
            return 1;
        }
        if (0 == abyTemp[1])
        {
            dwUnzipLen = (abyTemp[2]*256+abyTemp[3])*65536
                + abyTemp[4]*256 + abyTemp[5];
            if (dwUnzipLen > (1536*1024))
            {
                (void)MessageBox("Parse file failed, the application file size to long!");
                return 1;
            }
            strcat(strTempDir, "\\Temp.hjb");
            DeleteFile(strTempDir);
            fpTemp = fopen(strTempDir, "wb");
            if (NULL == fpTemp)
            {
                (void)MessageBox("Create file fault!");
                return 1;
            }
            dwCurLen = 0;
            while (1)
            {
                if (dwCurLen >= dwUnzipLen)
                {
                    fclose(fpTemp);
                    break;
                }
                dwReadLen = 2048;
                if ((dwCurLen+2048) > dwUnzipLen)
                {
                    dwReadLen = dwUnzipLen - dwCurLen;
                }
                iRet = fread(abyTemp, 1, dwReadLen, fp);
                if (iRet > 0)
                {
                    fwrite(abyTemp, 1, iRet, fpTemp);
                    dwCurLen += iRet;
                }
                else
                {
                    fclose(fpTemp);
                    break;
                }
            }
            m_iCurrentAppNo = -1;
            iRet = DownloadFile(strTempDir, FILE_TYPE_APPLICATION, 0);
            DeleteFile(strTempDir);
            return iRet;
        }
        (void)MessageBox("Parse file failed!");
        return 1;
    case FILE_TYPE_PARAM:
    case FILE_TYPE_DATA:
        if (   (m_iCurrentAppNo<0)
            && (m_iCurrentAppNo>24))
        {
            (void)MessageBox("The application number error!");
            return 1;
        }
        iRet = fread(abyTemp, 1, 21, fp);
        if (21 != iRet)
        {
            (void)MessageBox("Read file failed!");
            return 1;
        }
        if (0 == abyTemp[16])
        {
            abyTemp[15] = 0;
            dwUnzipLen = (abyTemp[17]*256+abyTemp[18])*65536
                + abyTemp[19]*256 + abyTemp[20];
            strcat(strTempDir, "\\");
            strcat(strTempDir, (char*)abyTemp);
            DeleteFile(strTempDir);
            fpTemp = fopen(strTempDir, "wb");
            if (NULL == fpTemp)
            {
                (void)MessageBox("Create file fault!");
                return 1;
            }
            dwCurLen = 0;
            while (1)
            {
                if (dwCurLen >= dwUnzipLen)
                {
                    fclose(fpTemp);
                    break;
                }
                dwReadLen = 2048;
                if ((dwCurLen+2048) > dwUnzipLen)
                {
                    dwReadLen = dwUnzipLen - dwCurLen;
                }
                iRet = fread(abyTemp, 1, dwReadLen, fp);
                if (iRet > 0)
                {
                    fwrite(abyTemp, 1, iRet, fpTemp);
                    dwCurLen += iRet;
                }
                else
                {
                    fclose(fpTemp);
                    break;
                }
            }
            iRet = DownloadFile(strTempDir, byFileType, m_iCurrentAppNo);
            DeleteFile(strTempDir);
            return iRet;
        }
        (void)MessageBox("Parse file failed!");
        return 1;
    case FILE_TYPE_FONT:
        iRet = fread(abyTemp, 1, 6, fp);
        if (6 != iRet)
        {
            (void)MessageBox("Read file failed!");
            return 1;
        }
        if (0 == abyTemp[1])
        {
            dwUnzipLen = (abyTemp[2]*256+abyTemp[3])*65536
                + abyTemp[4]*256 + abyTemp[5];
            if (dwUnzipLen > (960*1024))
            {
                (void)MessageBox("Parse bale file failed!");
                return 1;
            }
            strcat(strTempDir, "\\Temp.hjb");
            DeleteFile(strTempDir);
            fpTemp = fopen(strTempDir, "wb");
            if (NULL == fpTemp)
            {
                (void)MessageBox("Open file fault!");
                return 1;
            }
            dwCurLen = 0;
            while (1)
            {
                if (dwCurLen >= dwUnzipLen)
                {
                    fclose(fpTemp);
                    break;
                }
                dwReadLen = 2048;
                if ((dwCurLen+2048) > dwUnzipLen)
                {
                    dwReadLen = dwUnzipLen - dwCurLen;
                }
                iRet = fread(abyTemp, 1, dwReadLen, fp);
                if (iRet > 0)
                {
                    fwrite(abyTemp, 1, iRet, fpTemp);
                    dwCurLen += iRet;
                }
                else
                {
                    fclose(fpTemp);
                    break;
                }
            }
            iRet = DownloadFile(strTempDir, FILE_TYPE_FONT, 0);
            DeleteFile(strTempDir);
            return iRet;
        }
        (void)MessageBox("Parse file failed!");
        return 1;
    default:
        (void)MessageBox("Input file type error!");
        return 1;
    }
}

void CPageApplication::OnDblclkTreeFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // TODO: Add your control notification handler code here
    OnButtonShowAppInfo();
    *pResult = 0;
}
