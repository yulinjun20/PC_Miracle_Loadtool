// LoadToolsDlg.h : header file
//

#if !defined(AFX_LOADTOOLSDLG_H__1A42EE0E_788B_41DC_9B2B_7567322F870D__INCLUDED_)
#define AFX_LOADTOOLSDLG_H__1A42EE0E_788B_41DC_9B2B_7567322F870D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "PageApplication.h"
#include "PageCommSet.h"
#include "PageFileBale.h"
#include "PageEditParam.h"


/////////////////////////////////////////////////////////////////////////////
// CLoadToolsDlg dialog

class CLoadToolsDlg : public CDialog
{
// Construction
public:
	CLoadToolsDlg(CWnd* pParent = NULL);	// standard constructor

    CPageApplication m_PageApplication;
    CPageCommSet m_PageCommSet;
    CPageFileBale m_PageFileBale;
    CPageEditParam m_PageEditParam;


    CPropertySheet m_sheet;
    void ApplySheetMode(BOOL bFactory);  /* hide Comm set / Edit param */
    BOOL m_bFactorySheetMode;
// Dialog Data
	//{{AFX_DATA(CLoadToolsDlg)
	enum { IDD = IDD_LOADTOOLS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadToolsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLoadToolsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADTOOLSDLG_H__1A42EE0E_788B_41DC_9B2B_7567322F870D__INCLUDED_)
