#if !defined(AFX_PAGECOMMSET_H__BF7B84A2_D507_4DB5_AECA_1F812C3C02FB__INCLUDED_)
#define AFX_PAGECOMMSET_H__BF7B84A2_D507_4DB5_AECA_1F812C3C02FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageCommSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageCommSet dialog

class CPageCommSet : public CPropertyPage
{
    DECLARE_DYNCREATE(CPageCommSet)

// Construction
public:
    CPageCommSet();
    ~CPageCommSet();

    // 是否有线程在运行
    BOOL IsThreadRun();
    
// Dialog Data
    //{{AFX_DATA(CPageCommSet)
    enum { IDD = IDD_DIALOG_COMM_SET };
    int    m_iSerialPortNumber;
    int    m_iSerialPortBauds;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CPageCommSet)
    public:
    virtual BOOL OnKillActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CPageCommSet)
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeComboComBaud();
    afx_msg void OnSelchangeComboComPortNumber();
    afx_msg void OnRadioUseCom();
    afx_msg void OnRadioUseUsb();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGECOMMSET_H__BF7B84A2_D507_4DB5_AECA_1F812C3C02FB__INCLUDED_)
