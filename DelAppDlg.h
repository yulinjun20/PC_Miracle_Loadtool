#if !defined(AFX_DELAPPDLG_H__108CD4E1_AE7F_4AFD_BB53_7290F590D077__INCLUDED_)
#define AFX_DELAPPDLG_H__108CD4E1_AE7F_4AFD_BB53_7290F590D077__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DelAppDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDelAppDlg dialog

class CDelAppDlg : public CDialog
{
// Construction
public:
    CDelAppDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CDelAppDlg)
    enum { IDD = IDD_DIALOG_DEL_APP };
    CString    m_strAppName;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDelAppDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CDelAppDlg)
    virtual void OnCancel();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELAPPDLG_H__108CD4E1_AE7F_4AFD_BB53_7290F590D077__INCLUDED_)
