/****************************************************************************** 
Copyright (C) 2006~2007 ViewAt Technology Co., LTD.
All rights reserved.

System Name    :  LoadTools
Module Name    :  ViewAt file check
File   Name    :  ViewAtFile.c
Remark         :  本模块的主要作用是检查视融达文件的合法性，这些文件包括应用
                  程序文件、VOS文件、boot文件、字库文件、多任务打包文件和参数
                  文件
Revision       :  V1.00
Date           :  2007/09/13
Dir            :
error code     :
******************************************************************************/

#include "stdafx.h"
#include <windows.h>

#include <stdio.h>
#include <string.h>
#include "ViewAtFile.h"


/****************************************************************************
  函数名     :  int CheckApplication(char *strFileName)
  描述       :  检查应用程序的合法性
  输入参数   :  1、char *strFileName：应用程序名
  输出参数   :  无
  返回值     :  >0：表示应用程序文件的大小
                -1：该文件不是应用程序
                -2：无法打开应用程序文件
                -3：读文件错误
                -4：应用程序主函数入口地址错误
                -5：应用程序入口函数地址错误
                -6：应用程序信息结构入口地址错误
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-16   V1.0        创建
  1、 黄俊斌     2007-11-02   V1.1        新增应用程序三类入口地址合法性判断
****************************************************************************/
int CheckApplication(char *strFileName)
{
    FILE *fp;
    BYTE abyTemp[43];
    int iLen, iRet;
    DWORD dwOffset, dwTemp, dwLen, dwTemp1, dwTemp2;

    fp = fopen(strFileName, "rb");
    if (NULL == fp)
    {
        return -2;
    }
    fseek(fp, 0, SEEK_END);
    iLen = ftell(fp);
    fseek(fp, -42, SEEK_END);
    memset(abyTemp, 0, sizeof(abyTemp));
    iRet = fread(abyTemp, 1, 42, fp);
    if (42 != iRet)
    {
        fclose(fp);
        return -3;
    }
    if (   (0==memcmp(abyTemp,"VPOS306-MAPP",12))
        || (0==memcmp(abyTemp,"VPOS306-SAPP",12))  )
    {
        if (0 == memcmp(abyTemp,"VPOS306-MAPP",12))
        {
            dwOffset = 0x20c00000;
        }
        else
        {
            dwOffset = 0x20800000;
        }
        dwLen = iLen;
        fseek(fp, 0, SEEK_SET);
        iRet = fread(abyTemp, 1, 8, fp);
        if (8 != iRet)
        {
            fclose(fp);
            return -3;
        }
		dwTemp1 = (abyTemp[0x01]*256 + abyTemp[0x00])*65536;
        dwTemp2 = abyTemp[0x05]*256 + abyTemp[0x04];
		if(0 != (dwTemp2&0x8000))
        {
            dwTemp = dwTemp1 - (0x10000-dwTemp2);
        }
		else
        {
            dwTemp = dwTemp1 + dwTemp2;
        }
        dwTemp -= dwOffset;
        if (dwTemp > dwLen)
        {
            fclose(fp);
            return -4;
        }
        fseek(fp, 16, SEEK_SET);
        iRet = fread(abyTemp, 1, 8, fp);
        if (8 != iRet)
        {
            fclose(fp);
            return -3;
        }
		dwTemp1 = (abyTemp[0x01]*256 + abyTemp[0x00])*65536;
        dwTemp2 = abyTemp[0x05]*256 + abyTemp[0x04];
        if(0 != (dwTemp2&0x8000))
        {
            dwTemp = dwTemp1 - (0x10000-dwTemp2);
        }
		else
        {
            dwTemp = dwTemp1 + dwTemp2;
        }
        dwTemp -= dwOffset;
        if (dwTemp > dwLen)
        {
            fclose(fp);
            return -5;
        }
        fseek(fp, 32, SEEK_SET);
        iRet = fread(abyTemp, 1, 8, fp);
        fclose(fp);
        if (8 != iRet)
        {
            return -3;
        }
		dwTemp1 = (abyTemp[0x01]*256 + abyTemp[0x00])*65536;
        dwTemp2 = abyTemp[0x05]*256 + abyTemp[0x04];
		if(0 != (dwTemp2&0x8000))
        {
            dwTemp = dwTemp1 - (0x10000-dwTemp2);
        }
		else
        {
            dwTemp = dwTemp1 + dwTemp2;
        }
        dwTemp -= dwOffset;
        if (dwTemp > dwLen)
        {
            return -6;
        }
        return 0;
    }
    if (   (0!=memcmp(&abyTemp[30],"VPP202--MAPP",12))
        && (0!=memcmp(&abyTemp[30],"VPP202--SAPP",12))
        && (0!=memcmp(&abyTemp[30],"VPP210--MAPP",12))
        && (0!=memcmp(&abyTemp[30],"VPP210--SAPP",12))
        && (0!=memcmp(&abyTemp[30],"VPOS305-MAPP",12))
        && (0!=memcmp(&abyTemp[30],"VPOS305-SAPP",12)) 
        && (0!=memcmp(&abyTemp[30],"VPOS302-MAPP",12))
        && (0!=memcmp(&abyTemp[30],"VPOS302-SAPP",12))  )
    {
        fclose(fp);
        return -1;
    }
    fseek(fp, 0, SEEK_SET);
    iRet = fread(abyTemp, 1, 12, fp);
    fclose(fp);
    if (12 != iRet)
    {
        return -3;
    }
    iRet = (abyTemp[2]*65536+abyTemp[1]*256+abyTemp[0])*4+12;
    if (iRet > iLen)
    {
        return -4;
    }
    iRet = (abyTemp[6]*65536+abyTemp[5]*256+abyTemp[4])*4+16;
    if (iRet > iLen)
    {
        return -5;
    }
    iRet = (abyTemp[10]*65536+abyTemp[9]*256+abyTemp[8])*4+276;
    if (iRet > iLen)
    {
        return -6;
    }

    return iLen;
}

/****************************************************************************
  函数名     :  int CheckViewAtFont(char *strFileName)
  描述       :  字库文件检查函数
  输入参数   :  1、char *strFileName：字库文件名
  输出参数   :  无
  返回值     :  >0：表示字库文件的大小；
                -1：该文件不是ViewAt字库文件；
                -2：无法打开该字库文件。
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-16   V1.0        创建
****************************************************************************/
int CheckViewAtFont(char *strFileName)
{
    FILE *fp;
    char strTemp[17];
    int iLen;
    fp = fopen(strFileName, "rb");
    if (NULL == fp)
    {
        return -2;
    }
    fseek(fp, 0, SEEK_SET);
    memset(strTemp, 0, sizeof(strTemp));
    fread(strTemp, 1, 16, fp);
    if (   (0!=strcmp(strTemp,"ViewAt Font"))
        && (0!=strcmp(strTemp,"Compress VAFont")))
    {
        fclose(fp);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    iLen = ftell(fp);
    fclose(fp);
    return iLen;
}

/****************************************************************************
  函数名     :  int CheckVOS(char *strFileName)
  描述       :  检查VOS合法性
  输入参数   :  1、char *strFileName：VOS文件名
  输出参数   :  无
  返回值     :  -1：该文件不是VOS文件；
                -2：无法打开该文件；
                >0：表示VOS文件的大小。
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-16   V1.0        创建
  1、 黄俊斌     2008-06-16   V1.1        增加新的VOS类型识别
****************************************************************************/
int CheckVOS(char *strFileName)
{
    FILE *fp;
    char strTemp[11];
    int iLen;

    fp = fopen(strFileName, "rb");
    if (NULL == fp)
    {
        return -2;
    }
    fseek(fp, -10, SEEK_END);
    memset(strTemp, 0, sizeof(strTemp));
    fread(strTemp, 1, 10, fp);
    if (   (0 != strcmp(strTemp, "VPP202-VOS"))
        && (0 != strcmp(strTemp, "VPOS305VOS"))
        && (0 != strcmp(strTemp, "VPOS302VOS"))
        && (0 != strcmp(strTemp, "VPP210-VOS")))
    {
        fclose(fp);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    iLen = ftell(fp);
    fclose(fp);
    return iLen;
}

/****************************************************************************
  函数名     :  int CheckParam(char *strFileName)
  描述       :  检查参数文件的合法性
  输入参数   :  1、char *strFileName：参数文件名
  输出参数   :  无
  返回值     :  -1：该文件不是参数文件；
                -2：无法打开该文件；
                >0：表示参数文件的大小。
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-10-16   V1.0        创建
****************************************************************************/
int CheckParam(char *strFileName)
{
    FILE *fp;
    int iLen = 0;

    fp = fopen(strFileName, "rb");
    if (NULL == fp)
    {
        return -2;
    }
    fseek(fp, 0, SEEK_END);
    iLen = ftell(fp);
    fclose(fp);
    if ((iLen<=0) || (0 != (iLen%128)))
    {
        return -1;
    }
    return iLen;
}

/****************************************************************************
  函数名     :  int CheckDataFile(char *strFileName)
  描述       :  检查数据文件的合法性
  输入参数   :  1、char *strFileName：参数文件名
  输出参数   :  无
  返回值     :  -1：该文件不是数据文件；
                -2：无法打开该文件；
                >0：表示数据文件的大小。
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2008-05-20   V1.0        创建
****************************************************************************/
int CheckDataFile(char *strFileName)
{
    FILE *fp;
    int iLen = 0;

    fp = fopen(strFileName, "rb");
    if (NULL == fp)
    {
        return -2;
    }
    fseek(fp, 0, SEEK_END);
    iLen = ftell(fp);
    fclose(fp);
    if (iLen <= 0)
    {
        return -1;
    }
    return iLen;
}

/****************************************************************************
  函数名     :  
  描述       :  
  输入参数   :  
  输出参数   :  
  返回值     :  
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-13   V1.0        创建
****************************************************************************/
int CheckBoot(char *strFileName)
{
    return 0;
}

/****************************************************************************
  函数名     :  int CheckBale(char *strFileName)
  描述       :  打包文件合法性检查函数
  输入参数   :  1、char *strFileName：打包文件名
  输出参数   :  无
  返回值     :  -1：该文件不是VOS文件；
                -2：无法打开该文件；
                >0：表示打包文件的大小。
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2007-09-13   V1.0        创建
****************************************************************************/
int CheckBale(char *strFileName)
{
    FILE *fp;
    char strTemp[17];
    int iLen;

    fp = fopen(strFileName, "rb");
    if (NULL == fp)
    {
        return -2;
    }
    memset(strTemp, 0, sizeof(strTemp));
    fread(strTemp, 1, 16, fp);
    fseek(fp, 0, SEEK_END);
    iLen = ftell(fp);
    fclose(fp);
    if (0 != memcmp(strTemp,"ViewAt BaleFile",16))
    {
        return -1;
    }
    return iLen;
}









