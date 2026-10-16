/****************************************************************************** 
Copyright (C) 2006~2007 ViewAt Technology Co., LTD.
All rights reserved.

System Name    :  LoadTools
Module Name    :  ViewAt file check
File   Name    :  ViewAtFile.c
Remark         :  ��ģ�����Ҫ�����Ǽ�����ڴ��ļ��ĺϷ��ԣ���Щ�ļ�����Ӧ��
                  �����ļ���VOS�ļ���boot�ļ����ֿ��ļ������������ļ��Ͳ���
                  �ļ�
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
#include "EnvJson.h"


/****************************************************************************
  ������     :  int CheckApplication(char *strFileName)
  ����       :  ���Ӧ�ó���ĺϷ���
  �������   :  1��char *strFileName��Ӧ�ó�����
  �������   :  ��
  ����ֵ     :  >0����ʾӦ�ó����ļ��Ĵ�С
                -1�����ļ�����Ӧ�ó���
                -2���޷���Ӧ�ó����ļ�
                -3�����ļ�����
                -4��Ӧ�ó�����������ڵ�ַ����
                -5��Ӧ�ó�����ں�����ַ����
                -6��Ӧ�ó�����Ϣ�ṹ��ڵ�ַ����
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2007-10-16   V1.0        ����
  1�� �ƿ���     2007-11-02   V1.1        ����Ӧ�ó���������ڵ�ַ�Ϸ����ж�
****************************************************************************/
int CheckApplication(char *strFileName)
{
    FILE *fp;
    int iLen;

    /* Task bale App: size/open only; no tail-magic / entry-address. */
    fp = fopen(strFileName, "rb");
    if (NULL == fp)
    {
        return -2;
    }
    fseek(fp, 0, SEEK_END);
    iLen = ftell(fp);
    fclose(fp);
    if (iLen < (10*1024))
    {
        return -1;
    }
    if (iLen > (1024*1024))
    {
        return -1;
    }
    return iLen;
}


/****************************************************************************
  ������     :  int CheckViewAtFont(char *strFileName)
  ����       :  �ֿ��ļ���麯��
  �������   :  1��char *strFileName���ֿ��ļ���
  �������   :  ��
  ����ֵ     :  >0����ʾ�ֿ��ļ��Ĵ�С��
                -1�����ļ�����ViewAt�ֿ��ļ���
                -2���޷��򿪸��ֿ��ļ���
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2007-10-16   V1.0        ����
****************************************************************************/
int CheckViewAtFont(char *strFileName)
{
    FILE *fp;
    int iLen;

    /* Font: size 4KB~960KB only; no ViewAt Font header check. */
    fp = fopen(strFileName, "rb");
    if (NULL == fp)
    {
        return -2;
    }
    fseek(fp, 0, SEEK_END);
    iLen = ftell(fp);
    fclose(fp);
    if (iLen < (4*1024))
    {
        return -1;
    }
    if (iLen > (960*1024))
    {
        return -1;
    }
    return iLen;
}


/****************************************************************************
  ������     :  int CheckVOS(char *strFileName)
  ����       :  ���VOS�Ϸ���
  �������   :  1��char *strFileName��VOS�ļ���
  �������   :  ��
  ����ֵ     :  -1�����ļ�����VOS�ļ���
                -2���޷��򿪸��ļ���
                >0����ʾVOS�ļ��Ĵ�С��
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2007-10-16   V1.0        ����
  1�� �ƿ���     2008-06-16   V1.1        �����µ�VOS����ʶ��
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
  ������     :  int CheckParam(char *strFileName)
  ����       :  �������ļ��ĺϷ���
  �������   :  1��char *strFileName�������ļ���
  �������   :  ��
  ����ֵ     :  -1�����ļ����ǲ����ļ���
                -2���޷��򿪸��ļ���
                >0����ʾ�����ļ��Ĵ�С��
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2007-10-16   V1.0        ����
****************************************************************************/
int CheckParam(char *strFileName)
{
    /* Validate Miracle JSON .env (see Docs/EnvFormat.md). */
    return EnvJsonCheckFile(strFileName);
}

/****************************************************************************
  ������     :  int CheckDataFile(char *strFileName)
  ����       :  ��������ļ��ĺϷ���
  �������   :  1��char *strFileName�������ļ���
  �������   :  ��
  ����ֵ     :  -1�����ļ����������ļ���
                -2���޷��򿪸��ļ���
                >0����ʾ�����ļ��Ĵ�С��
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2008-05-20   V1.0        ����
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
  ������     :  
  ����       :  
  �������   :  
  �������   :  
  ����ֵ     :  
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2007-09-13   V1.0        ����
****************************************************************************/
int CheckBoot(char *strFileName)
{
    return 0;
}

/****************************************************************************
  ������     :  int CheckBale(char *strFileName)
  ����       :  ����ļ��Ϸ��Լ�麯��
  �������   :  1��char *strFileName������ļ���
  �������   :  ��
  ����ֵ     :  -1�����ļ�����VOS�ļ���
                -2���޷��򿪸��ļ���
                >0����ʾ����ļ��Ĵ�С��
  �޸���ʷ   :
      �޸���     �޸�ʱ��    �޸İ汾��   �޸�ԭ��
  1�� �ƿ���     2007-09-13   V1.0        ����
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









