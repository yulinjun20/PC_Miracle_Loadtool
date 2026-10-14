#if !defined(AFX_SHOWAPPINFO_H__F9D87600_BDE8_48FB_818E_76C392A1F013__INCLUDED_)
#define AFX_SHOWAPPINFO_H__F9D87600_BDE8_48FB_818E_76C392A1F013__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowAppInfo.h : header file
//
#include "LoadToolsDefine.h"

/////////////////////////////////////////////////////////////////////////////
// CShowAppInfo dialog

class CShowAppInfo : public CDialog
{
// Construction
public:
    CShowAppInfo(CWnd* pParent = NULL);   // standard constructor

    void SetAppInfo(APPLICATION_INFO *pAppInfo);
    void ByteToSting(BYTE *pbyInput, BYTE byInputLen, char *strOut);
    
// Dialog Data
    //{{AFX_DATA(CShowAppInfo)
    enum { IDD = IDD_DIALOG_SHOW_APP_INFO };
        // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CShowAppInfo)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CShowAppInfo)
        // NOTE: the ClassWizard will add member functions here
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWAPPINFO_H__F9D87600_BDE8_48FB_818E_76C392A1F013__INCLUDED_)
