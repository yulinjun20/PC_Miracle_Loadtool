// VPOSSelect.cpp : implementation file
//

#include "stdafx.h"
#include "base.h"
#include "LoadTools.h"
#include "VPOSSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVPOSSelect dialog

char g_strVposSelect[VPOS_TYPE_NUMBER][32] = {
    "VPOS301", "VPOS302", "VPOS303", "VPOS304", "VPOS305", "VPOS306",
    "VPOS307", "VPOS308", "VPOS309", "VPOS310",
    "VPP201", "VPP202", "VPP203", "VPP204", "VPP205", "VPP206",
    "VPP207", "VPP208", "VPP209", "VPP210", "VPP211"
};

BYTE g_abyPosType[VPOS_TYPE_NUMBER] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
    0x06, 0x07, 0x08, 0x09, 
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85,
    0x86, 0x87, 0x88, 0x89, 0x8a};

int g_iSelectIndex = 0;  // 用户选择的结果，-1表示用户取消，>=0表示用户选择OK

int g_iFunctionSelect = 0;  // 0：机型选择界面；1字库类型选择界面；2选择确认取消界面
                            // 3：230400bps波特率选择确认

extern CONFIG_INFO g_sConfigreInfo;
char g_strInputAppName[256];

CVPOSSelect::CVPOSSelect(CWnd* pParent /*=NULL*/)
    : CDialog(CVPOSSelect::IDD, pParent)
{
    //{{AFX_DATA_INIT(CVPOSSelect)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}


void CVPOSSelect::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CVPOSSelect)
    DDX_Control(pDX, IDC_COMBO_VPOS_TYPE, m_cbVposType);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVPOSSelect, CDialog)
    //{{AFX_MSG_MAP(CVPOSSelect)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVPOSSelect message handlers

BOOL CVPOSSelect::OnInitDialog()
{
    int i;

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
    m_cbVposType.ResetContent();
    switch (g_iFunctionSelect)
    {
    case 0: // IDC_EDIT_INFO
        SetDlgItemText(IDC_STATIC_SELECT_INFO, "Select VPOS type:");
        GetDlgItem(IDC_STATIC_MSG_INFO)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_INFO)->ShowWindow(SW_HIDE);
        for (i=0; i<VPOS_TYPE_NUMBER; i++)
        {
            m_cbVposType.AddString(&g_strVposSelect[i][0]);
        }
        m_cbVposType.SetCurSel(1);
        break;
    case 1:
        SetDlgItemText(IDC_STATIC_SELECT_INFO, "Select printer type:");
        GetDlgItem(IDC_STATIC_MSG_INFO)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_INFO)->ShowWindow(SW_HIDE);
        m_cbVposType.AddString("Thermal");
        m_cbVposType.AddString("Stylus");
        m_cbVposType.SetCurSel(0);
        break;
    case 2:
        SetDlgItemText(IDC_STATIC_SELECT_INFO, "");
        SetDlgItemText(IDC_STATIC_MSG_INFO, "Create file system?");
        GetDlgItem(IDC_STATIC_SELECT_INFO)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_COMBO_VPOS_TYPE)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_INFO)->ShowWindow(SW_HIDE);
        break;
    case 3:
        SetDlgItemText(IDC_STATIC_SELECT_INFO, "");
        SetDlgItemText(IDC_STATIC_MSG_INFO, "There only the PCI card can support the 230400bps, Enter/Cancel?");
        GetDlgItem(IDC_STATIC_SELECT_INFO)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_COMBO_VPOS_TYPE)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_INFO)->ShowWindow(SW_HIDE);
        break;
    case 4:
        SetDlgItemText(IDC_STATIC_SELECT_INFO, "");
        SetDlgItemText(IDC_STATIC_MSG_INFO, "Are you sure delete all application?");
        GetDlgItem(IDC_STATIC_SELECT_INFO)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_COMBO_VPOS_TYPE)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_INFO)->ShowWindow(SW_HIDE);
        break;
    case 5:
        SetDlgItemText(IDC_STATIC_SELECT_INFO, "Application name:");
        GetDlgItem(IDC_STATIC_MSG_INFO)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_COMBO_VPOS_TYPE)->ShowWindow(SW_HIDE);
//        GetDlgItem(IDC_STATIC_SELECT_INFO)->SetActiveWindow();
//        GetDlgItem(IDC_STATIC_SELECT_INFO)->SetCapture();
//        GetDlgItem(IDC_STATIC_SELECT_INFO)->SetFocus();
//        ((CEdit*)GetDlgItem(IDC_STATIC_SELECT_INFO))->SetSel(0, 0);
        break;
    case 6:
        SetDlgItemText(IDC_STATIC_SELECT_INFO, ""); 
        SetDlgItemText(IDOK, "Yes");  // IDOK
        SetDlgItemText(IDCANCEL, "No");  // IDOK
        SetDlgItemText(IDC_STATIC_MSG_INFO, "Do you want to input application name?");
        GetDlgItem(IDC_STATIC_SELECT_INFO)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_COMBO_VPOS_TYPE)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_INFO)->ShowWindow(SW_HIDE);
        break;
    default:
        break;
    }
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}


void CVPOSSelect::OnOK() 
{
    CString strTemp;
    // TODO: Add extra validation here
    switch (g_iFunctionSelect)
    {
    case 0:  // VPOS机型选择
        g_sConfigreInfo.m_byPosType = m_cbVposType.GetCurSel();
 //       g_iSelectIndex = m_cbVposType.GetCurSel();
        break;
    case 1:  // 打印机类型选择
        g_sConfigreInfo.m_byPrinterType = m_cbVposType.GetCurSel();
//        g_iSelectIndex = m_cbVposType.GetCurSel();
        break;
    case 2:  // 创建文件系统确认
    case 3:  // 串口卡230400bps速率确认
    case 4:  // 确认要删除所有应用程序确认
    case 6:  // 确认是否要输入应用名
        g_iSelectIndex = 0;
        break;
    case 5:  // 应用程序名输入
        GetDlgItemText(IDC_EDIT_INFO, strTemp);
        strcpy(g_strInputAppName, strTemp.GetBuffer(0));
        g_strInputAppName[32] = 0;
        if (0 == strlen(g_strInputAppName))
        {
            g_iSelectIndex = -1;
        }
        else
        {
            g_iSelectIndex = 0;
        }
        break;
    default:
        break;
    }
    
    CDialog::OnOK();
}

void CVPOSSelect::OnCancel() 
{
    // TODO: Add extra cleanup here
    g_iSelectIndex = -1;
    
	CDialog::OnCancel();
}

