# Microsoft Developer Studio Project File - Name="LoadTools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LoadTools - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LoadTools.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LoadTools.mak" CFG="LoadTools - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LoadTools - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LoadTools - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LoadTools - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 usbd.lib setupapi.lib /nologo /subsystem:windows /machine:I386 /out:"Release/LoadTools3.1.exe"
# SUBTRACT LINK32 /incremental:yes /debug

!ELSEIF  "$(CFG)" == "LoadTools - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 usbd.lib setupapi.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "LoadTools - Win32 Release"
# Name "LoadTools - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Base.cpp
# End Source File
# Begin Source File

SOURCE=.\Communication.cpp
# End Source File
# Begin Source File

SOURCE=.\DelAppDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FlatButton.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadTools.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadTools.rc
# End Source File
# Begin Source File

SOURCE=.\LoadToolsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PageApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\PageCommSet.cpp
# End Source File
# Begin Source File

SOURCE=.\PageEditParam.cpp
# End Source File
# Begin Source File

SOURCE=.\PageFileBale.cpp
# End Source File
# Begin Source File

SOURCE=.\PageFirmware.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowAppInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ViewAtFile.cpp
# End Source File
# Begin Source File

SOURCE=.\VPOSSelect.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Base.h
# End Source File
# Begin Source File

SOURCE=.\BaseMacro.h
# End Source File
# Begin Source File

SOURCE=.\Communication.h
# End Source File
# Begin Source File

SOURCE=.\DelAppDlg.h
# End Source File
# Begin Source File

SOURCE=.\FlatButton.h
# End Source File
# Begin Source File

SOURCE=.\LoadTools.h
# End Source File
# Begin Source File

SOURCE=.\LoadToolsDefine.h
# End Source File
# Begin Source File

SOURCE=.\LoadToolsDlg.h
# End Source File
# Begin Source File

SOURCE=.\PageApplication.h
# End Source File
# Begin Source File

SOURCE=.\PageCommSet.h
# End Source File
# Begin Source File

SOURCE=.\PageEditParam.h
# End Source File
# Begin Source File

SOURCE=.\PageFileBale.h
# End Source File
# Begin Source File

SOURCE=.\PageFirmware.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ShowAppInfo.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ViewAtFile.h
# End Source File
# Begin Source File

SOURCE=.\VPOSSelect.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00008.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico_open1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\LoadTools.ico
# End Source File
# Begin Source File

SOURCE=.\res\LoadTools.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Run.ico
# End Source File
# Begin Source File

SOURCE=.\res\Search1.ico
# End Source File
# Begin Source File

SOURCE=.\res\UP32x32x256.ico
# End Source File
# Begin Source File

SOURCE=".\res\¸´¼þ Search1.ico"
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
