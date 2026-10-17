// PageFirmware.cpp : implementation file
//

#include "stdafx.h"
#include "LoadTools.h"
#include "PageFirmware.h"
#include "Base.h"
#include "LoadToolsDlg.h"
#include "Communication.h"
#include "LoadToolsDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void G_MyShowMessage(int iMessageIndex);

extern BOOL g_bThreadRunFlag;

extern CONFIG_INFO g_sConfigreInfo;

extern void UpdateConfigure(void);

extern HANDLE g_lHandle;  // �߳̾��?

extern int g_iCommCheckType;

// ָ�����ع���LoadTools�������ָ�룬ֻҪ�������У���ָ���ָ��LoadTools������
extern CLoadToolsDlg *g_pCLoadToolsDlg;

extern DWORD g_adwBauds[10];

/////////////////////////////////////////////////////////////////////////////
// CPageFirmware property page

IMPLEMENT_DYNCREATE(CPageFirmware, CPropertyPage)

CPageFirmware::CPageFirmware() : CPropertyPage(CPageFirmware::IDD)
{
    //{{AFX_DATA_INIT(CPageFirmware)
    m_strVosFilePath = _T("");
    m_bVOSSelect = FALSE;
    //}}AFX_DATA_INIT
}

CPageFirmware::~CPageFirmware()
{
}

void CPageFirmware::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPageFirmware)
    DDX_Text(pDX, IDC_EDIT_VOS_FILE_PATH, m_strVosFilePath);
    DDX_Check(pDX, IDC_CHECK_VOS_SELECT, m_bVOSSelect);
    
    DDX_Control(pDX, IDC_BUTTON_DOWNLOAD_FIRMWARE, m_btnDownload);
    DDX_Control(pDX, IDC_BUTTON_STOP_FIRMWARE, m_btnStop);
    DDX_Control(pDX, IDC_BUTTON_OPEN_VOS_FILE, m_btnOpenVosFile);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageFirmware, CPropertyPage)
    //{{AFX_MSG_MAP(CPageFirmware)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_VOS_FILE, OnButtonOpenVosFile)
    ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD_FIRMWARE, OnButtonDownloadFirmware)
    ON_BN_CLICKED(IDC_BUTTON_STOP_FIRMWARE, OnButtonStopFirmware)
    ON_BN_CLICKED(IDC_CHECK_VOS_SELECT, OnCheckVosSelect)
    ON_EN_CHANGE(IDC_EDIT_VOS_FILE_PATH, OnChangeEditVosFilePath)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageFirmware message handlers

BOOL CPageFirmware::OnInitDialog()
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
    m_bVOSSelect = g_sConfigreInfo.m_byVOSSelect;
    m_strVosFilePath.Format(g_sConfigreInfo.m_strVOSFilePath);
    
    GetDlgItem(IDC_BUTTON_STOP_FIRMWARE)->EnableWindow(FALSE);
    
    UpdateData(FALSE);
    
    m_btnDownload.SetIcon(IDI_ICON_RUN, NULL, 50, 32);
    m_btnDownload.SetShowText(TRUE);
    m_btnDownload.SetActiveBgColor(RGB(200,200,200), FALSE);
    
    m_btnStop.SetIcon(IDI_ICON_STOP, NULL, 50, 32);
    m_btnStop.SetShowText(TRUE);
    m_btnStop.SetActiveBgColor(RGB(200,200,200), FALSE);

    m_btnOpenVosFile.SetIcon(IDI_ICON_OPEN1, NULL, 32, 32);
    m_btnOpenVosFile.SetShowText(TRUE);
    m_btnOpenVosFile.SetActiveBgColor(RGB(200,200,200), FALSE);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CPageFirmware::IsThreadRun()
{
    return g_bThreadRunFlag;
}

BOOL CPageFirmware::OnKillActive() 
{
    // TODO: Add your specialized code here and/or call the base class

    if( IsThreadRun() )
    {
        return FALSE;  // ���߳�����ʱ�������治��Ӧ�û�����¼�?
    }

    return CPropertyPage::OnKillActive();
}

void CPageFirmware::OnButtonOpenVosFile() 
{
    // TODO: Add your control notification handler code here
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

int ThreadDownloadFirmware(void *pTemp)
{
    int iRet;
    g_bThreadRunFlag = TRUE;
    g_pCLoadToolsDlg->m_PageFirmware.MyEnableWindows(FALSE);
    iRet = g_pCLoadToolsDlg->m_PageFirmware.DownloadStart();
    if (0 == iRet)
    {
        iRet = g_pCLoadToolsDlg->m_PageFirmware.DownloadProcess();
    }
    if (0 == iRet)
    {
        g_pCLoadToolsDlg->m_PageFirmware.DownloadEnd();
    }
    else
    {
        SerClose();
    }
    g_pCLoadToolsDlg->m_PageFirmware.MyEnableWindows(TRUE);
    g_bThreadRunFlag = FALSE;
    return 0;
    
}

void CPageFirmware::OnButtonDownloadFirmware() 
{
    // TODO: Add your control notification handler code here
    DWORD ThreadId;

    UpdateConfigure();  // �������ϵ����ñ��������û�����
    SaveConfigInfo();   // �����û����е����ñ��浽�����ļ���
    if (FALSE==m_bVOSSelect)
    {
        MyShowMessage("No select any download file!");
        return ;
    }
    m_bStopDownloadFlag = FALSE; 
    if (!g_bThreadRunFlag)
    {
        g_lHandle = CreateThread(NULL, 0, 
            (LPTHREAD_START_ROUTINE)ThreadDownloadFirmware, NULL, 0, &ThreadId);
        if (NULL == g_lHandle)
        {
            MyShowMessage("Create thread fault.");
        }
    }
}

void CPageFirmware::OnButtonStopFirmware() 
{
    // TODO: Add your control notification handler code here
    m_bStopDownloadFlag = TRUE;
    MyEnableWindows(TRUE);
}

void CPageFirmware::OnCheckVosSelect() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
}

void CPageFirmware::OnChangeEditVosFilePath() 
{
    // TODO: If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    
}

void CPageFirmware::MyShowMessage(char *strMsg)
{
    SetDlgItemText(IDC_STATIC_MESSAGE_PAGE_FIRMWARE, strMsg);
}

void CPageFirmware::MyEnableWindows(BOOL bFlag)
{
    GetDlgItem(IDC_BUTTON_DOWNLOAD_FIRMWARE)->EnableWindow(bFlag);
    GetDlgItem(IDC_BUTTON_STOP_FIRMWARE)->EnableWindow(!bFlag);
}

/****************************************************************************
  ������     :  int CPageApplication::Handshack()
  ����       :  ���ֺ���
  �������?  :  ��
  �������?  :  ��
  ����ֵ     :  0���ɹ���1������ʧ�ܣ�2���򿪴���ʧ��
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2007-10-08   V1.0        ����
****************************************************************************/
int CPageFirmware::Handshack()
{
    BYTE abyRecvBuff[200];
    int iRet, i;

    m_bStopDownloadFlag = FALSE;

    // Use UI-selected baud for the whole OS download; do not change baud mid-transfer.
    if (SerOpen(g_sConfigreInfo.m_iSerialPortNumber,
        g_adwBauds[g_sConfigreInfo.m_iSerialPortBaudsSelect]))
    {
        G_MyShowMessage(MSG_OPEN_SERIAL_PORT_FAILED);
        return 2;
    }

    // ����
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
                return 0;
            }
            else if ('S' == abyRecvBuff[0])
            {
                g_iCommCheckType = CRC32_CHECK_TYPE;
                return 0;
            }
        }
        i++;
        if (i > 10000)
        {
            G_MyShowMessage(MSG_HANDSHACK_FAILED);
            return 1;
        }
    }
}


int CPageFirmware::DownloadStart()
{
    return Handshack();
}

/****************************************************************************
  ������     :  
  ����       :  �ļ����ش������?
  �������?  :  
  �������?  :  
  ����ֵ     :  
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2007-10-08   V1.0        ����
****************************************************************************/
int CPageFirmware::DownloadProcess()
{
    int iRet;

    MyShowMessage("Handshack finish.");
    if (m_bVOSSelect)
    {
        MyShowMessage("Download OS");
        iRet = DownloadFile(m_strVosFilePath.GetBuffer(0), FILE_TYPE_VOS);
        return iRet;
    }
    return 0;
}

/****************************************************************************
  ������     :  
  ����       :  
  �������?  :  
  �������?  :  
  ����ֵ     :  
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2007-10-08   V1.0        ����
****************************************************************************/
int CPageFirmware::DownloadFile(char *strFileName, BYTE byFileType)
{
    FILE *fp;
    int iRet, iFileLen, iErrorCount, iReadLen, iSendPacketLen;
    char str[100];
    BYTE byCmd, abyTemp[DOWNLOAD_LEN_FIRM+10], abyCompress[DOWNLOAD_LEN_FIRM*2+100];
    WORD wPacketLen;
    DWORD dwCurrentLen, dwCompressLen, dwChunkPrefer;

    fp = fopen(strFileName, "rb");
    if (NULL == fp)
    {
        MessageBox("Can't open the file.");
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    iFileLen = ftell(fp);
//    MyShowMessage("�����ļ�...");
    switch (byFileType)
    {
    case FILE_TYPE_VOS:
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
        break;
    default:
        fclose(fp);
        return 7;
    }
    dwCurrentLen = 0;
    iErrorCount = 0;
    fseek(fp, 0, SEEK_SET);
    dwChunkPrefer = GetFileDlChunkPrefer();
    if (0 == dwChunkPrefer || dwChunkPrefer > DOWNLOAD_LEN_FIRM)
    {
        dwChunkPrefer = DOWNLOAD_LEN_FIRM;
    }
//    MyShowMessage("�����ļ�2");
    while (1)
    {
        if (dwCurrentLen >= (DWORD)iFileLen)
        {
            break;
        }
        sprintf(str, "Download OS...%d%%", (int)((dwCurrentLen * 100) / iFileLen));
        MyShowMessage(str);
        wPacketLen = (WORD)dwChunkPrefer;
        if ((dwCurrentLen + dwChunkPrefer) > (DWORD)iFileLen)
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
                memcpy(&abyTemp[5], abyCompress, dwCompressLen);
                iSendPacketLen = dwCompressLen+5;
            }
            else
            {
                abyTemp[4] = 0;  // data no zip
                iSendPacketLen = iReadLen + 5;
            }
            iErrorCount = 0;
            while (1)
            {
                (void)CmdSendPacket(abyTemp, (WORD)iSendPacketLen, byCmd);
                iRet = CmdRecvPacket(abyCompress, &wPacketLen, 500);
                if ((0==iRet) && (0==abyCompress[4]))
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
    }  // end while (1)
    fclose(fp);
    MyShowMessage("Save file...");
    byCmd++;
    CmdSendPacket(abyTemp, 0, byCmd);
    while (1)
    {
        iRet = CmdRecvPacket(abyTemp, &wPacketLen, 30000);
        if (0 != iRet)
        {
            return 10;
        }
        /* Device B1 success = 1-byte [0]; abyTemp[5] would be CRC if payLen==1. */
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


void CPageFirmware::DownloadEnd()
{
    BYTE abyTemp[2];

    (void)CmdSendPacket(abyTemp, 0, 0xD2);
    Sleep(50);
    SerClose();
    MyShowMessage("Download finish.");
}
