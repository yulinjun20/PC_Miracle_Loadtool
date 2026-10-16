// PageApplication.cpp : implementation file
//

#include "stdafx.h"
#include "LoadTools.h"
#include "PageApplication.h"
#include "EnvJson.h"
#include <stdio.h>
#define WM_FACTORY_UI_UPDATE  (WM_USER + 210)
#define WM_FACTORY_UI_ENABLE (WM_USER + 211)
#define WM_FACTORY_SCAN_MSG  (WM_USER + 212)
#define FAC_HS_NONE  0
#define FAC_HS_LA    1
#define FAC_HS_TS    2
/* Per-worker factory list slot for live Progress/Status. */
static __declspec(thread) int t_iFactoryUiSlot = -1;
extern DWORD g_adwBauds[10];
extern BOOL g_bThreadRunFlag;
extern int g_iCommCheckType;

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

HANDLE g_lHandle = NULL;  // ?????

extern CONFIG_INFO g_sConfigreInfo;
extern DWORD g_adwBauds[10];

extern void UpdateConfigure(void);

extern void G_MyShowMessage(int iMessageIndex);
/* CheckApplication not used for App manage download. */

BYTE g_abyTempSend[200];

DWORD g_adwBauds[10] = {115200, 57600, 38400, 28800, 19200, 14400, 9600, 230400, 460800, 921600};

BYTE g_abyAuthenKey[8] = {0x33, 0x00, 0x22, 0xf1, 0xc4, 0x3d, 0x25, 0x5e};

extern APPLICATION_INFO g_sMyAppInfo;  // ?????CShowAppInfo????????????????????

// ??????????LoadTools???????????????????��??????????LoadTools??????
extern CLoadToolsDlg *g_pCLoadToolsDlg;

extern int g_iCommCheckType;


/////////////////////////////////////////////////////////////////////////////
// CPageApplication property page

IMPLEMENT_DYNCREATE(CPageApplication, CPropertyPage)

CPageApplication::CPageApplication() : CPropertyPage(CPageApplication::IDD)
{
    m_iDownloadMode = DL_MODE_DEVELOPER;
    m_iProgressTotal = 0;
    m_iProgressDone = 0;
    m_bFactoryPagesHidden = FALSE;
    m_nFactoryActive = 0;
    m_bFactoryStopScan = FALSE;
	m_nFactoryDonePorts = 0;
    memset(m_factorySlots, 0, sizeof(m_factorySlots));
    InitializeCriticalSection(&m_csFactory);

    m_bStopDownloadFlag = FALSE;
    m_iCurrentAppNo = 0;
    //{{AFX_DATA_INIT(CPageApplication)
    m_strBaleFilePath = _T("");
    m_strFontFilePath = _T("");
    m_strParamFilePath = _T("");
    m_strVosFilePath = _T("");
    m_bAppSelect = FALSE;
    m_bBaleFileSelect = FALSE;
    m_bCreateFileSystemSelect = FALSE;
    m_bFontFileSelect = FALSE;
    m_bParamFileSelect = FALSE;
    m_bSetTimeSelect = FALSE;
    m_strDataFilePath = _T("");
    m_bDataFileSelect = FALSE;
    m_strScAppFilePath = _T("");
    m_strWifiFilePath = _T("");
    m_strLogoFilePath = _T("");
    m_bScAppSelect = FALSE;
    m_bWifiSelect = FALSE;
    m_bLogoSelect = FALSE;
    m_bVOSSelect = FALSE;
    //}}AFX_DATA_INIT
}

CPageApplication::~CPageApplication()
{
    DeleteCriticalSection(&m_csFactory);
}

void CPageApplication::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPageApplication)
    DDX_Control(pDX, IDC_TREE_FILE_LIST, m_tcVposFileList);
    DDX_Text(pDX, IDC_EDIT_APP_FILE_PATH, m_strAppFilePath);
    DDX_Text(pDX, IDC_EDIT_BALE_FILE_PATH, m_strBaleFilePath);
    DDX_Text(pDX, IDC_EDIT_FONT_FILE_PATH, m_strFontFilePath);
    DDX_Text(pDX, IDC_EDIT_PARAM_FILE_PATH, m_strParamFilePath);
    DDX_Check(pDX, IDC_CHECK_APP_SELECT, m_bAppSelect);
    DDX_Check(pDX, IDC_CHECK_BALE_FILE, m_bBaleFileSelect);
    DDX_Check(pDX, IDC_CHECK_FONT_FILE, m_bFontFileSelect);
    DDX_Check(pDX, IDC_CHECK_PARAM_FILE, m_bParamFileSelect);
    DDX_Check(pDX, IDC_CHECK_SET_TIME, m_bSetTimeSelect);
    DDX_Text(pDX, IDC_EDIT_DATA_FILE_PATH, m_strDataFilePath);
    DDX_Check(pDX, IDC_CHECK_DATA_FILE, m_bDataFileSelect);
    DDX_Text(pDX, IDC_EDIT_VOS_FILE_PATH, m_strVosFilePath);
    DDX_Check(pDX, IDC_CHECK_VOS_SELECT, m_bVOSSelect);
    DDX_Text(pDX, IDC_EDIT_SC_APP_FILE_PATH, m_strScAppFilePath);
    DDX_Check(pDX, IDC_CHECK_SC_APP_FILE, m_bScAppSelect);
    DDX_Text(pDX, IDC_EDIT_WIFI_FILE_PATH, m_strWifiFilePath);
    DDX_Check(pDX, IDC_CHECK_WIFI_FILE, m_bWifiSelect);
    DDX_Text(pDX, IDC_EDIT_LOGO_FILE_PATH, m_strLogoFilePath);
    DDX_Check(pDX, IDC_CHECK_LOGO_FILE, m_bLogoSelect);

    DDX_Control(pDX, IDC_BUTTON_BEGIN_DOWNLOAD_APP, m_btnDownload);
    DDX_Control(pDX, IDC_BUTTON_STOP_DOWNLOAD_APP, m_btnStop);

    DDX_Control(pDX, IDC_BUTTON_REFURBISH, m_btnReflush);
    DDX_Control(pDX, IDC_BUTTON_DELETE_APP, m_btnDeleteApp);
    DDX_Control(pDX, IDC_BUTTON_DELETE_APPEND_FILE, m_btnDeleteAppendFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_APP_FILE, m_btnOpenAppFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_FONT_FILE, m_btnOpenFontFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_BALE_FILE, m_btnOpenBaleFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_PARAM_FILE, m_btnOpenParamFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_DATA_FILE, m_btnOpenDataFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_VOS_FILE, m_btnOpenVosFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_SC_APP_FILE, m_btnOpenScAppFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_WIFI_FILE, m_btnOpenWifiFile);
    DDX_Control(pDX, IDC_BUTTON_OPEN_LOGO_FILE, m_btnOpenLogoFile);
    DDX_Control(pDX, IDC_LIST_FACTORY_STATUS, m_listFactoryStatus);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageApplication, CPropertyPage)
    //{{AFX_MSG_MAP(CPageApplication)
    ON_BN_CLICKED(IDC_BUTTON_REFURBISH, OnButtonRefurbish)
    ON_BN_CLICKED(IDC_BUTTON_DELETE_APP, OnButtonDeleteApp)
    ON_BN_CLICKED(IDC_BUTTON_DELETE_APPEND_FILE, OnButtonDeleteAppendFile)
    ON_BN_CLICKED(IDC_BUTTON_BEGIN_DOWNLOAD_APP, OnButtonBeginDownloadApp)
    ON_BN_CLICKED(IDC_BUTTON_STOP_DOWNLOAD_APP, OnButtonStopDownloadApp)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_APP_FILE, OnButtonOpenAppFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_FONT_FILE, OnButtonOpenFontFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_BALE_FILE, OnButtonOpenBaleFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_PARAM_FILE, OnButtonOpenParamFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_DATA_FILE, OnButtonOpenDataFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_VOS_FILE, OnButtonOpenVosFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_SC_APP_FILE, OnButtonOpenScAppFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_WIFI_FILE, OnButtonOpenWifiFile)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_LOGO_FILE, OnButtonOpenLogoFile)
    ON_BN_CLICKED(IDC_CHECK_APP_SELECT, OnCheckAppSelect)
    ON_BN_CLICKED(IDC_CHECK_BALE_FILE, OnCheckBaleFile)
    ON_BN_CLICKED(IDC_CHECK_CREATE_FILE_SYSTEM, OnCheckCreateFileSystem)
    ON_BN_CLICKED(IDC_CHECK_FONT_FILE, OnCheckFontFile)
    ON_BN_CLICKED(IDC_CHECK_PARAM_FILE, OnCheckParamFile)
    ON_BN_CLICKED(IDC_CHECK_SET_TIME, OnCheckSetTime)
    ON_BN_CLICKED(IDC_CHECK_DATA_FILE, OnCheckDataFile)
    ON_BN_CLICKED(IDC_CHECK_SC_APP_FILE, OnCheckScAppFile)
    ON_BN_CLICKED(IDC_CHECK_WIFI_FILE, OnCheckWifiFile)
    ON_BN_CLICKED(IDC_CHECK_LOGO_FILE, OnCheckLogoFile)
    ON_BN_CLICKED(IDC_RADIO_MODE_DEVELOPER, OnRadioModeDeveloper)
    ON_BN_CLICKED(IDC_RADIO_MODE_FACTORY, OnRadioModeFactory)
    ON_MESSAGE(WM_FACTORY_UI_UPDATE, OnFactoryUiUpdate)
    ON_MESSAGE(WM_FACTORY_UI_ENABLE, OnFactoryUiEnable)
    ON_MESSAGE(WM_FACTORY_SCAN_MSG, OnFactoryScanMsg)
    ON_BN_CLICKED(IDC_CHECK_VOS_SELECT, OnCheckVosSelect)
    ON_EN_CHANGE(IDC_EDIT_BALE_FILE_PATH, OnChangeEditBaleFilePath)
    ON_EN_CHANGE(IDC_EDIT_FONT_FILE_PATH, OnChangeEditFontFilePath)
    ON_EN_CHANGE(IDC_EDIT_PARAM_FILE_PATH, OnChangeEditParamFilePath)
    ON_EN_CHANGE(IDC_EDIT_APP_FILE_PATH, OnChangeEditAppFilePath)
    ON_EN_CHANGE(IDC_EDIT_DATA_FILE_PATH, OnChangeEditDataFilePath)
    ON_EN_CHANGE(IDC_EDIT_SC_APP_FILE_PATH, OnChangeEditScAppFilePath)
    ON_EN_CHANGE(IDC_EDIT_WIFI_FILE_PATH, OnChangeEditWifiFilePath)
    ON_EN_CHANGE(IDC_EDIT_LOGO_FILE_PATH, OnChangeEditLogoFilePath)
    ON_EN_CHANGE(IDC_EDIT_VOS_FILE_PATH, OnChangeEditVosFilePath)
    ON_NOTIFY(NM_DBLCLK, IDC_TREE_FILE_LIST, OnDblclkTreeFileList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageApplication message handlers

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
BOOL CPageApplication::OnInitDialog()
{
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
        g_sConfigreInfo.m_byScAppSelect = 0;
        g_sConfigreInfo.m_byWifiSelect = 0;
        g_sConfigreInfo.m_byLogoSelect = 0;
    }
    m_bAppSelect              = g_sConfigreInfo.m_byApplicationSelect;
    m_bFontFileSelect         = g_sConfigreInfo.m_byFontSelect;
    m_bParamFileSelect        = g_sConfigreInfo.m_byParamSelect;
    g_sConfigreInfo.m_byChangeSNSelect = 0;  /* SN removed */
    m_bBaleFileSelect         = g_sConfigreInfo.m_byBaleFileSelect;
    m_bCreateFileSystemSelect = FALSE;
    g_sConfigreInfo.m_byCreateFileSystemSelect = 0;
    m_bSetTimeSelect          = g_sConfigreInfo.m_bySetTimeSelect;
    m_bDataFileSelect         = g_sConfigreInfo.m_byDataFileSelect;
    m_bScAppSelect            = g_sConfigreInfo.m_byScAppSelect;
    m_bWifiSelect             = g_sConfigreInfo.m_byWifiSelect;
    m_bLogoSelect             = g_sConfigreInfo.m_byLogoSelect;

    /* App path: remember single last path (slot 0). */
    if (g_sConfigreInfo.m_byAppFilePathNumber > MAX_APP_PATH_NUMBER)
    {
        g_sConfigreInfo.m_byAppFilePathNumber = 0;
    }
    if (0 != g_sConfigreInfo.m_byAppFilePathNumber
        && g_sConfigreInfo.m_byAppPathCurrentSelect < g_sConfigreInfo.m_byAppFilePathNumber)
    {
        m_strAppFilePath.Format("%s",
            &g_sConfigreInfo.m_strAppFilePath[g_sConfigreInfo.m_byAppPathCurrentSelect][0]);
        /* Collapse history to one last path. */
        if (0 != g_sConfigreInfo.m_byAppPathCurrentSelect)
        {
            memcpy(&g_sConfigreInfo.m_strAppFilePath[0][0],
                &g_sConfigreInfo.m_strAppFilePath[g_sConfigreInfo.m_byAppPathCurrentSelect][0],
                256);
        }
        memset(&g_sConfigreInfo.m_strAppFilePath[1][0], 0,
            sizeof(g_sConfigreInfo.m_strAppFilePath) - 256);
        g_sConfigreInfo.m_byAppFilePathNumber = 1;
        g_sConfigreInfo.m_byAppPathCurrentSelect = 0;
    }
    else
    {
        m_strAppFilePath.Format("");
        memset(g_sConfigreInfo.m_strAppFilePath, 0, sizeof(g_sConfigreInfo.m_strAppFilePath));
        g_sConfigreInfo.m_byAppFilePathNumber = 0;
        g_sConfigreInfo.m_byAppPathCurrentSelect = 0;
    }

    m_strBaleFilePath.Format(g_sConfigreInfo.m_strBaleFilePath);
    m_strFontFilePath.Format(g_sConfigreInfo.m_strFontFilePath);
    m_strParamFilePath.Format(g_sConfigreInfo.m_strParamFilePath);
    m_strDataFilePath.Format(g_sConfigreInfo.m_strDataFilePath);
    m_strVosFilePath.Format(g_sConfigreInfo.m_strVOSFilePath);
    m_strScAppFilePath.Format(g_sConfigreInfo.m_strScAppFilePath);
    m_strWifiFilePath.Format(g_sConfigreInfo.m_strWifiFilePath);
    m_strLogoFilePath.Format(g_sConfigreInfo.m_strLogoFilePath);
    m_bVOSSelect = g_sConfigreInfo.m_byVOSSelect;

    g_sConfigreInfo.m_strSerialNumber[0] = 0;

    /* Sync time optional; default off. */
    m_bSetTimeSelect = FALSE;
    g_sConfigreInfo.m_bySetTimeSelect = 0;
    
    UpdateData(FALSE);

    GetDlgItem(IDC_BUTTON_STOP_DOWNLOAD_APP)->EnableWindow(FALSE);
    GetDlgItem(IDC_CHECK_SET_TIME)->EnableWindow(TRUE);

    m_iDownloadMode = DL_MODE_DEVELOPER;
    m_iProgressTotal = 0;
    m_iProgressDone = 0;
    m_bFactoryPagesHidden = FALSE;
    m_nFactoryActive = 0;
    m_bFactoryStopScan = FALSE;
	m_nFactoryDonePorts = 0;
    memset(m_factorySlots, 0, sizeof(m_factorySlots));
    InitializeCriticalSection(&m_csFactory);

    /* Hide legacy device tree; init factory list columns (English). */
    {
        CWnd *pTree = GetDlgItem(IDC_TREE_FILE_LIST);
        if (pTree) pTree->ShowWindow(SW_HIDE);
        CWnd *p;
        p = GetDlgItem(IDC_BUTTON_REFURBISH); if (p) p->ShowWindow(SW_HIDE);
        p = GetDlgItem(IDC_BUTTON_DELETE_APP); if (p) p->ShowWindow(SW_HIDE);
        p = GetDlgItem(IDC_BUTTON_DELETE_APPEND_FILE); if (p) p->ShowWindow(SW_HIDE);
    }
    if (m_listFactoryStatus.GetSafeHwnd())
    {
        m_listFactoryStatus.DeleteAllItems();
        while (m_listFactoryStatus.DeleteColumn(0));
        m_listFactoryStatus.InsertColumn(0, "COM", LVCFMT_LEFT, 70);
        m_listFactoryStatus.InsertColumn(1, "SN", LVCFMT_LEFT, 120);
        m_listFactoryStatus.InsertColumn(2, "Progress", LVCFMT_LEFT, 70);
        m_listFactoryStatus.InsertColumn(3, "Status", LVCFMT_LEFT, 120);
        m_listFactoryStatus.ShowWindow(SW_HIDE);
    }
    CheckRadioButton(IDC_RADIO_MODE_DEVELOPER, IDC_RADIO_MODE_FACTORY, IDC_RADIO_MODE_DEVELOPER);
    memset(&m_cfgDev, 0, sizeof(m_cfgDev));
    memset(&m_cfgFac, 0, sizeof(m_cfgFac));
    CaptureSelToCfg(m_cfgDev);
    LoadFactoryCfgFile();
    ApplyModeUI(FALSE);


    
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

    m_btnOpenVosFile.SetIcon(IDI_ICON_OPEN1, NULL, 32, 32);
    m_btnOpenVosFile.SetShowText(TRUE);
    m_btnOpenVosFile.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnOpenScAppFile.SetIcon(IDI_ICON_OPEN1, NULL, 32, 32);
    m_btnOpenScAppFile.SetShowText(TRUE);
    m_btnOpenScAppFile.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnOpenWifiFile.SetIcon(IDI_ICON_OPEN1, NULL, 32, 32);
    m_btnOpenWifiFile.SetShowText(TRUE);
    m_btnOpenWifiFile.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnOpenLogoFile.SetIcon(IDI_ICON_OPEN1, NULL, 32, 32);
    m_btnOpenLogoFile.SetShowText(TRUE);
    m_btnOpenLogoFile.SetActiveBgColor(RGB(200,200,200), FALSE);
    
    m_btnDeleteAppendFile.SetIcon(IDI_ICON_DELETE2, NULL, 35, 32);
    m_btnDeleteAppendFile.SetShowText(TRUE);
    m_btnDeleteAppendFile.SetActiveBgColor(RGB(200,200,200), FALSE);
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
BOOL CPageApplication::IsThreadRun()
{
    return g_bThreadRunFlag;
}

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
BOOL CPageApplication::OnKillActive() 
{
    // TODO: Add your specialized code here and/or call the base class
    
    if( IsThreadRun() )
    {
        return FALSE;  // ?????????????????��????????????
    }

    return CPropertyPage::OnKillActive();
}

/****************************************************************************
  ??????     :  
  ????       :  ?????????????
  ???????   :  1??void *pTemp??????????????????????????????????
  ???????   :  ??
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
void CPageApplication::OnButtonRefurbish() 
{
    // TODO: Add your control notification handler code here
    DWORD ThreadId;

    UpdateConfigure();  // ???????????????????????????
    SaveConfigInfo();   // ??????????��???????��?????????
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
  ??????     :  
  ????       :  ???????
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
int CPageApplication::DownloadStart()
{
    int iRet;

    iRet = Handshack();
    if (0 != iRet)
    {
        return iRet;
    }
    /* D5: authentication permanently off */
    return 0;
}


/****************************************************************************
  ??????     :  
  ????       :  ??????????????????????
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
void CPageApplication::DownloadEnd()
{
    BYTE abyTemp[2];

    (void)Crc16SendPacket(abyTemp, 0, CMD_FINSH_ALL);
    Sleep(50);
    SerClose();
}

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
int CPageApplication::UpdateTreeList()
{
    BYTE abyTemp[1200];
    WORD wLen = 0;
    int iRet, iAppNum, i, j, iAppendCount, k;

    char str[256];

    MyShowMessage("Refurbish infomation");

    /* SN read/write removed: show POS type only. */
    memset(&abyTemp[100], ' ', 200);
    m_abyVposType[15] = 0;
    memcpy(&abyTemp[100], "POS type:", 9);
    memcpy(&abyTemp[109], m_abyVposType, strlen((char*)m_abyVposType));
    abyTemp[163] = 0;

    m_tcVposFileList.DeleteAllItems();
    m_tcVposFileList.InsertItem((char*)&abyTemp[100]);

    // ?????��????��?
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
    // Application         1000000 Bytes       ????��???
    // Data File           200000 Bytes        data.tel
    // Param File          10000 Bytes         EnvFile
    // Font                960000 Bytes        FontFile

    for (i=0; i<MAX_APPLICATION_NUMBER; i++)
    {
        m_saAppInfo[i].m_byValidityFlag = 0;
    }

    // ???????��????????
    for (i=0; i<iAppNum; i++)
    {
        m_saAppInfo[i].m_byAppNum = abyTemp[i*6+6];
        m_saAppInfo[i].m_byAppStatus = abyTemp[i*6+7];
        memcpy(m_saAppInfo[i].m_abyAppFileSize, &abyTemp[i*6+8], 4);
    }

    // ?????��??????
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

    // ?????��??????
    for (i=0; i<iAppNum; i++)
    {
        memset(str, ' ', sizeof(str));
        // ??��?????????
        sprintf(str, "%2d Application                   ", m_saAppInfo[i].m_byAppNum);
        iRet = m_saAppInfo[i].m_abyAppFileSize[0]*256*65536
            + m_saAppInfo[i].m_abyAppFileSize[1]*65536
            + m_saAppInfo[i].m_abyAppFileSize[2]*256
            + m_saAppInfo[i].m_abyAppFileSize[3];
        sprintf(&str[23], "%8d bytes", iRet);
        strcat(&str[29], "                ");
        m_saAppInfo[i].m_sAppInfo.m_abyAppName[31] = 0;
        sprintf(&str[43], "%s", m_saAppInfo[i].m_sAppInfo.m_abyAppName);

        // ????????????????
        m_saAppInfo[i].m_hTree = m_tcVposFileList.InsertItem(str);
    }

    // ?????????????
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
  ??????     :  
  ????       :  ?????????
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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
    // ??????
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
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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
            cAppDlg.DoModal();  // ??????��??????????????
            return ;
        }
    }
    MessageBox("You have not select the application!");
}

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
int ThreadFactoryManager(void *pTemp);
int ThreadDownload(void *pTemp)
{
    int iRet;
    int iLastError = 0;
    CPageApplication *pPage = &g_pCLoadToolsDlg->m_PageApplication;
    BOOL bOs;
    BOOL bAppSide;

    g_bThreadRunFlag = TRUE;
    pPage->MyEnableWindows(FALSE);

    bOs = pPage->m_bVOSSelect;
    bAppSide = (pPage->m_bAppSelect
        || pPage->m_bScAppSelect
        || pPage->m_bWifiSelect
        || pPage->m_bFontFileSelect
        || pPage->m_bParamFileSelect
        || pPage->m_bDataFileSelect
        || pPage->m_bLogoSelect
        || pPage->m_bBaleFileSelect
        || pPage->m_bSetTimeSelect);

    /* OS uses firmware protocol (serial + CmdSendPacket); then App-side session. */
    if (bOs)
    {
        iRet = pPage->DownloadOsStart();
        if (0 == iRet)
        {
            iRet = pPage->DownloadOsFile(pPage->m_strVosFilePath.GetBuffer(0));
            if (0 == iRet)
            {
                pPage->DownloadOsEnd();
            }
            else
            {
                SerClose();
                iLastError = iRet;
            }
        }
        else
        {
            if (2 != iRet)
            {
                SerClose();
            }
            iLastError = iRet;
        }
    }

    if (bAppSide)
    {
        iRet = pPage->DownloadStart();
        if (0 == iRet)
        {
            iRet = pPage->DownloadProcess();
            if (0 != iRet)
            {
                iLastError = iRet;
            }
            pPage->DownloadEnd();
        }
        else if (2 == iRet)
        {
            /* Open port failed: message already set; do not Send Finish. */
            iLastError = iRet;
        }
        else
        {
            /* Handshake / user-stop failed after port open. */
            SerClose();
            iLastError = iRet;
            /* Keep G_MyShowMessage text (Handshack failed / User stop). */
        }
    }
    else if (bOs)
    {
        if (0 == iLastError)
        {
            pPage->MyShowMessage("Download success!");
        }
        else
        {
            pPage->MyShowMessage("Download finished with errors!");
        }
    }

    pPage->MyEnableWindows(TRUE);
    g_bThreadRunFlag = FALSE;
    return 0;
}

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
void CPageApplication::OnButtonBeginDownloadApp() 
{
    // TODO: Add your control notification handler code here
    DWORD ThreadId;

    if (!ValidateBeforeStart())
    {
        return;
    }
    if (DL_MODE_FACTORY == m_iDownloadMode)
    {
        UpdateConfigure();
        SaveConfigInfo();
        CaptureSelToCfg(m_cfgFac);
        SaveFactoryCfgFile();
        m_bStopDownloadFlag = FALSE;
        m_bFactoryStopScan = FALSE;
        FactoryInitSlots();
        MyShowMessage("Factory: scanning...");
        if (!g_bThreadRunFlag)
        {
            g_lHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFactoryManager,
                this, 0, &ThreadId);
            if (NULL == g_lHandle)
            {
                G_MyShowMessage(MSG_CREATE_THREAD_FAILED);
            }
        }
        return;
    }

    /* Download page: local validation done. */
        UpdateConfigure();  // ???????????????????????????
    SaveConfigInfo();   // ??????????��???????��?????????
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
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
void CPageApplication::OnButtonStopDownloadApp() 
{
    m_bStopDownloadFlag = TRUE;
    m_bFactoryStopScan = TRUE; /* D40: stop all + stop scan */
    /* Do not flip Start/Stop here for factory: manager re-enables when scan ends.
       For developer, re-enable Start so UI is not stuck if thread exits oddly. */
    if (DL_MODE_DEVELOPER == m_iDownloadMode)
        MyEnableWindows(TRUE);
    else
        MyShowMessage("Factory: stopping...");
}

/****************************************************************************
  ??????     :  void CPageApplication::OnButtonOpenAppFile() 
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-09-03   V1.0        ????
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
        SaveAppFilePath();
    }
}

/****************************************************************************
  ??????     :  void CPageApplication::SaveAppFilePath()
  ????       :  ?????��????��????????��???��??
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-09-03   V1.0        ????
****************************************************************************/
void CPageApplication::SaveAppFilePath()
{
    memset(g_sConfigreInfo.m_strAppFilePath, 0, sizeof(g_sConfigreInfo.m_strAppFilePath));
    if (!m_strAppFilePath.IsEmpty())
    {
        strncpy(&g_sConfigreInfo.m_strAppFilePath[0][0],
            m_strAppFilePath.GetBuffer(0), 255);
        g_sConfigreInfo.m_strAppFilePath[0][255] = 0;
        g_sConfigreInfo.m_byAppFilePathNumber = 1;
    }
    else
    {
        g_sConfigreInfo.m_byAppFilePathNumber = 0;
    }
    g_sConfigreInfo.m_byAppPathCurrentSelect = 0;
}

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
void CPageApplication::OnCheckAppSelect() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    ApplySyncDownloadMutex(FALSE);
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
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
void CPageApplication::OnCheckBaleFile() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    ApplySyncDownloadMutex(FALSE);
    if (m_bBaleFileSelect)
    {
        m_bAppSelect = FALSE;
        m_bCreateFileSystemSelect = FALSE;
        m_bScAppSelect = FALSE;
        m_bWifiSelect = FALSE;
        m_bFontFileSelect = FALSE;
        m_bParamFileSelect = FALSE;
        m_bDataFileSelect = FALSE;
        m_bLogoSelect = FALSE;
        UpdateData(FALSE);
    }
}

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
void CPageApplication::OnCheckCreateFileSystem() 
{
    /* Create file system feature removed. */
    m_bCreateFileSystemSelect = FALSE;
}

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
void CPageApplication::OnCheckFontFile() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    ApplySyncDownloadMutex(FALSE);
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
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
void CPageApplication::OnCheckParamFile() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    ApplySyncDownloadMutex(FALSE);
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
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
/* OnCheckSerialNumber removed with SN UI. */

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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

/* OnChangeEditVposSerialNumber removed with SN feature. */

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
void CPageApplication::MyShowMessage(char *strMsg)
{
    SetDlgItemText(IDC_STATIC_PAGE_APP_MESSAGE, strMsg);
}

void CPageApplication::OnChangeEditAppFilePath()
{
    UpdateData(TRUE);
    SaveAppFilePath();
}

/* StringAddOne removed with SN feature. */

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
void CPageApplication::MyEnableWindows(BOOL bFlag)
{
    GetDlgItem(IDC_BUTTON_BEGIN_DOWNLOAD_APP)->EnableWindow(bFlag);
    GetDlgItem(IDC_BUTTON_STOP_DOWNLOAD_APP)->EnableWindow(!bFlag);
}


/****************************************************************************
  ??????     :  int CPageApplication::Handshack()
  ????       :  ???????
  ???????   :  ??
  ???????   :  ??
  ?????     :  0???????1??????????2??????????
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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

    // ????
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
            /* Optional SN CRC16 frame (protocol 10.5); drain so Start RSP is not confused. */
            {
                BYTE abySn[128];
                WORD wSnLen = 0;
                (void)Crc16RecvPacket(abySn, &wSnLen, 80);
            }
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
    int iRet, i;

    m_bStopDownloadFlag = FALSE;

    /* Use UI-selected baud; skip open when factory already bound the port. */
    if (!SerIsBound())
    {
        if (SerOpen(g_sConfigreInfo.m_iSerialPortNumber+1,
            g_adwBauds[g_sConfigreInfo.m_iSerialPortBaudsSelect]))
        {
            G_MyShowMessage(MSG_OPEN_SERIAL_PORT_FAILED);
            return 2;
        }
    }
    else
    {
        /* Factory scan already completed L/A (and drained SN). Do NOT send L again:
           device is in command loop and will not reply 'A'. */
        m_bStopDownloadFlag = FALSE;
        return 0;
    }

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
    /* Optional SN CRC16 frame (protocol 10.5); same as FactoryQuickHandshake.
       Device may send it after 'A'; must consume or DownloadFile Start RSP
       will read leftover frame (hs_ver=1 treated as status fail). */
    {
        BYTE abySn[128];
        WORD wSnLen = 0;
        (void)Crc16RecvPacket(abySn, &wSnLen, 80);
    }
    return 0;
}


/****************************************************************************
  ??????     :  int CPageApplication::Authentication()
  ????       :  ???????
  ???????   :  ??
  ???????   :  ??
  ?????     :  0??????? 1?????
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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
        G_MyShowMessage(MSG_AUTHENTICATION_ERROR);   // ??????
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
        G_MyShowMessage(MSG_AUTHENTICATION_ERROR);   // ??????
        return 1;
    }
    return 0;
}

/****************************************************************************
  ??????     :  
  ????       :  ??????????????
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
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
  ??????     :  
  ????       :  ??????????????
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
int CPageApplication::DownloadProcess()
{
    int i, iRet;
    int iLastError = 0;
    BOOL bAppOk = FALSE;
    BYTE byAttachAppNo = 0;

    m_bCreateFileSystemSelect = FALSE;
    m_iProgressTotal = CountSelectedDownloadItems();
    m_iProgressDone = 0;

    /* Sync-only path (mutually exclusive with downloads). */
    if (m_bSetTimeSelect && 0 == CountSelectedDownloadItems())
    {
        iRet = SetVposTime();
        if (0 != iRet)
        {
            MyShowMessage("Sync time failed!");
            return iRet;
        }
        MyShowMessage("Sync time success!");
        return 0;
    }

    if (   m_bBaleFileSelect
        || m_bFontFileSelect
        || m_bParamFileSelect  || m_bAppSelect
        || m_bScAppSelect || m_bWifiSelect || m_bLogoSelect
        || m_bDataFileSelect)
    {
        /* Transfer order (receive). Install order on device: OS->Font->Logo->Param->Data->SC->WIFI->App (D53).
           OS already handled in ThreadDownload via legacy path.
           Bale = PC unpack then per-item DownloadFile (D54). Continue-on-fail (D68). */
        if (m_bFontFileSelect)
        {
            i = DownloadFile(m_strFontFilePath.GetBuffer(0), FILE_TYPE_FONT, 0);
            m_iProgressDone++;
            if (0 != i) iLastError = i;
        }
        if (m_bLogoSelect)
        {
            i = DownloadFile(m_strLogoFilePath.GetBuffer(0), FILE_TYPE_LOGO, 0);
            m_iProgressDone++;
            if (0 != i) iLastError = i;
        }
        if (m_bParamFileSelect)
        {
            byAttachAppNo = 0;
            i = DownloadFile(m_strParamFilePath.GetBuffer(0), FILE_TYPE_PARAM, byAttachAppNo);
            m_iProgressDone++;
            if (0 != i) iLastError = i;
        }
        if (m_bDataFileSelect)
        {
            i = DownloadFile(m_strDataFilePath.GetBuffer(0), FILE_TYPE_DATA, 0);
            m_iProgressDone++;
            if (0 != i) iLastError = i;
        }
        if (m_bScAppSelect)
        {
            i = DownloadFile(m_strScAppFilePath.GetBuffer(0), FILE_TYPE_SC_APP, 0);
            m_iProgressDone++;
            if (0 != i) iLastError = i;
        }
        if (m_bWifiSelect)
        {
            i = DownloadFile(m_strWifiFilePath.GetBuffer(0), FILE_TYPE_WIFI, 0);
            m_iProgressDone++;
            if (0 != i) iLastError = i;
        }
        if (m_bAppSelect)
        {
            i = DownloadFile(m_strAppFilePath.GetBuffer(0), FILE_TYPE_APPLICATION, 0);
            m_iProgressDone++;
            if (0 != i)
            {
                iLastError = i;
                bAppOk = FALSE;
            }
            else
            {
                bAppOk = TRUE;
            }
        }
        if (m_bBaleFileSelect)
        {
            i = DownloadBaleFile();
            m_iProgressDone++;
            if (0 != i) iLastError = i;
        }
        if (0 == iLastError)
        {
            MyShowMessage("Download success!");
        }
        else
        {
            char szSum[160];
            sprintf(szSum, "Download finished with errors! (last code %d)", iLastError);
            MyShowMessage(szSum);
        }
        return iLastError;
    }
    return 0;
}



/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
int CPageApplication::DownloadFile(char *strFileName, BYTE byFileType,
                                   BYTE byAppNo)
{
    FILE *fp;
    int iRet, iFileLen, iErrorCount, iReadLen;
    char strMsg[128];
    char szBaseName[FILE_DL_NAME_MAX + 4];
    BYTE abySend[FILE_DL_CHUNK_MAX + 64];
    BYTE abyRecv[512];
    BYTE abyHash[4];
    BYTE abyChunkCrc[4];
    WORD wPacketLen;
    WORD wChunkAccept;
    DWORD dwOffset;
    DWORD dwChunkMax;
    BYTE byNameLen;
    BYTE bySlot;
    char *p;
    int iSendLen;
    BYTE byReboot;

    fp = fopen(strFileName, "rb");
    if (NULL == fp)
    {
        MyShowMessage("Can't open file!");
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    iFileLen = ftell(fp);
    if (iFileLen <= 0)
    {
        MyShowMessage("File size error!");
        fclose(fp);
        return 1;
    }

    switch (byFileType)
    {
    case FILE_TYPE_APPLICATION:
        m_iCurrentAppNo = -1;
        if (iFileLen > (1024 * 1024))
        {
            MyShowMessage("App file size must less than 1MB!");
            fclose(fp);
            return 1;
        }
        if (iFileLen < (10 * 1024))
        {
            MyShowMessage("App file too small!");
            fclose(fp);
            return 1;
        }
        strcpy(strMsg, "App");
        break;
    case FILE_TYPE_PARAM:
        if (iFileLen > ENV_JSON_MAX_FILE_SIZE)
        {
            MyShowMessage("Param file size too big!");
            fclose(fp);
            return 1;
        }
        if (EnvJsonCheckFile(strFileName) <= 0)
        {
            MyShowMessage("Param JSON check failed!");
            fclose(fp);
            return 1;
        }
        strcpy(strMsg, "Param");
        break;
    case FILE_TYPE_DATA:
        if (iFileLen > (1560 * 1024))
        {
            MyShowMessage("Data file size too big!");
            fclose(fp);
            return 1;
        }
        strcpy(strMsg, "Data");
        break;
    case FILE_TYPE_FONT:
        if (iFileLen > (960 * 1024) || iFileLen < (4 * 1024))
        {
            MyShowMessage("Font file size error!");
            fclose(fp);
            return 1;
        }
        strcpy(strMsg, "Font");
        break;
    case FILE_TYPE_SC_APP:
        strcpy(strMsg, "SC App");
        break;
    case FILE_TYPE_WIFI:
        strcpy(strMsg, "WIFI");
        break;
    case FILE_TYPE_LOGO:
        strcpy(strMsg, "Logo");
        break;
    default:
        fclose(fp);
        MyShowMessage("File type error!");
        return 4;
    }

    /* Whole-file CRC32 */
    {
        BYTE *pAll = (BYTE *)malloc((size_t)iFileLen);
        if (NULL == pAll)
        {
            fclose(fp);
            MyShowMessage("Out of memory!");
            return 1;
        }
        fseek(fp, 0, SEEK_SET);
        if ((int)fread(pAll, 1, iFileLen, fp) != iFileLen)
        {
            free(pAll);
            fclose(fp);
            MyShowMessage("Read file failed!");
            return 1;
        }
        QuickCrc32(pAll, (DWORD)iFileLen, abyHash);
        free(pAll);
    }

    /* name: Data uses basename; others empty (D3) */
    byNameLen = 0;
    szBaseName[0] = 0;
    if (FILE_TYPE_DATA == byFileType)
    {
        p = strrchr(strFileName, '\\');
        if (NULL == p) p = strrchr(strFileName, '/');
        if (NULL == p) p = strFileName; else p++;
        strncpy(szBaseName, p, FILE_DL_NAME_MAX);
        szBaseName[FILE_DL_NAME_MAX] = 0;
        byNameLen = (BYTE)strlen(szBaseName);
    }

    bySlot = 0;
    if (FILE_TYPE_APPLICATION == byFileType)
    {
        bySlot = byAppNo;
    }

    /* Start 0xB210 */
    memset(abySend, 0, sizeof(abySend));
    abySend[0] = FILE_DL_PROTO_VER;
    abySend[1] = byFileType;
    abySend[2] = bySlot;
    abySend[3] = byNameLen;
    abySend[4] = (BYTE)(((DWORD)iFileLen >> 24) & 0xff);
    abySend[5] = (BYTE)(((DWORD)iFileLen >> 16) & 0xff);
    abySend[6] = (BYTE)(((DWORD)iFileLen >> 8) & 0xff);
    abySend[7] = (BYTE)((DWORD)iFileLen & 0xff);
    memcpy(&abySend[8], abyHash, 4);
    /* reserved 12..13 = 0 */
    abySend[14] = (BYTE)((FILE_DL_CHUNK_MAX >> 8) & 0xff);
    abySend[15] = (BYTE)(FILE_DL_CHUNK_MAX & 0xff);
    if (byNameLen > 0)
    {
        memcpy(&abySend[16], szBaseName, byNameLen);
    }
    iSendLen = 16 + byNameLen;

    UpdateTotalProgress(strMsg);
    iRet = Crc16SendPacket(abySend, (WORD)iSendLen, CMD_FILE_DL_START_REQ);
    if (0 != iRet)
    {
        fclose(fp);
        MyShowMessage("Send failed!");
        return 6;
    }
    iRet = Crc16RecvPacket(abyRecv, &wPacketLen, 3000);
    /* wPacketLen = total frame (DATA_LEN+7); status at [5]; chunk_accept at [7..8]
       Start RSP DATA is 10 bytes => total 17; min check 12 (status+ver+chunk). */
    if (0 != iRet)
    {
        fclose(fp);
        MyShowMessage("Start: no response / timeout!");
        return 5;
    }
    if (wPacketLen < 12)
    {
        fclose(fp);
        MyShowMessage("Start: bad response length!");
        return 5;
    }
    if (0 != abyRecv[5])
    {
        char szRj[80];
        sprintf(szRj, "Start rejected! status=%u", (unsigned)abyRecv[5]);
        fclose(fp);
        MyShowMessage(szRj);
        return 5;
    }
    wChunkAccept = (WORD)(abyRecv[7] * 256 + abyRecv[8]);
    if (0 == wChunkAccept || wChunkAccept > FILE_DL_CHUNK_MAX)
    {
        wChunkAccept = FILE_DL_CHUNK_MAX;
    }
    dwChunkMax = wChunkAccept;

    dwOffset = 0;
    iErrorCount = 0;
    fseek(fp, 0, SEEK_SET);
    while (dwOffset < (DWORD)iFileLen)
    {
        if (m_bStopDownloadFlag)
        {
            AbortFileSession(1);
            fclose(fp);
            MyShowMessage("User stop download.");
            return 1;
        }
        {
            char szCur[160];
            int pctFile = (int)((dwOffset * 100) / (DWORD)iFileLen);
            sprintf(szCur, "%s %d%% (%d/%d)", strMsg, pctFile,
                m_iProgressDone, m_iProgressTotal);
            MyShowMessage(szCur);
        }
        wPacketLen = (WORD)dwChunkMax;
        if ((dwOffset + dwChunkMax) > (DWORD)iFileLen)
        {
            wPacketLen = (WORD)(iFileLen - dwOffset);
        }
        iReadLen = fread(&abySend[10], 1, wPacketLen, fp);
        if (iReadLen <= 0)
        {
            break;
        }
        abySend[0] = (BYTE)((dwOffset >> 24) & 0xff);
        abySend[1] = (BYTE)((dwOffset >> 16) & 0xff);
        abySend[2] = (BYTE)((dwOffset >> 8) & 0xff);
        abySend[3] = (BYTE)(dwOffset & 0xff);
        abySend[4] = (BYTE)((iReadLen >> 8) & 0xff);
        abySend[5] = (BYTE)(iReadLen & 0xff);
        QuickCrc32(&abySend[10], (DWORD)iReadLen, abyChunkCrc);
        memcpy(&abySend[6], abyChunkCrc, 4);
        iSendLen = 10 + iReadLen;

        iErrorCount = 0;
        while (1)
        {
            iRet = Crc16SendPacket(abySend, (WORD)iSendLen, CMD_FILE_DL_DATA_REQ);
            if (0 != iRet)
            {
                iErrorCount++;
            }
            else
            {
                iRet = Crc16RecvPacket(abyRecv, &wPacketLen, 2000);
                if ((0 == iRet) && (wPacketLen >= 8) && (0 == abyRecv[5]))
                {
                    break;
                }
                iErrorCount++;
            }
            if (iErrorCount > 3)
            {
                AbortFileSession(2);
                fclose(fp);
                MyShowMessage("Data transfer failed!");
                return 5;
            }
        }
        dwOffset += (DWORD)iReadLen;
    }

    /* End 0xB214 */
    MyShowMessage("Saving file...");
    abySend[0] = (BYTE)(((DWORD)iFileLen >> 24) & 0xff);
    abySend[1] = (BYTE)(((DWORD)iFileLen >> 16) & 0xff);
    abySend[2] = (BYTE)(((DWORD)iFileLen >> 8) & 0xff);
    abySend[3] = (BYTE)((DWORD)iFileLen & 0xff);
    memcpy(&abySend[4], abyHash, 4);
    iRet = Crc16SendPacket(abySend, 8, CMD_FILE_DL_END_REQ);
    if (0 != iRet)
    {
        fclose(fp);
        MyShowMessage("Send failed!");
        return 6;
    }
    iRet = Crc16RecvPacket(abyRecv, &wPacketLen, 30000);
    if ((0 != iRet) || (wPacketLen < 8) || (0 != abyRecv[5]))
    {
        fclose(fp);
        MyShowMessage("End / verify failed!");
        return 5;
    }
    byReboot = 0;
    if (wPacketLen >= 9)
    {
        byReboot = abyRecv[6];
    }
    if (FILE_TYPE_APPLICATION == byFileType)
    {
        /* App number may not be in new End RSP; keep previous semantics soft. */
        if (m_iCurrentAppNo < 0)
        {
            m_iCurrentAppNo = byAppNo;
        }
    }
    fclose(fp);

    if (1 == byReboot)
    {
        if (0 != WaitDeviceAfterReboot())
        {
            return 5;
        }
    }
    MyShowMessage("Success!");
    return 0;
}


void CPageApplication::OnCheckSetTime() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    ApplySyncDownloadMutex(TRUE);
}

/****************************************************************************
  ??????     :  
  ????       :  
  ???????   :  
  ???????   :  
  ?????     :  
  ??????   :
      ?????     ??????    ???��??   ??????
  1?? ?????     2007-10-11   V1.0        ????
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
    ApplySyncDownloadMutex(FALSE);
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
    /* Default app no 0 for standalone Param/Data; App task overwrites after download. */
    m_iCurrentAppNo = 0;

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
        /* Allow app no 0 for standalone Param/Data in bale (single-app / global). */
        if ((m_iCurrentAppNo < 0) || (m_iCurrentAppNo > 24))
        {
            m_iCurrentAppNo = 0;
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


/****************************************************************************
  OS download handlers and firmware-protocol helpers (merged from Firmware page)
****************************************************************************/
void CPageApplication::OnButtonOpenVosFile()
{
    static char BASED_CODE szMergeFilter[] = _T("Open File (*.bin)|*.bin||)");
    CString filter;
    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, m_strVosFilePath,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, filter, 0);
    if (dlg.DoModal() == IDOK)
    {
        m_strVosFilePath = dlg.GetPathName();
        UpdateData(FALSE);
    }
}

void CPageApplication::OnCheckVosSelect()
{
    UpdateData(TRUE);
    ApplySyncDownloadMutex(FALSE);
}

void CPageApplication::OnChangeEditVosFilePath()
{
    UpdateData(TRUE);
}

void CPageApplication::OnButtonOpenScAppFile()
{
    static char BASED_CODE szMergeFilter[] = _T("Open File (*.*)|*.*||)");
    CString filter;
    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, m_strScAppFilePath,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, filter, 0);
    if (dlg.DoModal() == IDOK)
    {
        m_strScAppFilePath = dlg.GetPathName();
        UpdateData(FALSE);
    }
}

void CPageApplication::OnCheckScAppFile()
{
    UpdateData(TRUE);
    ApplySyncDownloadMutex(FALSE);
    if (m_bBaleFileSelect)
    {
        if (m_bScAppSelect)
        {
            m_bScAppSelect = FALSE;
            UpdateData(FALSE);
            MessageBox("If you want to download SC App, you can't select the bale file!");
        }
    }
}

void CPageApplication::OnChangeEditScAppFilePath()
{
    UpdateData(TRUE);
}

void CPageApplication::OnButtonOpenWifiFile()
{
    static char BASED_CODE szMergeFilter[] = _T("Open File (*.*)|*.*||)");
    CString filter;
    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, m_strWifiFilePath,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, filter, 0);
    if (dlg.DoModal() == IDOK)
    {
        m_strWifiFilePath = dlg.GetPathName();
        UpdateData(FALSE);
    }
}

void CPageApplication::OnCheckWifiFile()
{
    UpdateData(TRUE);
    ApplySyncDownloadMutex(FALSE);
    if (m_bBaleFileSelect)
    {
        if (m_bWifiSelect)
        {
            m_bWifiSelect = FALSE;
            UpdateData(FALSE);
            MessageBox("If you want to download WIFI, you can't select the bale file!");
        }
    }
}

void CPageApplication::OnChangeEditWifiFilePath()
{
    UpdateData(TRUE);
}

void CPageApplication::OnButtonOpenLogoFile()
{
    static char BASED_CODE szMergeFilter[] = _T("Open File (*.*)|*.*||)");
    CString filter;
    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, m_strLogoFilePath,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, filter, 0);
    if (dlg.DoModal() == IDOK)
    {
        m_strLogoFilePath = dlg.GetPathName();
        UpdateData(FALSE);
    }
}

void CPageApplication::OnCheckLogoFile()
{
    UpdateData(TRUE);
    ApplySyncDownloadMutex(FALSE);
    if (m_bBaleFileSelect)
    {
        if (m_bLogoSelect)
        {
            m_bLogoSelect = FALSE;
            UpdateData(FALSE);
            MessageBox("If you want to download Logo, you can't select the bale file!");
        }
    }
}

void CPageApplication::OnChangeEditLogoFilePath()
{
    UpdateData(TRUE);
}

int CPageApplication::DownloadOsStart()
{
    BYTE abyRecvBuff[200];
    int iRet, i;

    m_bStopDownloadFlag = FALSE;

    /* OS download: skip open when factory already bound the port. */
    if (!SerIsBound())
    {
        if (SerOpen(g_sConfigreInfo.m_iSerialPortNumber+1,
            g_adwBauds[g_sConfigreInfo.m_iSerialPortBaudsSelect]))
        {
            G_MyShowMessage(MSG_OPEN_SERIAL_PORT_FAILED);
            return 2;
        }
    }

    G_MyShowMessage(MSG_CONNECTING);
    i = 0;
    for (;;)
    {
        if (m_bStopDownloadFlag)
        {
            G_MyShowMessage(MSG_USER_STOP_DOWNLOAD);
            m_bStopDownloadFlag = FALSE;
            return 1;
        }
        SerWrite((BYTE *)"T" , 1);
        abyRecvBuff[0] = 0;
        iRet = SerRead(abyRecvBuff, 1, 20);
        if (0 == iRet)
        {
            if ('R' == abyRecvBuff[0])
            {
                g_iCommCheckType = LRC_CHECK_TYPE;
                break;
            }
            else if ('S' == abyRecvBuff[0])
            {
                g_iCommCheckType = CRC32_CHECK_TYPE;
                break;
            }
        }
        i++;
        if (i > 10000)
        {
            G_MyShowMessage(MSG_HANDSHACK_FAILED);
            return 1;
        }
    }
        /* Protocol: after T/S (or T/R), PC must send Q and wait for P before 0xB0.
     * Hard-fail if P missing - do not skip. */
    {
        int k;
        int gotP = 0;
        MyShowMessage("OS Q/P...");
        SerWrite((BYTE *)"Q", 1);
        for (k = 0; k < 250; k++) /* ~5s, match device Q-wait */
        {
            abyRecvBuff[0] = 0;
            iRet = SerRead(abyRecvBuff, 1, 20);
            if ((0 == iRet) && (abyRecvBuff[0] == 'P'))
            {
                gotP = 1;
                break;
            }
            if (m_bStopDownloadFlag)
            {
                G_MyShowMessage(MSG_USER_STOP_DOWNLOAD);
                m_bStopDownloadFlag = FALSE;
                return 1;
            }
        }
        if (!gotP)
        {
            MyShowMessage("OS Q/P failed (no P)");
            return 1;
        }
        MyShowMessage("OS handshake OK");
        PurgeCommBuf(PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
    }
    return 0;
}

int CPageApplication::DownloadOsFile(char *strFileName)
{
    FILE *fp;
    int iRet, iFileLen, iErrorCount, iReadLen, iSendPacketLen;
    char str[100];
    BYTE byCmd, abyTemp[DOWNLOAD_LEN_FIRM+10], abyCompress[DOWNLOAD_LEN_FIRM*2+100];
    WORD wPacketLen;
    DWORD dwCurrentLen, dwCompressLen;

    MyShowMessage("Download OS");
    fp = fopen(strFileName, "rb");
    if (NULL == fp)
    {
        MessageBox("Can't open the file.");
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    iFileLen = ftell(fp);
    byCmd = 0xb0;
    if (iFileLen > (2*1024*1024))
    {
        fclose(fp);
        MessageBox("OS file size error(>2MB), please ensure the OS file is the right");
        return 4;
    }
    else if (iFileLen < (50*1024))
    {
        fclose(fp);
        MessageBox("OS file size error(<50K bytes), please ensure the OS file is the right!");
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
        {
            int _pct = (iFileLen > 0) ? (int)((dwCurrentLen * 100) / iFileLen) : 0;
            sprintf(str, "Download OS...%d%%", _pct);
            MyShowMessage(str);
            if (t_iFactoryUiSlot >= 0)
            {
                char _prog[16];
                sprintf(_prog, "%d%%", _pct);
                FactoryPostStatus(t_iFactoryUiSlot, _prog, "Downloading OS");
            }
        }
        wPacketLen = DOWNLOAD_LEN_FIRM;
        if ((dwCurrentLen+DOWNLOAD_LEN_FIRM) > (DWORD)iFileLen)
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
            /* Device MyDownload rejects compress (flag!=0). Always send raw. */
            abyTemp[4] = 0;  // data no zip
            iSendPacketLen = iReadLen + 5;
            iErrorCount = 0;
            while (1)
            {
                if (m_bStopDownloadFlag)
                {
                    fclose(fp);
                    MyShowMessage("User stop download.");
                    return 1;
                }
                (void)CmdSendPacket(abyTemp, (WORD)iSendPacketLen, byCmd);
                iRet = CmdRecvPacket(abyCompress, &wPacketLen, 500);
                if ((0==iRet) && (0==abyCompress[4]) && (0xff != abyCompress[5]))
                {
                    break;
                }
                iErrorCount++;
                if (iErrorCount > 8)
                {
                    fclose(fp);
                    MyShowMessage("Receive timeout!");
                    return 9;
                }
            }
            dwCurrentLen += iReadLen;
        }
        else
        {
            break;
        }
    }
    fclose(fp);
    MyShowMessage("Save file...");
    if (t_iFactoryUiSlot >= 0)
        FactoryPostStatus(t_iFactoryUiSlot, "100%", "Saving OS");
    byCmd++;
    CmdSendPacket(abyTemp, 0, byCmd);
    while (1)
    {
        if (m_bStopDownloadFlag)
        {
            MyShowMessage("User stop download.");
            return 1;
        }
        iRet = CmdRecvPacket(abyTemp, &wPacketLen, 30000);
        if (0 != iRet)
        {
            MyShowMessage("OS end: receive timeout/error!");
            return 10;
        }
        /* Device B1 success = 1-byte payload [0]. Frame *pwLen=payload+8;
         * abyTemp[5] would be CRC32 �� old code treated it as erase percent. */
        {
            WORD payLen = (wPacketLen >= 8) ? (WORD)(wPacketLen - 8) : 0;
            if (1 == payLen)
            {
                if (0 == abyTemp[4])
                {
                    MyShowMessage("Download success.");
                    return 0;
                }
                MyShowMessage("OS end: device rejected.");
                return 11;
            }
        }
        switch (abyTemp[4])
        {
        case 0:
            if (abyTemp[5] <= 100)
            {
                sprintf(str, "Erase flash...%d%%", abyTemp[5]);
                MyShowMessage(str);
                if (t_iFactoryUiSlot >= 0)
                {
                    char _prog[16];
                    sprintf(_prog, "%d%%", (int)abyTemp[5]);
                    FactoryPostStatus(t_iFactoryUiSlot, _prog, "Erase flash");
                }
            }
            else
            {
                sprintf(str, "Erase flash fault.");
                MyShowMessage(str);
                return 11;
            }
            break;
        case 1:
            if (abyTemp[5] < 100)
            {
                sprintf(str, "Write flash...%d%%", abyTemp[5]);
                MyShowMessage(str);
                if (t_iFactoryUiSlot >= 0)
                {
                    char _prog[16];
                    sprintf(_prog, "%d%%", (int)abyTemp[5]);
                    FactoryPostStatus(t_iFactoryUiSlot, _prog, "Write flash");
                }
            }
            else if (100 == abyTemp[5])
            {
                MyShowMessage("Download success.");
                return 0;
            }
            else
            {
                sprintf(str, "Write flash fault.");
                MyShowMessage(str);
                return 11;
            }
            break;
        case 3:
            if (abyTemp[5] <= 100)
            {
                sprintf(str, "Check...%d%%", abyTemp[5]);
                MyShowMessage(str);
                if (t_iFactoryUiSlot >= 0)
                {
                    char _prog[16];
                    sprintf(_prog, "%d%%", (int)abyTemp[5]);
                    FactoryPostStatus(t_iFactoryUiSlot, _prog, "Check");
                }
                if (100 == abyTemp[5])
                {
                    MyShowMessage("Check success, save file...");
                }
            }
            else
            {
                sprintf(str, "Check failed=%d", abyTemp[5]);
                MyShowMessage(str);
                return 11;
            }
            break;
        default:
            return 12;
        }
    }
}

void CPageApplication::DownloadOsEnd()
{
    BYTE abyTemp[2];

    (void)CmdSendPacket(abyTemp, 0, 0xD2);
    Sleep(50);
    SerClose();
    MyShowMessage("OS download finish.");
}

/* ===== BEGIN DownloadProtocol_v1 DownloadFile replacement helpers ===== */

int CPageApplication::CountSelectedDownloadItems()
{
    int n = 0;
    if (m_bVOSSelect) n++;
    if (m_bFontFileSelect) n++;
    if (m_bLogoSelect) n++;
    if (m_bParamFileSelect) n++;
    if (m_bDataFileSelect) n++;
    if (m_bScAppSelect) n++;
    if (m_bWifiSelect) n++;
    if (m_bAppSelect) n++;
    if (m_bBaleFileSelect) n++;
    return n;
}

void CPageApplication::UpdateTotalProgress(const char *szItem)
{
    char sz[160];
    int pct = 0;
    if (m_iProgressTotal > 0)
    {
        pct = (m_iProgressDone * 100) / m_iProgressTotal;
        if (pct > 100) pct = 100;
    }
    if (szItem && szItem[0])
    {
        sprintf(sz, "%s  %d/%d  %d%%", szItem, m_iProgressDone, m_iProgressTotal, pct);
    }
    else
    {
        sprintf(sz, "Progress  %d/%d  %d%%", m_iProgressDone, m_iProgressTotal, pct);
    }
    MyShowMessage(sz);
}

void CPageApplication::AbortFileSession(BYTE byReason)
{
    BYTE aby[8];
    WORD wLen = 0;
    aby[0] = byReason;
    (void)Crc16SendPacket(aby, 1, CMD_FILE_DL_ABORT_REQ);
    (void)Crc16RecvPacket(aby, &wLen, 500);
}

int CPageApplication::WaitDeviceAfterReboot()
{
    /* D58: poll handshake up to 20s; Stop cancels. Used when End.reboot=1. */
    BYTE abyRecvBuff[32];
    int iRet;
    DWORD dwBegin = GetTickCount();
    MyShowMessage("Waiting device...");
    SerClose();
    while ((GetTickCount() - dwBegin) < (DWORD)OS_REBOOT_WAIT_MS)
    {
        if (m_bStopDownloadFlag)
        {
            MyShowMessage("User stop download.");
            return 1;
        }
        if (0 != SerOpen(g_sConfigreInfo.m_iSerialPortNumber + 1,
            g_adwBauds[g_sConfigreInfo.m_iSerialPortBaudsSelect]))
        {
            Sleep(200);
            continue;
        }
        SerWrite((BYTE *)"L", 1);
        abyRecvBuff[0] = 0;
        iRet = SerRead(abyRecvBuff, 1, 20);
        if ((0 == iRet) && (abyRecvBuff[0] == 'A'))
        {
            MyShowMessage("Device ready.");
            return 0;
        }
        SerClose();
        Sleep(50);
    }
    MyShowMessage("Wait device timeout.");
    return 1;
}

void CPageApplication::ApplySyncDownloadMutex(BOOL bFromSync)
{
    /* D55: Sync time mutually exclusive with any download checkbox. */
    UpdateData(TRUE);
    if (bFromSync)
    {
        if (m_bSetTimeSelect)
        {
            m_bVOSSelect = FALSE;
            m_bAppSelect = FALSE;
            m_bScAppSelect = FALSE;
            m_bWifiSelect = FALSE;
            m_bFontFileSelect = FALSE;
            m_bParamFileSelect = FALSE;
            m_bDataFileSelect = FALSE;
            m_bLogoSelect = FALSE;
            m_bBaleFileSelect = FALSE;
            UpdateData(FALSE);
        }
    }
    else
    {
        if (m_bVOSSelect || m_bAppSelect || m_bScAppSelect || m_bWifiSelect
            || m_bFontFileSelect || m_bParamFileSelect || m_bDataFileSelect
            || m_bLogoSelect || m_bBaleFileSelect)
        {
            if (m_bSetTimeSelect)
            {
                m_bSetTimeSelect = FALSE;
                UpdateData(FALSE);
            }
        }
    }
}

BOOL CPageApplication::ValidateBeforeStart()
{
    UpdateData(TRUE);

    if (FALSE == m_bVOSSelect && FALSE == m_bAppSelect && FALSE == m_bScAppSelect
        && FALSE == m_bWifiSelect && FALSE == m_bFontFileSelect
        && FALSE == m_bParamFileSelect && FALSE == m_bDataFileSelect
        && FALSE == m_bLogoSelect && FALSE == m_bBaleFileSelect
        && FALSE == m_bSetTimeSelect)
    {
        MyShowMessage("Please select files to download!");
        return FALSE;
    }

    /* Sync-only is OK; otherwise Sync must not mix (UI mutex). */
    if (m_bSetTimeSelect && CountSelectedDownloadItems() > 0)
    {
        MyShowMessage("Sync time cannot mix with download!");
        return FALSE;
    }

    struct { BOOL sel; CString *path; const char *emptyMsg; const char *missMsg; } items[] = {
        { m_bVOSSelect, &m_strVosFilePath, "Please set OS file path!", "OS file not found!" },
        { m_bAppSelect, &m_strAppFilePath, "Please set App file path!", "App file not found!" },
        { m_bScAppSelect, &m_strScAppFilePath, "Please set SC App file path!", "SC App file not found!" },
        { m_bWifiSelect, &m_strWifiFilePath, "Please set WIFI file path!", "WIFI file not found!" },
        { m_bFontFileSelect, &m_strFontFilePath, "Please set Font file path!", "Font file not found!" },
        { m_bParamFileSelect, &m_strParamFilePath, "Please set Param file path!", "Param file not found!" },
        { m_bDataFileSelect, &m_strDataFilePath, "Please set Data file path!", "Data file not found!" },
        { m_bLogoSelect, &m_strLogoFilePath, "Please set Logo file path!", "Logo file not found!" },
        { m_bBaleFileSelect, &m_strBaleFilePath, "Please set Bale file path!", "Bale file not found!" },
    };
    int i;
    for (i = 0; i < (int)(sizeof(items)/sizeof(items[0])); i++)
    {
        if (!items[i].sel) continue;
        if (items[i].path->IsEmpty())
        {
            MyShowMessage((char*)items[i].emptyMsg);
            return FALSE;
        }
        FILE *fp = fopen(items[i].path->GetBuffer(0), "rb");
        if (NULL == fp)
        {
            MyShowMessage((char*)items[i].missMsg);
            return FALSE;
        }
        fclose(fp);
    }

    /* D67 OS size */
    if (m_bVOSSelect)
    {
        FILE *fp = fopen(m_strVosFilePath.GetBuffer(0), "rb");
        if (fp)
        {
            fseek(fp, 0, SEEK_END);
            long sz = ftell(fp);
            fclose(fp);
            if (sz > (2*1024*1024) || sz < (50*1024))
            {
                MyShowMessage("OS file size out of range!");
                return FALSE;
            }
        }
    }
    /* D66 Param JSON */
    if (m_bParamFileSelect)
    {
        if (EnvJsonCheckFile(m_strParamFilePath.GetBuffer(0)) <= 0)
        {
            MyShowMessage("Param JSON check failed!");
            return FALSE;
        }
    }
    return TRUE;
}

void CPageApplication::ApplyModeUI(BOOL bFactory)
{
    CWnd *p;
    /* D26: no device tree either mode */
    p = GetDlgItem(IDC_TREE_FILE_LIST); if (p) p->ShowWindow(SW_HIDE);
    p = GetDlgItem(IDC_BUTTON_REFURBISH); if (p) p->ShowWindow(SW_HIDE);
    p = GetDlgItem(IDC_BUTTON_DELETE_APP); if (p) p->ShowWindow(SW_HIDE);
    p = GetDlgItem(IDC_BUTTON_DELETE_APPEND_FILE); if (p) p->ShowWindow(SW_HIDE);

    p = GetDlgItem(IDC_LIST_FACTORY_STATUS);
    if (p) p->ShowWindow(bFactory ? SW_SHOW : SW_HIDE);
    p = GetDlgItem(IDC_STATIC_FACTORY_ACTIVE);
    if (p)
    {
        p->ShowWindow(bFactory ? SW_SHOW : SW_HIDE);
        if (bFactory) SetDlgItemText(IDC_STATIC_FACTORY_ACTIVE, "Active: 0/12");
    }

    /* D34: Sync time developer only */
    p = GetDlgItem(IDC_CHECK_SET_TIME);
    if (p)
    {
        if (bFactory)
        {
            m_bSetTimeSelect = FALSE;
            UpdateData(FALSE);
            p->ShowWindow(SW_HIDE);
        }
        else
        {
            p->ShowWindow(SW_SHOW);
        }
    }

    CheckRadioButton(IDC_RADIO_MODE_DEVELOPER, IDC_RADIO_MODE_FACTORY,
        bFactory ? IDC_RADIO_MODE_FACTORY : IDC_RADIO_MODE_DEVELOPER);

    if (g_pCLoadToolsDlg)
    {
        g_pCLoadToolsDlg->ApplySheetMode(bFactory);
    }
}




void CPageApplication::OnRadioModeDeveloper()
{
    if (g_bThreadRunFlag)
    {
        MessageBox("Stop download before switching mode!");
        CheckRadioButton(IDC_RADIO_MODE_DEVELOPER, IDC_RADIO_MODE_FACTORY,
            (DL_MODE_FACTORY == m_iDownloadMode) ? IDC_RADIO_MODE_FACTORY : IDC_RADIO_MODE_DEVELOPER);
        return;
    }
    if (DL_MODE_FACTORY == m_iDownloadMode)
    {
        CaptureSelToCfg(m_cfgFac);
        SaveFactoryCfgFile();
        ApplyCfgToSel(m_cfgDev);
    }
    m_iDownloadMode = DL_MODE_DEVELOPER;
    ApplyModeUI(FALSE);
}

void CPageApplication::OnRadioModeFactory()
{
    if (g_bThreadRunFlag)
    {
        MessageBox("Stop download before switching mode!");
        CheckRadioButton(IDC_RADIO_MODE_DEVELOPER, IDC_RADIO_MODE_FACTORY,
            (DL_MODE_FACTORY == m_iDownloadMode) ? IDC_RADIO_MODE_FACTORY : IDC_RADIO_MODE_DEVELOPER);
        return;
    }
    if (DL_MODE_DEVELOPER == m_iDownloadMode)
    {
        CaptureSelToCfg(m_cfgDev);
        LoadFactoryCfgFile();
        ApplyCfgToSel(m_cfgFac);
    }
    m_iDownloadMode = DL_MODE_FACTORY;
    ApplyModeUI(TRUE);
}


void CPageApplication::CaptureSelToCfg(DL_SEL_CFG &cfg)
{
    UpdateData(TRUE);
    cfg.bVOS = m_bVOSSelect; cfg.bApp = m_bAppSelect; cfg.bSc = m_bScAppSelect;
    cfg.bWifi = m_bWifiSelect; cfg.bFont = m_bFontFileSelect; cfg.bParam = m_bParamFileSelect;
    cfg.bData = m_bDataFileSelect; cfg.bLogo = m_bLogoSelect; cfg.bBale = m_bBaleFileSelect;
    cfg.bSync = m_bSetTimeSelect;
    memset(cfg.szVos,0,256); strncpy(cfg.szVos, m_strVosFilePath, 255);
    memset(cfg.szApp,0,256); strncpy(cfg.szApp, m_strAppFilePath, 255);
    memset(cfg.szSc,0,256); strncpy(cfg.szSc, m_strScAppFilePath, 255);
    memset(cfg.szWifi,0,256); strncpy(cfg.szWifi, m_strWifiFilePath, 255);
    memset(cfg.szFont,0,256); strncpy(cfg.szFont, m_strFontFilePath, 255);
    memset(cfg.szParam,0,256); strncpy(cfg.szParam, m_strParamFilePath, 255);
    memset(cfg.szData,0,256); strncpy(cfg.szData, m_strDataFilePath, 255);
    memset(cfg.szLogo,0,256); strncpy(cfg.szLogo, m_strLogoFilePath, 255);
    memset(cfg.szBale,0,256); strncpy(cfg.szBale, m_strBaleFilePath, 255);
}

void CPageApplication::ApplyCfgToSel(const DL_SEL_CFG &cfg)
{
    m_bVOSSelect = cfg.bVOS; m_bAppSelect = cfg.bApp; m_bScAppSelect = cfg.bSc;
    m_bWifiSelect = cfg.bWifi; m_bFontFileSelect = cfg.bFont; m_bParamFileSelect = cfg.bParam;
    m_bDataFileSelect = cfg.bData; m_bLogoSelect = cfg.bLogo; m_bBaleFileSelect = cfg.bBale;
    m_bSetTimeSelect = cfg.bSync;
    m_strVosFilePath = cfg.szVos; m_strAppFilePath = cfg.szApp; m_strScAppFilePath = cfg.szSc;
    m_strWifiFilePath = cfg.szWifi; m_strFontFilePath = cfg.szFont; m_strParamFilePath = cfg.szParam;
    m_strDataFilePath = cfg.szData; m_strLogoFilePath = cfg.szLogo; m_strBaleFilePath = cfg.szBale;
    UpdateData(FALSE);
}

void CPageApplication::LoadFactoryCfgFile()
{
    FILE *fp = fopen("config_factory.hjb", "rb");
    if (!fp) { memset(&m_cfgFac, 0, sizeof(m_cfgFac)); return; }
    fread(&m_cfgFac, 1, sizeof(m_cfgFac), fp);
    fclose(fp);
}

void CPageApplication::SaveFactoryCfgFile()
{
    FILE *fp = fopen("config_factory.hjb", "wb");
    if (!fp) return;
    fwrite(&m_cfgFac, 1, sizeof(m_cfgFac), fp);
    fclose(fp);
}


/****************************************************************************
  Factory one-to-many implementation
****************************************************************************/
static int EnumComPorts(int *ports, int maxPorts)
{
    char buf[65536];
    char *p;
    int n = 0;
    int i, j;
    DWORD nRet = QueryDosDevice(NULL, buf, sizeof(buf) / sizeof(buf[0]));
    if (0 == nRet)
    {
        /* Fallback: probe COM1..COM256 */
        for (i = 1; i <= 256 && n < maxPorts; i++)
        {
            char name[32];
            HANDLE h;
            sprintf(name, "\\.\\COM%d", i);
            h = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                OPEN_EXISTING, 0, NULL);
            if (h != INVALID_HANDLE_VALUE)
            {
                CloseHandle(h);
                ports[n++] = i;
            }
        }
    }
    else
    {
        for (p = buf; *p; p += strlen(p) + 1)
        {
            int num;
            if (0 != strncmp(p, "COM", 3))
                continue;
            num = atoi(p + 3);
            if (num <= 0)
                continue;
            if (n < maxPorts)
                ports[n++] = num;
        }
    }
    /* Unique + ascending sort so scan order is stable. */
    for (i = 0; i < n; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (ports[j] == ports[i])
            {
                ports[j] = ports[n - 1];
                n--;
                j--;
            }
        }
    }
    for (i = 0; i < n; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (ports[j] < ports[i])
            {
                int t = ports[i];
                ports[i] = ports[j];
                ports[j] = t;
            }
        }
    }
    return n;
}

void CPageApplication::FactoryInitSlots()
{
    int i;
    EnterCriticalSection(&m_csFactory);
    for (i = 0; i < FACTORY_MAX_SLOTS; i++)
    {
        if (m_factorySlots[i].hThread)
        {
            /* should not happen at start */
        }
        memset(&m_factorySlots[i], 0, sizeof(m_factorySlots[i]));
        m_factorySlots[i].hComm = INVALID_HANDLE_VALUE;
        m_factorySlots[i].listRow = -1;
    }
    m_nFactoryActive = 0;
    m_nFactoryDonePorts = 0;
    LeaveCriticalSection(&m_csFactory);
    if (m_listFactoryStatus.GetSafeHwnd())
        m_listFactoryStatus.DeleteAllItems();
    FactoryUpdateActiveLabel();
}

int CPageApplication::FactoryCountBusy()
{
    int i, n = 0;
    EnterCriticalSection(&m_csFactory);
    for (i = 0; i < FACTORY_MAX_SLOTS; i++)
    {
        if (m_factorySlots[i].busy)
            n++;
    }
    LeaveCriticalSection(&m_csFactory);
    return n;
}

void CPageApplication::FactoryUpdateActiveLabel()
{
    char sz[80];
    int n = FactoryCountBusy();
    if (n >= FACTORY_MAX_SLOTS)
        sprintf(sz, "Active: %d/12  Slots full", n);
    else
        sprintf(sz, "Active: %d/12", n);
    if (GetSafeHwnd())
        SetDlgItemText(IDC_STATIC_FACTORY_ACTIVE, sz);
}

void CPageApplication::FactoryPostStatus(int slot, const char *prog, const char *stat)
{
    if (slot < 0 || slot >= FACTORY_MAX_SLOTS)
        return;
    EnterCriticalSection(&m_csFactory);
    if (prog)
    {
        strncpy(m_factorySlots[slot].progress, prog, sizeof(m_factorySlots[slot].progress) - 1);
        m_factorySlots[slot].progress[sizeof(m_factorySlots[slot].progress) - 1] = 0;
    }
    if (stat)
    {
        strncpy(m_factorySlots[slot].status, stat, sizeof(m_factorySlots[slot].status) - 1);
        m_factorySlots[slot].status[sizeof(m_factorySlots[slot].status) - 1] = 0;
    }
    LeaveCriticalSection(&m_csFactory);
    if (GetSafeHwnd())
        PostMessage(WM_FACTORY_UI_UPDATE, (WPARAM)slot, 0);
}

void CPageApplication::FactoryUpdateListRow(int slot)
{
    char szCom[32];
    FACTORY_SLOT *ps;
    int row;
    if (slot < 0 || slot >= FACTORY_MAX_SLOTS)
        return;
    if (!m_listFactoryStatus.GetSafeHwnd())
        return;
    EnterCriticalSection(&m_csFactory);
    ps = &m_factorySlots[slot];
    row = ps->listRow;
    sprintf(szCom, "COM%d", ps->portNo);
    if (row < 0)
    {
        row = m_listFactoryStatus.InsertItem(m_listFactoryStatus.GetItemCount(), szCom);
        ps->listRow = row;
    }
    else
    {
        m_listFactoryStatus.SetItemText(row, 0, szCom);
    }
    m_listFactoryStatus.SetItemText(row, 1, ps->sn);
    m_listFactoryStatus.SetItemText(row, 2, ps->progress);
    m_listFactoryStatus.SetItemText(row, 3, ps->status);
    LeaveCriticalSection(&m_csFactory);
    FactoryUpdateActiveLabel();
}

LRESULT CPageApplication::OnFactoryUiUpdate(WPARAM wParam, LPARAM lParam)
{
    FactoryUpdateListRow((int)wParam);
    return 0;
}

LRESULT CPageApplication::OnFactoryUiEnable(WPARAM wParam, LPARAM lParam)
{
    /* wParam: TRUE => idle (Start on, Stop off); FALSE => running (Start off, Stop on) */
    (void)lParam;
    MyEnableWindows(wParam ? TRUE : FALSE);
    return 0;
}

LRESULT CPageApplication::OnFactoryScanMsg(WPARAM wParam, LPARAM lParam)
{
    char *msg = (char *)lParam;
    (void)wParam;
    if (msg)
    {
        MyShowMessage(msg);
        free(msg);
    }
    return 0;
}

void CPageApplication::FactoryPostEnable(BOOL bStartEnable)
{
    if (GetSafeHwnd())
        PostMessage(WM_FACTORY_UI_ENABLE, (WPARAM)(bStartEnable ? TRUE : FALSE), 0);
}

void CPageApplication::FactoryPostScanMsg(const char *msg)
{
    char *copy;
    if (!msg || !GetSafeHwnd())
        return;
    copy = (char *)malloc(strlen(msg) + 1);
    if (!copy)
        return;
    strcpy(copy, msg);
    if (!PostMessage(WM_FACTORY_SCAN_MSG, 0, (LPARAM)copy))
        free(copy);
}


BOOL CPageApplication::FactoryPortIsDone(int portNo)
{
    int i;
    BOOL found = FALSE;
    EnterCriticalSection(&m_csFactory);
    for (i = 0; i < m_nFactoryDonePorts; i++)
    {
        if (m_factoryDonePorts[i] == portNo)
        {
            found = TRUE;
            break;
        }
    }
    LeaveCriticalSection(&m_csFactory);
    return found;
}

void CPageApplication::FactoryMarkPortDone(int portNo)
{
    int i;
    EnterCriticalSection(&m_csFactory);
    for (i = 0; i < m_nFactoryDonePorts; i++)
    {
        if (m_factoryDonePorts[i] == portNo)
        {
            LeaveCriticalSection(&m_csFactory);
            return;
        }
    }
    if (m_nFactoryDonePorts < 256)
    {
        m_factoryDonePorts[m_nFactoryDonePorts] = portNo;
        m_factoryDoneMiss[m_nFactoryDonePorts] = 0;
        m_nFactoryDonePorts++;
    }
    LeaveCriticalSection(&m_csFactory);
}

/* Unlock Success only when COM vanishes from enum (unplug). */
void CPageApplication::FactoryPruneDonePorts(const int *ports, int nPorts)
{
    int i, j, k;
    EnterCriticalSection(&m_csFactory);
    for (i = 0; i < m_nFactoryDonePorts; )
    {
        int p = m_factoryDonePorts[i];
        BOOL still = FALSE;
        for (j = 0; j < nPorts; j++)
        {
            if (ports[j] == p)
            {
                still = TRUE;
                break;
            }
        }
        if (still)
        {
            m_factoryDoneMiss[i] = 0; /* still present: keep Success lock */
            i++;
        }
        else
        {
            m_factoryDoneMiss[i]++;
            /* 3 consecutive scan misses (~6s) => treat as unplug */
            if (m_factoryDoneMiss[i] >= 3)
            {
                for (k = i; k < m_nFactoryDonePorts - 1; k++)
                {
                    m_factoryDonePorts[k] = m_factoryDonePorts[k + 1];
                    m_factoryDoneMiss[k] = m_factoryDoneMiss[k + 1];
                }
                m_nFactoryDonePorts--;
            }
            else
                i++;
        }
    }
    LeaveCriticalSection(&m_csFactory);
}

int CPageApplication::FactoryAllocSlot(int portNo, HANDLE hComm, const char *sn)
{
    int i;
    EnterCriticalSection(&m_csFactory);
    for (i = 0; i < FACTORY_MAX_SLOTS; i++)
    {
        if (0 == m_factorySlots[i].busy)
        {
            memset(&m_factorySlots[i], 0, sizeof(m_factorySlots[i]));
            m_factorySlots[i].busy = 1;
            m_factorySlots[i].portNo = portNo;
            m_factorySlots[i].hComm = hComm;
            m_factorySlots[i].listRow = -1;
            m_factorySlots[i].hThread = NULL;
            m_factorySlots[i].hsKind = FAC_HS_NONE;
            m_factorySlots[i].osCheckType = 0;
            if (sn && sn[0])
                strncpy(m_factorySlots[i].sn, sn, 32);
            strcpy(m_factorySlots[i].progress, "0%");
            strcpy(m_factorySlots[i].status, "Starting");
            InterlockedIncrement(&m_nFactoryActive);
            LeaveCriticalSection(&m_csFactory);
            return i;
        }
    }
    LeaveCriticalSection(&m_csFactory);
    return -1;
}

void CPageApplication::FactoryReleaseSlot(int slot)
{
    HANDLE h;
    if (slot < 0 || slot >= FACTORY_MAX_SLOTS)
        return;
    EnterCriticalSection(&m_csFactory);
    h = m_factorySlots[slot].hComm;
    m_factorySlots[slot].hComm = INVALID_HANDLE_VALUE;
    m_factorySlots[slot].busy = 0;
    m_factorySlots[slot].hThread = NULL;
    InterlockedDecrement(&m_nFactoryActive);
    LeaveCriticalSection(&m_csFactory);
    if (h != NULL && h != INVALID_HANDLE_VALUE)
        SerClosePort(h);
    FactoryUpdateActiveLabel();
}

/* Quick L/A handshake for scan; open already done. Optional SN after 'A'. */
static int FactoryQuickHandshakeLA(HANDLE h, char *snOut, int snMax, volatile BOOL *pStop)
{
    BYTE b;
    int i, iRet;
    WORD wLen;
    BYTE aby[128];

    if (snOut && snMax > 0)
        snOut[0] = 0;
    SerBind(h);
    for (i = 0; i < 25; i++) /* ~0.5s */
    {
        if (pStop && *pStop)
        {
            SerUnbind();
            return 1;
        }
        SerWrite((BYTE *)"L", 1);
        b = 0;
        iRet = SerRead(&b, 1, 20);
        if ((0 == iRet) && (b == 'A'))
        {
            /* Optional SN CRC16 frame (10.5); short wait */
            iRet = Crc16RecvPacket(aby, &wLen, 80);
            if ((0 == iRet) && (wLen >= 8))
            {
                /* DATA at [5]: hs_ver, hs_status, sn_len, sn... */
                BYTE snLen = aby[7];
                if ((0 == aby[6]) && snLen > 0 && snLen <= 32 && snOut)
                {
                    int copy = snLen;
                    if (copy >= snMax)
                        copy = snMax - 1;
                    memcpy(snOut, &aby[8], copy);
                    snOut[copy] = 0;
                }
            }
            /* keep bound for caller; unbind after alloc decision */
            return 0;
        }
    }
    SerUnbind();
    return 1;
}

/* OS-first factory scan: full legacy handshake T/S(or R) then Q/P while
 * device is still in Q-wait (UsbFileDl do_legacy_os_receive). Worker must
 * NOT re-send Q; it starts 0xB0 immediately. */
static int FactoryQuickHandshakeTS(HANDLE h, int *pCheckType, volatile BOOL *pStop)
{
    BYTE b;
    int i, iRet, k, gotP;

    if (pCheckType)
        *pCheckType = CRC32_CHECK_TYPE;
    SerBind(h);
    for (i = 0; i < 25; i++) /* ~0.5s */
    {
        if (pStop && *pStop)
        {
            SerUnbind();
            return 1;
        }
        SerWrite((BYTE *)"T", 1);
        b = 0;
        iRet = SerRead(&b, 1, 20);
        if (0 == iRet)
        {
            if ('R' == b)
            {
                if (pCheckType)
                    *pCheckType = LRC_CHECK_TYPE;
                break;
            }
            if ('S' == b)
            {
                if (pCheckType)
                    *pCheckType = CRC32_CHECK_TYPE;
                break;
            }
        }
        if (i == 24)
        {
            SerUnbind();
            return 1;
        }
    }
    /* Immediately Q/P - device answered S/R and is waiting for Q now. */
    gotP = 0;
    SerWrite((BYTE *)"Q", 1);
    for (k = 0; k < 250; k++) /* ~5s */
    {
        if (pStop && *pStop)
        {
            SerUnbind();
            return 1;
        }
        b = 0;
        iRet = SerRead(&b, 1, 20);
        if ((0 == iRet) && (b == 'P'))
        {
            gotP = 1;
            break;
        }
    }
    if (!gotP)
    {
        SerUnbind();
        return 1;
    }
    PurgeCommBuf(PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
    return 0;
}

int CPageApplication::FactoryRunDeviceSession(int slot)
{
    int iRet = 0;
    int iLast = 0;
    BOOL bOs, bAppSide;
    FACTORY_SLOT *ps = &m_factorySlots[slot];

    t_iFactoryUiSlot = slot;
    SerBind(ps->hComm);
    FactoryPostStatus(slot, "0%", "Connected");

    bOs = m_bVOSSelect;
    bAppSide = (m_bAppSelect || m_bScAppSelect || m_bWifiSelect || m_bFontFileSelect
        || m_bParamFileSelect || m_bDataFileSelect || m_bLogoSelect || m_bBaleFileSelect);

    if (bOs)
    {
        FactoryPostStatus(slot, "0%", "OS...");
        if (FAC_HS_TS == ps->hsKind)
        {
            /* Scan already completed T/S + Q/P; device is in MyDownload. */
            g_iCommCheckType = ps->osCheckType;
            FactoryPostStatus(slot, "0%", "OS handshake OK");
            iRet = 0;
        }
        else
        {
            /* L/A scan (or no prior OS hs): full T + Q/P via DownloadOsStart */
            iRet = DownloadOsStart();
        }
        if (0 == iRet)
        {
            iRet = DownloadOsFile(m_strVosFilePath.GetBuffer(0));
            if (0 == iRet)
            {
                DownloadOsEnd(); /* SerClose no-ops when external */
                FactoryPostStatus(slot, "OS done", "OS OK");
            }
            else
            {
                iLast = iRet;
                FactoryPostStatus(slot, "OS fail", "OS failed");
            }
        }
        else
        {
            iLast = iRet;
            FactoryPostStatus(slot, "OS fail", "OS handshake fail");
        }
        /* Re-bind if DownloadOsEnd cleared TLS */
        if (ps->hComm != INVALID_HANDLE_VALUE)
            SerBind(ps->hComm);
    }

    if (bAppSide && !m_bStopDownloadFlag && !m_bFactoryStopScan)
    {
        FactoryPostStatus(slot, "0%", "Downloading...");
        /* If scan used L/A, SerialHandshack skips (SerIsBound). If scan used T/S
           for OS first, device is back waiting for L after OS �?DownloadStart does L. */
        iRet = DownloadStart();
        if (0 == iRet)
        {
            iRet = DownloadProcess();
            if (0 != iRet)
                iLast = iRet;
            DownloadEnd();
        }
        else
        {
            iLast = iRet;
            if (2 != iRet)
                DownloadEnd();
        }
    }

    SerUnbind();
    if (0 == iLast)
        FactoryPostStatus(slot, "100%", "Success");
    else if (m_bStopDownloadFlag || m_bFactoryStopScan)
        FactoryPostStatus(slot, "-", "Stopped");
    else
        FactoryPostStatus(slot, "-", "Failed");
    t_iFactoryUiSlot = -1;
    return iLast;
}

struct FACTORY_WORKER_ARG {
    CPageApplication *page;
    int slot;
};

static int ThreadFactoryWorker(void *p)
{
    FACTORY_WORKER_ARG *arg = (FACTORY_WORKER_ARG *)p;
    CPageApplication *page = arg->page;
    int slot = arg->slot;
    int portNo;
    int iRet;

    portNo = page->m_factorySlots[slot].portNo;
    delete arg;
    iRet = page->FactoryRunDeviceSession(slot);
    if (0 == iRet)
        page->FactoryMarkPortDone(portNo);
    page->FactoryReleaseSlot(slot);
    return 0;
}

static BOOL FactoryPortBusy(CPageApplication *page, int portNo)
{
    int i;
    EnterCriticalSection(&page->m_csFactory);
    for (i = 0; i < FACTORY_MAX_SLOTS; i++)
    {
        if (page->m_factorySlots[i].busy && page->m_factorySlots[i].portNo == portNo)
        {
            LeaveCriticalSection(&page->m_csFactory);
            return TRUE;
        }
    }
    LeaveCriticalSection(&page->m_csFactory);
    return FALSE;
}

int ThreadFactoryManager(void *pTemp)
{
    CPageApplication *page = (CPageApplication *)pTemp;
    DWORD baud;
    int ports[256];
    int nPorts, i, slot;
    HANDLE h;
    char sn[36];
    DWORD tid;

    g_bThreadRunFlag = TRUE;
    page->FactoryPostEnable(FALSE); /* Start off, Stop on �?via UI thread */
    page->FactoryPostScanMsg("Factory: scanning...");

    baud = g_adwBauds[g_sConfigreInfo.m_iSerialPortBaudsSelect];

    while (!page->m_bFactoryStopScan && !page->m_bStopDownloadFlag)
    {
        BOOL bNeedOs = page->m_bVOSSelect;
        BOOL bNeedApp = (page->m_bAppSelect || page->m_bScAppSelect || page->m_bWifiSelect
            || page->m_bFontFileSelect || page->m_bParamFileSelect || page->m_bDataFileSelect
            || page->m_bLogoSelect || page->m_bBaleFileSelect);
        char szScan[128];
        int nBusy;

        page->FactoryUpdateActiveLabel();
        nBusy = page->FactoryCountBusy();
        if (nBusy >= FACTORY_MAX_SLOTS)
        {
            page->FactoryUpdateActiveLabel();
            Sleep(500);
            continue;
        }

        nPorts = EnumComPorts(ports, 256);
        page->FactoryPruneDonePorts(ports, nPorts);
        nBusy = page->FactoryCountBusy();
        sprintf(szScan, "Factory: scanning %d COM, active %d/12...", nPorts, nBusy);
        page->FactoryPostScanMsg(szScan);

        if (0 == nPorts)
        {
            page->FactoryPostScanMsg("Factory: no COM ports found (check drivers)");
            {
                int w;
                for (w = 0; w < 20 && !page->m_bFactoryStopScan && !page->m_bStopDownloadFlag; w++)
                    Sleep(100);
            }
            continue;
        }

        for (i = 0; i < nPorts; i++)
        {
            int hsKind = FAC_HS_NONE;
            int osCheck = CRC32_CHECK_TYPE;

            if (page->m_bFactoryStopScan || page->m_bStopDownloadFlag)
                break;
            if (page->FactoryCountBusy() >= FACTORY_MAX_SLOTS)
                break;
            if (FactoryPortBusy(page, ports[i]))
                continue;
            if (page->FactoryPortIsDone(ports[i]))
                continue;

            /* D64: open before handshake; silent skip on fail */
            if (0 != SerOpenPort(ports[i], baud, &h))
                continue;

            sn[0] = 0;
            /* OS selected => T/S first (device ServeFromPc). App-only => L/A. */
            if (bNeedOs)
            {
                if (0 != FactoryQuickHandshakeTS(h, &osCheck, &page->m_bFactoryStopScan))
                {
                    SerUnbind();
                    SerClosePort(h);
                    continue;
                }
                hsKind = FAC_HS_TS;
            }
            else if (bNeedApp)
            {
                if (0 != FactoryQuickHandshakeLA(h, sn, sizeof(sn), &page->m_bFactoryStopScan))
                {
                    SerUnbind();
                    SerClosePort(h);
                    continue;
                }
                hsKind = FAC_HS_LA;
            }
            else
            {
                SerClosePort(h);
                continue;
            }
            SerUnbind();

            if (page->FactoryCountBusy() >= FACTORY_MAX_SLOTS)
            {
                SerClosePort(h);
                break;
            }

            slot = page->FactoryAllocSlot(ports[i], h, sn);
            if (slot < 0)
            {
                SerClosePort(h);
                break;
            }
            page->m_factorySlots[slot].hsKind = hsKind;
            page->m_factorySlots[slot].osCheckType = osCheck;
            page->FactoryPostStatus(slot, "0%", "Queuing");
            {
                FACTORY_WORKER_ARG *arg = new FACTORY_WORKER_ARG;
                arg->page = page;
                arg->slot = slot;
                page->m_factorySlots[slot].hThread = CreateThread(NULL, 0,
                    (LPTHREAD_START_ROUTINE)ThreadFactoryWorker, arg, 0, &tid);
                if (NULL == page->m_factorySlots[slot].hThread)
                {
                    page->FactoryPostStatus(slot, "-", "Thread fail");
                    page->FactoryReleaseSlot(slot);
                }
            }
        }

        /* D19: 2s between free-port scan rounds */
        {
            int w;
            for (w = 0; w < 20 && !page->m_bFactoryStopScan && !page->m_bStopDownloadFlag; w++)
                Sleep(100);
        }
    }

    /* Wait workers */
    for (;;)
    {
        if (page->FactoryCountBusy() <= 0)
            break;
        Sleep(200);
    }

    page->FactoryPostScanMsg(page->m_bFactoryStopScan || page->m_bStopDownloadFlag
        ? "Factory stopped." : "Factory idle.");
    page->FactoryPostEnable(TRUE); /* Start on, Stop off */
    page->FactoryUpdateActiveLabel();
    g_bThreadRunFlag = FALSE;
    return 0;
}
