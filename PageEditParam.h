#if !defined(AFX_PAGEEDITPARAM_H__152BE24C_0CAA_4009_AFF8_AA6013D2543C__INCLUDED_)
#define AFX_PAGEEDITPARAM_H__152BE24C_0CAA_4009_AFF8_AA6013D2543C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageEditParam.h : header file
//

#include "FlatButton.h"

#define INFO_CHANGE_HINT		"Save it?"
#define INFO_OPERATE_HINT		"Clue on"

#define SCROLL_STEP_LEN          10
#define PARAM_NAME_MAX_LEN        7
#define PARAM_CONTENT_MAX_LEN   119

#define PARAM_NAME_LEN        8
#define PARAM_CONTENT_LEN   120
#define MAX_PARAM_NUMBER    600
#define PARAM_ITEM_LEN    (PARAM_NAME_LEN+PARAM_CONTENT_LEN)

typedef struct {
    char strName[PARAM_NAME_LEN];
    char strContent[PARAM_CONTENT_LEN];
} PARAM_STRUCT;

/////////////////////////////////////////////////////////////////////////////
// CPageEditParam dialog

class CPageEditParam : public CPropertyPage
{
    DECLARE_DYNCREATE(CPageEditParam)

// Construction
public:
    CPageEditParam();
    ~CPageEditParam();
    
    // 是否有线程在运行
    BOOL IsThreadRun();

    void ReadParamFile();

    void SaveParamFile();

    void InterfaceToBuffer();

    void BufferToInterface();

    void InterfaceValidCheck();

    int m_iStartLine;           // 当前界面显示的初始行，取值从0开始
    int m_iParamLineNumber;     // 缓冲区内参数的总行数
    BOOL m_bEditChangeFlag;     // 修改标志，FALSE未修改，TRUE已修改

    PARAM_STRUCT m_sParamStruct[MAX_PARAM_NUMBER];  // 存放页面信息
    DWORD m_dwTimer;   // 定时器编号


// Dialog Data
    //{{AFX_DATA(CPageEditParam)
    enum { IDD = IDD_DIALOG_EDIT_PARAM };
    CScrollBar    m_sbScrollBarEdit;
    CString    m_strParamFilePath;

    CFlatButton m_btnOpenParamFile;
    CFlatButton m_btnSaveParamFile;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CPageEditParam)
    public:
    virtual BOOL OnKillActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CPageEditParam)
    virtual BOOL OnInitDialog();
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnButtonOpenSaveParamFile();
    afx_msg void OnButtonSaveParam();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnTimer(UINT nIDEvent);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEEDITPARAM_H__152BE24C_0CAA_4009_AFF8_AA6013D2543C__INCLUDED_)
