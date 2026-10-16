#if !defined(AFX_PAGEEDITPARAM_H__152BE24C_0CAA_4009_AFF8_AA6013D2543C__INCLUDED_)
#define AFX_PAGEEDITPARAM_H__152BE24C_0CAA_4009_AFF8_AA6013D2543C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageEditParam.h : header file
//

#include "FlatButton.h"
#include "EnvJson.h"

#define INFO_CHANGE_HINT		"Save it?"
#define INFO_OPERATE_HINT		"Clue on"

#define SCROLL_STEP_LEN          10
/* JSON env limits (see EnvJson.h / Docs/EnvFormat.md) */
#define PARAM_NAME_MAX_LEN      ENV_JSON_NAME_MAX
#define PARAM_CONTENT_MAX_LEN   ENV_JSON_VALUE_MAX

#define PARAM_NAME_LEN        (ENV_JSON_NAME_MAX + 1)
#define PARAM_CONTENT_LEN     (ENV_JSON_VALUE_MAX + 1)
#define MAX_PARAM_NUMBER      ENV_JSON_MAX_ITEMS
#define PARAM_ITEM_LEN        (PARAM_NAME_LEN + PARAM_CONTENT_LEN)
/* Temp buffer for UI get/set must fit the longer of name/value */
#define PARAM_TEMP_BUF_LEN    PARAM_CONTENT_LEN

typedef ENV_PARAM_ITEM PARAM_STRUCT;


/////////////////////////////////////////////////////////////////////////////
// CPageEditParam dialog

class CPageEditParam : public CPropertyPage
{
    DECLARE_DYNCREATE(CPageEditParam)

// Construction
public:
    CPageEditParam();
    ~CPageEditParam();
    
    // �Ƿ����߳�������
    BOOL IsThreadRun();

    void ReadParamFile();

    void SaveParamFile();

    void InterfaceToBuffer();

    void BufferToInterface();

    void InterfaceValidCheck();

    int m_iStartLine;           // ��ǰ������ʾ�ĳ�ʼ�У�ȡֵ��0��ʼ
    int m_iParamLineNumber;     // �������ڲ�����������
    BOOL m_bEditChangeFlag;     // �޸ı�־��FALSEδ�޸ģ�TRUE���޸�

    PARAM_STRUCT m_sParamStruct[MAX_PARAM_NUMBER];  // ���ҳ����Ϣ
    DWORD m_dwTimer;   // ��ʱ�����


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
