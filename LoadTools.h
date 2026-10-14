// LoadTools.h : main header file for the LOADTOOLS application
//

#if !defined(AFX_LOADTOOLS_H__707E5D5F_7BF5_4932_81C5_DB0837B51DEC__INCLUDED_)
#define AFX_LOADTOOLS_H__707E5D5F_7BF5_4932_81C5_DB0837B51DEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"        // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLoadToolsApp:
// See LoadTools.cpp for the implementation of this class
//

class CLoadToolsApp : public CWinApp
{
public:
    CLoadToolsApp();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CLoadToolsApp)
    public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

// Implementation

    //{{AFX_MSG(CLoadToolsApp)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADTOOLS_H__707E5D5F_7BF5_4932_81C5_DB0837B51DEC__INCLUDED_)
