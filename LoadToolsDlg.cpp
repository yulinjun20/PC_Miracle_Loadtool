// LoadToolsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LoadTools.h"
#include "LoadToolsDlg.h"
#include "Base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 指向下载工具LoadTools主界面的指针，只要程序运行，该指针就指向LoadTools主界面
CLoadToolsDlg *g_pCLoadToolsDlg = NULL;

extern void PortClose();

// 线程运行标志，FALSE:没有任何线程未运行，TRUE:有线程运行
BOOL g_bThreadRunFlag = FALSE;
extern CONFIG_INFO g_sConfigreInfo;

void UpdateConfigure(void)
{
    // Firmware 界面配置保存

    strcpy(g_sConfigreInfo.m_strBootFilePath, 
        g_pCLoadToolsDlg->m_PageFirmware.m_strBootFilePath.GetBuffer(0));

    strcpy(g_sConfigreInfo.m_strVOSFilePath, 
        g_pCLoadToolsDlg->m_PageFirmware.m_strVosFilePath.GetBuffer(0));

    g_sConfigreInfo.m_byVOSSelect 
        = g_pCLoadToolsDlg->m_PageFirmware.m_bVOSSelect;

    g_sConfigreInfo.m_byBootSelect 
        = g_pCLoadToolsDlg->m_PageFirmware.m_bBootSelect;



    // CommSet界面配置保存

//    g_sConfigreInfo.m_byCommType = 0;
    
    g_sConfigreInfo.m_iSerialPortBaudsSelect
        = g_pCLoadToolsDlg->m_PageCommSet.m_iSerialPortBauds;

    g_sConfigreInfo.m_iSerialPortNumber
        = g_pCLoadToolsDlg->m_PageCommSet.m_iSerialPortNumber;



    // Application界面配置保存

    g_sConfigreInfo.m_byApplicationSelect
        = g_pCLoadToolsDlg->m_PageApplication.m_bAppSelect;

    g_sConfigreInfo.m_byBaleFileSelect
        = g_pCLoadToolsDlg->m_PageApplication.m_bBaleFileSelect;

    g_sConfigreInfo.m_byFontSelect
        = g_pCLoadToolsDlg->m_PageApplication.m_bFontFileSelect;

    g_sConfigreInfo.m_byParamSelect
        = g_pCLoadToolsDlg->m_PageApplication.m_bParamFileSelect;

    g_sConfigreInfo.m_byCreateFileSystemSelect
        = g_pCLoadToolsDlg->m_PageApplication.m_bCreateFileSystemSelect;

    g_sConfigreInfo.m_byChangeSNSelect
        = g_pCLoadToolsDlg->m_PageApplication.m_bDownloadSNSelect;

    g_sConfigreInfo.m_bySetTimeSelect
        = g_pCLoadToolsDlg->m_PageApplication.m_bSetTimeSelect;

    g_sConfigreInfo.m_byDataFileSelect
        = g_pCLoadToolsDlg->m_PageApplication.m_bDataFileSelect;

    strcpy(g_sConfigreInfo.m_strBaleFilePath,
        g_pCLoadToolsDlg->m_PageApplication.m_strBaleFilePath.GetBuffer(0));

    strcpy(g_sConfigreInfo.m_strFontFilePath,
        g_pCLoadToolsDlg->m_PageApplication.m_strFontFilePath.GetBuffer(0));

    strcpy(g_sConfigreInfo.m_strParamFilePath,
        g_pCLoadToolsDlg->m_PageApplication.m_strParamFilePath.GetBuffer(0));

    strcpy(g_sConfigreInfo.m_strSerialNumber,
        g_pCLoadToolsDlg->m_PageApplication.m_strVposSN.GetBuffer(0));
    
    strcpy(g_sConfigreInfo.m_strDataFilePath,
        g_pCLoadToolsDlg->m_PageApplication.m_strDataFilePath.GetBuffer(0));

    // Bale界面配置保存
//    g_sConfigreInfo.m_byLoadType = g_pCLoadToolsDlg->m_PageFileBale.m_iVposType
}


/****************************************************************************
  函数名     :  int G_GetActivePageIndex()
  描述       :  获取当前活动属性页索引号
  输入参数   :  无
  输出参数   :  无
  返回值     :  当前活动属性页的索引号
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-08-31   V1.0        创建
****************************************************************************/
int G_GetActivePageIndex()
{
    return g_pCLoadToolsDlg->m_sheet.GetActiveIndex();
}

/****************************************************************************
  函数名     :  void G_MyShowMessage(int iMessageIndex)
  描述       :  在当前活动的属性页上显示信息。
                全局函数，主要是起到用户界面与本地下载模块分离开来
  输入参数   :  1、int iMessageIndex：信息索引号
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-08-31   V1.0        创建
****************************************************************************/
void G_MyShowMessage(int iMessageIndex)
{
    char strTemp[256];
    int iRet;
    int iPageIndex;

    iRet = G_MyGetMessage(iMessageIndex, strTemp);
    if (0 != iRet)
    {
        return ;
    }
    iPageIndex = g_pCLoadToolsDlg->m_sheet.GetActiveIndex();
    switch (iPageIndex)
    {
    case 0:
        g_pCLoadToolsDlg->m_PageApplication.MyShowMessage(strTemp);
        break;
    case 1:
        g_pCLoadToolsDlg->m_PageFirmware.MyShowMessage(strTemp);
        break;
    }
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
        // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadToolsDlg dialog

CLoadToolsDlg::CLoadToolsDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CLoadToolsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CLoadToolsDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    if (0 != GetConfigureInfo())
    {
        MessageBox("There has a fatal error in reading configure file, close the application please!");
    }
}

void CLoadToolsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CLoadToolsDlg)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLoadToolsDlg, CDialog)
    //{{AFX_MSG_MAP(CLoadToolsDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadToolsDlg message handlers

BOOL CLoadToolsDlg::OnInitDialog()
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

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon
    // TODO: Add extra initialization here
    m_sheet.AddPage(&m_PageApplication);
    m_sheet.AddPage(&m_PageFirmware);
    m_sheet.AddPage(&m_PageCommSet);
    m_sheet.AddPage(&m_PageFileBale);
    m_sheet.AddPage(&m_PageEditParam);

        // Load Miracle logo into top static control
    CStatic* pLogo = (CStatic*)GetDlgItem(IDC_STATIC_MIRACLE_LOGO);
    if (pLogo != NULL)
    {
        if (m_bmpLogo.LoadBitmap(IDB_MIRACLE_LOGO))
        {
            pLogo->SetBitmap((HBITMAP)m_bmpLogo.GetSafeHandle());
        }
    }

    m_sheet.Create(this, WS_CHILD | WS_VISIBLE, WS_EX_CONTROLPARENT);

    // Leave a strip at top for the Miracle logo (~53 px, left-aligned)
    m_sheet.SetWindowPos(NULL, -10, 60, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    
    g_pCLoadToolsDlg = this; // 保存当前界面指针

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLoadToolsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLoadToolsDlg::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLoadToolsDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}




void CLoadToolsDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
    g_pCLoadToolsDlg->m_PageApplication.m_bStopDownloadFlag = TRUE;
    g_pCLoadToolsDlg->m_PageFirmware.m_bStopDownloadFlag = TRUE;
    Sleep(50);
    PortClose();
	CDialog::OnClose();
}
