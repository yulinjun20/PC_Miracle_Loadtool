#if !defined(AFX_VPOSSELECT_H__E0F8FDFD_C5D2_4F1D_9DB8_2D87665429CC__INCLUDED_)
#define AFX_VPOSSELECT_H__E0F8FDFD_C5D2_4F1D_9DB8_2D87665429CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VPOSSelect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVPOSSelect dialog

#define VPOS_TYPE_NUMBER  21

class CVPOSSelect : public CDialog
{
// Construction
public:
    CVPOSSelect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CVPOSSelect)
    enum { IDD = IDD_DIALOG_VPOS_TYPE_SELECT };
    CComboBox    m_cbVposType;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CVPOSSelect)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CVPOSSelect)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VPOSSELECT_H__E0F8FDFD_C5D2_4F1D_9DB8_2D87665429CC__INCLUDED_)
