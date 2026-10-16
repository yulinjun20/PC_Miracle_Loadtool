#if !defined(AFX_PAGEFIRMWARE_H__48C3123B_5778_4DC5_B4AA_698B694A9415__INCLUDED_)
#define AFX_PAGEFIRMWARE_H__48C3123B_5778_4DC5_B4AA_698B694A9415__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageFirmware.h : header file
//

#include "FlatButton.h"

/////////////////////////////////////////////////////////////////////////////
// CPageFirmware dialog

class CPageFirmware : public CPropertyPage
{
    DECLARE_DYNCREATE(CPageFirmware)

// Construction
public:
    CPageFirmware();
    ~CPageFirmware();

    // 是否有线程在运行
    BOOL IsThreadRun();
    
    void MyShowMessage(char *strMsg);

    BOOL m_bStopDownloadFlag;
    
    BYTE m_abyVposType[16];

    void MyEnableWindows(BOOL bFlag);

    int Handshack();
    
    int DownloadStart();
    
    int DownloadProcess();

    int DownloadFile(char *strFileName, BYTE byFileType);
    
    void DownloadEnd();
    
// Dialog Data
    //{{AFX_DATA(CPageFirmware)
    enum { IDD = IDD_DIALOG_FIRMWARE };
    CString    m_strVosFilePath;
    BOOL    m_bVOSSelect;

    CFlatButton m_btnDownload;
    CFlatButton m_btnStop;
    CFlatButton m_btnOpenVosFile;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CPageFirmware)
    public:
    virtual BOOL OnKillActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CPageFirmware)
    virtual BOOL OnInitDialog();
    afx_msg void OnButtonOpenVosFile();
    afx_msg void OnButtonDownloadFirmware();
    afx_msg void OnButtonStopFirmware();
    afx_msg void OnCheckVosSelect();
    afx_msg void OnChangeEditVosFilePath();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEFIRMWARE_H__48C3123B_5778_4DC5_B4AA_698B694A9415__INCLUDED_)
