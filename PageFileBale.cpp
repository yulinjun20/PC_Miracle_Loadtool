// PageFileBale.cpp : implementation file
//

#include "stdafx.h"
#include "LoadTools.h"
#include "PageFileBale.h"
#include "LoadToolsDefine.h"
#include "VPOSSelect.h"
#include "Base.h"
#include "DelAppDlg.h"
#include "ViewAtFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bThreadRunFlag;

extern int g_iSelectIndex;

extern int g_iFunctionSelect;      // 0：机型选择界面；1字库类型选择界面

extern char g_strDelAppName[33];

extern char g_strCurrentDir[300];

extern char g_strInputAppName[256];

char *g_strBaleConfigFileName = "BaleConfig.hjb";

extern char g_strVposSelect[VPOS_TYPE_NUMBER][32];

extern BYTE g_abyPosType[VPOS_TYPE_NUMBER];

extern CONFIG_INFO g_sConfigreInfo;

extern void UpdateConfigure(void);
extern int CheckDataFile(char *strFileName);
extern void DwordToByte(DWORD dwIn, BYTE *pbyOut);
extern void AddFileToBaleFile(FILE *fpBaleFile, FILE *fp, int *piOutLen);
extern int FileSign(char *strFileName);


/////////////////////////////////////////////////////////////////////////////
// CPageFileBale property page

IMPLEMENT_DYNCREATE(CPageFileBale, CPropertyPage)

CPageFileBale::CPageFileBale() : CPropertyPage(CPageFileBale::IDD)
{
    //{{AFX_DATA_INIT(CPageFileBale)
    //}}AFX_DATA_INIT
}

CPageFileBale::~CPageFileBale()
{
}

void CPageFileBale::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPageFileBale)
    DDX_Control(pDX, IDC_TREE_TASK_LIST, m_tcTreeCtrlTaskList);
    
    DDX_Control(pDX, IDC_BUTTON_ADD_APP, m_btnAddAppFile);
    DDX_Control(pDX, IDC_BUTTON_ADD_FONT, m_btnAddFontFile);
    DDX_Control(pDX, IDC_BUTTON_ADD_PARAM, m_btnAddParamFile);
    DDX_Control(pDX, IDC_BUTTON_ADD_DATA_FILE, m_btnAddDataFile);
    DDX_Control(pDX, IDC_BUTTON_ADD_LOGO, m_btnAddLogoFile);
    DDX_Control(pDX, IDC_BUTTON_ADD_WIFI, m_btnAddWifiFile);
    DDX_Control(pDX, IDC_BUTTON_ADD_APP_DELETE, m_btnAddDeleteApp);
    DDX_Control(pDX, IDC_BUTTON_ADD_CREATE_FILE_SYSTEM, m_btnAddCreateFileSystem);
    DDX_Control(pDX, IDC_BUTTON_DEL_TASK, m_btnDeleteTask);
    DDX_Control(pDX, IDC_BUTTON_START_BALE, m_btnStartBale);
    DDX_Control(pDX, IDC_BUTTON_ADD_VOS, m_btnAddVosFile);
    DDX_Control(pDX, IDC_BUTTON_ADD_DEL_ALL_APP, m_btnAddDelAllApp);
    DDX_Control(pDX, IDC_BUTTON_SAVE_INFO, m_btnSaveInfo);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageFileBale, CPropertyPage)
    //{{AFX_MSG_MAP(CPageFileBale)
    ON_BN_CLICKED(IDC_BUTTON_ADD_APP, OnButtonAddApp)
    ON_BN_CLICKED(IDC_BUTTON_ADD_FONT, OnButtonAddFont)
    ON_BN_CLICKED(IDC_BUTTON_ADD_PARAM, OnButtonAddParam)
    ON_BN_CLICKED(IDC_BUTTON_ADD_CREATE_FILE_SYSTEM, OnButtonAddCreateFileSystem)
    ON_BN_CLICKED(IDC_BUTTON_ADD_APP_DELETE, OnButtonAddAppDelete)
    ON_BN_CLICKED(IDC_BUTTON_DEL_TASK, OnButtonDelTask)
    ON_BN_CLICKED(IDC_BUTTON_START_BALE, OnButtonStartBale)
    ON_BN_CLICKED(IDC_BUTTON_ADD_VOS, OnButtonAddVos)
    ON_BN_CLICKED(IDC_BUTTON_ADD_DEL_ALL_APP, OnButtonDelAllApp)
    ON_BN_CLICKED(IDC_RADIO_REMOTE_DOWNLOAD, OnRadioRemoteDownload)
    ON_BN_CLICKED(IDC_RADIO_LOCAL_DOWNLOAD, OnRadioLocalDownload)
    ON_BN_CLICKED(IDC_BUTTON_SAVE_INFO, OnButtonSaveInfo)
    ON_BN_CLICKED(IDC_BUTTON_ADD_DATA_FILE, OnButtonAddDataFile)
    ON_BN_CLICKED(IDC_BUTTON_ADD_LOGO, OnButtonAddLogo)
    ON_BN_CLICKED(IDC_BUTTON_ADD_WIFI, OnButtonAddWifi)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageFileBale message handlers

BOOL CPageFileBale::OnInitDialog()
{
//    int i;
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
    
    m_bAddVosFlag = FALSE;
    m_bAddScAppFlag = FALSE;
    m_bAddLogoFlag = FALSE;
    m_bAddWifiFlag = FALSE;
    m_iAppFileNumber = 0;
    m_iAppendFileNumber = 0;

    m_iDelAppTaskNumber = 0;
    m_sDelAllApp.m_byValidFlag = OBJECT_INVALID;
    m_sCreateSystem.m_byValidFlag = OBJECT_INVALID;

    UpdateData(FALSE);

    m_btnAddAppFile.SetIcon(IDI_ICON_UP,NULL, 40, 32);
    m_btnAddAppFile.SetShowText(TRUE);
    m_btnAddAppFile.SetActiveBgColor(RGB(200,200,200), FALSE);
    SetDlgItemText(IDC_BUTTON_ADD_APP, "App");
        
    m_btnAddFontFile.SetIcon(IDI_ICON_UP,NULL, 40, 32);
    m_btnAddFontFile.SetShowText(TRUE);
    m_btnAddFontFile.SetActiveBgColor(RGB(200,200,200), FALSE);
    
    m_btnAddParamFile.SetIcon(IDI_ICON_UP,NULL, 40, 32);
    m_btnAddParamFile.SetShowText(TRUE);
    m_btnAddParamFile.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnAddDataFile.SetIcon(IDI_ICON_UP,NULL, 40, 32);
    m_btnAddDataFile.SetShowText(TRUE);
    m_btnAddDataFile.SetActiveBgColor(RGB(200,200,200), FALSE);
    SetDlgItemText(IDC_BUTTON_ADD_DATA_FILE, "SC App");

    m_btnAddLogoFile.SetIcon(IDI_ICON_UP,NULL, 40, 32);
    m_btnAddLogoFile.SetShowText(TRUE);
    m_btnAddLogoFile.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnAddWifiFile.SetIcon(IDI_ICON_UP,NULL, 40, 32);
    m_btnAddWifiFile.SetShowText(TRUE);
    m_btnAddWifiFile.SetActiveBgColor(RGB(200,200,200), FALSE);
    /* WiFi packing removed: download WiFi only on Download page. */
    GetDlgItem(IDC_BUTTON_ADD_WIFI)->ShowWindow(SW_HIDE);

    m_btnAddDeleteApp.SetIcon(IDI_ICON_UP,NULL, 40, 32);
    m_btnAddDeleteApp.SetShowText(TRUE);
    m_btnAddDeleteApp.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnAddCreateFileSystem.SetIcon(IDI_ICON_UP, NULL, 40, 32);
    m_btnAddCreateFileSystem.SetShowText(TRUE);
    m_btnAddCreateFileSystem.SetActiveBgColor(RGB(200,200,200), FALSE);
    /* Data / Del.* / Remote / Create FS removed from Local-only Task bale. */
    GetDlgItem(IDC_BUTTON_ADD_CREATE_FILE_SYSTEM)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BUTTON_ADD_APP_DELETE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BUTTON_ADD_DEL_ALL_APP)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_RADIO_REMOTE_DOWNLOAD)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CHECK_RMOTE_COMPRESS)->ShowWindow(SW_HIDE);
    
    m_btnDeleteTask.SetIcon(IDI_ICON_DELETE1, NULL, 45, 32);
    m_btnDeleteTask.SetShowText(TRUE);
    m_btnDeleteTask.SetActiveBgColor(RGB(200,200,200), FALSE);
    
    m_btnStartBale.SetIcon(IDI_ICON_BALE, NULL, 55, 32);
    m_btnStartBale.SetShowText(TRUE);
    m_btnStartBale.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnAddVosFile.SetIcon(IDI_ICON_UP, NULL, 40, 32);
    m_btnAddVosFile.SetShowText(TRUE);
    m_btnAddVosFile.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnAddDelAllApp.SetIcon(IDI_ICON_UP, NULL, 40, 32);
    m_btnAddDelAllApp.SetShowText(TRUE);
    m_btnAddDelAllApp.SetActiveBgColor(RGB(200,200,200), FALSE);
    
    m_btnSaveInfo.SetIcon(IDI_ICON_SAVE_FILE, NULL, 45, 32);
    m_btnSaveInfo.SetShowText(TRUE);
    m_btnSaveInfo.SetActiveBgColor(RGB(200,200,200), FALSE);

    ((CButton *)GetDlgItem(IDC_CHECK_RMOTE_COMPRESS))->SetCheck(TRUE);

    /* Task bale: Local load only; App / SC / OS / Font / Param / Logo. */
    g_sConfigreInfo.m_byLoadType = LOCAL_DOWNLOAD;
    ((CButton *)GetDlgItem(IDC_RADIO_LOCAL_DOWNLOAD))->SetCheck(TRUE);
    ((CButton *)GetDlgItem(IDC_RADIO_REMOTE_DOWNLOAD))->SetCheck(FALSE);
    GetDlgItem(IDC_BUTTON_ADD_VOS)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_ADD_DEL_ALL_APP)->EnableWindow(FALSE);
    GetConfig();
    return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CPageFileBale::IsThreadRun()
{
    return g_bThreadRunFlag;
}

BOOL CPageFileBale::OnKillActive() 
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
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-07   V1.0        创建
****************************************************************************/
void CPageFileBale::OnButtonAddApp() 
{
    // TODO: Add your control notification handler code here
    int i;
    char strDisplayInfo[BALE_DISPLAY_INFO_LEN];
	static char BASED_CODE szMergeFilter[] = _T("Application(*.bin)|*.bin||)");
    CString filter, strTemp;

    if (m_iAppFileNumber >= APP_FILE_MAX_NUM)
    {
        MessageBox("Application number is maximal");
        return ;
    }

    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, strTemp, 
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST,filter, 0);

    if (dlg.DoModal() == IDOK)
    {
        strTemp = dlg.GetPathName();
        i = CheckApplication(strTemp.GetBuffer(0));
        switch (i)
        {
        case -1:
            MessageBox("The file is not ViewAt application!");
            return;
        case -2:
            MessageBox("Open application file error!");
            return;
        case -3:
            MessageBox("Read application file error!");
            return;
        case -4:
            MessageBox("Application's main function entry address error!");
            return;
        case -5:
            MessageBox("Application's event function entry address error!");
            return;
        case -6:
            MessageBox("Application's application information struct function entry address error!");
            return;
        default:
            break;
        }

        /* UN650: skip VPOS type dialog; keep a fixed default for bale header. */
        if (g_sConfigreInfo.m_byPosType >= VPOS_TYPE_NUMBER)
        {
            g_sConfigreInfo.m_byPosType = 0;
        }
        memset(strDisplayInfo, 0, sizeof(strDisplayInfo));
        if (_snprintf(strDisplayInfo, sizeof(strDisplayInfo), "{APP}{%s}{%s}",
                &g_strVposSelect[g_sConfigreInfo.m_byPosType][0],
                strTemp.GetBuffer(0)) < 0)
        {
            MessageBox("Application path is too long.");
            return ;
        }
        strDisplayInfo[sizeof(strDisplayInfo) - 1] = 0;

        for (i=0; i<m_iAppFileNumber; i++)
        {
            if (0 == strcmp(strDisplayInfo,(char*)m_sAppFileInfo[i].m_abyDisplayInfo))
            {
                MessageBox("The application is already in the task list!");
                return ;
            }
        }
        strcpy((char*)m_sAppFileInfo[m_iAppFileNumber].m_abyDisplayInfo, 
            strDisplayInfo);
        m_sAppFileInfo[m_iAppFileNumber].m_hTree 
            = m_tcTreeCtrlTaskList.InsertItem(strDisplayInfo);
        m_iAppFileNumber++;
        /* One App per task: gray out and disable the button once added. */
        GetDlgItem(IDC_BUTTON_ADD_APP)->EnableWindow(FALSE);
        UpdateData(FALSE);    // 更新界面
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
  1、 黄俊斌     2007-09-07   V1.0        创建
****************************************************************************/
void CPageFileBale::OnButtonAddFont() 
{
    // TODO: Add your control notification handler code here
    int iRet;
    static char BASED_CODE szMergeFilter[] = "Font (*.bin)|*.bin||";
    CString filter, strTemp;
    LPCSTR pszPath;

    /* FlatButton may still hold mouse capture from the click; release before modal. */
    m_btnAddFontFile.CancelHover();

    filter = szMergeFilter;
    CFileDialog dlg(TRUE, NULL, NULL,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, filter, this);

    if (dlg.DoModal() != IDOK)
    {
        return ;
    }

    strTemp = dlg.GetPathName();
    pszPath = (LPCSTR)strTemp;
    iRet = CheckViewAtFont((char*)pszPath);
    if (-2 == iRet)
    {
        MessageBox("Open font file error!");
        return ;
    }
    if (-1 == iRet)
    {
        MessageBox("The file is not ViewAt font file!");
        return ;
    }

    /* UN650: one shared UFL font; skip Thermal/Stylus dialog, always THERMAL. */
    g_sConfigreInfo.m_byPrinterType = 0;
    if (_snprintf((char*)m_sFontFileInfo.m_abyDisplayInfo, BALE_DISPLAY_INFO_LEN,
            "{FONT}{THERMAL}{%s}", pszPath) < 0)
    {
        MessageBox("Font path is too long.");
        return ;
    }
    m_sFontFileInfo.m_abyDisplayInfo[BALE_DISPLAY_INFO_LEN - 1] = 0;
    m_sFontFileInfo.m_hTree = m_tcTreeCtrlTaskList.InsertItem(
        (char*)m_sFontFileInfo.m_abyDisplayInfo);
    m_btnAddFontFile.CancelHover();
    GetDlgItem(IDC_BUTTON_ADD_FONT)->EnableWindow(FALSE);
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-07   V1.0        创建
  2、 黄俊斌     2008-05-19   V1.1        增加带有所属应用名的参数
****************************************************************************/
/* One Param per bale task list: gray Add Param when any PARAM/PARA exists. */
static void UpdateParamAddButton(CPageFileBale *pPage)
{
    int i;
    int iHas = 0;

    for (i = 0; i < pPage->m_iAppendFileNumber; i++)
    {
        if (   (BALE_TASK_PARAM == pPage->m_sAppendFileInf[i].m_byTaskType)
            || (BALE_TASK_PARA == pPage->m_sAppendFileInf[i].m_byTaskType) )
        {
            iHas = 1;
            break;
        }
    }
    pPage->GetDlgItem(IDC_BUTTON_ADD_PARAM)->EnableWindow(iHas ? FALSE : TRUE);
}

void CPageFileBale::OnButtonAddParam() 
{
    // TODO: Add your control notification handler code here
    HTREEITEM hTempTree;
    int i, iFlag = 0, iAppNameSelectFlag = 0;

    hTempTree = m_tcTreeCtrlTaskList.GetSelectedItem();
    for (i=0; i<m_iAppFileNumber; i++)
    {
        if (hTempTree == m_sAppFileInfo[i].m_hTree)
        {
            iFlag = 1;
            break;
        }
    }
    /* Local: attach to selected App when possible; else standalone Param. */
    if (0 == iFlag)
    {
        iAppNameSelectFlag = 1; /* standalone */
    }
    else
    {
        iAppNameSelectFlag = 0; /* under selected App */
    }

	static char BASED_CODE szMergeFilter[] = _T("Param file(*.*)|*.*||)");
    CString filter, strTemp;
    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, strTemp, 
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST,filter, 0);

    if (dlg.DoModal() != IDOK)
    {
        return ;
    }

    strTemp = dlg.GetPathName();
    if (!CheckParamFileNameValidate(strTemp.GetBuffer(0)))
    {
        MessageBox("Param file name's size must be 1 to 15 bytes.");
        return ;
    }
    i = CheckParam(strTemp.GetBuffer(0));
    if (-2 == i)
    {
        MessageBox("Open param file failed.");
        return ;
    }
    if (-1 == i)
    {
        MessageBox("This file is not a param file.");
        return ;
    }

    if (m_iAppendFileNumber >= APPEND_FILE_MAX_NUM)
    {
        MessageBox("Append file number is maximal.");
        return ;
    }
    if (0 == iAppNameSelectFlag)
    {
        m_sAppendFileInf[m_iAppendFileNumber].m_hFatherTree = hTempTree;
        m_sAppendFileInf[m_iAppendFileNumber].m_byTaskType = BALE_TASK_PARAM;
        if (_snprintf((char*)m_sAppendFileInf[m_iAppendFileNumber].m_abyDisplayInfo,
                BALE_DISPLAY_INFO_LEN, "{PARAM}{%s}", strTemp.GetBuffer(0)) < 0)
        {
            MessageBox("Param path is too long.");
            return ;
        }
        for (i=0; i<m_iAppendFileNumber; i++)
        {
            if (BALE_TASK_PARAM == m_sAppendFileInf[i].m_byTaskType)
            {
                iFlag = strcmp(
                    (char*)m_sAppendFileInf[m_iAppendFileNumber].m_abyDisplayInfo,
                    (char*)m_sAppendFileInf[i].m_abyDisplayInfo);
                if (0 == iFlag)
                {
                    MessageBox("You can not add a param to the same application.");
                    return ;
                }
            }
        }
        m_sAppendFileInf[m_iAppendFileNumber].m_hTree 
            = m_tcTreeCtrlTaskList.InsertItem(
            (char*)m_sAppendFileInf[m_iAppendFileNumber].m_abyDisplayInfo, 
            hTempTree, TVI_SORT);
        m_iAppendFileNumber++;
        m_tcTreeCtrlTaskList.Expand(hTempTree, TVE_EXPAND);
        UpdateParamAddButton(this);
    }
    else
    {
        /* Standalone Param for Local (no App name dialog). */
        m_sAppendFileInf[m_iAppendFileNumber].m_byTaskType = BALE_TASK_PARA;
        if (_snprintf((char*)m_sAppendFileInf[m_iAppendFileNumber].m_abyDisplayInfo,
                BALE_DISPLAY_INFO_LEN, "{PARA}{_}{%s}",
                strTemp.GetBuffer(0)) < 0)
        {
            MessageBox("Param path is too long.");
            return ;
        }
        for (i=0; i<m_iAppendFileNumber; i++)
        {
            if (BALE_TASK_PARA == m_sAppendFileInf[i].m_byTaskType)
            {
                iFlag = strcmp(
                    (char*)m_sAppendFileInf[m_iAppendFileNumber].m_abyDisplayInfo,
                    (char*)m_sAppendFileInf[i].m_abyDisplayInfo);
                if (0 == iFlag)
                {
                    MessageBox("You can't add a param to the same application.");
                    return ;
                }
            }
        }
        m_sAppendFileInf[m_iAppendFileNumber].m_hTree 
            = m_tcTreeCtrlTaskList.InsertItem(
            (char*)m_sAppendFileInf[m_iAppendFileNumber].m_abyDisplayInfo);
        m_iAppendFileNumber++;
        UpdateParamAddButton(this);
    }
}

/****************************************************************************
  Function   :  void CPageFileBale::OnButtonAddCreateFileSystem()
  Note       :  Create file system feature removed; stub kept for message map.
****************************************************************************/
void CPageFileBale::OnButtonAddCreateFileSystem()
{
    // Create file system feature removed.
}

/****************************************************************************
  Function   :  void CPageFileBale::OnButtonAddAppDelete()
****************************************************************************/
void CPageFileBale::OnButtonAddAppDelete() 
{
    // TODO: Add your control notification handler code here
    char strDisplayInfo[300];

    if (m_iDelAppTaskNumber >= APP_FILE_MAX_NUM)
    {
        MessageBox("The delete application task has arrived maximal.");
        return ;
    }

    CDelAppDlg SDlg;
    if (SDlg.DoModal() != IDOK)
    {
        return ;
    }
    if (0 == strlen(g_strDelAppName))
    {
        MessageBox("The application file name you delete can't be null.");
    }
    else
    {
        sprintf(strDisplayInfo, "{DELAPP}{%s}", g_strDelAppName);

        strcpy((char*)m_sDelAppInfo[m_iDelAppTaskNumber].m_abyDisplayInfo, 
            strDisplayInfo);

        m_sDelAppInfo[m_iDelAppTaskNumber].m_hTree 
            = m_tcTreeCtrlTaskList.InsertItem(strDisplayInfo);

        m_iDelAppTaskNumber++;

        if (m_iDelAppTaskNumber >= APP_FILE_MAX_NUM)
        {
            GetDlgItem(IDC_BUTTON_ADD_APP_DELETE)->EnableWindow(FALSE);
        }
        UpdateData(FALSE);    // 更新界面
    }
}

/****************************************************************************
  函数名     :  void CPageFileBale::OnButtonDelTask() 
  描述       :  响应删除配置任务按钮函数，删除界面上的和配置上的任务
  输入参数   :  无
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-07   V1.0        创建
  2、 黄俊斌     2007-05-17   V1.1        修改增加远程下载打包功能
****************************************************************************/
void CPageFileBale::OnButtonDelTask() 
{
    // TODO: Add your control notification handler code here
    HTREEITEM hTempTree;
    int i, j;

    // 获取用户选中的项的句柄
    hTempTree = m_tcTreeCtrlTaskList.GetSelectedItem();

    // 删除选中项
    if (  (OBJECT_VALID==m_sCreateSystem.m_byValidFlag) 
        && (hTempTree==m_sCreateSystem.m_hTree))
    {
        m_tcTreeCtrlTaskList.DeleteItem(hTempTree);
        GetDlgItem(IDC_BUTTON_ADD_APP_DELETE)->EnableWindow(TRUE);
        if (REMOTE_DOWNLOAD == g_sConfigreInfo.m_byLoadType)
        {
            GetDlgItem(IDC_BUTTON_ADD_DEL_ALL_APP)->EnableWindow(TRUE);
        }
        GetDlgItem(IDC_BUTTON_ADD_CREATE_FILE_SYSTEM)->EnableWindow(TRUE);
        m_sCreateSystem.m_byValidFlag = OBJECT_INVALID;
        for (i=0; i<m_iDelAppTaskNumber; i++)
        {
            m_tcTreeCtrlTaskList.InsertItem((char*)m_sDelAppInfo[i].m_abyDisplayInfo);
        }
        IntiPosType();
        return ;
    }
    else if (  (OBJECT_VALID==m_sDelAllApp.m_byValidFlag) 
        && (hTempTree==m_sDelAllApp.m_hTree))
    {
        m_tcTreeCtrlTaskList.DeleteItem(hTempTree);
        GetDlgItem(IDC_BUTTON_ADD_APP_DELETE)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_ADD_DEL_ALL_APP)->EnableWindow(TRUE);
        m_sDelAllApp.m_byValidFlag = OBJECT_INVALID;
        for (i=0; i<m_iDelAppTaskNumber; i++)
        {
            m_tcTreeCtrlTaskList.InsertItem((char*)m_sDelAppInfo[i].m_abyDisplayInfo);
        }
        IntiPosType();
        return ;
    }
    else if (  (!GetDlgItem(IDC_BUTTON_ADD_FONT)->IsWindowEnabled()) 
        && (hTempTree == m_sFontFileInfo.m_hTree))
    {
        m_tcTreeCtrlTaskList.DeleteItem(hTempTree);
        GetDlgItem(IDC_BUTTON_ADD_FONT)->EnableWindow(TRUE);
        IntiPosType();
        return ;
    }

    for (i=0; i<m_iAppendFileNumber; i++)
    {
        if (hTempTree == m_sAppendFileInf[i].m_hTree)
        {
            m_tcTreeCtrlTaskList.DeleteItem(hTempTree);
            m_iAppendFileNumber--;
            if (i != m_iAppendFileNumber)
            {
                memcpy(&m_sAppendFileInf[i], 
                    &m_sAppendFileInf[m_iAppendFileNumber], 
                    sizeof(APPEND_FILE_LIST));
            }
            UpdateParamAddButton(this);
            IntiPosType();
            return ;
        }
    }
    
    for (i=0; i<m_iDelAppTaskNumber; i++)
    {
        if (hTempTree == m_sDelAppInfo[i].m_hTree)
        {
            m_tcTreeCtrlTaskList.DeleteItem(hTempTree);
            m_iDelAppTaskNumber--;
            if (i != m_iDelAppTaskNumber)
            {
                memcpy(&m_sDelAppInfo[i], &m_sDelAppInfo[m_iDelAppTaskNumber], 
                    sizeof(COMMON_FILE_LIST));
            }
            IntiPosType();
            return ;
        }
    }

    
    for (i=0; i<m_iAppFileNumber; i++)
    {
        if (hTempTree == m_sAppFileInfo[i].m_hTree)
        {
            m_tcTreeCtrlTaskList.DeleteItem(hTempTree);
            m_iAppFileNumber--;
            if (0 == m_iAppFileNumber)
            {
                GetDlgItem(IDC_BUTTON_ADD_APP)->EnableWindow(TRUE);
            }
            if (i != m_iAppFileNumber)
            {
                memcpy(&m_sAppFileInfo[i], &m_sAppFileInfo[m_iAppFileNumber], 
                    sizeof(COMMON_FILE_LIST));
            }
            for (j=0; j<m_iAppendFileNumber; j++)
            {
                if (   (   (BALE_TASK_PARAM==m_sAppendFileInf[j].m_byTaskType)
                        || (BALE_TASK_DATAM==m_sAppendFileInf[j].m_byTaskType) )
                    && (hTempTree==m_sAppendFileInf[j].m_hFatherTree
                        || hTempTree==m_sAppendFileInf[j].m_hTree))
                {
                    m_iAppendFileNumber--;
                    if (j != m_iAppendFileNumber)
                    {
                        memcpy(&m_sAppendFileInf[j], 
                            &m_sAppendFileInf[m_iAppendFileNumber],
                            sizeof(APPEND_FILE_LIST));
                    }
                    j--;
                }
            }
            UpdateParamAddButton(this);
            IntiPosType();
            return ;
        }
    }
    if ((m_bAddVosFlag) && (hTempTree==m_sVosFileInfo.m_hTree))
    {
        m_tcTreeCtrlTaskList.DeleteItem(m_sVosFileInfo.m_hTree);
        m_bAddVosFlag = FALSE;
        GetDlgItem(IDC_BUTTON_ADD_VOS)->EnableWindow(TRUE);
        IntiPosType();
        return;
    }
    if ((m_bAddScAppFlag) && (hTempTree==m_sScAppFileInfo.m_hTree))
    {
        m_tcTreeCtrlTaskList.DeleteItem(m_sScAppFileInfo.m_hTree);
        m_bAddScAppFlag = FALSE;
        GetDlgItem(IDC_BUTTON_ADD_DATA_FILE)->EnableWindow(TRUE);
        IntiPosType();
        return;
    }
    if ((m_bAddLogoFlag) && (hTempTree==m_sLogoFileInfo.m_hTree))
    {
        m_tcTreeCtrlTaskList.DeleteItem(m_sLogoFileInfo.m_hTree);
        m_bAddLogoFlag = FALSE;
        GetDlgItem(IDC_BUTTON_ADD_LOGO)->EnableWindow(TRUE);
        IntiPosType();
        return;
    }
    /* WiFi packing removed: ignore leftover WIFI tree nodes if any. */
    if ((m_bAddWifiFlag) && (hTempTree==m_sWifiFileInfo.m_hTree))
    {
        m_tcTreeCtrlTaskList.DeleteItem(m_sWifiFileInfo.m_hTree);
        m_bAddWifiFlag = FALSE;
        IntiPosType();
    }
}

/****************************************************************************
  函数名     :  void CPageFileBale::OnButtonStartBale()
  描述       :  响应打包按钮函数，首先将配置保存，然后将配置的任务打包成一个文件
  输入参数   :  无
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-07   V1.0        创建
  2、 黄俊斌     2007-05-17   V1.1        修改增加远程下载打包功能
****************************************************************************/
void CPageFileBale::OnButtonStartBale() 
{
    // TODO: Add your control notification handler code here
    int i, j, k, iOffset, iTaskNum, iLen;
    BYTE strTemp[1000], strTail[1500], strFileName[300];
    char *p;
    FILE *fpBale, *fpTemp;

    static char BASED_CODE szMergeFilter[] =
        _T("Bale file (*.bin)|*.bin|All Files (*.*)|*.*||");
    CString filter, strSavePath;
    filter = szMergeFilter;

    UpdateConfigure();  // 将界面上的配置保存在配置缓冲中
    SaveConfigInfo();   // 将配置缓冲中的配置保存到配置文件中

    i = GetTaskNumber();
    if (i <= 0)
    {
        MessageBox("Not configure task.");
        return ;
    }
    SaveConfig();

    /* Default extension .bin so Save As appends .bin when user omits it. */
    CFileDialog dlg(FALSE, _T("bin"), _T("bale.bin"),
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, filter, 0);
    if (dlg.DoModal() != IDOK)
    {
        return ;
    }
    strSavePath = dlg.GetPathName();
    fpBale = fopen(strSavePath.GetBuffer(0), "wb");
    if (NULL == fpBale)
    {
        MessageBox("Create file error.");
        return ;
    }
    strcpy((char*)strTemp, "ViewAt BaleFile");
    fwrite(strTemp, 1, 16, fpBale);
    iOffset = 16;

    iTaskNum = 0;

    // 创建文件系统任务和删除应用任务
    if (0) /* create file system removed */
    {
        ;
    }
    else if (OBJECT_VALID==m_sDelAllApp.m_byValidFlag)
    {
        DwordToByte((DWORD)iOffset, &strTail[iTaskNum*4]);
        memset(strTemp, 0, sizeof(strTemp));
        strTemp[0] = BALE_TASK_DEL_ALL_APP;   // 删除所有应用
        fwrite(strTemp, 1, 1, fpBale);
        memcpy(strTemp, "12598755", 8);
        fwrite(strTemp, 1, 8, fpBale);
        iOffset += 9;
        iTaskNum++;
    }
    else
    {
        for (i=0; i<m_iDelAppTaskNumber; i++)
        {
            DwordToByte((DWORD)iOffset, &strTail[iTaskNum*4]);
            memset(strTemp, 0, sizeof(strTemp));
            strTemp[0] = BALE_TASK_DEL_APP; // 删除应用
            strcpy((char*)(strTemp+1), (char*)(&m_sDelAppInfo[i].m_abyDisplayInfo[9]));
            iLen = strlen((char*)(strTemp+1));
            if (iLen >= 2)
            {
                strTemp[iLen] = 0;
            }
            fwrite(strTemp, 1, 33, fpBale);
            iOffset += 33;
            iTaskNum++;
        }
    }

    // 应用程序任务和附属文件
    for (i=0; i<m_iAppFileNumber; i++)
    {
        DwordToByte((DWORD)iOffset, &strTail[iTaskNum*4]);
        p = strstr((char*)&m_sAppFileInfo[i].m_abyDisplayInfo[6], "}{");
        if (NULL == p)
        {
            continue;
        }
        p += 2;
        strcpy((char*)strFileName, (char*)p);
        iLen = strlen((char*)strFileName);
        if (iLen < 2)
        {
            continue;
        }
        strFileName[iLen-1] = 0;
        fpTemp = fopen((char*)strFileName, "rb");
        if (NULL != fpTemp)
        {
            memset(strTemp, 0, sizeof(strTemp));
            strTemp[0] = BALE_TASK_APPLICATION;  // 下载应用程序
            strTemp[1] = g_abyPosType[g_sConfigreInfo.m_byPosType];
            fwrite((char*)strTemp, 1, 2, fpBale);

            AddFileToBaleFile(fpBale, fpTemp, &k);
            fclose(fpTemp);
            iOffset += k+2;
            iTaskNum++;
            for (j=0; j<m_iAppendFileNumber; j++)
            {
                if ( (m_sAppFileInfo[i].m_hTree == m_sAppendFileInf[j].m_hFatherTree)
                  && (   (BALE_TASK_PARAM == m_sAppendFileInf[j].m_byTaskType)
                      || (BALE_TASK_DATAM == m_sAppendFileInf[j].m_byTaskType) ) )
                {
                    memset(strFileName, 0, sizeof(strFileName));
                    strcpy((char*)strFileName, 
                        (char*)&m_sAppendFileInf[j].m_abyDisplayInfo[8]);
                    iLen = strlen((char*)strFileName);
                    strFileName[iLen-1] = 0;
                    for (k=iLen-1; k>0; k--)
                    {
                        if ('\\' == strFileName[k-1])
                        {
                            break;
                        }
                    }
                    fpTemp = fopen((char*)strFileName, "rb");
                    if (NULL != fpTemp)
                    {
                        strTemp[0] = m_sAppendFileInf[j].m_byTaskType;  // 文件类型
                        fwrite(strTemp, 1, 1, fpBale);
                        memset(strTemp, 0, sizeof(strTemp));
                        strcpy((char*)strTemp, (char*)&strFileName[k]);
                        strTemp[15] = 0;
                        fwrite(strTemp, 1, 16, fpBale);
                        AddFileToBaleFile(fpBale, fpTemp, &k);
                        fclose(fpTemp);
                        DwordToByte((DWORD)iOffset, &strTail[iTaskNum*4]);
                        iOffset += k+17;
                        iTaskNum++;
                    }
                }
            }
        }
    }

    for (j=0; j<m_iAppendFileNumber; j++)
    {
        if (   (BALE_TASK_PARA==m_sAppendFileInf[j].m_byTaskType)
            || (BALE_TASK_DATA==m_sAppendFileInf[j].m_byTaskType) ) 
        {
            memset(strFileName, 0, sizeof(strFileName));
            p = strstr((char*)&m_sAppendFileInf[j].m_abyDisplayInfo[7], "}{");
            if (NULL == p)
            {
                break;
            }
            p += 2;
            strcpy((char*)strFileName, p);
            iLen = strlen((char*)strFileName);
            strFileName[iLen-1] = 0;
            for (k=iLen-1; k>0; k--)
            {
                if ('\\' == strFileName[k-1])
                {
                    break;
                }
            }
            fpTemp = fopen((char*)strFileName, "rb");
            if (NULL != fpTemp)
            {
                strTemp[0] = m_sAppendFileInf[j].m_byTaskType;  // 文件类型
                fwrite(strTemp, 1, 1, fpBale);

                i = 0;
                memset(strTemp, 0, 40);
                while (i < 31)
                {
                    strTemp[i] = m_sAppendFileInf[j].m_abyDisplayInfo[7+i];
                    if ('}'==strTemp[i])
                    {
                        strTemp[i] = 0;
                        break;
                    }
                    i++;
                }
                strTemp[31] = 0;
                fwrite(strTemp, 1, 32, fpBale);

                memset(strTemp, 0, sizeof(strTemp));
                strcpy((char*)strTemp, (char*)&strFileName[k]);
                strTemp[15] = 0;
                fwrite(strTemp, 1, 16, fpBale);

                AddFileToBaleFile(fpBale, fpTemp, &k);
                fclose(fpTemp);
                DwordToByte((DWORD)iOffset, &strTail[iTaskNum*4]);
                iOffset += k+1+16+32;
                iTaskNum++;
            }
        }
    }

    // 字库任务
    if (!GetDlgItem(IDC_BUTTON_ADD_FONT)->IsWindowEnabled())
    {
        memset(strTemp, 0, sizeof(strTemp));
        p = strstr((char*)m_sFontFileInfo.m_abyDisplayInfo, "{THERMAL}");
        strTemp[0] = BALE_TASK_FONT;  // 下载字库
        if (NULL == p)
        {
            strTemp[1] = 2;  // 针式打印机
            strcpy((char*)strFileName,
                (char*)(&m_sFontFileInfo.m_abyDisplayInfo[15]));
        }
        else
        {
            strTemp[1] = 1;  // 热敏打印机
            strcpy((char*)strFileName,
                (char*)(&m_sFontFileInfo.m_abyDisplayInfo[16]));
        }
        iLen = strlen((char*)strFileName);
        strFileName[iLen-1] = 0;
        fpTemp = fopen((char*)strFileName, "rb");
        if (NULL != fpTemp)
        {
            fwrite(strTemp, 1, 2, fpBale);
            AddFileToBaleFile(fpBale, fpTemp, &k);
            fclose(fpTemp);
            DwordToByte((DWORD)iOffset, &strTail[iTaskNum*4]);
            iOffset += 2+k;
            iTaskNum++;
        }
    }
    
    // VOS任务
    if (m_bAddVosFlag)
    {
        DwordToByte((DWORD)iOffset, &strTail[iTaskNum*4]);
        p = strstr((char*)m_sVosFileInfo.m_abyDisplayInfo, "}{");
        if (NULL != p) p = strstr(p + 2, "}{");  // {OS|VOS}{pos}{path}
        if (NULL != p)
        {
            p += 2;
        strcpy((char*)strFileName, (char*)p);
        iLen = strlen((char*)strFileName);
        strFileName[iLen-1] = 0;
        fpTemp = fopen((char*)strFileName, "rb");
        if (NULL != fpTemp)
        {
            strTemp[0] = BALE_TASK_VOS;  // 下载VOS
            strTemp[1] = g_abyPosType[g_sConfigreInfo.m_byPosType];
            fwrite(strTemp, 1, 2, fpBale);
            AddFileToBaleFile(fpBale, fpTemp, &k);
            fclose(fpTemp);
            DwordToByte((DWORD)iOffset, &strTail[iTaskNum*4]);
            iOffset += 2+k;
            iTaskNum++;
        }
        }
    }

    /* SC App task (same envelope as OS/App: type + pos + payload). */
    if (m_bAddScAppFlag)
    {
        DwordToByte((DWORD)iOffset, &strTail[iTaskNum*4]);
        p = strstr((char*)m_sScAppFileInfo.m_abyDisplayInfo, "}{");
        if (NULL != p) p = strstr(p + 2, "}{"); /* {SCAPP}{pos}{path} */
        if (NULL != p)
        {
            p += 2;
            strcpy((char*)strFileName, (char*)p);
            iLen = strlen((char*)strFileName);
            strFileName[iLen-1] = 0;
            fpTemp = fopen((char*)strFileName, "rb");
            if (NULL != fpTemp)
            {
                strTemp[0] = BALE_TASK_SC_APP;
                strTemp[1] = g_abyPosType[g_sConfigreInfo.m_byPosType];
                fwrite(strTemp, 1, 2, fpBale);
                AddFileToBaleFile(fpBale, fpTemp, &k);
                fclose(fpTemp);
                iOffset += 2 + k;
                iTaskNum++;
            }
        }
    }

    /* Logo: type + pos + payload (same envelope as SC). */
    if (m_bAddLogoFlag)
    {
        DwordToByte((DWORD)iOffset, &strTail[iTaskNum*4]);
        p = strstr((char*)m_sLogoFileInfo.m_abyDisplayInfo, "}{");
        if (NULL != p) p = strstr(p + 2, "}{"); /* {LOGO}{pos}{path} */
        if (NULL != p)
        {
            p += 2;
            strcpy((char*)strFileName, (char*)p);
            iLen = strlen((char*)strFileName);
            strFileName[iLen-1] = 0;
            fpTemp = fopen((char*)strFileName, "rb");
            if (NULL != fpTemp)
            {
                strTemp[0] = BALE_TASK_LOGO;
                strTemp[1] = g_abyPosType[g_sConfigreInfo.m_byPosType];
                fwrite(strTemp, 1, 2, fpBale);
                AddFileToBaleFile(fpBale, fpTemp, &k);
                fclose(fpTemp);
                iOffset += 2 + k;
                iTaskNum++;
            }
        }
    }

    /* WiFi packing removed (use Download page). Old bale with WIFI still downloads. */

    strTail[iTaskNum*4] = (BYTE)iTaskNum;
    fwrite(strTail, 1, iTaskNum*4+1, fpBale);
    fclose(fpBale);
    if (REMOTE_DOWNLOAD == g_sConfigreInfo.m_byLoadType)
    {
        i = FileSign(strSavePath.GetBuffer(0));
    }
    else
    {
        i = 0;
    }
    if (0 == i)
    {
        MessageBox("Success.");
    }
    else
    {
        MessageBox("Packet error.");
    }
}

/****************************************************************************
  函数名     :  int CPageFileBale::GetTaskNumber()
  描述       :  获取任务个数。
                注意：当有重建文件系统任务的时候，不将删除应用任务统计进来
  输入参数   :  无
  输出参数   :  无
  返回值     :  任务个数
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-07   V1.0        创建
  2、 黄俊斌     2008-05-17   V1.1        增加删除所有应用和VOS类型
****************************************************************************/
int CPageFileBale::GetTaskNumber()
{
    int iCount = 0;
    
    iCount += m_iAppFileNumber;
    iCount += m_iAppendFileNumber;
    if (OBJECT_VALID == m_sCreateSystem.m_byValidFlag)
    {
        iCount++;
    }
    else if (OBJECT_VALID == m_sDelAllApp.m_byValidFlag)
    {
        iCount++;
    }
    else
    {
        iCount += m_iDelAppTaskNumber;
    }
    if (!GetDlgItem(IDC_BUTTON_ADD_FONT)->IsWindowEnabled())
    {
        iCount++;
    }
    if (m_bAddVosFlag)
    {
        iCount++;
    }
    if (m_bAddScAppFlag)
    {
        iCount++;
    }
    if (m_bAddLogoFlag)
    {
        iCount++;
    }
    return iCount;
}

/****************************************************************************
  函数名     :  void CPageFileBale::IntiPosType()
  描述       :  初始化机型选择。
                注意：当有重建文件系统任务的时候，不将删除应用任务统计进来
  输入参数   :  无
  输出参数   :  无
  返回值     :  任务个数
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2008-05-19   V1.0        创建
****************************************************************************/
void CPageFileBale::IntiPosType()
{
    if ((0==m_iAppFileNumber) && (!m_bAddVosFlag) && (!m_bAddScAppFlag))
    {
        g_sConfigreInfo.m_byPosType = 0xff;
    }
}

/****************************************************************************
  函数名     :  void CPageFileBale::SaveConfig()
  描述       :  保存配置信息，将配置内容保存到配置文件中
  输入参数   :  无
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-07   V1.0        创建
  2、 黄俊斌     2007-05-17   V1.1        修改增加远程下载打包功能
****************************************************************************/
void CPageFileBale::SaveConfig()
{
    int i, j;
    FILE *fp;
    char strTemp[300];

    strcpy(strTemp, g_strCurrentDir);
    strcat(strTemp, "\\");
    strcat(strTemp, g_strBaleConfigFileName);
    DeleteFile(strTemp);
    fp = fopen(strTemp, "wb");
    if (NULL == fp)
    {
        MessageBox("Create configure file error.");
        return ;
    }
    // Create file system feature removed: never persist CREATESYSTEM.
    if (OBJECT_VALID==m_sDelAllApp.m_byValidFlag)
    {
        fwrite(m_sDelAllApp.m_abyDisplayInfo, 1, 
            strlen((char*)m_sDelAllApp.m_abyDisplayInfo), fp);
        fwrite("\n", 1, 1, fp);
    }
    else
    {
        for (i=0; i<m_iDelAppTaskNumber; i++)
        {
            fwrite(m_sDelAppInfo[i].m_abyDisplayInfo, 1, 
                strlen((char*)m_sDelAppInfo[i].m_abyDisplayInfo), fp);
            fwrite("\n", 1, 1, fp);
        }
    }
    if (!GetDlgItem(IDC_BUTTON_ADD_FONT)->IsWindowEnabled())
    {
        fwrite(m_sFontFileInfo.m_abyDisplayInfo, 1, 
            strlen((char*)m_sFontFileInfo.m_abyDisplayInfo), fp);
        fwrite("\n", 1, 1, fp);
    }
    for (i=0; i<m_iAppFileNumber; i++)
    {
        fwrite(m_sAppFileInfo[i].m_abyDisplayInfo, 1, 
            strlen((char*)m_sAppFileInfo[i].m_abyDisplayInfo), fp);
        fwrite("\n", 1, 1, fp);
        for (j=0; j<m_iAppendFileNumber; j++)
        {
            if (   (   (BALE_TASK_PARAM==m_sAppendFileInf[j].m_byTaskType)
                    || (BALE_TASK_DATAM==m_sAppendFileInf[j].m_byTaskType) )
                && (m_sAppFileInfo[i].m_hTree==m_sAppendFileInf[j].m_hFatherTree) )
            {
                fwrite(m_sAppendFileInf[j].m_abyDisplayInfo, 1, 
                    strlen((char*)m_sAppendFileInf[j].m_abyDisplayInfo), fp);
                fwrite("\n", 1, 1, fp);
            }
        }
    }
    
    for (j=0; j<m_iAppendFileNumber; j++)
    {
        if ( (BALE_TASK_PARA==m_sAppendFileInf[j].m_byTaskType)
          || (BALE_TASK_DATA==m_sAppendFileInf[j].m_byTaskType) )
        {
            fwrite(m_sAppendFileInf[j].m_abyDisplayInfo, 1, 
                strlen((char*)m_sAppendFileInf[j].m_abyDisplayInfo), fp);
            fwrite("\n", 1, 1, fp);
        }
    }

    if (!GetDlgItem(IDC_BUTTON_ADD_VOS)->IsWindowEnabled())
    {
        fwrite(m_sVosFileInfo.m_abyDisplayInfo, 1, 
            strlen((char*)m_sVosFileInfo.m_abyDisplayInfo), fp);
        fwrite("\n", 1, 1, fp);
    }
    if (m_bAddScAppFlag)
    {
        fwrite(m_sScAppFileInfo.m_abyDisplayInfo, 1,
            strlen((char*)m_sScAppFileInfo.m_abyDisplayInfo), fp);
        fwrite("\n", 1, 1, fp);
    }
    if (m_bAddLogoFlag)
    {
        fwrite(m_sLogoFileInfo.m_abyDisplayInfo, 1,
            strlen((char*)m_sLogoFileInfo.m_abyDisplayInfo), fp);
        fwrite("\n", 1, 1, fp);
    }
    /* Do not persist {WIFI}: packing WiFi removed. */
    fclose(fp);
}

/****************************************************************************
  函数名     :  void CPageFileBale::GetConfig()
  描述       :  从配置文件中读取配置内容，并调用解析和显示函数显示配置内容
  输入参数   :  无
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-07   V1.0        创建
****************************************************************************/
void CPageFileBale::GetConfig()
{
    int i, j;
    char *strBuffer;
    FILE *fp;
    char strTemp[300];

    strcpy(strTemp, g_strCurrentDir);
    strcat(strTemp, "\\");
    strcat(strTemp, g_strBaleConfigFileName);
    fp = fopen(strTemp, "rb");
    if (NULL == fp)
    {
        return ;
    }

    fseek(fp, 0, SEEK_END);
    j = ftell(fp);
    if (j < 10)
    {
        fclose(fp);
        return ;
    }
    strBuffer = (char*)malloc(j+2);
    if (NULL == strBuffer)
    {
        fclose(fp);
        MessageBox("Get stack memory error");
        return ;
    }

    memset(strBuffer, 0, j+2);
    fseek(fp, 0, SEEK_SET);
    i = fread(strBuffer, 1, j, fp);
    fclose(fp);
    if (i < j)
    {
        free(strBuffer);
        return ;
    }
    ParseAndDisplayConfig(strBuffer, i);
    free(strBuffer);
}

/****************************************************************************
  函数名     :  void CPageFileBale::ParseAndDisplayConfig(char *strInConfig, 
                     int iConfigLen)
  描述       :  解析并显示配置内容
  输入参数   :  1、char *strInConfig：配置文件内容
                2、int iConfigLen：配置内容的长度
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-07   V1.0        创建
  2、 黄俊斌     2007-05-17   V1.1        修改增加远程下载打包功能
****************************************************************************/
void CPageFileBale::ParseAndDisplayConfig(char *strInConfig, int iConfigLen)
{
    int i, j = 0;
    int iLineOverflow = 0;
    char strTemp[BALE_DISPLAY_INFO_LEN], *p;

    m_iDelAppTaskNumber = 0;
    m_iAppFileNumber = 0;
    m_iAppendFileNumber = 0;
    m_bAddVosFlag = FALSE;
    m_bAddScAppFlag = FALSE;
    m_bAddLogoFlag = FALSE;
    m_bAddWifiFlag = FALSE;

    m_tcTreeCtrlTaskList.DeleteAllItems();
    for (i=0; i<iConfigLen; i++)
    {
        if (0x0a != strInConfig[i])
        {
            /* Drop overlong / corrupt lines instead of writing past strTemp. */
            if (j >= (int)(sizeof(strTemp) - 1))
            {
                iLineOverflow = 1;
                continue;
            }
            strTemp[j++] = strInConfig[i];
        }
        else
        {
            strTemp[j] = 0;
            j = 0;
            if (iLineOverflow)
            {
                iLineOverflow = 0;
                continue;  /* skip corrupt line longer than buffer */
            }
            if (0 == strTemp[0])
            {
                continue;
            }

            p = strstr(strTemp, "{CREATESYSTEM}");
            if (NULL!=p)
            {
                // Create file system feature removed: ignore CREATESYSTEM in config.
            }

            p = strstr(strTemp, "{DELALLAPP}");
            if (   (NULL!=p) 
                && (OBJECT_INVALID==m_sCreateSystem.m_byValidFlag)
                && (OBJECT_INVALID==m_sDelAllApp.m_byValidFlag))
            {
                strcpy((char*)m_sDelAllApp.m_abyDisplayInfo, strTemp);
                m_sDelAllApp.m_hTree = m_tcTreeCtrlTaskList.InsertItem(strTemp);
                m_sDelAllApp.m_byValidFlag = OBJECT_VALID;
                GetDlgItem(IDC_BUTTON_ADD_APP_DELETE)->EnableWindow(FALSE);
                GetDlgItem(IDC_BUTTON_ADD_DEL_ALL_APP)->EnableWindow(FALSE);
            }

            p = strstr(strTemp, "{DELAPP}");
            if ((NULL != p) && (m_iDelAppTaskNumber < APP_FILE_MAX_NUM))
            {
                strcpy((char*)m_sDelAppInfo[m_iDelAppTaskNumber].m_abyDisplayInfo, 
                    strTemp);
                if (OBJECT_INVALID == m_sCreateSystem.m_byValidFlag)
                {
                    m_sDelAppInfo[m_iDelAppTaskNumber].m_hTree 
                        = m_tcTreeCtrlTaskList.InsertItem(strTemp);
                }
                m_iDelAppTaskNumber++;
            }

            p = strstr(strTemp, "{FONT}");
            if (NULL != p)
            {
                if (GetDlgItem(IDC_BUTTON_ADD_FONT)->IsWindowEnabled())
                {
                    strcpy((char*)m_sFontFileInfo.m_abyDisplayInfo, strTemp);
                    m_sFontFileInfo.m_hTree = m_tcTreeCtrlTaskList.InsertItem(strTemp);
                    GetDlgItem(IDC_BUTTON_ADD_FONT)->EnableWindow(FALSE);
                }
            }

            p = strstr(strTemp, "{APP}");
            if ((NULL != p) && (m_iAppFileNumber < APP_FILE_MAX_NUM))
            {
                strcpy((char*)m_sAppFileInfo[m_iAppFileNumber].m_abyDisplayInfo, 
                    strTemp);
                m_sAppFileInfo[m_iAppFileNumber].m_hTree 
                    = m_tcTreeCtrlTaskList.InsertItem(strTemp);
                m_iAppFileNumber++;
                GetDlgItem(IDC_BUTTON_ADD_APP)->EnableWindow(FALSE);
            }

            if (m_iAppendFileNumber < APPEND_FILE_MAX_NUM)
            {
                m_sAppendFileInf[m_iAppendFileNumber].m_byTaskType = BALE_TASK_PARAM;
                p = strstr(strTemp, "{PARAM}");
                if (NULL == p)
                {
                    m_sAppendFileInf[m_iAppendFileNumber].m_byTaskType = BALE_TASK_DATAM;
                    p = strstr(strTemp, "{DATAM}");
                }
                if ((NULL!=p) && (0!=m_iAppFileNumber))
                {
                    strcpy((char*)m_sAppendFileInf[m_iAppendFileNumber].m_abyDisplayInfo, 
                        strTemp);
                    m_sAppendFileInf[m_iAppendFileNumber].m_hFatherTree
                        = m_sAppFileInfo[m_iAppFileNumber-1].m_hTree;
                    m_sAppendFileInf[m_iAppendFileNumber].m_hTree
                        = m_tcTreeCtrlTaskList.InsertItem(strTemp, 
                        m_sAppFileInfo[m_iAppFileNumber-1].m_hTree, TVI_SORT);
                    m_tcTreeCtrlTaskList.Expand(
                        m_sAppFileInfo[m_iAppFileNumber-1].m_hTree, TVE_EXPAND);
                    m_iAppendFileNumber++;
                    UpdateParamAddButton(this);
                }
            }

            if (m_iAppendFileNumber < APPEND_FILE_MAX_NUM)
            {
                m_sAppendFileInf[m_iAppendFileNumber].m_byTaskType = BALE_TASK_PARA;
                p = strstr(strTemp, "{PARA}");
                if (NULL == p)
                {
                    m_sAppendFileInf[m_iAppendFileNumber].m_byTaskType = BALE_TASK_DATA;
                    p = strstr(strTemp, "{DATA}");
                }
                if (NULL != p)
                {
                    strcpy((char*)m_sAppendFileInf[m_iAppendFileNumber].m_abyDisplayInfo, 
                        strTemp);
                    m_sAppendFileInf[m_iAppendFileNumber].m_hTree
                        = m_tcTreeCtrlTaskList.InsertItem(strTemp);
                    m_iAppendFileNumber++;
                    if (BALE_TASK_PARA == m_sAppendFileInf[m_iAppendFileNumber - 1].m_byTaskType)
                    {
                        UpdateParamAddButton(this);
                    }
                }
            }

            p = strstr(strTemp, "{OS}");
            if (NULL == p)
            {
                p = strstr(strTemp, "{VOS}");  // legacy display tag
            }
            if (NULL != p)
            {
                if (GetDlgItem(IDC_BUTTON_ADD_VOS)->IsWindowEnabled())
                {
                    strcpy((char*)m_sVosFileInfo.m_abyDisplayInfo, strTemp);
                    m_sVosFileInfo.m_hTree = m_tcTreeCtrlTaskList.InsertItem(strTemp);
                    m_bAddVosFlag = TRUE;
                    GetDlgItem(IDC_BUTTON_ADD_VOS)->EnableWindow(FALSE);
                }
            }

            p = strstr(strTemp, "{SCAPP}");
            if ((NULL != p) && (!m_bAddScAppFlag))
            {
                strcpy((char*)m_sScAppFileInfo.m_abyDisplayInfo, strTemp);
                m_sScAppFileInfo.m_hTree = m_tcTreeCtrlTaskList.InsertItem(strTemp);
                m_bAddScAppFlag = TRUE;
                GetDlgItem(IDC_BUTTON_ADD_DATA_FILE)->EnableWindow(FALSE);
            }

            p = strstr(strTemp, "{LOGO}");
            if ((NULL != p) && (!m_bAddLogoFlag))
            {
                strcpy((char*)m_sLogoFileInfo.m_abyDisplayInfo, strTemp);
                m_sLogoFileInfo.m_hTree = m_tcTreeCtrlTaskList.InsertItem(strTemp);
                m_bAddLogoFlag = TRUE;
                GetDlgItem(IDC_BUTTON_ADD_LOGO)->EnableWindow(FALSE);
            }

            /* Legacy {WIFI} in bale config is ignored (packing WiFi removed). */
        }
    }
}

/****************************************************************************
  函数名     :  BOOL CPageFileBale::CheckParamFileNameValidate(char *strInName)
  描述       :  检查参数文件名的有效性，参数文件名不能为空或者长度大于15字节
  输入参数   :  1、char *strInName：参数文件名
  输出参数   :  无
  返回值     :  TRUE：有效的参数文件名 / FALSE：无效的参数文件名
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-07   V1.0        创建
****************************************************************************/
BOOL CPageFileBale::CheckParamFileNameValidate(char *strInName)
{
    int i, iLen, iMark;

    iLen  = strlen(strInName);
    iMark = 0;
    for (i=0; i<iLen; i++)
    {
        if ('\\' == strInName[i])
        {
            iMark = i;
        }
    }
    if (iMark == (iLen-1))
    {
        return FALSE;
    }
    if ((iLen-iMark) > 16)
    {
        return FALSE;
    }
    return TRUE;
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2008-05-16   V1.0        创建
****************************************************************************/
void CPageFileBale::OnButtonAddVos() 
{
    // TODO: Add your control notification handler code here
    char strDisplayInfo[BALE_DISPLAY_INFO_LEN];
    static char BASED_CODE szMergeFilter[] = _T("OS(*.bin)|*.bin||)");
    CString filter, strTemp;

    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, strTemp, 
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST,filter, 0);

    if (dlg.DoModal() == IDOK)
    {
        strTemp = dlg.GetPathName();
        /* UN650 OS bin has no ViewAt VOS tail tag; align with Download (no CheckVOS). */

        /* UN650: skip VPOS type dialog; keep a fixed default for bale header. */
        if (g_sConfigreInfo.m_byPosType >= VPOS_TYPE_NUMBER)
        {
            g_sConfigreInfo.m_byPosType = 0;
        }
        memset(strDisplayInfo, 0, sizeof(strDisplayInfo));
        if (_snprintf(strDisplayInfo, sizeof(strDisplayInfo), "{OS}{%s}{%s}",
                &g_strVposSelect[g_sConfigreInfo.m_byPosType][0],
                strTemp.GetBuffer(0)) < 0)
        {
            MessageBox("OS path is too long.");
            return ;
        }
        strDisplayInfo[sizeof(strDisplayInfo) - 1] = 0;

        strcpy((char*)m_sVosFileInfo.m_abyDisplayInfo, 
            strDisplayInfo);
        m_sVosFileInfo.m_hTree 
            = m_tcTreeCtrlTaskList.InsertItem(strDisplayInfo);
        m_bAddVosFlag = TRUE;
        UpdateData(FALSE);    // 更新界面
        GetDlgItem(IDC_BUTTON_ADD_VOS)->EnableWindow(FALSE);
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
  1、 黄俊斌     2008-05-16   V1.0        创建
****************************************************************************/
void CPageFileBale::OnButtonDelAllApp() 
{
    // TODO: Add your control notification handler code here
    int i;

    g_iFunctionSelect = 4;  // 设置成“确认要删除所有应用程序吗？”选择界面
    CVPOSSelect SDlg;
    if (SDlg.DoModal() != IDOK)
    {
        return ;
    }
    strcpy((char*)m_sDelAllApp.m_abyDisplayInfo, "{DELALLAPP}");
    m_sDelAllApp.m_hTree = m_tcTreeCtrlTaskList.InsertItem(
        (char*)m_sDelAllApp.m_abyDisplayInfo);
    m_sDelAllApp.m_byValidFlag = OBJECT_VALID;
    GetDlgItem(IDC_BUTTON_ADD_APP_DELETE)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_ADD_DEL_ALL_APP)->EnableWindow(FALSE);
    for (i=0; i<m_iDelAppTaskNumber; i++)
    {
        m_tcTreeCtrlTaskList.DeleteItem(m_sDelAppInfo[i].m_hTree);
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
  1、 黄俊斌     2008-05-16   V1.0        创建
****************************************************************************/
void CPageFileBale::OnRadioRemoteDownload() 
{
    /* Remote load disabled: stay on Local. */
    g_sConfigreInfo.m_byLoadType = LOCAL_DOWNLOAD;
    ((CButton *)GetDlgItem(IDC_RADIO_LOCAL_DOWNLOAD))->SetCheck(TRUE);
    ((CButton *)GetDlgItem(IDC_RADIO_REMOTE_DOWNLOAD))->SetCheck(FALSE);
    GetDlgItem(IDC_BUTTON_ADD_VOS)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_ADD_DEL_ALL_APP)->EnableWindow(FALSE);
    MessageBox("Remote load is disabled. Use Local load.");
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2008-05-16   V1.0        创建
****************************************************************************/
void CPageFileBale::OnRadioLocalDownload() 
{
    g_sConfigreInfo.m_byLoadType = LOCAL_DOWNLOAD;
    GetDlgItem(IDC_BUTTON_ADD_VOS)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_ADD_DEL_ALL_APP)->EnableWindow(FALSE);
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2008-05-19   V1.0        创建
****************************************************************************/
void CPageFileBale::OnButtonSaveInfo() 
{
    // TODO: Add your control notification handler code here
    int i;
    
    UpdateConfigure();  // 将界面上的配置保存在配置缓冲中
    SaveConfigInfo();   // 将配置缓冲中的配置保存到配置文件中

    i = GetTaskNumber();
    if (i <= 0)
    {
        MessageBox("No configure task.");
        return ;
    }
    SaveConfig();
    MessageBox("Save configure success.");
}

void CPageFileBale::OnButtonAddDataFile() 
{
    /* Button caption is SC App (Data download removed from Task bale). */
    char strDisplayInfo[BALE_DISPLAY_INFO_LEN];
    static char BASED_CODE szMergeFilter[] = _T("SC App(*.bin)|*.bin|All(*.*)|*.*||)");
    CString filter, strTemp;

    if (m_bAddScAppFlag)
    {
        MessageBox("SC App is already in the task list.");
        return;
    }

    filter = szMergeFilter;
    CFileDialog dlg(TRUE, NULL, strTemp,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, filter, 0);
    if (dlg.DoModal() != IDOK)
    {
        return;
    }
    strTemp = dlg.GetPathName();

    /* UN650: skip VPOS type dialog; keep a fixed default for bale header. */
    if (g_sConfigreInfo.m_byPosType >= VPOS_TYPE_NUMBER)
    {
        g_sConfigreInfo.m_byPosType = 0;
    }

    memset(strDisplayInfo, 0, sizeof(strDisplayInfo));
    if (_snprintf(strDisplayInfo, sizeof(strDisplayInfo), "{SCAPP}{%s}{%s}",
            &g_strVposSelect[g_sConfigreInfo.m_byPosType][0],
            strTemp.GetBuffer(0)) < 0)
    {
        MessageBox("SC App path is too long.");
        return;
    }
    strDisplayInfo[sizeof(strDisplayInfo) - 1] = 0;
    strcpy((char*)m_sScAppFileInfo.m_abyDisplayInfo, strDisplayInfo);
    m_sScAppFileInfo.m_hTree = m_tcTreeCtrlTaskList.InsertItem(strDisplayInfo);
    m_bAddScAppFlag = TRUE;
    GetDlgItem(IDC_BUTTON_ADD_DATA_FILE)->EnableWindow(FALSE);
    UpdateData(FALSE);
}

void CPageFileBale::OnButtonAddLogo()
{
    char strDisplayInfo[BALE_DISPLAY_INFO_LEN];
    static char BASED_CODE szMergeFilter[] = _T("Logo (*.*)|*.*||)");
    CString filter, strTemp;

    if (m_bAddLogoFlag)
    {
        MessageBox("Logo is already in the task list.");
        return;
    }

    m_btnAddLogoFile.CancelHover();
    filter = szMergeFilter;
    CFileDialog dlg(TRUE, NULL, strTemp,
        OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST, filter, this);
    if (dlg.DoModal() != IDOK)
    {
        return;
    }
    strTemp = dlg.GetPathName();

    if (g_sConfigreInfo.m_byPosType >= VPOS_TYPE_NUMBER)
    {
        g_sConfigreInfo.m_byPosType = 0;
    }

    memset(strDisplayInfo, 0, sizeof(strDisplayInfo));
    if (_snprintf(strDisplayInfo, sizeof(strDisplayInfo), "{LOGO}{%s}{%s}",
            &g_strVposSelect[g_sConfigreInfo.m_byPosType][0],
            strTemp.GetBuffer(0)) < 0)
    {
        MessageBox("Logo path is too long.");
        return;
    }
    strDisplayInfo[sizeof(strDisplayInfo) - 1] = 0;
    strcpy((char*)m_sLogoFileInfo.m_abyDisplayInfo, strDisplayInfo);
    m_sLogoFileInfo.m_hTree = m_tcTreeCtrlTaskList.InsertItem(strDisplayInfo);
    m_bAddLogoFlag = TRUE;
    m_btnAddLogoFile.CancelHover();
    GetDlgItem(IDC_BUTTON_ADD_LOGO)->EnableWindow(FALSE);
    UpdateData(FALSE);
}

void CPageFileBale::OnButtonAddWifi()
{
    /* Packing WiFi removed; use Download page WIFI checkbox. */
    m_btnAddWifiFile.CancelHover();
    MessageBox("WiFi is not available in Task bale. Use Download page.");
}
