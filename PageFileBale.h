#if !defined(AFX_PAGEFILEBALE_H__61BA3700_1CA6_423F_B655_3E91C70EA7B2__INCLUDED_)
#define AFX_PAGEFILEBALE_H__61BA3700_1CA6_423F_B655_3E91C70EA7B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageFileBale.h : header file
//

#include "FlatButton.h"

#define OBJECT_VALID    1
#define OBJECT_INVALID  0

#define APP_FILE_MAX_NUM      25
#define APPEND_FILE_MAX_NUM   50

typedef struct _CLEAR_TASK_TYPE_
{
    BYTE m_abyDisplayInfo[299];   // 界面上显示的信息
    BYTE m_byValidFlag;           // 对象的有效性，0表示无效，1表示有效
    HTREEITEM m_hTree;            // 对象句柄
}CLEAR_TASK_TYPE;

typedef struct _COMMON_FILE_LIST_
{
    BYTE m_abyDisplayInfo[299];   // 界面上显示的信息
    HTREEITEM m_hTree;            // 对象句柄
}COMMON_FILE_LIST;

typedef struct _APPEND_FILE_LIST_
{
    // 2：参数文件；3：数据文件；6：参数文件，带有应用名；7：数据文件，带有应用名
    BYTE m_byTaskType;

    BYTE m_abyDisplayInfo[271];   // 界面上显示的信息
    HTREEITEM m_hTree;            // 对象句柄
    HTREEITEM m_hFatherTree;            // 对象句柄
}APPEND_FILE_LIST;


/////////////////////////////////////////////////////////////////////////////
// CPageFileBale dialog

class CPageFileBale : public CPropertyPage
{
    DECLARE_DYNCREATE(CPageFileBale)

// Construction
public:
    CPageFileBale();
    ~CPageFileBale();

    // 是否有线程在运行
    BOOL IsThreadRun();

    // 获取任务个数。
    int GetTaskNumber();

    void IntiPosType();

    // 保存配置信息，将配置内容保存到配置文件中
    void SaveConfig();

    // 从配置文件中读取配置内容，并调用解析和显示函数显示配置内容
    void GetConfig();

    // 解析并显示配置内容
    void ParseAndDisplayConfig(char *strInConfig, int iConfigLen);

    // 检查参数文件名的有效性，参数文件名不能为空或者长度大于15字节
    BOOL CheckParamFileNameValidate(char *strInName);

    int m_iAppFileNumber;

    COMMON_FILE_LIST m_sAppFileInfo[APP_FILE_MAX_NUM];
    int m_iAppendFileNumber;
    APPEND_FILE_LIST m_sAppendFileInf[APPEND_FILE_MAX_NUM];

    int m_iDelAppTaskNumber;  // 删除应用的任务个数

    CLEAR_TASK_TYPE m_sDelAppInfo[APP_FILE_MAX_NUM];

    CLEAR_TASK_TYPE m_sCreateSystem;
    
    CLEAR_TASK_TYPE m_sDelAllApp;

    COMMON_FILE_LIST m_sFontFileInfo;

    COMMON_FILE_LIST m_sVosFileInfo;
    BOOL m_bAddVosFlag;

    
// Dialog Data
    //{{AFX_DATA(CPageFileBale)
    enum { IDD = IDD_DIALOG_FILE_BALE };
    CTreeCtrl    m_tcTreeCtrlTaskList;
    CFlatButton m_btnAddAppFile;
    CFlatButton m_btnAddFontFile;
    CFlatButton m_btnAddParamFile;
    CFlatButton m_btnAddDataFile;
    CFlatButton m_btnAddDeleteApp;
    CFlatButton m_btnAddCreateFileSystem;
    CFlatButton m_btnDeleteTask;
    CFlatButton m_btnStartBale;
    CFlatButton m_btnAddVosFile;
    CFlatButton m_btnAddDelAllApp;  //IDC_BUTTON_SAVE_INFO
    CFlatButton m_btnSaveInfo;
    //}}AFX_DATAAFX_DATAAFX_DATAAFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CPageFileBale)
    public:
    virtual BOOL OnKillActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CPageFileBale)
    virtual BOOL OnInitDialog();
    afx_msg void OnButtonAddApp();
    afx_msg void OnButtonAddFont();
    afx_msg void OnButtonAddParam();
    afx_msg void OnButtonAddCreateFileSystem();
    afx_msg void OnButtonAddAppDelete();
    afx_msg void OnButtonDelTask();
    afx_msg void OnButtonStartBale();
    afx_msg void OnButtonAddVos();
    afx_msg void OnButtonDelAllApp();
    afx_msg void OnRadioRemoteDownload();
    afx_msg void OnRadioLocalDownload();
    afx_msg void OnButtonSaveInfo();
    afx_msg void OnButtonAddDataFile();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEFILEBALE_H__61BA3700_1CA6_423F_B655_3E91C70EA7B2__INCLUDED_)
