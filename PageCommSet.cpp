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

extern int g_iFunctionSelect;

extern CONFIG_INFO g_sConfigreInfo;
extern int SaveConfigInfo(void);

/////////////////////////////////////////////////////////////////////////////
// CPageCommSet property page

IMPLEMENT_DYNCREATE(CPageCommSet, CPropertyPage)

CPageCommSet::CPageCommSet() : CPropertyPage(CPageCommSet::IDD)
{
    //{{AFX_DATA_INIT(CPageCommSet)
    m_iSerialPortNumber = 0;
    m_iSerialPortBauds = 9; /* default 921600 (g_adwBauds[9]) */
    m_bStopOnDownloadFail = FALSE;
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
    DDX_Check(pDX, IDC_CHECK_STOP_ON_FAIL, m_bStopOnDownloadFail);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageCommSet, CPropertyPage)
    //{{AFX_MSG_MAP(CPageCommSet)
    ON_CBN_SELCHANGE(IDC_COMBO_COM_BAUD, OnSelchangeComboComBaud)
    ON_CBN_SELCHANGE(IDC_COMBO_COM_PORT_NUMBER, OnSelchangeComboComPortNumber)
    ON_BN_CLICKED(IDC_RADIO_PACKET_512, OnRadioPacket512)
    ON_BN_CLICKED(IDC_RADIO_PACKET_4096, OnRadioPacket4096)
    ON_BN_CLICKED(IDC_BUTTON_COM_REFRESH, OnButtonComRefresh)
    ON_BN_CLICKED(IDC_CHECK_STOP_ON_FAIL, OnCheckStopOnFail)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CPageCommSet message handlers

/* Enumerate present COM ports (same idea as PageApplication::EnumComPorts). */
static int EnumDetectedComPorts(int *ports, int maxPorts)
{
    char buf[65536];
    char *p;
    int n = 0;
    int i, j;
    DWORD nRet = QueryDosDevice(NULL, buf, sizeof(buf) / sizeof(buf[0]));
    if (0 == nRet)
    {
        for (i = 1; i <= 256 && n < maxPorts; i++)
        {
            char name[32];
            HANDLE h;
            sprintf(name, "\\\\.\\COM%d", i);
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

static int FindComPortIndex(CComboBox *pPort, int iPortNum)
{
    int i, n;
    if (NULL == pPort || iPortNum <= 0)
    {
        return -1;
    }
    n = pPort->GetCount();
    for (i = 0; i < n; ++i)
    {
        if ((int)pPort->GetItemData(i) == iPortNum)
        {
            return i;
        }
    }
    return -1;
}

static int GetComboComPortNumber(CComboBox *pPort)
{
    int iSel;
    if (NULL == pPort)
    {
        return 0;
    }
    iSel = pPort->GetCurSel();
    if (iSel < 0)
    {
        return 0;
    }
    return (int)pPort->GetItemData(iSel);
}

/* Fill Port num with detected COMs only; ItemData holds real port number. */
static void FillComPortList(CComboBox *pPort)
{
    int ports[256];
    int n, i;
    char szPort[16];
    if (NULL == pPort)
    {
        return;
    }
    pPort->ResetContent();
    n = EnumDetectedComPorts(ports, 256);
    for (i = 0; i < n; ++i)
    {
        int idx;
        sprintf(szPort, "COM%d", ports[i]);
        idx = pPort->AddString(szPort);
        if (idx >= 0)
        {
            pPort->SetItemData(idx, (DWORD)ports[i]);
        }
    }
}

BOOL CPageCommSet::OnInitDialog()
{
    CDialog::OnInitDialog();

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

    /* Startup: enumerate detected ports once (same as Refresh). */
    FillComPortList((CComboBox *)GetDlgItem(IDC_COMBO_COM_PORT_NUMBER));

    /* USB mode UI removed; always use serial. */
    g_sConfigreInfo.m_byCommType = 0;

    {
        CComboBox *pPort = (CComboBox *)GetDlgItem(IDC_COMBO_COM_PORT_NUMBER);
        int iIdx = FindComPortIndex(pPort, g_sConfigreInfo.m_iSerialPortNumber);
        if (iIdx < 0)
        {
            if (NULL != pPort && pPort->GetCount() > 0)
            {
                iIdx = 0;
                g_sConfigreInfo.m_iSerialPortNumber = (int)pPort->GetItemData(0);
                (void)SaveConfigInfo();
            }
            else
            {
                iIdx = 0;
                g_sConfigreInfo.m_iSerialPortNumber = 0;
            }
        }
        m_iSerialPortNumber = iIdx;
    }
    m_iSerialPortBauds = g_sConfigreInfo.m_iSerialPortBaudsSelect;
    if (m_iSerialPortBauds < 0 || m_iSerialPortBauds > 9)
    {
        m_iSerialPortBauds = 9; /* default 921600 */
        g_sConfigreInfo.m_iSerialPortBaudsSelect = 9;
    }

    {
        DWORD dwChunk = GetFileDlChunkPrefer();
        g_sConfigreInfo.m_dwPacketSize = dwChunk;
        if (512 == dwChunk)
        {
            ((CButton *)GetDlgItem(IDC_RADIO_PACKET_512))->SetCheck(TRUE);
            ((CButton *)GetDlgItem(IDC_RADIO_PACKET_4096))->SetCheck(FALSE);
        }
        else
        {
            ((CButton *)GetDlgItem(IDC_RADIO_PACKET_512))->SetCheck(FALSE);
            ((CButton *)GetDlgItem(IDC_RADIO_PACKET_4096))->SetCheck(TRUE);
        }
    }

    if (g_sConfigreInfo.m_byStopOnDownloadFail > 1)
        g_sConfigreInfo.m_byStopOnDownloadFail = 0;
    m_bStopOnDownloadFail = (0 != g_sConfigreInfo.m_byStopOnDownloadFail);

    UpdateData(FALSE);

    return TRUE;
}

void CPageCommSet::OnSelchangeComboComBaud()
{
    UpdateData(TRUE);
    if (7 == m_iSerialPortBauds)
    {
        g_iFunctionSelect = 3;
        CVPOSSelect SDlg;
        if (SDlg.DoModal() != IDOK)
        {
            m_iSerialPortBauds = g_sConfigreInfo.m_iSerialPortBaudsSelect;
            UpdateData(FALSE);
            return ;
        }
    }
    g_sConfigreInfo.m_iSerialPortBaudsSelect = m_iSerialPortBauds;
    (void)SaveConfigInfo();
}

void CPageCommSet::OnSelchangeComboComPortNumber()
{
    UpdateData(TRUE);
    g_sConfigreInfo.m_iSerialPortNumber = GetComboComPortNumber(
        (CComboBox *)GetDlgItem(IDC_COMBO_COM_PORT_NUMBER));
    (void)SaveConfigInfo();
}

BOOL CPageCommSet::IsThreadRun()
{
    return g_bThreadRunFlag;
}

BOOL CPageCommSet::OnKillActive()
{
    if( IsThreadRun() )
    {
        return FALSE;
    }

    return CPropertyPage::OnKillActive();
}

void CPageCommSet::OnRadioPacket512()
{
    g_sConfigreInfo.m_dwPacketSize = 512;
    (void)SaveConfigInfo();
}

void CPageCommSet::OnRadioPacket4096()
{
    g_sConfigreInfo.m_dwPacketSize = 4096;
    (void)SaveConfigInfo();
}

void CPageCommSet::OnCheckStopOnFail()
{
    UpdateData(TRUE);
    g_sConfigreInfo.m_byStopOnDownloadFail = m_bStopOnDownloadFail ? 1 : 0;
    (void)SaveConfigInfo();
}

void CPageCommSet::OnButtonComRefresh()
{
    CComboBox *pPort = (CComboBox *)GetDlgItem(IDC_COMBO_COM_PORT_NUMBER);
    int iPort = GetComboComPortNumber(pPort);
    int iIdx;
    if (iPort <= 0)
    {
        iPort = g_sConfigreInfo.m_iSerialPortNumber;
    }
    FillComPortList(pPort);
    iIdx = FindComPortIndex(pPort, iPort);
    if (iIdx < 0)
    {
        if (NULL != pPort && pPort->GetCount() > 0)
        {
            iIdx = 0;
            g_sConfigreInfo.m_iSerialPortNumber = (int)pPort->GetItemData(0);
        }
        else
        {
            iIdx = 0;
            g_sConfigreInfo.m_iSerialPortNumber = 0;
        }
        (void)SaveConfigInfo();
    }
    else
    {
        g_sConfigreInfo.m_iSerialPortNumber = iPort;
    }
    m_iSerialPortNumber = iIdx;
    UpdateData(FALSE);
}

int CPageCommSet::GetSelectedComPortNumber()
{
    /* Property page may not be created yet (never opened Comm set tab).
       Calling GetDlgItem with m_hWnd==NULL asserts in winocc.cpp:88. */
    if (!GetSafeHwnd())
    {
        return g_sConfigreInfo.m_iSerialPortNumber;
    }
    {
        int iPort = GetComboComPortNumber(
            (CComboBox *)GetDlgItem(IDC_COMBO_COM_PORT_NUMBER));
        if (iPort > 0)
        {
            return iPort;
        }
    }
    return g_sConfigreInfo.m_iSerialPortNumber;
}
