// DelAppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LoadTools.h"
#include "DelAppDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


char g_strDelAppName[33];
/////////////////////////////////////////////////////////////////////////////
// CDelAppDlg dialog


CDelAppDlg::CDelAppDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CDelAppDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDelAppDlg)
    m_strAppName = _T("");
    //}}AFX_DATA_INIT
    memset(g_strDelAppName, 0, sizeof(g_strDelAppName));
}


void CDelAppDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDelAppDlg)
    DDX_Text(pDX, IDC_EDIT_APP_NAME, m_strAppName);
    DDV_MaxChars(pDX, m_strAppName, 32);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelAppDlg, CDialog)
    //{{AFX_MSG_MAP(CDelAppDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelAppDlg message handlers

void CDelAppDlg::OnCancel() 
{
    // TODO: Add extra cleanup here
    CDialog::OnCancel();
}

void CDelAppDlg::OnOK() 
{
    // TODO: Add extra validation here
    UpdateData(TRUE);
    strcpy(g_strDelAppName, m_strAppName.GetBuffer(0));
    CDialog::OnOK();
}
