// PageCommSet.cpp : implementation file
//

#include "stdafx.h"
#include "LoadTools.h"
#include "PageCommSet.h"
#include "Base.h"
#include "VPOSSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bThreadRunFlag;

extern int g_iFunctionSelect;      // 0：机型选择界面；1字库类型选择界面

extern CONFIG_INFO g_sConfigreInfo;

/////////////////////////////////////////////////////////////////////////////
// CPageCommSet property page

IMPLEMENT_DYNCREATE(CPageCommSet, CPropertyPage)

CPageCommSet::CPageCommSet() : CPropertyPage(CPageCommSet::IDD)
{
    //{{AFX_DATA_INIT(CPageCommSet)
    m_iSerialPortNumber = 0;
    m_iSerialPortBauds = 0;
    //}}AFX_DATA_INIT
}

CPageCommSet::~CPageCommSet()
{
}

void CPageCommSet::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPageCommSet)
    DDX_CBIndex(pDX, IDC_COMBO_COM_PORT_NUMBER, m_iSerialPortNumber);
    DDX_CBIndex(pDX, IDC_COMBO_COM_BAUD, m_iSerialPortBauds);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageCommSet, CPropertyPage)
    //{{AFX_MSG_MAP(CPageCommSet)
    ON_CBN_SELCHANGE(IDC_COMBO_COM_BAUD, OnSelchangeComboComBaud)
    ON_CBN_SELCHANGE(IDC_COMBO_COM_PORT_NUMBER, OnSelchangeComboComPortNumber)
    ON_BN_CLICKED(IDC_RADIO_USE_COM, OnRadioUseCom)
    ON_BN_CLICKED(IDC_RADIO_USE_USB, OnRadioUseUsb)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageCommSet message handlers

BOOL CPageCommSet::OnInitDialog()
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

    /* Port list: COM1..COM99 (index 0..98 -> SerOpen(index+1)). Replace RC DLGINIT. */
    {
        CComboBox *pPort = (CComboBox *)GetDlgItem(IDC_COMBO_COM_PORT_NUMBER);
        if (pPort != NULL)
        {
            int i;
            char szPort[16];
            pPort->ResetContent();
            for (i = 1; i <= 99; ++i)
            {
                sprintf(szPort, "COM%d", i);
                pPort->AddString(szPort);
            }
        }
    }

    if (0 == g_sConfigreInfo.m_byCommType)
    {
        ((CButton *)GetDlgItem(IDC_RADIO_USE_COM))->SetCheck(TRUE);
        ((CButton *)GetDlgItem(IDC_RADIO_USE_USB))->SetCheck(FALSE);
    }
    else
    {
        ((CButton *)GetDlgItem(IDC_RADIO_USE_COM))->SetCheck(FALSE);
        ((CButton *)GetDlgItem(IDC_RADIO_USE_USB))->SetCheck(TRUE);
    }
    GetDlgItem(IDC_RADIO_USE_COM)->EnableWindow(TRUE);
    GetDlgItem(IDC_RADIO_USE_USB)->EnableWindow(FALSE);

    m_iSerialPortNumber = g_sConfigreInfo.m_iSerialPortNumber;
    if (m_iSerialPortNumber < 0 || m_iSerialPortNumber > 98)
    {
        m_iSerialPortNumber = 0;
        g_sConfigreInfo.m_iSerialPortNumber = 0;
    }
    m_iSerialPortBauds = g_sConfigreInfo.m_iSerialPortBaudsSelect;
    if (m_iSerialPortBauds < 0 || m_iSerialPortBauds > 9)
    {
        m_iSerialPortBauds = 0;
        g_sConfigreInfo.m_iSerialPortBaudsSelect = 0;
    }

    UpdateData(FALSE);
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPageCommSet::OnSelchangeComboComBaud() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if (7 == m_iSerialPortBauds)
    {
        g_iFunctionSelect = 3;  // 设置成“只有PCI插槽的串口卡才能支持230400bps的速率，确认/取消？”选择界面
        CVPOSSelect SDlg;
        if (SDlg.DoModal() != IDOK)
        {
            m_iSerialPortBauds = g_sConfigreInfo.m_iSerialPortBaudsSelect;
            UpdateData(FALSE);
            return ;
        }
    }
    g_sConfigreInfo.m_iSerialPortBaudsSelect = m_iSerialPortBauds;
}

void CPageCommSet::OnSelchangeComboComPortNumber() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    g_sConfigreInfo.m_iSerialPortNumber = m_iSerialPortNumber;
}

BOOL CPageCommSet::IsThreadRun()
{
    return g_bThreadRunFlag;
}

BOOL CPageCommSet::OnKillActive() 
{
    // TODO: Add your specialized code here and/or call the base class
    
    if( IsThreadRun() )
    {
        return FALSE;  // 有线程在跑时，本界面不响应用户点击事件
    }

    return CPropertyPage::OnKillActive();
}

void CPageCommSet::OnRadioUseCom() 
{
    // TODO: Add your control notification handler code here
    g_sConfigreInfo.m_byCommType = 0;
}

void CPageCommSet::OnRadioUseUsb() 
{
    // TODO: Add your control notification handler code here
    g_sConfigreInfo.m_byCommType = 1;
}
