#if !defined(AFX_PAGEAPPLICATION_H__8AA0B35A_8608_4C52_B9C0_F2E7486A9AC8__INCLUDED_)
#define AFX_PAGEAPPLICATION_H__8AA0B35A_8608_4C52_B9C0_F2E7486A9AC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageApplication.h : header file
//
#include "LoadToolsDefine.h"
#include "FlatButton.h"
/////////////////////////////////////////////////////////////////////////////
// CPageApplication dialog


class CPageApplication : public CPropertyPage
{
    DECLARE_DYNCREATE(CPageApplication)

// Construction
public:
    CPageApplication();
    ~CPageApplication();

    
    BOOL IsThreadRun();        // 是否有线程在运行

    APPLICATION_LIST m_saAppInfo[MAX_APPLICATION_NUMBER];

    CString m_strAppFilePath;  // 保存当前选择的应用程序保存路径
    BOOL m_bStopDownloadFlag;
    BYTE m_abyVposType[16];

    // 当前正在下载的应用程序编号，-1表示当前没有下载过应用程序，0~24表示
    // 当前下载过应用程序
    int m_iCurrentAppNo;

    void MyShowMessage(char *strMsg);  // 在消息静态文本处显示消息

    void InsertOneAppFilePath();   // 插入一条应用程序路径

    // 数字字符串加一操作
    void StringAddOne(BYTE *pbyInData, BYTE *pbyOutData);

    void MyEnableWindows(BOOL bFlag);

    int Handshack();
    int UsbHandshack();
    int SerialHandshack();
    
    int Authentication();

    int DownloadStart();

    void DownloadEnd();

    int UpdateTreeList();

    int DownloadProcess();

    int DownloadBaleFile();

    int ParseBaleFile(FILE *fp, DWORD dwOffset, BYTE byFileType);

    int DeleteFileProcess(BYTE *pbyInData);

    int SetVposTime();

    int DownloadFile(char *strFileName, BYTE byFileType, BYTE byAppNo);
    
// Dialog Data
    //{{AFX_DATA(CPageApplication)
    enum { IDD = IDD_DIALOG_APPLICATION };
    CTreeCtrl    m_tcVposFileList;       // VPOS上的显示信息树
    CComboBox    m_cbAppFilePath;        // 应用程序路径组合框
    CString    m_strBaleFilePath;          // 打包文件存放路径
    CString    m_strFontFilePath;          // 字库文件存放路径
    CString    m_strParamFilePath;         // 参数文件存放路径
    CString    m_strVposSN;                // 序列号编辑控件影射的字符串
    CString    m_strDataFilePath;          // 数据文件存放路径
    BOOL    m_bAppSelect;               // 应用程序选中标志
    BOOL    m_bBaleFileSelect;          // 打包文件选中标志
    BOOL    m_bCreateFileSystemSelect;  // 创建文件系统选中标志
    BOOL    m_bFontFileSelect;          // 字库文件选中标志
    BOOL    m_bParamFileSelect;         // 参数文件选中标志
    BOOL    m_bDownloadSNSelect;        // 下载序列号选中标志
    BOOL    m_bSetTimeSelect;           // 设置时间选中标志
    BOOL    m_bDataFileSelect;          // 数据文件选中标志

    CFlatButton m_btnDownload;
    CFlatButton m_btnStop;
    CFlatButton m_btnDeleteApp;
    CFlatButton m_btnReflush;

    CFlatButton m_btnOpenAppFile;
    CFlatButton m_btnOpenFontFile;
    CFlatButton m_btnOpenBaleFile;
    CFlatButton m_btnOpenParamFile;
    CFlatButton m_btnOpenDataFile;
    
    CFlatButton m_btnDeleteAppendFile;
    CFlatButton m_btnShowAppInfo;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CPageApplication)
    public:
    virtual BOOL OnKillActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CPageApplication)
    virtual BOOL OnInitDialog();
    afx_msg void OnButtonRefurbish();
    afx_msg void OnButtonDeleteApp();
    afx_msg void OnButtonDeleteAppendFile();
    afx_msg void OnButtonShowAppInfo();
    afx_msg void OnButtonBeginDownloadApp();
    afx_msg void OnButtonStopDownloadApp();
    afx_msg void OnButtonOpenAppFile();
    afx_msg void OnButtonOpenFontFile();
    afx_msg void OnButtonOpenBaleFile();
    afx_msg void OnButtonOpenParamFile();
    afx_msg void OnButtonOpenDataFile();
    afx_msg void OnCheckAppSelect();
    afx_msg void OnCheckBaleFile();
    afx_msg void OnCheckCreateFileSystem();
    afx_msg void OnCheckFontFile();
    afx_msg void OnCheckParamFile();
    afx_msg void OnCheckSerialNumber();
    afx_msg void OnCheckSetTime();
    afx_msg void OnCheckDataFile();
    afx_msg void OnChangeEditBaleFilePath();
    afx_msg void OnChangeEditFontFilePath();
    afx_msg void OnChangeEditParamFilePath();
    afx_msg void OnChangeEditVposSerialNumber();
    afx_msg void OnEditchangeComboAppFilePath();
    afx_msg void OnSelchangeComboAppFilePath();
    afx_msg void OnChangeEditDataFilePath();
    afx_msg void OnDblclkTreeFileList(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEAPPLICATION_H__8AA0B35A_8608_4C52_B9C0_F2E7486A9AC8__INCLUDED_)
