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

    CString m_strAppFilePath;  // 保存当前选择的应用程序保存路�?
    BOOL m_bStopDownloadFlag;
    BYTE m_abyVposType[16];

    // 当前正在下载的应用程序编号，-1表示当前没有下载过应用程序，0~24表示
    // 当前下载过应用程�?
    int m_iCurrentAppNo;

    void MyShowMessage(char *strMsg);  // 在消息静态文本处显示消息

    void SaveAppFilePath();   // save last App path to config

    // 数字字符串加一操作

    void MyEnableWindows(BOOL bFlag);

    int Handshack();
    int UsbHandshack();
    int SerialHandshack();
    
    int Authentication();

    int DownloadStart();

    /* byBaleContinue=1: Finish DATA[0]=1, device resumes download after OS reboot. */
    void DownloadEnd(BYTE byBaleContinue = 0);
    /* Finish DATA[0]=2: discard pending on device, do not install (fail/Stop). */
    void DownloadAbandon(void);
    /* Finish DATA[0]=3: keep pending, defer install, wait for OS 'T'. */
    void DownloadEndDeferOs(void);
    void ClearDownloadProgress(void);
    /* 1: bale already finished install cycle (OS path); skip caller DownloadEnd. */
    BOOL m_bBaleSkipFinalFinish;

    int UpdateTreeList();

    int DownloadProcess();

    int DownloadBaleFile();

    int ParseBaleFile(FILE *fp, DWORD dwOffset, BYTE byFileType);

    int DeleteFileProcess(BYTE *pbyInData);

    int SetVposTime();

    int DownloadFile(char *strFileName, BYTE byFileType, BYTE byAppNo);

    int DownloadOsStart();
    int DownloadOsFile(char *strFileName);
    void DownloadOsEnd();

    /* DownloadProtocol_v1 helpers */
    int  m_iDownloadMode;          /* DL_MODE_DEVELOPER / DL_MODE_FACTORY */
    int  m_iProgressTotal;
    int  m_iProgressDone;
    CString m_strProgressCurrent;
    CListCtrl m_listFactoryStatus;
    BOOL m_bFactoryPagesHidden;


    /* D15: separate Developer / Factory download selections */
    struct DL_SEL_CFG {
        BOOL bVOS, bApp, bSc, bWifi, bFont, bParam, bData, bLogo, bBale, bSync;
        char szVos[256], szApp[256], szSc[256], szWifi[256], szFont[256];
        char szParam[256], szData[256], szLogo[256], szBale[256];
    };
    DL_SEL_CFG m_cfgDev;
    DL_SEL_CFG m_cfgFac;
    void CaptureSelToCfg(DL_SEL_CFG &cfg);
    void ApplyCfgToSel(const DL_SEL_CFG &cfg);
    void LoadFactoryCfgFile();
    void SaveFactoryCfgFile();
    void ApplyModeUI(BOOL bFactory);
    void ApplySyncDownloadMutex(BOOL bFromSync);
    void ApplyOsDownloadMutex(BOOL bFromOs);
    BOOL ValidateBeforeStart();
    void UpdateTotalProgress(const char *szItem);
    void SetDownloadProgress(int pct);
    int  CountSelectedDownloadItems();
    void AbortFileSession(BYTE byReason);
    int  WaitDeviceAfterReboot();

    /* Factory one-to-many (D12..) */
    enum { FACTORY_SLOT_MAX = FACTORY_MAX_SLOTS };
    struct FACTORY_SLOT {
        volatile LONG busy;
        int portNo;              /* 1-based COM number */
        HANDLE hComm;
        HANDLE hThread;
        char sn[36];
        char progress[40];
        char status[80];
        int listRow;
        volatile LONG done;
        int result;
        int hsKind;              /* 0=none 1=L/A done 2=T/S done */
        int osCheckType;         /* g_iCommCheckType after T/S */
    };
    FACTORY_SLOT m_factorySlots[FACTORY_SLOT_MAX];
    CRITICAL_SECTION m_csFactory;
    volatile LONG m_nFactoryActive;
    BOOL m_bFactoryStopScan;
    /* COMs finished Success; unlock only when port vanishes (unplug) or new Start. */
    int  m_factoryDonePorts[256];
    int  m_factoryDoneMiss[256]; /* consecutive enum misses; unlock after 3 */
    int  m_nFactoryDonePorts;

    void FactoryInitSlots();
    void FactoryUpdateActiveLabel();
    void FactoryUpdateListRow(int slot);
    int  FactoryCountBusy();
    int  FactoryAllocSlot(int portNo, HANDLE hComm, const char *sn);
    void FactoryReleaseSlot(int slot);
    int  FactoryRunDeviceSession(int slot); /* OS legacy + new protocol */
    void FactoryPostStatus(int slot, const char *prog, const char *stat);
    afx_msg LRESULT OnFactoryUiUpdate(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnFactoryUiEnable(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnFactoryScanMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnAppShowMsg(WPARAM wParam, LPARAM lParam);
    void FactoryPostEnable(BOOL bStartEnable);
    void FactoryPostScanMsg(const char *msg);
    BOOL FactoryPortIsDone(int portNo);
    void FactoryMarkPortDone(int portNo);
    void FactoryPruneDonePorts(const int *ports, int nPorts);

    
// Dialog Data
    //{{AFX_DATA(CPageApplication)
    enum { IDD = IDD_DIALOG_APPLICATION };
    CTreeCtrl    m_tcVposFileList;       // VPOS上的显示信息�?
    CString    m_strBaleFilePath;          // 打包文件存放路径
    CString    m_strFontFilePath;          // 字库文件存放路径
    CString    m_strParamFilePath;         // 参数文件存放路径
    CString    m_strDataFilePath;          // 数据文件存放路径
    CString    m_strVosFilePath;           // OS file path
    CString    m_strScAppFilePath;         // SC App path (placeholder)
    CString    m_strWifiFilePath;          // WIFI path (placeholder)
    CString    m_strLogoFilePath;          // Logo path (placeholder)
    BOOL    m_bAppSelect;               // 应用程序选中标志
    BOOL    m_bBaleFileSelect;          // 打包文件选中标志
    BOOL    m_bBaleWholePkg;            // temp: send whole bale, device unpacks
    BOOL    m_bCreateFileSystemSelect;  // 创建文件系统选中标志
    BOOL    m_bFontFileSelect;          // 字库文件选中标志
    BOOL    m_bParamFileSelect;         // 参数文件选中标志
    BOOL    m_bSetTimeSelect;           // 设置时间选中标志
    BOOL    m_bDataFileSelect;
    BOOL    m_bScAppSelect;
    BOOL    m_bWifiSelect;
    BOOL    m_bLogoSelect;
    BOOL    m_bPubKeyL1Select;
    BOOL    m_bPubKeyL2Select;
    BOOL    m_bPubKeyL3Select;
    CString m_strPubKeyL1Path;
    CString m_strPubKeyL2Path;
    CString m_strPubKeyL3Path;
    BOOL    m_bVOSSelect;               // OS file select          // 数据文件选中标志

    CFlatButton m_btnDownload;
    CFlatButton m_btnStop;
    CFlatButton m_btnDeleteApp;
    CFlatButton m_btnReflush;

    CFlatButton m_btnOpenAppFile;
    CFlatButton m_btnOpenFontFile;
    CFlatButton m_btnOpenBaleFile;
    CFlatButton m_btnOpenParamFile;
    CFlatButton m_btnOpenDataFile;
    CFlatButton m_btnOpenVosFile;
    CFlatButton m_btnOpenScAppFile;
    CFlatButton m_btnOpenWifiFile;
    CFlatButton m_btnOpenLogoFile;
    CFlatButton m_btnOpenPubKeyL1;
    CFlatButton m_btnOpenPubKeyL2;
    CFlatButton m_btnOpenPubKeyL3;
    
    CFlatButton m_btnDeleteAppendFile;
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
    afx_msg void OnButtonOpenVosFile();
    afx_msg void OnButtonOpenScAppFile();
    afx_msg void OnButtonOpenWifiFile();
    afx_msg void OnButtonOpenLogoFile();
    afx_msg void OnButtonOpenPubKeyL1();
    afx_msg void OnButtonOpenPubKeyL2();
    afx_msg void OnButtonOpenPubKeyL3();
    afx_msg void OnCheckVosSelect();
    afx_msg void OnChangeEditVosFilePath();
    afx_msg void OnCheckAppSelect();
    afx_msg void OnCheckBaleFile();
    afx_msg void OnCheckBaleWholePkg();
    void UpdateBaleWholePkgUi();
    afx_msg void OnCheckCreateFileSystem();
    afx_msg void OnCheckFontFile();
    afx_msg void OnCheckParamFile();
    afx_msg void OnCheckSetTime();
    afx_msg void OnCheckDataFile();
    afx_msg void OnCheckScAppFile();
    afx_msg void OnCheckWifiFile();
    afx_msg void OnCheckLogoFile();
    afx_msg void OnCheckPubKeyL1();
    afx_msg void OnCheckPubKeyL2();
    afx_msg void OnCheckPubKeyL3();
    afx_msg void OnRadioModeDeveloper();
    afx_msg void OnRadioModeFactory();
    afx_msg void OnChangeEditBaleFilePath();
    afx_msg void OnChangeEditFontFilePath();
    afx_msg void OnChangeEditParamFilePath();
    afx_msg void OnChangeEditAppFilePath();
    afx_msg void OnChangeEditDataFilePath();
    afx_msg void OnChangeEditScAppFilePath();
    afx_msg void OnChangeEditWifiFilePath();
    afx_msg void OnChangeEditLogoFilePath();
    afx_msg void OnChangeEditPubKeyL1Path();
    afx_msg void OnChangeEditPubKeyL2Path();
    afx_msg void OnChangeEditPubKeyL3Path();
    afx_msg void OnDblclkTreeFileList(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGEAPPLICATION_H__8AA0B35A_8608_4C52_B9C0_F2E7486A9AC8__INCLUDED_)
