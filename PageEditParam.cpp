// PageEditParam.cpp : implementation file
//

#include "stdafx.h"
#include "LoadTools.h"
#include "PageEditParam.h"
#include "Base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CONFIG_INFO g_sConfigreInfo;

extern BOOL g_bThreadRunFlag;

extern int SaveConfigInfo(void);

int g_iScrollNumberID[SCROLL_STEP_LEN]     = {
    IDC_STATIC_NUMBER1,
    IDC_STATIC_NUMBER2, 
    IDC_STATIC_NUMBER3, 
    IDC_STATIC_NUMBER4,
    IDC_STATIC_NUMBER5, 
    IDC_STATIC_NUMBER6, 
    IDC_STATIC_NUMBER7,
    IDC_STATIC_NUMBER8, 
    IDC_STATIC_NUMBER9, 
    IDC_STATIC_NUMBER10};

int g_iScrollParamLen[SCROLL_STEP_LEN] = {
    IDC_STATIC_LENGTH1,
    IDC_STATIC_LENGTH2, 
    IDC_STATIC_LENGTH3, 
    IDC_STATIC_LENGTH4,
    IDC_STATIC_LENGTH5,
    IDC_STATIC_LENGTH6,
    IDC_STATIC_LENGTH7,
    IDC_STATIC_LENGTH8,
    IDC_STATIC_LENGTH9,
    IDC_STATIC_LENGTH10};

int g_iScrollParamName[SCROLL_STEP_LEN]    = {
    IDC_EDIT_PARAM_NAME1,
    IDC_EDIT_PARAM_NAME2,
    IDC_EDIT_PARAM_NAME3,
    IDC_EDIT_PARAM_NAME4,
    IDC_EDIT_PARAM_NAME5,
    IDC_EDIT_PARAM_NAME6,
    IDC_EDIT_PARAM_NAME7,
    IDC_EDIT_PARAM_NAME8,
    IDC_EDIT_PARAM_NAME9,
    IDC_EDIT_PARAM_NAME10};

int g_iScrollParamContent[SCROLL_STEP_LEN] = {
    IDC_EDIT_PARAM_CONTENT1,
    IDC_EDIT_PARAM_CONTENT2,
    IDC_EDIT_PARAM_CONTENT3,
    IDC_EDIT_PARAM_CONTENT4,
    IDC_EDIT_PARAM_CONTENT5,
    IDC_EDIT_PARAM_CONTENT6,
    IDC_EDIT_PARAM_CONTENT7,
    IDC_EDIT_PARAM_CONTENT8,
    IDC_EDIT_PARAM_CONTENT9,
    IDC_EDIT_PARAM_CONTENT10};

/////////////////////////////////////////////////////////////////////////////
// CPageEditParam property page

IMPLEMENT_DYNCREATE(CPageEditParam, CPropertyPage)

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-04   V1.0        创建
****************************************************************************/
CPageEditParam::CPageEditParam() : CPropertyPage(CPageEditParam::IDD)
{
    m_iStartLine = 0;
    m_bEditChangeFlag = FALSE;
    m_iParamLineNumber = 0;
    //{{AFX_DATA_INIT(CPageEditParam)
    m_strParamFilePath = _T("");
    //}}AFX_DATA_INIT
}

CPageEditParam::~CPageEditParam()
{
}

void CPageEditParam::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPageEditParam)
    DDX_Control(pDX, IDC_SCROLLBAR_PARAM_EDIT, m_sbScrollBarEdit);
    DDX_Text(pDX, IDC_EDIT_PARAM_FILE_SAVE_PATH, m_strParamFilePath);
    
    DDX_Control(pDX, IDC_BUTTON_OPEN_SAVE_PARAM_FILE, m_btnOpenParamFile);
    DDX_Control(pDX, IDC_BUTTON_SAVE_PARAM, m_btnSaveParamFile);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageEditParam, CPropertyPage)
    //{{AFX_MSG_MAP(CPageEditParam)
    ON_WM_VSCROLL()
    ON_BN_CLICKED(IDC_BUTTON_OPEN_SAVE_PARAM_FILE, OnButtonOpenSaveParamFile)
    ON_BN_CLICKED(IDC_BUTTON_SAVE_PARAM, OnButtonSaveParam)
    ON_WM_MOUSEWHEEL()
    ON_WM_TIMER()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageEditParam message handlers
/****************************************************************************
  函数名     :  BOOL CPageEditParam::OnInitDialog()
  描述       :  窗口初始化函数
  输入参数   :  无
  输出参数   :  无
  返回值     :  TRUE / FALSE
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-04   V1.0        创建
****************************************************************************/
BOOL CPageEditParam::OnInitDialog()
{
    int i, iLen; 
    char strTemp[10];

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
    ReadParamFile();
    m_sbScrollBarEdit.SetScrollRange(0, MAX_PARAM_NUMBER-1);
    SCROLLINFO si;
    si.cbSize=sizeof(SCROLLINFO);
    si.nPage=10;
    si.fMask=SIF_PAGE;
    m_sbScrollBarEdit.SetScrollInfo(&si);
    
    for (i=0; i<SCROLL_STEP_LEN; i++)
    {
        sprintf(strTemp, "%d", i+1+m_iStartLine);
        SetDlgItemText(g_iScrollNumberID[i], strTemp);

        SetDlgItemText(g_iScrollParamName[i], m_sParamStruct[i].strName);
        SetDlgItemText(g_iScrollParamContent[i], m_sParamStruct[i].strContent);

        iLen = strlen(m_sParamStruct[i].strContent);
        sprintf(strTemp, "%d", iLen);
        SetDlgItemText(g_iScrollParamLen[i], strTemp);
    }
    m_strParamFilePath.Format(g_sConfigreInfo.m_strParamEditFilepath);
    UpdateData(FALSE);
    m_dwTimer = SetTimer(1, 200, 0);
    GetDlgItem(IDC_BUTTON_SAVE_PARAM)->EnableWindow(FALSE);

    m_btnOpenParamFile.SetIcon(IDI_ICON_OPEN1, NULL, 40, 32);
    m_btnOpenParamFile.SetShowText(TRUE);
    m_btnOpenParamFile.SetActiveBgColor(RGB(200,200,200), FALSE);
    
    m_btnSaveParamFile.SetIcon(IDI_ICON_SAVE, NULL, 50, 32);
    m_btnSaveParamFile.SetShowText(TRUE);
    m_btnSaveParamFile.SetActiveBgColor(RGB(200,200,200), FALSE);
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CPageEditParam::IsThreadRun()
{
    if (m_bEditChangeFlag)
    {
        return m_bEditChangeFlag;
    }
    return g_bThreadRunFlag;
}

/****************************************************************************
  函数名     :  
  描述       :  拦截用户切换属性页事件，在用户切换属性页之前提示用户保存当前修改
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-04   V1.0        创建
****************************************************************************/
BOOL CPageEditParam::OnKillActive() 
{
    // TODO: Add your specialized code here and/or call the base class
    
    if( IsThreadRun() )
    {
        int iRet;

        if (m_bEditChangeFlag)
        {
            iRet =MessageBox(INFO_CHANGE_HINT, INFO_OPERATE_HINT,MB_YESNO|MB_ICONINFORMATION);
            if (iRet ==IDYES)
            {
                OnButtonSaveParam();
            }
        }
    }

    return CPropertyPage::OnKillActive();
}

/****************************************************************************
  函数名     :  void CPageEditParam::OnVScroll(UINT nSBCode, UINT nPos, 
                                   CScrollBar* pScrollBar) 
  描述       :  垂直滑块事件响应函数
  输入参数   :  1、UINT nSBCode：事件类型
                2、UINT nPos：响应事件前的位置
                3、CScrollBar* pScrollBar：响应事件的滑块指针
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-04   V1.0        创建
****************************************************************************/
void CPageEditParam::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    // TODO: Add your message handler code here and/or call default
    if (pScrollBar->GetDlgCtrlID() == IDC_SCROLLBAR_PARAM_EDIT)
    {
        int nCurrentPos = pScrollBar->GetScrollPos();
        switch(nSBCode)
        {
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            pScrollBar->SetScrollPos(nPos);
            break;
        case SB_LINEUP:
            pScrollBar->SetScrollPos(nCurrentPos-1);
            break;
        case SB_LINEDOWN:
            pScrollBar->SetScrollPos(nCurrentPos+1);
            break;
        case SB_PAGEUP:
            pScrollBar->SetScrollPos(nCurrentPos-SCROLL_STEP_LEN);
            break;
        case SB_PAGEDOWN:
            pScrollBar->SetScrollPos(nCurrentPos+SCROLL_STEP_LEN);
            break;
        }
        nCurrentPos = pScrollBar->GetScrollPos();
        if (nCurrentPos != m_iStartLine)
        {
            InterfaceToBuffer();
            m_iStartLine = nCurrentPos;
            if ((m_iStartLine+SCROLL_STEP_LEN) > MAX_PARAM_NUMBER)
            {
                m_iStartLine = MAX_PARAM_NUMBER-SCROLL_STEP_LEN-1;
            }
            BufferToInterface();
        }
    }

    CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

/****************************************************************************
  函数名     :  void CPageEditParam::OnButtonOpenSaveParamFile()
  描述       :  响应打开文件按钮
  输入参数   :  无
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-04   V1.0        创建
****************************************************************************/
void CPageEditParam::OnButtonOpenSaveParamFile() 
{
    // TODO: Add your control notification handler code here
	static char BASED_CODE szMergeFilter[] = _T("open file(*.env)||_*.env||)");
    CString filter;
    filter = szMergeFilter;

    CFileDialog dlg(TRUE, NULL, m_strParamFilePath, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST,filter, 0);
    if (dlg.DoModal() == IDOK)
    {
        m_strParamFilePath = dlg.GetPathName();
        strcpy(g_sConfigreInfo.m_strParamEditFilepath, 
            m_strParamFilePath.GetBuffer(0));  // 更新配置内容
        UpdateData(FALSE);    // 更新界面
        ReadParamFile();      // 读取参数文件内容
        BufferToInterface();
    }
}

/****************************************************************************
  函数名     :  void CPageEditParam::OnButtonSaveParam()
  描述       :  响应参数保存按键
  输入参数   :  无
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-04   V1.0        创建
****************************************************************************/
void CPageEditParam::OnButtonSaveParam() 
{
    // TODO: Add your control notification handler code here
    InterfaceToBuffer();
    SaveConfigInfo();  // 保存配置信息
    SaveParamFile();   // 保存参数文件内容
    m_bEditChangeFlag = FALSE;
    GetDlgItem(IDC_BUTTON_SAVE_PARAM)->EnableWindow(FALSE);
}

/****************************************************************************
  函数名     :  void CPageEditParam::ReadParamFile()
  描述       :  读取参数文件内容，将读取的内容保存到缓冲区中
  输入参数   :  无
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-04   V1.0        创建
****************************************************************************/
void CPageEditParam::ReadParamFile()
{
    FILE *fp;

    m_iStartLine = 0;

    memset(m_sParamStruct, 0, MAX_PARAM_NUMBER*PARAM_ITEM_LEN);
    // 读取配置文件，要先确保配置文件路径不为空
    if (0 != strlen(g_sConfigreInfo.m_strParamEditFilepath))
    {
        fp = fopen(g_sConfigreInfo.m_strParamEditFilepath, "rb");
        if (NULL != fp)
        {
            (void)fread(m_sParamStruct, 1, MAX_PARAM_NUMBER*PARAM_ITEM_LEN, fp);
            fclose(fp);
        }
    }
}


/****************************************************************************
  函数名     :  void CPageEditParam::SaveParamFile()
  描述       :  保存参数文件内容，保存之前先删除文件
  输入参数   :  无
  输出参数   :  无
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-03   V1.0        创建
****************************************************************************/
void CPageEditParam::SaveParamFile()
{
    FILE *fp;
    int i, iLen;

    // 查找有效的参数开始位置
    for (i=MAX_PARAM_NUMBER; i>0; i--)
    {
        iLen = strlen(m_sParamStruct[i-1].strName);
        if (0 != iLen)
        {
            m_iParamLineNumber = i;
            break;
        }
    }

    // 保存配置文件，要先确保配置文件路径不为空
    if (0 != strlen(g_sConfigreInfo.m_strParamEditFilepath))
    {
        DeleteFile(g_sConfigreInfo.m_strParamEditFilepath);  // 先删除参数文件
        fp = fopen(g_sConfigreInfo.m_strParamEditFilepath, "wb");
        if (NULL != fp)
        {
            fseek(fp, 0, SEEK_SET);
            fwrite(m_sParamStruct, 1, 
                m_iParamLineNumber*PARAM_ITEM_LEN, fp);
            fclose(fp);
        }
        else
        {
            MessageBox("Save param file fault!");
        }
    }
    else
    {
        // TODO: Add your control notification handler code here
	    static char BASED_CODE szMergeFilter[] = _T("Save file(*.env)||_*.env||)");
        CString filter;
        filter = szMergeFilter;

        CFileDialog dlg(FALSE, NULL, m_strParamFilePath, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST,filter, 0);
        if (dlg.DoModal() == IDOK)
        {
            m_strParamFilePath = dlg.GetPathName();
            strcpy(g_sConfigreInfo.m_strParamEditFilepath, 
                m_strParamFilePath.GetBuffer(0));  // 更新配置内容

            UpdateData(FALSE);    // 更新界面

            DeleteFile(g_sConfigreInfo.m_strParamEditFilepath);  // 先删除参数文件
            fp = fopen(g_sConfigreInfo.m_strParamEditFilepath, "wb");
            if (NULL != fp)
            {
                fseek(fp, 0, SEEK_SET);
                fwrite(m_sParamStruct, 1, 
                    m_iParamLineNumber*PARAM_ITEM_LEN, fp);
                fclose(fp);
            }
            else
            {
                MessageBox("Save param file fault!");
            }
        }
    }
}

/****************************************************************************
  函数名     :  
  描述       :  将界面上的数据更新到缓冲区内
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-05   V1.0        创建
****************************************************************************/
void CPageEditParam::InterfaceToBuffer()
{
    int i;
    char strTemp[PARAM_ITEM_LEN];

    // 将界面上的数据全部拷贝下来
    for (i=0; i<SCROLL_STEP_LEN; i++)
    {
        GetDlgItemText(g_iScrollParamName[i], strTemp, PARAM_ITEM_LEN);
        strTemp[PARAM_NAME_LEN-1] = 0;
        if (0 != strcmp(m_sParamStruct[m_iStartLine+i].strName,strTemp))
        {
            memset(m_sParamStruct[m_iStartLine+i].strName, 0, PARAM_NAME_LEN);
            strcpy(m_sParamStruct[m_iStartLine+i].strName, strTemp);
            m_bEditChangeFlag = TRUE;
            GetDlgItem(IDC_BUTTON_SAVE_PARAM)->EnableWindow(TRUE);
        }

        GetDlgItemText(g_iScrollParamContent[i], strTemp, PARAM_ITEM_LEN);
        strTemp[PARAM_CONTENT_LEN-1] = 0;
        if (0 != strcmp(m_sParamStruct[m_iStartLine+i].strContent,strTemp))
        {
            memset(m_sParamStruct[m_iStartLine+i].strContent, 0, PARAM_CONTENT_LEN);
            strcpy(m_sParamStruct[m_iStartLine+i].strContent, strTemp);
            m_bEditChangeFlag = TRUE;
            GetDlgItem(IDC_BUTTON_SAVE_PARAM)->EnableWindow(TRUE);
        }
    }
}

/****************************************************************************
  函数名     :  
  描述       :  将缓冲内的数据更新到界面上
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-05   V1.0        创建
****************************************************************************/
void CPageEditParam::BufferToInterface()
{
    int i, iLen, iLen2;
    char strTemp[PARAM_ITEM_LEN];
    char strInterface[PARAM_ITEM_LEN];

    // 显示有效的参数项信息
    for (i=0; i<SCROLL_STEP_LEN; i++)
    {
        GetDlgItemText(g_iScrollNumberID[i], strInterface, PARAM_ITEM_LEN);
        sprintf(strTemp, "%d", m_iStartLine+i+1);
        if (0 != strcmp(strTemp, strInterface))
        {
            SetDlgItemText(g_iScrollNumberID[i], strTemp);
            iLen = strlen(strTemp);
            ((CEdit*)GetDlgItem(g_iScrollNumberID[i]))->SetSel(iLen, iLen);
        }

        GetDlgItemText(g_iScrollParamContent[i], strInterface, PARAM_ITEM_LEN);
        if (0 != strcmp(m_sParamStruct[m_iStartLine+i].strContent,strInterface))
        {
            SetDlgItemText(g_iScrollParamContent[i], m_sParamStruct[m_iStartLine+i].strContent);
            iLen = strlen(m_sParamStruct[m_iStartLine+i].strName);
            ((CEdit*)GetDlgItem(g_iScrollParamContent[i]))->SetSel(iLen, iLen);
        }

        GetDlgItemText(g_iScrollParamName[i], strInterface, PARAM_ITEM_LEN);
        iLen = strlen(m_sParamStruct[m_iStartLine+i].strName);
        if (0 != strcmp(m_sParamStruct[m_iStartLine+i].strName,strInterface))
        {
            SetDlgItemText(g_iScrollParamName[i], m_sParamStruct[m_iStartLine+i].strName);
            ((CEdit*)GetDlgItem(g_iScrollParamName[i]))->SetSel(iLen, iLen);
        }

        iLen2 = strlen(m_sParamStruct[m_iStartLine+i].strContent);
        if ((0==iLen2) || (0==iLen))
        {
            sprintf(strTemp, "");
        }
        else
        {
            sprintf(strTemp, "%d", iLen2);
        }
        SetDlgItemText(g_iScrollParamLen[i], strTemp);
    }
}

/****************************************************************************
  函数名     :  
  描述       :  界面输入合法性检查函数
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-05   V1.0        创建
  2、 黄俊斌     2008-05-15   V1.1        修改使之能够拷贝参数内容
****************************************************************************/
void CPageEditParam::InterfaceValidCheck()
{
    char strTemp[128];
    int i, iLen;
    int iFlag;

    InterfaceToBuffer();

    iFlag = 0;

    // 参数名字字符串超长检查，并对超长部分进行截断
    for (i=0; i<SCROLL_STEP_LEN; i++)
    {
        GetDlgItemText(g_iScrollParamName[i], strTemp, PARAM_ITEM_LEN);
        strTemp[PARAM_ITEM_LEN-1] = 0;
        iLen = strlen(strTemp);
        if (iLen > PARAM_NAME_MAX_LEN)
        {
            iLen = PARAM_NAME_MAX_LEN;
            strTemp[PARAM_NAME_MAX_LEN] = 0;
            SetDlgItemText(g_iScrollParamName[i], strTemp);
            iFlag = 1;
            ((CEdit*)GetDlgItem(g_iScrollParamName[i]))->SetSel(iLen, iLen);
        }
    }

    // 参数数据字符串超长检查，并对超长部分进行截断
    for (i=0; i<SCROLL_STEP_LEN; i++)
    {
        GetDlgItemText(g_iScrollParamContent[i], strTemp, PARAM_ITEM_LEN);
        strTemp[PARAM_ITEM_LEN-1] = 0;
        iLen = strlen(strTemp);
        if (iLen > PARAM_CONTENT_MAX_LEN)
        {
            iLen = PARAM_CONTENT_MAX_LEN;
            strTemp[PARAM_CONTENT_MAX_LEN] = 0;
            SetDlgItemText(g_iScrollParamContent[i], strTemp);
            iFlag = 1;
            ((CEdit*)GetDlgItem(g_iScrollParamContent[i]))->SetSel(iLen, iLen);
        }
    }
    if ((m_bEditChangeFlag) || (1==iFlag))
    {
        BufferToInterface();
    }
}

/****************************************************************************
  函数名     :  
  描述       :  鼠标滑轮响应函数
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-05   V1.0        创建
****************************************************************************/
BOOL CPageEditParam::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
    // TODO: Add your message handler code here and/or call default
    
    WORD nStepNum;
    POINT point;

    point = pt;
    ScreenToClient(&point);

    if (((nFlags==MK_MBUTTON)||(nFlags==0))
        && ((point.x>24)&&(point.x<535)) 
        && ((point.y>125)&&(point.y<340)))
    {
        InterfaceToBuffer();
        nStepNum = abs(zDelta) / 120;
        if (zDelta > 0)                // up
        {
            if (m_iStartLine > nStepNum)
                m_iStartLine -= nStepNum;
            else
                m_iStartLine = 0;
        }
        else                        // down
        {
            if (m_iStartLine < (WORD)(MAX_PARAM_NUMBER-nStepNum-SCROLL_STEP_LEN))
                m_iStartLine += nStepNum;
            else
                m_iStartLine = MAX_PARAM_NUMBER - SCROLL_STEP_LEN;
        }

        m_sbScrollBarEdit.SetScrollPos(m_iStartLine, TRUE);
        BufferToInterface();
        CWnd *pWndLoad = GetDlgItem(IDC_EDIT_PARAM_NAME1);
        GotoDlgCtrl(pWndLoad);
    }
    return CPropertyPage::OnMouseWheel(nFlags, zDelta, pt);
}

/****************************************************************************
  函数名     :  
  描述       :  定时检查响应函数，每200毫秒响应一次
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-05   V1.0        创建
****************************************************************************/
void CPageEditParam::OnTimer(UINT nIDEvent) 
{
    // TODO: Add your message handler code here and/or call default
    InterfaceValidCheck();
    CPropertyPage::OnTimer(nIDEvent);
}


