/********************************************************************************* 
Copyright (C) 2006~2007 ViewAt Technology Co., LTD.  
All rights reserved.                       
                                                                                 
System Name    :  LoadTools                                            
Module Name    :  Base operate module                            
File   Name    :  Base.c                                                  
Revision       :  V1.00                                                     
Date           :  2007/08/30           
Dir            :  
error code     :                                                                                              
***********************************************************************************/

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Base.h"
#include "BaseMacro.h"

const char *g_strConfigFileName = "config.hjb"; // ??????????????

char g_strErrorInfo[ERROR_INFO_MAX_LENGTH];  // ?????????
char g_strCurrentDir[300];
char g_strCurrentPath[300];
char g_cFlag = 0;

int g_iLangeType = 1;    // 0:Chinese, 1:English
CONFIG_INFO g_sConfigreInfo;




void DwordToByte(DWORD dwIn, BYTE *pbyOut)
{
    pbyOut[0] = (BYTE)((dwIn/65536)/256);
    pbyOut[1] = (BYTE)((dwIn/65536)%256);
    pbyOut[2] = (BYTE)((dwIn%65536)/256);
    pbyOut[3] = (BYTE)(dwIn%256);
}

void InitConfig(CONFIG_INFO *psConfig)
{
    memset(psConfig, 0, sizeof(CONFIG_INFO));
    psConfig->m_byPosType = 0xff;  // NO select POS type
    psConfig->m_byPrinterType = 0xff;  // NO select Priniter type
    memcpy(psConfig->m_abySeed, "\x03\xf2\xb1\x4c\x33\x1c\x3e\xd7", 8);
    memcpy(psConfig->m_abyVerFlag, "\x00\x01", 2);  // ?·Ú???0.1
}


// Clear path fields that point to missing files (stale history from old machines).
static void ClearPathIfMissing(char *pszPath, int iSize)
{
    FILE *fpCheck;

    if (NULL == pszPath || 0 == pszPath[0])
    {
        return;
    }
    fpCheck = fopen(pszPath, "rb");
    if (NULL == fpCheck)
    {
        memset(pszPath, 0, iSize);
        return;
    }
    fclose(fpCheck);
}

static void PruneMissingConfigPaths(void)
{
    char szKeep[256];
    BYTE bySelect;

    /* Keep only the current App path (single last path; drop history). */
    memset(szKeep, 0, sizeof(szKeep));
    if (g_sConfigreInfo.m_byAppFilePathNumber > MAX_APP_PATH_NUMBER)
    {
        g_sConfigreInfo.m_byAppFilePathNumber = 0;
    }
    bySelect = g_sConfigreInfo.m_byAppPathCurrentSelect;
    if (0 != g_sConfigreInfo.m_byAppFilePathNumber
        && bySelect < g_sConfigreInfo.m_byAppFilePathNumber
        && 0 != g_sConfigreInfo.m_strAppFilePath[bySelect][0])
    {
        memcpy(szKeep, g_sConfigreInfo.m_strAppFilePath[bySelect], 256);
    }
    memset(g_sConfigreInfo.m_strAppFilePath, 0, sizeof(g_sConfigreInfo.m_strAppFilePath));
    if (0 != szKeep[0])
    {
        memcpy(g_sConfigreInfo.m_strAppFilePath[0], szKeep, 256);
        ClearPathIfMissing(g_sConfigreInfo.m_strAppFilePath[0], 256);
    }
    if (0 != g_sConfigreInfo.m_strAppFilePath[0][0])
    {
        g_sConfigreInfo.m_byAppFilePathNumber = 1;
    }
    else
    {
        g_sConfigreInfo.m_byAppFilePathNumber = 0;
    }
    g_sConfigreInfo.m_byAppPathCurrentSelect = 0;

    ClearPathIfMissing(g_sConfigreInfo.m_strBootFilePath, sizeof(g_sConfigreInfo.m_strBootFilePath));
    ClearPathIfMissing(g_sConfigreInfo.m_strVOSFilePath, sizeof(g_sConfigreInfo.m_strVOSFilePath));
    ClearPathIfMissing(g_sConfigreInfo.m_strFontFilePath, sizeof(g_sConfigreInfo.m_strFontFilePath));
    ClearPathIfMissing(g_sConfigreInfo.m_strBaleFilePath, sizeof(g_sConfigreInfo.m_strBaleFilePath));
    ClearPathIfMissing(g_sConfigreInfo.m_strParamFilePath, sizeof(g_sConfigreInfo.m_strParamFilePath));
    ClearPathIfMissing(g_sConfigreInfo.m_strDataFilePath, sizeof(g_sConfigreInfo.m_strDataFilePath));
    ClearPathIfMissing(g_sConfigreInfo.m_strParamEditFilepath, sizeof(g_sConfigreInfo.m_strParamEditFilepath));
    ClearPathIfMissing(g_sConfigreInfo.m_strScAppFilePath, sizeof(g_sConfigreInfo.m_strScAppFilePath));
    ClearPathIfMissing(g_sConfigreInfo.m_strWifiFilePath, sizeof(g_sConfigreInfo.m_strWifiFilePath));
    ClearPathIfMissing(g_sConfigreInfo.m_strLogoFilePath, sizeof(g_sConfigreInfo.m_strLogoFilePath));
    if (g_sConfigreInfo.m_byScAppSelect > 1) g_sConfigreInfo.m_byScAppSelect = 0;
    if (g_sConfigreInfo.m_byWifiSelect > 1) g_sConfigreInfo.m_byWifiSelect = 0;
    if (g_sConfigreInfo.m_byLogoSelect > 1) g_sConfigreInfo.m_byLogoSelect = 0;
}

// ret=-1:error ret=0:success
int GetConfigureInfo(void)
{
    FILE *fp;
    int len;

    memset(g_strErrorInfo, 0, ERROR_INFO_MAX_LENGTH);

    if (0 == g_cFlag)
    {
        GetCurrentDirectory(300, g_strCurrentDir);
        g_cFlag = 1;
        strcpy(g_strCurrentPath, g_strCurrentDir);
        strcat(g_strCurrentPath, "\\");
        strcat(g_strCurrentPath, g_strConfigFileName);
    }

    len = sizeof(CONFIG_INFO);

    // read configure file
    fp = fopen(g_strCurrentPath, "r");
    if (NULL == fp)
    {
        fp = fopen(g_strCurrentPath, "w+");
        if (NULL == fp)
        {
            strcpy(g_strErrorInfo, "Create configure file failed!");
            return -1;
        }
        InitConfig(&g_sConfigreInfo);
        fseek(fp, 0, SEEK_SET);
        fwrite(&g_sConfigreInfo, sizeof(char), len, fp);
    }
    else
    {
        fseek(fp, 0, SEEK_SET);
        memset(&g_sConfigreInfo, 0, sizeof(CONFIG_INFO));
        fread(&g_sConfigreInfo, sizeof(char), len, fp);
        if (0 != memcmp(g_sConfigreInfo.m_abyVerFlag, "\x00\x01", 2))
        {
            InitConfig(&g_sConfigreInfo);
        }
        else
        {
            PruneMissingConfigPaths();
        }
    }
    fclose(fp);

    // Persist pruned paths so stale history does not come back next launch.
    (void)SaveConfigInfo();
    return 0;
}

// ret=-1:error ret=0:success
int SaveConfigInfo(void)
{
    FILE *fp;
    int iRet;
    memset(g_strErrorInfo, 0, ERROR_INFO_MAX_LENGTH);

    fp = fopen(g_strCurrentPath, "w+");
    if (NULL == fp)
    {
        strcpy(g_strErrorInfo, "Create configure file failed!");
        return -1;
    }
    iRet = fseek(fp, 0, SEEK_SET);
    iRet = fwrite(&g_sConfigreInfo, 1, sizeof(CONFIG_INFO), fp);
    iRet = fclose(fp);
    return 0;
}

/****************************************************************************
  ??????     :  void CPageApplication::GetRandom(BYTE *pbyRandomOut)
  ????       :  ????????????
  ???????   :  ??
  ???????   :  1??BYTE *pbyRandomOut??????????????8?????
  ?????     :  ??
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1?? ?????     2007-10-08   V1.0        ????
****************************************************************************/
void GetRandom(BYTE *pbyRandomOut)
{
    SYSTEMTIME sTime;
    BYTE abyTemp[8];
    
    GetLocalTime(&sTime);
    abyTemp[0] = (BYTE)sTime.wYear;
    abyTemp[1] = (BYTE)sTime.wMonth;
    abyTemp[2] = (BYTE)sTime.wDay;
    abyTemp[3] = (BYTE)sTime.wHour;
    abyTemp[4] = (BYTE)sTime.wMinute;
    abyTemp[5] = (BYTE)sTime.wSecond;
    abyTemp[6] = (BYTE)(sTime.wMilliseconds/256);
    abyTemp[7] = (BYTE)(sTime.wMilliseconds%256);
    des(abyTemp, g_sConfigreInfo.m_abySeed, pbyRandomOut, ENCRYPT);
    memcpy(g_sConfigreInfo.m_abySeed, pbyRandomOut, 8);
    (void)SaveConfigInfo();
}



static BYTE DataBuf[5200];            
static DWORD CharSym[420], SymChar[420];
static DWORD SymFreq[420], SymCum[420], PosCum[5100];   
static DWORD MatchPos, MatchLen, LNode[5100], RNode[5360], FNode[5100]; 

static void AddNode(DWORD n)
{
    DWORD i, j, temp;
    BYTE *key;
    long cmp;
    
    cmp = 1;  
    key = &DataBuf[n];  
    j = 4097 + key[0];
    RNode[n] = LNode[n] = 4096;  
    MatchLen = 0;
    while(1) {
        if (cmp >= 0) {
            if (RNode[j] != 4096) j = RNode[j];
            else {  
                RNode[j] = n;  
                FNode[n] = j;  
                return;  
            }
        } 
        else {
            if (LNode[j] != 4096) j = LNode[j];
            else {  
                LNode[j] = n;  
                FNode[n] = j;  
                return;  
            }
        }
        for (i = 1; i < 60; i++)
            if ((cmp = key[i] - DataBuf[j + i]) != 0)  break;
            if (i > 2) {
                if (i > MatchLen) {
                    MatchPos = (n - j) & 4095;
                    if ((MatchLen = i) >= 60) break;
                } 
                else if (i == MatchLen) {
                    if ((temp = (n - j) & 4095) < MatchPos) MatchPos = temp;
                }
            }
    }
    FNode[n] = FNode[j];  
    LNode[n] = LNode[j];  
    RNode[n] = RNode[j];
    FNode[LNode[j]] = n;  
    FNode[RNode[j]] = n;
    if (RNode[FNode[j]] == j) RNode[FNode[j]] = n;
    else LNode[FNode[j]] = n;
    FNode[j] = 4096;  
}

static void DelNode(DWORD n)  
{
    DWORD  i;
    
    if (FNode[n] == 4096) return;  
    if (RNode[n] == 4096) i = LNode[n];
    else if (LNode[n] == 4096) i = RNode[n];
    else {
        i = LNode[n];
        if (RNode[i] != 4096) {
            do {  
                i = RNode[i];  
            } while (RNode[i] != 4096);
            RNode[FNode[i]] = LNode[i];  
            FNode[LNode[i]] = FNode[i];
            LNode[i] = LNode[n];  
            FNode[LNode[n]] = i;
        }
        RNode[i] = RNode[n];  
        FNode[RNode[n]] = i;
    }
    FNode[i] = FNode[n];
    if (RNode[FNode[n]] == n) RNode[FNode[n]] = i;
    else LNode[FNode[n]] = i;
    FNode[n] = 4096;
}

static void UpdateNode(DWORD sym)
{
    DWORD i, j, k, ch;
    
    if (SymCum[0] >= 0x7FFF) {
        j = 0;
        for (i = 314; i > 0; i--) {
            SymCum[i] = j;
            j += (SymFreq[i] = (SymFreq[i] + 1) >> 1);
        }
        SymCum[0] = j;
    }
    for (i = sym; SymFreq[i] == SymFreq[i - 1]; i--) ;
    if (i < sym) {
        k = SymChar[i];   
        ch = SymChar[sym];
        SymChar[i] = ch;  
        SymChar[sym] = k;
        CharSym[k] = sym; 
        CharSym[ch] = i;
    }
    SymFreq[i]++;
    while (--i > 0) SymCum[i]++;
    SymCum[0]++;
}


// ????????????????VC?¡À???Release?·Ú???????????default??????
// ??????? Maximize Speed ???? Minimize Size?????????????????????????
int Compress(BYTE *srcbuf, BYTE *dstbuf, DWORD srclen, DWORD *dstlen)
{
    DWORD i, j, k, s, len, LMatchLen;
    DWORD low,high,written,shifts,sym, range;  
    BYTE ch,mask,*srcend,*sp, *dp;
    
    low = 0;
    shifts = 0;  
    written = 4;
    ch = 0;
    mask = 128;
    high = 0x20000;
    srcend = srcbuf + srclen;
    sp = srcbuf;
    dp = dstbuf;
    
    *dp++ = (BYTE)(srclen>>24);
    *dp++ = (BYTE)(srclen>>16);
    *dp++ = (BYTE)(srclen>>8);
    *dp++ = (BYTE)(srclen);
    
    SymCum[314] = 0;
    for (k = 314; k >= 1; k--) {
        j = k - 1;
        CharSym[j] = k;  
        SymChar[k] = j;
        SymFreq[k] = 1;
        SymCum[k - 1] = SymCum[k] + SymFreq[k];
    }
    SymFreq[0] = 0;  
    PosCum[4096] = 0;
    for (i = 4096; i >= 1; i--){
        PosCum[i - 1] = PosCum[i] + 10000 / (i + 200);
    }
    for (i = 4096 + 1; i <= 4096 + 256; i++) RNode[i] = 4096;    
    for (i = 0; i < 4096; i++) FNode[i] = 4096;    
    
    s = 0;  k = 4096 - 60;
    for (i = s; i < k; i++) DataBuf[i] = ' ';  
    for (len = 0; (len < 60) && (sp < srcend); len++){
        DataBuf[k + len] = *(sp++);  
    }
    for (i = 1; i <= 60; i++) AddNode(k - i);  
    AddNode(k); 
    do {
        if (MatchLen > len) MatchLen = len;  
        if (MatchLen <= 2) {
            MatchLen = 1; 
            sym = CharSym[DataBuf[k]];
            range = high - low;
            high = low + (range * SymCum[sym - 1]) / SymCum[0];
            low += (range * SymCum[sym]) / SymCum[0];
            while(1) {
                if (high <= 0x10000) {
                    if ((mask >>= 1) == 0) {
                        *(dp++) = ch;
                        ch = 0;
                        mask = 128;
                        written++;    
                    }
                    for ( ; shifts > 0; shifts--) {
                        ch |= mask;
                        if ((mask >>= 1) == 0) {
                            *(dp++) = ch;
                            ch = 0;
                            mask = 128;
                            written++;    
                        }
                    }
                } 
                else if (low >= 0x10000) {
                    ch |= mask;
                    if ((mask >>= 1) == 0) {
                        *(dp++) = ch;
                        ch = 0;
                        mask = 128;
                        written++;                        
                    }
                    for ( ; shifts > 0; shifts--) {
                        if ((mask >>= 1) == 0) {
                            *(dp++) = ch;
                            ch = 0;
                            mask = 128;
                            written++;                            
                        }
                    }
                    low -= 0x10000;
                    high -= 0x10000;
                } 
                else if (low >= 0x8000 && high <= 0x18000) {
                    shifts++;
                    low -= 0x8000;
                    high -= 0x8000;
                } 
                else break;
                low += low;  
                high += high;
            }
            UpdateNode(sym);
        } 
        else {
            sym = CharSym[255 - 2 + MatchLen];
            range = high - low;
            high = low + (range * SymCum[sym - 1]) / SymCum[0];
            low += (range * SymCum[sym]) / SymCum[0];
            while(1) {
                if (high <= 0x10000) {
                    if ((mask >>= 1) == 0) {
                        *(dp++) = ch;
                        ch = 0;
                        mask = 128;
                        written++;                    
                    }
                    for ( ; shifts > 0; shifts--) {
                        ch |= mask;
                        if ((mask >>= 1) == 0) {
                            *(dp++) = ch;
                            ch = 0;
                            mask = 128;
                            written++;                            
                        }
                    }
                } 
                else if (low >= 0x10000) {
                    ch |= mask;
                    if ((mask >>= 1) == 0) {
                        *(dp++) = ch;
                        ch = 0;
                        mask = 128;
                        written++;                        
                    }
                    for ( ; shifts > 0; shifts--) {
                        if ((mask >>= 1) == 0) {
                            *(dp++) = ch;
                            ch = 0;
                            mask = 128;
                            written++;                    
                        }
                    }
                    low -= 0x10000;
                    high -= 0x10000;
                } 
                else if (low >= 0x8000 && high <= 0x18000) {
                    shifts++;
                    low -= 0x8000;
                    high -= 0x8000;
                } 
                else break;
                low += low;  
                high += high;
            }
            UpdateNode(sym);
            range = high - low;
            high = low + (range * PosCum[MatchPos - 1]) / PosCum[0];
            low += (range * PosCum[MatchPos]) / PosCum[0];
            while(1)  {
                if (high <= 0x10000) {
                    if ((mask >>= 1) == 0) {
                        *(dp++) = ch;
                        ch = 0;
                        mask = 128;
                        written++;                    
                    }
                    for ( ; shifts > 0; shifts--) {
                        ch |= mask;
                        if ((mask >>= 1) == 0) {
                            *(dp++) = ch;
                            ch = 0;
                            mask = 128;
                            written++;                        
                        }
                    }
                } 
                else {
                    if (low >= 0x10000) {
                        ch |= mask;
                        if ((mask >>= 1) == 0) {
                            *(dp++) = ch;
                            ch = 0;
                            mask = 128;
                            written++;                        
                        }
                        for ( ; shifts > 0; shifts--) {
                            if ((mask >>= 1) == 0) {
                                *(dp++) = ch;
                                ch = 0;
                                mask = 128;
                                written++;                        
                            }
                        }
                        low -= 0x10000;
                        high -= 0x10000;
                    } 
                    else {
                        if ((low >= 0x8000) && (high <= 0x18000)) {
                            shifts++;
                            low -= 0x8000;
                            high -= 0x8000;
                        } 
                        else {
                            break;
                        }
                    }
                }
                low += low;
                high += high;
            }
        }
        LMatchLen = MatchLen;
        for (i = 0; (i < LMatchLen) && (sp < srcend); i++) {
            j = *(sp++);
            DelNode(s);
            DataBuf[s] = (BYTE)j;
            if (s < 60 - 1)
                DataBuf[s + 4096] = (BYTE)j;
            s = (s + 1) & 4095;
            k = (k + 1) & 4095;
            AddNode(k);
        }
        while (i++ < LMatchLen) {
            DelNode(s);
            s = (s + 1) & 4095;
            k = (k + 1) & 4095;
            if (--len) AddNode(k);
        }
    } while (len > 0);
    shifts++;
    if (low < 0x8000) {
        if ((mask >>= 1) == 0) {
            *(dp++) = ch;
            ch = 0;
            mask = 128;
            written++;
        }
        for ( ; shifts > 0; shifts--) {
            ch |= mask;
            if ((mask >>= 1) == 0) {
                *(dp++) = ch;
                ch = 0;
                mask = 128;
                written++;        
            }
        }
    } 
    else {
        ch |= mask;
        if ((mask >>= 1) == 0) {
            *(dp++) = ch;
            ch = 0;
            mask = 128;
            written++;        
        }
        for ( ; shifts > 0; shifts--) {
            if ((mask >>= 1) == 0) {
                *(dp++) = ch;
                ch = 0;
                mask = 128;
                written++;        
            }
        }
    }
    for (i = 0; i < 7; i++) {
        if ((mask >>= 1) == 0) {
            *(dp++) = ch;
            ch = 0;
            mask = 128;
            written++;
        }
    }
    *dstlen = written;
    return 0;
}



static DWORD GetSym(DWORD x)
{
    DWORD i, j, k;
    
    i = 1;  
    j = 314;
    while (i < j) 
    {
        k = (i + j) / 2;
        if (SymCum[k] > x)
            i = k + 1;  
        else
            j = k;
    }
    return i;
}

DWORD GetPos(DWORD x)
{
    DWORD i, j, k;
    
    i = 1;  
    j = 4096;
    while (i < j) 
    {
        k = (i + j) / 2;
        if (PosCum[k] > x)
            i = k + 1;  
        else
            j = k;
    }
    return (i - 1);
}

int Decompress(BYTE *srcbuf, BYTE *dstbuf, DWORD srclen, DWORD *dstlen)    
{
    BYTE ch, mask, *srcend, *dstend, *sp, *dp;
    DWORD i, r, j, k, c, low, high, value, range, sym;
    
    ch = 0;
    mask = 0;
    low = 0;
    value = 0;
    high = 0x20000;
    sp = srcbuf;
    dp = dstbuf;
    
    i = *sp++;
    i <<= 8;
    i += *sp++;
    i <<= 8;
    i += *sp++;
    i <<= 8;
    i += *sp++;
    *dstlen=i;
    
    srcend = srcbuf + srclen;
    dstend = dstbuf + i;
    for (i = 0; i < 15 + 2; i++) 
    {
        value *= 2;
        if ((mask >>= 1) == 0) 
        {
            ch = (sp >= srcend) ? 0 : *(sp++);
            mask = 128;
        }
        value += ((ch & mask) != 0);
    }
    
    SymCum[314] = 0;
    for (k = 314; k >= 1; k--) 
    {
        j = k - 1;
        CharSym[j] = k; 
        SymChar[k] = j;
        SymFreq[k] = 1;
        SymCum[k - 1] = SymCum[k] + SymFreq[k];
    }
    SymFreq[0] = 0;  
    PosCum[4096] = 0;
    for (i = 4096; i >= 1; i--)
        PosCum[i - 1] = PosCum[i] + 10000 / (i + 200);
    
    for (i = 0; i < 4096 - 60; i++)
        DataBuf[i] = ' ';
    r = 4096 - 60;
    while (dp < dstend) 
    {
        range = high - low;
        sym = GetSym((DWORD)(((value - low + 1) * SymCum[0] - 1) / range));
        high = low + (range * SymCum[sym - 1]) / SymCum[0];
        low += (range * SymCum[sym]) / SymCum[0];
        while(1)  
        {
            if (low >= 0x10000) 
            {
                value -= 0x10000;  
                low -= 0x10000;  
                high -= 0x10000;
            } 
            else if (low >= 0x8000 && high <= 0x18000) 
            {
                value -= 0x8000;  
                low -= 0x8000;  
                high -= 0x8000;
            } 
            else if (high > 0x10000)
                break;
            low += low;  
            high += high;
            value *= 2;
            if ((mask >>= 1) == 0) 
            {
                ch = (sp >= srcend) ? 0 : *(sp++);
                mask = 128;
            }
            value += ((ch & mask) != 0);
        }
        c = SymChar[sym];
        UpdateNode(sym);
        if (c < 256) 
        {
            if (dp >= dstend)
                return -1;
            *(dp++) = (BYTE)c;
            DataBuf[r++] = (BYTE)c;
            r &= 4095;
        } 
        else 
        {
            j = c - 255 + 2;
            range = high - low;
            i = GetPos((DWORD)
                (((value - low + 1) * PosCum[0] - 1) / range));
            high = low + (range * PosCum[i    ]) / PosCum[0];
            low += (range * PosCum[i + 1]) / PosCum[0];
            while(1)  
            {
                if (low >= 0x10000) 
                {
                    value -= 0x10000;  
                    low -= 0x10000;  
                    high -= 0x10000;
                } 
                else if (low >= 0x8000 && high <= 0x18000) 
                {
                    value -= 0x8000;  
                    low -= 0x8000; 
                    high -= 0x8000;
                } 
                else if (high > 0x10000)
                    break;
                low += low;  
                high += high;
                value *= 2;
                if ((mask >>= 1) == 0) 
                {
                    ch = (sp >= srcend) ? 0 : *(sp++);
                    mask = 128;
                }
                value += ((ch & mask) != 0);
            }
            i = (r - i - 1) & 4095;
            for (k = 0; k < j; k++) 
            {
                c = DataBuf[(i + k) & 4095];
                if (dp >= dstend)
                    return -1;
                *(dp++) = (BYTE)c;
                DataBuf[r++] = (BYTE)c;
                r &= 4095;
            }        
        }
    }
    return 0;
}







static  void Init_bit_tab (BYTE *dest,BYTE *source,BYTE n);
static  BYTE puissance(BYTE puissance);
static  void Bin_to_Hex(char *vect,char *source);
static  void Vect_Permutation(BYTE *vect,BYTE n_vect,
                                        BYTE *regle,BYTE n_regle);
static  void Xor(BYTE *vect1,BYTE *vect2,BYTE num_byte);
static  void S_Box_Calc(BYTE *vect);
static  void Init_4bit_tab(BYTE *dest,BYTE *source);
static  void Left_shifts(BYTE *vect,BYTE n);
static  void Calcul_sous_cles(BYTE *DesKey);

BYTE IP[64] =
{
    58,50,42,34,26,18,10,2,
        60,52,44,36,28,20,12,4,
        62,54,46,38,30,22,14,6,
        64,56,48,40,32,24,16,8,
        57,49,41,33,25,17,9,1,
        59,51,43,35,27,19,11,3,
        61,53,45,37,29,21,13,5,
        63,55,47,39,31,23,15,7
};


BYTE IPinv[64]=
{ 
    40,8,48,16,56,24,64,32,
        39,7,47,15,55,23,63,31,
        38,6,46,14,54,22,62,30,
        37,5,45,13,53,21,61,29,
        36,4,44,12,52,20,60,28,
        35,3,43,11,51,19,59,27,
        34,2,42,10,50,18,58,26,
        33,1,41,9,49,17,57,25
};


BYTE E[48]=
{        
    32,1,2,3,4,5,
        4,5,6,7,8,9,
        8,9,10,11,12,13,
        12,13,14,15,16,17,
        16,17,18,19,20,21,
        20,21,22,23,24,25,
        24,25,26,27,28,29,
        28,29,30,31,32,1
};


BYTE S1[64]=
{
    14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
        0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
        4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
        15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13
};

BYTE S2[64]=
{
    15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
        3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
        0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
        13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9
};

BYTE S3[64]=
{ 
    10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
        13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
        13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
        1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12
};

BYTE S4[64]=
{ 
    7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
        13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
        10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
        3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14
};

BYTE S5[64]=
{ 
        2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
        14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
        4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
        11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3
};

BYTE S6[64]=
{ 
        12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
        10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
        9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
        4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13
};

BYTE S7[64]=
{ 
    4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
    13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
    1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
    6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12
};

BYTE S8[64]=
{
    13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
    1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
    7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
    2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
};


BYTE P[32]=
{ 
    16,7,20,21,
    29,12,28,17,
    1,15,23,26,
    5,18,31,10,
    2,8,24,14,
    32,27,3,9,
    19,13,30,6,
    22,11,4,25
};


BYTE PC1[56] =
{        
    57,49,41,33,25,17,9,
    1,58,50,42,34,26,18,
    10,2,59,51,43,35,27,
    19,11,3,60,52,44,36,
    63,55,47,39,31,23,15,
    7,62,54,46,38,30,22,
    14,6,61,53,45,37,29,
    21,13,5,28,20,12,4
};


BYTE PC2[48] =
{        
    14,17,11,24,1,5,
    3,28,15,6,21,10,
    23,19,12,4,26,8,
    16,7,27,20,13,2,
    41,52,31,37,47,55,
    30,40,51,45,33,48,
    44,49,39,56,34,53,
    46,42,50,36,29,32
};


BYTE LS[17] = {0,1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};


BYTE Ki[17][49];


void Init_bit_tab(BYTE *dest,BYTE *source,BYTE n)
{
    BYTE masque,i,j;
    
    for(i=0;i<n;i++) {
        masque=0x80;
        for(j=0;j<8;j++) {
            dest[8*i+j] = (source[i] & masque) >> (7-j);
            masque >>= 1;
        }
    }
}



/****************************************************************************
 Bin_to_Hex()   :
    range la valeur hexa sur 8 octets d'un nombre binaire de 64 bits
*****************************************************************************/
void Bin_to_Hex(BYTE *vect,BYTE *source)
{
    BYTE i,j,masque;
    
    memset(vect,0,8);
    for(i=0; i <8; i++) {
        masque=7;
        for(j=0; j<8; j++) {
            vect[i] += (puissance(masque)) * source[i*8+j];
            --masque;
        }
    }
}

BYTE puissance(BYTE puissance)
{
    BYTE res = 1, i;
    
    for(i=1 ; i <= puissance; i++) 
        res *= 2;
    
    return(res);
}


void Vect_Permutation(BYTE *vect,BYTE n_vect,BYTE *regle,BYTE n_regle)
{
    BYTE buff[65],i;
    
    memset(buff,0,65);
    memcpy(buff,vect,n_vect);
    
    for(i=0;i<n_regle;i++) 
        vect[i] = buff[regle[i]-1];
    
}


void S_Box_Calc(BYTE *vect)
{
    BYTE *S_Box[8];
    unsigned lig,col,i;
    
    S_Box[0]=S1;
    S_Box[1]=S2;
    S_Box[2]=S3;
    S_Box[3]=S4;
    S_Box[4]=S5;
    S_Box[5]=S6;
    S_Box[6]=S7;
    S_Box[7]=S8;
    
    for(i=0;i<8;i++) {
        col= 8*vect[1+6*i] + 4*vect[2+6*i] + 2*vect[3+6*i] + vect[4+6*i];
        lig= 2*vect[6*i] + vect[5+6*i];
        Init_4bit_tab(&vect[4*i],&S_Box[i][col+lig*16]);
    }
}


void Init_4bit_tab(BYTE *dest,BYTE *source)
{
    BYTE masque,i;
    
    masque=0x08;
    for(i=0; i<4; i++) {
        dest[i] = (*source & masque)>>(3-i);
        masque >>= 1;
    }
}


void Xor(BYTE *vect1,BYTE * vect2,BYTE num_byte)
{
    
    BYTE i;
    
    for(i=0; i<num_byte; i++) 
        vect1[i] ^= vect2[i];
}


void Left_shifts(BYTE *vect,BYTE n)
{
    BYTE i,tmp_vect28,tmp_vect0;

    for(i=0; i<n; i++) {
        tmp_vect0 = vect[0];
        memcpy(vect,&vect[1],27);
        vect[27] = tmp_vect0;

        tmp_vect28 = vect[28];
        memcpy(&vect[28],&vect[29],27);
        vect[55] = tmp_vect28;
    }
}

void Calcul_sous_cles(BYTE *DesKey)
{
    BYTE i;
    BYTE Kb[65],inter_key[57];
    
    Init_bit_tab(&Kb[1],DesKey,8);                           
    Vect_Permutation(&Kb[1],64,PC1,56);
    
    for(i=1; i<=16; i++) {
        Left_shifts(&Kb[1],LS[i]);
        memcpy(&inter_key[1],&Kb[1],56);
        Vect_Permutation(&inter_key[1],56,PC2,48);
        memcpy(&Ki[i][1],&inter_key[1],48);
    }
}

void des(BYTE *DES_DATA, BYTE *DES_RESULT, BYTE *DesKey, char cryp_decrypt)
{
    BYTE right32_bit[32],i;
    BYTE Data_B[81];
    
    
    Init_bit_tab(&Data_B[1],DES_DATA,8);
    Vect_Permutation(&Data_B[1],64,IP,64);
    
    Calcul_sous_cles(DesKey);
    
    /******************* boucle principale de 15 iterations */
    for(i=1; i<=15; i++) {
        
        memcpy(right32_bit,&Data_B[33],32);
        Vect_Permutation(&Data_B[33],32,E,48);
        
        switch(cryp_decrypt) {
        case ENCRYPT:
            Xor(&Data_B[33],&Ki[i][1],48);
            break;
            
        case DECRYPT:
            Xor(&Data_B[33],&Ki[17-i][1],48);
            break;
        }
        
        S_Box_Calc(&Data_B[33]);
        Vect_Permutation(&Data_B[33],32,P,32);
        Xor(&Data_B[33],&Data_B[1],32);
        memcpy(&Data_B[1],right32_bit,32);
        
    }
    
    /******************************** 16 iteration *****/
    
    memcpy(right32_bit,&Data_B[33],32);
    Vect_Permutation(&Data_B[33],32,E,48);
    
    if(cryp_decrypt==ENCRYPT)
        Xor(&Data_B[33],&Ki[16][1],48);
    else
        Xor(&Data_B[33],&Ki[1][1],48);
    
    S_Box_Calc(&Data_B[33]);
    Vect_Permutation(&Data_B[33],32,P,32);
    Xor(&Data_B[1],&Data_B[33],32);
    memcpy(&Data_B[33],right32_bit,32);
    Vect_Permutation(&Data_B[1],64,IPinv,64);
    
    Bin_to_Hex(DES_RESULT,&Data_B[1]);
    
}




static int G_GetChnMessage(int iIndex, char *strOut)
{
    switch (iIndex)
    {
    case MSG_CREATE_THREAD_FAILED:
        strcpy(strOut, MSG_CHN_CREATE_THREAD_FAILED);
        return 0;
    case MSG_OPEN_SERIAL_PORT_FAILED:
        strcpy(strOut, MSG_CHN_OPEN_SERIAL_PORT_FAILED);
        return 0;
    case MSG_CONNECTING:
        strcpy(strOut, MSG_CHN_CONNECTING);
        return 0;
    case MSG_USER_STOP_DOWNLOAD:
        strcpy(strOut, MSG_CHN_USER_STOP_DOWNLOAD);
        return 0;
    case MSG_HANDSHACK_FAILED:
        strcpy(strOut, MSG_CHN_HANDSHACK_FAILED);
        return 0;
    case MSG_CHANGE_BAUD_FAILED:
        strcpy(strOut, MSG_CHN_CHANGE_BAUD_FAILED);
        return 0;
    case MSG_SEND_FAILED:
        strcpy(strOut, MSG_CHN_SEND_FAILED);
        return 0;
    case MSG_RECEIVE_FAILED:
        strcpy(strOut, MSG_CHN_RECEIVE_FAILED);
        return 0;
    case MSG_CHANGE_BAUD:
        strcpy(strOut, MSG_CHN_CHANGE_BAUD);
        return 0;
    case MSG_AUTHENTICATION:
        strcpy(strOut, MSG_CHN_AUTHENTICATION);
        return 0;
    case MSG_OPEN_USB_PORT_FAILED:
        strcpy(strOut, MSG_CHN_OPEN_USB_PORT_FAILED);
        return 0;
    default:
        return 1;
    }
}

static int G_GetEngMessage(int iIndex, char *strOut)
{
    switch (iIndex)
    {
    case MSG_CREATE_THREAD_FAILED:
        strcpy(strOut, MSG_ENG_CREATE_THREAD_FAILED);
        return 0;
    case MSG_OPEN_SERIAL_PORT_FAILED:
        strcpy(strOut, MSG_ENG_OPEN_SERIAL_PORT_FAILED);
        return 0;
    case MSG_CONNECTING:
        strcpy(strOut, MSG_ENG_CONNECTING);
        return 0;
    case MSG_USER_STOP_DOWNLOAD:
        strcpy(strOut, MSG_ENG_USER_STOP_DOWNLOAD);
        return 0;
    case MSG_HANDSHACK_FAILED:
        strcpy(strOut, MSG_ENG_HANDSHACK_FAILED);
        return 0;
    case MSG_CHANGE_BAUD_FAILED:
        strcpy(strOut, MSG_ENG_CHANGE_BAUD_FAILED);
        return 0;
    case MSG_SEND_FAILED:
        strcpy(strOut, MSG_ENG_SEND_FAILED);
        return 0;
    case MSG_RECEIVE_FAILED:
        strcpy(strOut, MSG_ENG_RECEIVE_FAILED);
        return 0;
    case MSG_CHANGE_BAUD:
        strcpy(strOut, MSG_ENG_CHANGE_BAUD);
        return 0;
    case MSG_AUTHENTICATION:
        strcpy(strOut, MSG_ENG_AUTHENTICATION);
        return 0;
    case MSG_OPEN_USB_PORT_FAILED:
        strcpy(strOut, MSG_ENG_OPEN_USB_PORT_FAILED);
        return 0;
    default:
        return 1;
    }
}


int G_MyGetMessage(int iIndex, char *strOut)
{
    switch (g_iLangeType)
    {
    case LANGE_CHINESE:
        return G_GetChnMessage(iIndex, strOut);
    case LANGE_ENGLISH:
        return G_GetEngMessage(iIndex, strOut);
    default:
        return 1;
    }
}

/*
    NN.C - natural numbers routines

    Copyright (c) J.S.A.Kapp 1994 - 1995.

    RSAEURO - RSA Library compatible with RSAREF(tm) 2.0.

    All functions prototypes are the Same as for RSAREF(tm).
    To aid compatiblity the source and the files follow the
    same naming comventions that RSAREF(tm) uses.  This should aid
    direct importing to you applications.

    This library is legal everywhere outside the US.  And should
    NOT be imported to the US and used there.

    All Trademarks Acknowledged.

    Revision hisitory
        0.90 First revision, this revision was the basic routines.
        Routines slower than final revision.

        0.91 Second revision, this is the current revision, all
        routines have been altered for speed increases.  Also the
        addition of assembler equivalents.
*/

//extern void dmult(DWORD a, DWORD b, DWORD *cHigh, DWORD *cLow);

DWORD subdigitmult(DWORD *a, DWORD *b, DWORD c, DWORD *d, DWORD digits);
DWORD NN_DigitBits(DWORD a);


/****************************************************************************
  ??????     :  void dmult(DWORD a, DWORD b, DWORD *cHigh, DWORD *cLow)
  ????       :  32¦Ë????????????¨²?????????windows??
  ???????   :  1??DWORD a : ????
                2??DWORD b : ????
  ???????   :  1??DWORD *cHigh : ?????32¦Ë
                2??DWORD *cLow : ?????32¦Ë
  ?????     :  ??
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1?? ?????     2007-01-05  v1.0         ????
****************************************************************************/
unsigned __int64 g_qwTemp;
#define dmult(a, b, cHigh, cLow) \
{\
    g_qwTemp = a;\
    g_qwTemp *= b;\
    *cHigh = (DWORD)(g_qwTemp>>32);\
    *cLow = (DWORD)(g_qwTemp);\
}


/* Assigns a = 0. */
/****************************************************************************
  ??????     :
  ????       :  ??????a??????????????0
  ???????   :  1??DWORD digits : ????a????????
  ???????   :  1??DWORD *a : ??????
  ?????     :  ??
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
void NN_AssignZero(DWORD *a, DWORD digits)
{
    if(digits)
    {
        do
        {
            *a++ = 0;
        }while(--digits);
    }
}

/* Computes a = b - c. Returns borrow.
     Lengths: a[digits], b[digits], c[digits].
 */
/****************************************************************************
  ??????     :  DWORD NN_Sub(DWORD *a, DWORD *b, DWORD *c, DWORD digits)
  ????       :  ???? a = b - c.
  ???????   :  1??DWORD *b : ???????ï…??¦Ë???????¦Ë???
                2??DWORD *c : ?????ï…??¦Ë???????¦Ë???
                3??DWORD digits : ????b??c????????
  ???????   :  1??DWORD *a : ???????????????b??c????¦Ë???????¦Ë???
  ?????     :  ?????¦Ë?????0?????§ß?¦Ë??1????§ß?¦Ë
      ? : ?§ß?¦Ë?????b§³????c
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
DWORD NN_Sub(DWORD *a, DWORD *b, DWORD *c, DWORD digits)
{
    DWORD temp, borrow = 0;

    if(digits)
    {
        do
        {
            if((temp = (*b++) - borrow) == MAX_NN_DIGIT)
            {
                temp = MAX_NN_DIGIT - *c++;
            }
            else
            {
                if((temp -= *c) > (MAX_NN_DIGIT - *c++))
                    borrow = 1;
                else
                    borrow = 0;
            }
            *a++ = temp;
        }while(--digits);
    }

    return(borrow);
}

/* Computes a = b * c.
     Lengths: a[2*digits], b[digits], c[digits].
     Assumes digits < MAX_NN_DIGITS.
*/
/****************************************************************************
  ??????     :  void NN_Mult(DWORD *a, DWORD *b, DWORD *c, DWORD digits)
  ????       :  ????a = b * c??????????digits < MAX_NN_DIGITS.
  ???????   :  1??DWORD *b : ????????b????¦Ë???????¦Ë???????b[digits]
                2??DWORD *c : ????????c????¦Ë???????¦Ë???????c[digits]
                3??DWORD digits : ????????b??c?????
  ???????   :  1??DWORD *a : b??c???????¦Ë???????¦Ë???????a[2*digits]
  ?????     :  ??
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
void NN_Mult(DWORD *a, DWORD *b, DWORD *c, DWORD digits)
{
    DWORD t[2*MAX_NN_DIGITS];
    DWORD dhigh, dlow, carry;
    DWORD bDigits, cDigits, i, j;

    NN_AssignZero(t, 2 * digits);

    bDigits = NN_Digits(b, digits);
    cDigits = NN_Digits(c, digits);

    for (i = 0; i < bDigits; i++)
    {
        carry = 0;
        if(*(b+i) != 0)
        {
            for(j = 0; j < cDigits; j++)
            {
                dmult(*(b+i), *(c+j), &dhigh, &dlow);
                if((*(t+(i+j)) = *(t+(i+j)) + carry) < carry)
                    carry = 1;
                else
                    carry = 0;
                if((*(t+(i+j)) += dlow) < dlow)
                    carry++;
                carry += dhigh;
            }
        }
        *(t+(i+cDigits)) += carry;
    }
    NN_Assign(a, t, 2 * digits);
}

/* Computes a = c div d and b = c mod d.

     Lengths: a[cDigits], b[dDigits], c[cDigits], d[dDigits].
     Assumes d > 0, cDigits < 2 * MAX_NN_DIGITS,
                     dDigits < MAX_NN_DIGITS.
*/
/****************************************************************************
  ??????     :
  ????       :  ???? a = c div d and b = c mod d.
  ???????   :  1??DWORD *c : ???????ï…??¦Ë???????¦Ë???????c[digits]
                2??DWORD cDigits : ????c?????
                3??DWORD *d : ???????ï…??¦Ë???????¦Ë???????d[digits]
                4??DWORD dDigits : ????d?????
  ???????   :  1??DWORD *a : ??????????¦Ë???????¦Ë???????a[digits]
                2??DWORD *b : ?????????¦Ë???????¦Ë???????b[digits]
  ?????     :  ??
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
void NN_Div(DWORD *a, DWORD *b, DWORD *c, DWORD cDigits, DWORD *d, DWORD dDigits)
{
    DWORD ai, cc[2*MAX_NN_DIGITS+1], dd[MAX_NN_DIGITS], s;
    DWORD t[2], u, v, *ccptr;
    WORD aHigh, aLow, cHigh, cLow;
    int i;
    DWORD ddDigits, shift;

    ddDigits = NN_Digits(d, dDigits);
    if(ddDigits == 0)
        return;

    shift = NN_DIGIT_BITS - NN_DigitBits(d[ddDigits-1]);
    NN_AssignZero(cc, ddDigits);
    cc[cDigits] = NN_LShift(cc, c, shift, cDigits);
    (void)NN_LShift(dd, d, shift, ddDigits);
    s = dd[ddDigits-1];

    NN_AssignZero(a, cDigits);

    for (i = cDigits-ddDigits; i >= 0; i--)
    {
        if (s == MAX_NN_DIGIT)
            ai = cc[i+ddDigits];
        else
        {
            ccptr = &cc[i+ddDigits-1];

            s++;
            cHigh = (WORD)HIGH_HALF(s);
            cLow = (WORD)LOW_HALF(s);

            *t = *ccptr;
            *(t+1) = *(ccptr+1);

            if (cHigh == MAX_NN_HALF_DIGIT)
                aHigh = (WORD)HIGH_HALF(*(t+1));
            else
                aHigh = (WORD)(*(t+1) / (cHigh + 1));
            u = (DWORD)aHigh * (DWORD)cLow;
            v = (DWORD)aHigh * (DWORD)cHigh;
            if ((*t -= TO_HIGH_HALF(u)) > (MAX_NN_DIGIT - TO_HIGH_HALF(u)))
                t[1]--;
            *(t+1) -= HIGH_HALF(u);
            *(t+1) -= v;

            while ((*(t+1) > cHigh) ||
                         ((*(t+1) == cHigh) && (*t >= TO_HIGH_HALF(cLow))))
            {
                if ((*t -= TO_HIGH_HALF(cLow)) > MAX_NN_DIGIT - TO_HIGH_HALF(cLow))
                    t[1]--;
                *(t+1) -= cHigh;
                aHigh++;
            }

            if (cHigh == MAX_NN_HALF_DIGIT)
                aLow = (WORD)LOW_HALF(*(t+1));
            else
                aLow = (WORD)((TO_HIGH_HALF(*(t+1)) + HIGH_HALF(*t)) / (cHigh + 1));
            u = (DWORD)aLow * (DWORD)cLow;
            v = (DWORD)aLow * (DWORD)cHigh;
            if ((*t -= u) > (MAX_NN_DIGIT - u))
                t[1]--;
            if ((*t -= TO_HIGH_HALF(v)) > (MAX_NN_DIGIT - TO_HIGH_HALF(v)))
                t[1]--;
            *(t+1) -= HIGH_HALF(v);

            while ((*(t+1) > 0) || ((*(t+1) == 0) && *t >= s))
            {
                if ((*t -= s) > (MAX_NN_DIGIT - s))
                    t[1]--;
                aLow++;
            }

            ai = TO_HIGH_HALF(aHigh) + aLow;
            s--;
        }

        cc[i+ddDigits] -= subdigitmult(&cc[i], &cc[i], ai, dd, ddDigits);

        while (cc[i+ddDigits] || (NN_Cmp(&cc[i], dd, ddDigits) >= 0))
        {
            ai++;
            cc[i+ddDigits] -= NN_Sub(&cc[i], &cc[i], dd, ddDigits);
        }

        a[i] = ai;
    }

    NN_AssignZero(b, dDigits);
    (void)NN_RShift(b, cc, shift, ddDigits);
}


/* Computes a = b * 2^c (i.e., shifts left c bits), returning carry.
     Requires c < NN_DIGIT_BITS. */
/****************************************************************************
  ??????     :
  ????       :  ???????????¦Ë???? a = b * 2^c ???????¦Ë???
  ???????   :  1??DWORD *b : ?????¦Ë?????
                2??DWORD c : ?????????¦Ë????c????????§³??32
                3??DWORD digits : ????b?????
  ???????   :  1??DWORD *a : ??????
  ?????     :  ?????¦Ë?
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
DWORD NN_LShift(DWORD *a, DWORD *b, DWORD c, DWORD digits)
{
    DWORD temp, carry = 0;
    DWORD t;

    if(c < NN_DIGIT_BITS)
    {
        if(digits)
        {
            t = NN_DIGIT_BITS - c;
            do
            {
                temp = *b++;
                *a++ = (temp << c) | carry;
                carry = c ? (temp >> t) : 0;
            }while(--digits);
        }
    }

    return (carry);
}

/* Computes a = b div 2^c (i.e., shifts right c bits), returning carry.

     Requires: c < NN_DIGIT_BITS. */
/****************************************************************************
  ??????     :
  ????       :  ?????????????? a = b div 2^c
  ???????   :  1??DWORD *b : ???????¦Ë????
                2??DWORD c : ????????¦Ë??
                3??DWORD digits : ????b?????
  ???????   :  1??DWORD *a : ??¦Ë?????
  ?????     :  ???????¦Ë???
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
DWORD NN_RShift(DWORD *a, DWORD *b, DWORD c, DWORD digits)
{
    DWORD temp, carry = 0;
    DWORD t;

    if(c < NN_DIGIT_BITS)
    {
        if(digits)
        {
            t = NN_DIGIT_BITS - c;
            do
            {
                digits--;
                temp = *(b+digits);
                *(a+digits) = (temp >> c) | carry;
                carry = c ? (temp << t) : 0;
            }while(digits);
        }
    }

    return (carry);
}

/* Computes a = b mod c.

     Lengths: a[cDigits], b[bDigits], c[cDigits].
     Assumes c > 0, bDigits < 2 * MAX_NN_DIGITS, cDigits < MAX_NN_DIGITS.
*/
/****************************************************************************
  ??????     :
  ????       :  ???? a = b mod c.
  ???????   :  1??DWORD *b : ????????¦Ë???????¦Ë???????b[bDigits]
                2??DWORD bDigits : ????b????????¦Ë???
                3??DWORD *c : ?????????¦Ë???????¦Ë???????c[cDigits]
                4??DWORD cDigits : ????c?????
  ???????   :  1??DWORD *a : ????????????????????????c????¦Ë???????¦Ë
                ???????a[cDigits]
  ?????     :
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
void NN_Mod(DWORD *a, DWORD *b, DWORD bDigits, DWORD *c, DWORD cDigits)
{
    DWORD t[2 * MAX_NN_DIGITS];

    NN_Div(t, a, b, bDigits, c, cDigits);
}

/* Computes a = b * c mod d.

   Lengths: a[digits], b[digits], c[digits], d[digits].
   Assumes d > 0, digits < MAX_NN_DIGITS.
 */
/****************************************************************************
  ??????     :
  ????       :  ???? a = b * c mod d.
  ???????   :  1??DWORD *b : ????????¦Ë???????¦Ë???????b[digits]
                2??DWORD *c : ????????¦Ë???????¦Ë???????c[digits]
                3??DWORD *d : ?????¦Ë???????¦Ë???????d[digits]
                4??DWORD digits : b??c??d?????
  ???????   :  1??DWORD *a : ??????????¦Ë???????¦Ë???????a[digits]
  ?????     :  ??
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
void NN_ModMult(DWORD *a, DWORD *b, DWORD *c, DWORD *d, DWORD digits)
{
    DWORD t[2*MAX_NN_DIGITS];

    NN_Mult(t, b, c, digits);
    NN_Mod(a, t, 2 * digits, d, digits);
}

/* Computes a = b^c mod d.

   Lengths: a[dDigits], b[dDigits], c[cDigits], d[dDigits].
     Assumes d > 0, cDigits > 0, dDigits < MAX_NN_DIGITS.
 */
 /****************************************************************************
  ??????     :
  ????       :  a = b^c mod d????
  ???????   :  1??DWORD *b : ????????¦Ë???????¦Ë???????b[dDigits]
                2??DWORD *c : ???????¦Ë???????¦Ë???????c[cDigits]
                3??DWORD cDigits : ???????
                4??DWORD *d : ?????¦Ë???????¦Ë???????d[dDigits]
                5??DWORD dDigits : ??????
  ???????   :  1??DWORD *a : ??????????¦Ë???????¦Ë???????a[dDigits]
  ?????     :  ??
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
void NN_ModExp(DWORD *a, DWORD *b, DWORD *c, DWORD cDigits, DWORD *d, DWORD dDigits)
{
    DWORD bPower[3][MAX_NN_DIGITS], ci, t[MAX_NN_DIGITS];
    int i;
    DWORD ciBits, j, s;

    /* Store b, b^2 mod d, and b^3 mod d.
     */
    NN_Assign(bPower[0], b, dDigits);
    NN_ModMult(bPower[1], bPower[0], b, d, dDigits);
    NN_ModMult(bPower[2], bPower[1], b, d, dDigits);

    NN_ASSIGN_DIGIT(t, 1, dDigits);

    cDigits = NN_Digits(c, cDigits);
    for (i = cDigits - 1; i >= 0; i--)
    {
        ci = c[i];
        ciBits = NN_DIGIT_BITS;

        /* Scan past leading zero bits of most significant digit.
         */
        if (i == (int)(cDigits - 1))
        {
            while (! DIGIT_2MSB(ci))
            {
                ci <<= 2;
                ciBits -= 2;
            }
        }

        for (j = 0; j < ciBits; j += 2, ci <<= 2)
        {
        /* Compute t = t^4 * b^s mod d, where s = two MSB's of ci. */
            NN_ModMult(t, t, t, d, dDigits);
            NN_ModMult(t, t, t, d, dDigits);
            if ((s = DIGIT_2MSB (ci)) != 0)
            NN_ModMult(t, t, bPower[s-1], d, dDigits);
        }
    }

    NN_Assign(a, t, dDigits);
}

/* Returns sign of a - b. */
/****************************************************************************
  ??????     :
  ????       :  ???????a??????b???§³
  ???????   :  1??DWORD *a : ??????????ï…??¦Ë???????¦Ë???????a[digits]
                2??DWORD *b : ??????????ï…??¦Ë???????¦Ë???????b[digits]
                3??DWORD digits : ????a??????b?????
  ???????   :  ??
  ?????     :  ?????????a=b?????0????a>b?????1????a<b?????-1
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
int NN_Cmp(DWORD *a, DWORD *b, DWORD digits)
{

    if(digits)
    {
        do
        {
            digits--;
            if(*(a+digits) > *(b+digits))
                return(1);
            if(*(a+digits) < *(b+digits))
                return(-1);
        }while(digits);
    }

    return (0);
}

/****************************************************************************
  ??????     :
  ????       :  ???????????a = b.
  ???????   :  1??DWORD *b : ????ï…??¦Ë???????¦Ë???????b[digits]
                2??DWORD digits : ????b?????
  ???????   :  1??DWORD *a : ???????????¦Ë???????¦Ë???????a[digits]
  ?????     :  ??
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
void NN_Assign(DWORD *a, DWORD *b, DWORD digits)
{
    if(digits)
    {
        do
        {
            *a++ = *b++;
        }while(--digits);
    }
}

/* Returns the significant length of a in digits. */
/****************************************************************************
  ??????     :
  ????       :  ???????????§¹?????????????
  ???????   :  1??DWORD *a : ?????????ï…??¦Ë?????¦Ë?????????a[digits]
                2??DWORD digits : ??????????
  ???????   :  ??
  ?????     :  ????????¦Ë??
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
DWORD NN_Digits(DWORD *a, DWORD digits)
{

    if(digits)
    {
        digits--;
        do
        {
            if(*(a+digits))
                break;
        }while(digits--);

        return(digits + 1);
    }

    return(digits);
}

/****************************************************************************
  ??????     :
  ????       :
  ???????   :
  ???????   :
  ?????     :
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
DWORD subdigitmult(DWORD *a, DWORD *b, DWORD c, DWORD *d, DWORD digits)
{
    DWORD borrow, thigh, tlow;
    DWORD i;

    borrow = 0;

    if(c != 0)
    {
        for(i = 0; i < digits; i++)
        {
            dmult(c, d[i], &thigh, &tlow);
            if((a[i] = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
                borrow = 1;
            else
                borrow = 0;
            if((a[i] -= tlow) > (MAX_NN_DIGIT - tlow))
                borrow++;
            borrow += thigh;
        }
    }

    return (borrow);
}

/* Returns the significant length of a in bits, where a is a digit. */
/****************************************************************************
  ??????     :
  ????       :  ????a????§¹?????¦Ë??
  ???????   :  1??DWORD a : ??????
  ???????   :  ??
  ?????     :  ??§¹?????¦Ë??
       ?    :  ?????????a=0x08000100????????28
                          a=3,????2
                          ???????0?????a=0
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1??
****************************************************************************/
DWORD NN_DigitBits(DWORD a)
{
    DWORD i;

    for (i = 0; i < NN_DIGIT_BITS; i++, a >>= 1)
    {
        if (a == 0)
            break;
    }
    return (i);
}

/****************************************************************************
  ??????     :
  ????       :  ??8¦Ë??????????????????32¦Ë????????????ï…?4??????????
                ???32¦Ë?????????
  ???????   :  1??BYTE *pbyIn????????8¦Ë????????????ï…??¦Ë???????¦Ë???
                2??DWORD dwInLen?????????pbyIn?????
  ???????   :  1??DWORD *pdwOut??????????????????¦Ë???????¦Ë???
  ?????     :  ?????????????????????0??????????????
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1?? ?????     2007-01-05  v1.0         ????
****************************************************************************/
DWORD BytesToDwords(BYTE *pbyIn, DWORD dwInLen, DWORD *pdwOut)
{
    DWORD dwTempLen;
    int iTemp;

    if ((NULL==pbyIn) || (NULL==pdwOut) || (0==dwInLen))
    {
        return 0;
    }
    dwTempLen = 0;
    iTemp = dwInLen-1;
    while (1)
    {
        if (iTemp < 3)
        {
            break;
        }
        pdwOut[dwTempLen] = pbyIn[iTemp-3]*0x01000000
                  + pbyIn[iTemp-2]*0x00010000
                  + pbyIn[iTemp-1]*0x00000100
                  + pbyIn[iTemp];
        dwTempLen++;
        iTemp -= 4;
    }
    switch (iTemp)
    {
    case 0:
        pdwOut[dwTempLen++] = pbyIn[0];
        break;
    case 1:
        pdwOut[dwTempLen++] = pbyIn[0]*0x00000100 + pbyIn[1];
        break;
    case 2:
        pdwOut[dwTempLen++] = pbyIn[0]*0x00010000 + pbyIn[1]*0x00000100 + pbyIn[2];
        break;
    default:
        break;
    }
    return dwTempLen;
}

/****************************************************************************
  ??????     :
  ????       :  ??32¦Ë??????????????8¦Ë?????????
  ???????   :  1??DWORD *pdwIn : ?????????ï…??¦Ë?????¦Ë???
                2??DWORD dwInLen : ????pdwIn?????
  ???????   :  1??BYTE *pbyOut : ????????????¦Ë???????¦Ë???
  ?????     :  ???????????
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1?? ?????     2007-01-05  v1.0         ????
****************************************************************************/
DWORD DwordsToBytes(DWORD *pdwIn, DWORD dwInLen, BYTE *pbyOut)
{
    DWORD i;
    for (i=0; i<dwInLen; i++)
    {
        pbyOut[i*4  ] = (BYTE)(pdwIn[dwInLen-i-1]/0x01000000);
        pbyOut[i*4+1] = (BYTE)(pdwIn[dwInLen-i-1]/0x00010000);
        pbyOut[i*4+2] = (BYTE)(pdwIn[dwInLen-i-1]/0x00000100);
        pbyOut[i*4+3] = (BYTE)(pdwIn[dwInLen-i-1]);
    }
    return 4*dwInLen;
}

/****************************************************************************
  ??????     :  DWORD RSACompute(BYTE *pbyModule,DWORD dwModuleLen,BYTE *pbyExp,
                              DWORD dwExpLen, BYTE *pbyDataIn,BYTE *pbyDataOut)
  ????       :  RSA?????????????
  ???       :  ?¨²???????RSA?????????????????????????¨°???????????
                ??(pbyModule,pbyExp)???????????????§Þ????????¨´????????
                ????§ß????????????????????????¦Ë???????BYTE(8 bits)???¦Ë??
                ?¨²????????????????2048 bits ??RSA????
  ???       :  ????pbyDataIn????§³???pbyModule
  ???????   :  1??BYTE *pbyModule : ???RSA???????????????????n=p*q????
                   ????¦Ë???????¦Ë???????›¥
                2??DWORD dwModuleLen : ????????BYTE???¦Ë??????¦¶1??256????
                3??BYTE *pbyExp : ???RSA?????????????????????e??????¦Ë??
                   ?????¦Ë???????›¥
                4??DWORD dwExpLen : ??????????BYTE???¦Ë??????¦¶??1??256??
                   ??????dwExpLen<=dwModuleLen????
                5??BYTE *pbyDataIn : ?????????????????????dwModuleLen??
  ???????   :  1??BYTE *pbyDataOut : ????????????????????dwModuleLen
  ?????     :  0?????
                -1??????????????????????????????????
                -2??????????????dwModuleLen???????????dwExpLen?0??????
                -3?????????dwModuleLen????????????dwExpLen????????
                -4???pbyDataIn???????pbyModule??????
                -5???dwExpLen????dwModuleLen??????
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1?? ?????     2007-01-05  v1.0         ????
****************************************************************************/
int RSACompute(BYTE *pbyModule, DWORD dwModuleLen, BYTE *pbyExp,
                DWORD dwExpLen, BYTE *pbyDataIn, BYTE *pbyDataOut)
{
    DWORD adwTempModule[MAX_NN_LEN],
          adwTempExp[MAX_NN_LEN],
          adwTempDataIn[MAX_NN_LEN],
          adwTempDataOut[MAX_NN_LEN];
    DWORD dwTempModuleLen, dwTempExpLen;
    BYTE abyTempOut[MAX_NN_LEN*4];

    if (   (NULL==pbyModule)
        || (NULL==pbyExp)
        || (NULL==pbyDataIn)
        || (NULL==pbyDataOut)  )
    {
        return -1;
    }
    if ((0==dwModuleLen) || (0==dwExpLen))
    {
        return -2;
    }
    if ((dwModuleLen>(MAX_NN_LEN*4)) || (dwExpLen>(MAX_NN_LEN*4)))
    {
        return -3;
    }
    if (memcmp(pbyDataIn,pbyModule,dwModuleLen)>=0)
    {
        return -4;
    }
    if (dwExpLen > dwModuleLen)
    {
        return -5;
    }

    memset(adwTempModule, 0, MAX_NN_LEN*sizeof(DWORD));
    memset(adwTempExp, 0, MAX_NN_LEN*sizeof(DWORD));
    memset(adwTempDataIn, 0, MAX_NN_LEN*sizeof(DWORD));
    memset(adwTempDataOut, 0, MAX_NN_LEN*sizeof(DWORD));

    dwTempModuleLen = BytesToDwords(pbyModule, dwModuleLen, adwTempModule);
    dwTempExpLen = BytesToDwords(pbyExp, dwExpLen, adwTempExp);
    (void)BytesToDwords(pbyDataIn, dwModuleLen, adwTempDataIn);
    NN_ModExp(adwTempDataOut, adwTempDataIn, adwTempExp, dwTempExpLen, adwTempModule, dwTempModuleLen);
    (void)DwordsToBytes(adwTempDataOut, dwTempModuleLen, abyTempOut);
    memcpy(pbyDataOut, &abyTempOut[dwTempModuleLen*4-dwModuleLen], dwModuleLen);

    return 0;
}



/* NIST Secure Hash Algorithm */
/* heavily modified by Uwe Hollerbach <uh@alumni.caltech edu> */
/* from Peter C. Gutmann's implementation as found in */
/* Applied Cryptography by Bruce Schneier */
/* Further modifications to include the "UNRAVEL" stuff, below */

/* This code is in the public domain */

/* UNRAVEL should be fastest & biggest */
/* UNROLL_LOOPS should be just as big, but slightly slower */
/* both undefined should be smallest and slowest */

#define UNRAVEL
/* #define UNROLL_LOOPS */



//#include "endian.h"

/* SHA f()-functions */

#define f1(x,y,z)    ((x & y) | (~x & z))
#define f2(x,y,z)    (x ^ y ^ z)
#define f3(x,y,z)    ((x & y) | (x & z) | (y & z))
#define f4(x,y,z)    (x ^ y ^ z)

/* SHA constants */

#define CONST1        0x5a827999L
#define CONST2        0x6ed9eba1L
#define CONST3        0x8f1bbcdcL
#define CONST4        0xca62c1d6L

/* truncate to 32 bits -- should be a null op on 32-bit machines */

#define T32(x)    ((x) & 0xffffffffL)

/* 32-bit rotate */

#define R32(x,n)    T32(((x << n) | (x >> (32 - n))))

/* the generic case, for when the overall rotation is not unraveled */

#define FG(n)    \
    T = T32(R32(A,5) + f##n(B,C,D) + E + *WP++ + CONST##n);    \
    E = D; D = C; C = R32(B,30); B = A; A = T

/* specific cases, for when the overall rotation is unraveled */

#define FA(n)    \
    T = T32(R32(A,5) + f##n(B,C,D) + E + *WP++ + CONST##n); B = R32(B,30)

#define FB(n)    \
    E = T32(R32(T,5) + f##n(A,B,C) + D + *WP++ + CONST##n); A = R32(A,30)

#define FC(n)    \
    D = T32(R32(E,5) + f##n(T,A,B) + C + *WP++ + CONST##n); T = R32(T,30)

#define FD(n)    \
    C = T32(R32(D,5) + f##n(E,T,A) + B + *WP++ + CONST##n); E = R32(E,30)

#define FE(n)    \
    B = T32(R32(C,5) + f##n(D,E,T) + A + *WP++ + CONST##n); D = R32(D,30)

#define FT(n)    \
    A = T32(R32(B,5) + f##n(C,D,E) + T + *WP++ + CONST##n); C = R32(C,30)

/* do SHA transformation */

static void sha_transform(SHA_INFO *sha_info)
{
    int i;
    BYTE *dp;
    unsigned long T, A, B, C, D, E, W[80], *WP;

    dp = sha_info->data;

/*
the following makes sure that at least one code block below is
traversed or an error is reported, without the necessity for nested
preprocessor if/else/endif blocks, which are a great pain in the
nether regions of the anatomy...
*/
#undef SWAP_DONE

#if (SHA_BYTE_ORDER == 1234)
#define SWAP_DONE
    for (i = 0; i < 16; ++i) 
    {
        T = *((unsigned long *) dp);
        dp += 4;
        W[i] =  ((T << 24) & 0xff000000) | ((T <<  8) & 0x00ff0000) |
            ((T >>  8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
    }
#endif /* SHA_BYTE_ORDER == 1234 */

#if (SHA_BYTE_ORDER == 4321)
#define SWAP_DONE
    for (i = 0; i < 16; ++i)
    {
        T = *((unsigned long *) dp);
        dp += 4;
        W[i] = T32(T);
    }
#endif /* SHA_BYTE_ORDER == 4321 */

#if (SHA_BYTE_ORDER == 12345678)
#define SWAP_DONE
    for (i = 0; i < 16; i += 2) 
    {
        T = *((unsigned long *) dp);
        dp += 8;
        W[i] =  ((T << 24) & 0xff000000) | ((T <<  8) & 0x00ff0000) |
            ((T >>  8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
        T >>= 32;
        W[i+1] = ((T << 24) & 0xff000000) | ((T <<  8) & 0x00ff0000) |
            ((T >>  8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
    }
#endif /* SHA_BYTE_ORDER == 12345678 */

#if (SHA_BYTE_ORDER == 87654321)
#define SWAP_DONE
    for (i = 0; i < 16; i += 2) {
        T = *((unsigned long *) dp);
        dp += 8;
        W[i] = T32(T >> 32);
        W[i+1] = T32(T);
    }
#endif /* SHA_BYTE_ORDER == 87654321 */

//#ifndef SWAP_DONE
//#error Unknown byte order -- you need to add code here
//#endif /* SWAP_DONE */
    
    for (i = 16; i < 80; ++i) {
        W[i] = W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16];
#if (SHA_VERSION == 1)
        W[i] = R32(W[i], 1);
#endif /* SHA_VERSION */
    }
    A = sha_info->digest[0];
    B = sha_info->digest[1];
    C = sha_info->digest[2];
    D = sha_info->digest[3];
    E = sha_info->digest[4];
    WP = W;
#ifdef UNRAVEL
    FA(1); FB(1); FC(1); FD(1); FE(1); FT(1); FA(1); FB(1); FC(1); FD(1);
    FE(1); FT(1); FA(1); FB(1); FC(1); FD(1); FE(1); FT(1); FA(1); FB(1);
    FC(2); FD(2); FE(2); FT(2); FA(2); FB(2); FC(2); FD(2); FE(2); FT(2);
    FA(2); FB(2); FC(2); FD(2); FE(2); FT(2); FA(2); FB(2); FC(2); FD(2);
    FE(3); FT(3); FA(3); FB(3); FC(3); FD(3); FE(3); FT(3); FA(3); FB(3);
    FC(3); FD(3); FE(3); FT(3); FA(3); FB(3); FC(3); FD(3); FE(3); FT(3);
    FA(4); FB(4); FC(4); FD(4); FE(4); FT(4); FA(4); FB(4); FC(4); FD(4);
    FE(4); FT(4); FA(4); FB(4); FC(4); FD(4); FE(4); FT(4); FA(4); FB(4);
    sha_info->digest[0] = T32(sha_info->digest[0] + E);
    sha_info->digest[1] = T32(sha_info->digest[1] + T);
    sha_info->digest[2] = T32(sha_info->digest[2] + A);
    sha_info->digest[3] = T32(sha_info->digest[3] + B);
    sha_info->digest[4] = T32(sha_info->digest[4] + C);
#else /* !UNRAVEL */
#ifdef UNROLL_LOOPS
    FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1);
    FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1);
    FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2);
    FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2);
    FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3);
    FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3);
    FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4);
    FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4);
#else /* !UNROLL_LOOPS */
    for (i =  0; i < 20; ++i) { FG(1); }
    for (i = 20; i < 40; ++i) { FG(2); }
    for (i = 40; i < 60; ++i) { FG(3); }
    for (i = 60; i < 80; ++i) { FG(4); }
#endif /* !UNROLL_LOOPS */
    sha_info->digest[0] = T32(sha_info->digest[0] + A);
    sha_info->digest[1] = T32(sha_info->digest[1] + B);
    sha_info->digest[2] = T32(sha_info->digest[2] + C);
    sha_info->digest[3] = T32(sha_info->digest[3] + D);
    sha_info->digest[4] = T32(sha_info->digest[4] + E);
#endif /* !UNRAVEL */
}

/* initialize the SHA digest */

void sha_init(SHA_INFO *sha_info)
{
    sha_info->digest[0] = 0x67452301L;
    sha_info->digest[1] = 0xefcdab89L;
    sha_info->digest[2] = 0x98badcfeL;
    sha_info->digest[3] = 0x10325476L;
    sha_info->digest[4] = 0xc3d2e1f0L;
    sha_info->count_lo = 0L;
    sha_info->count_hi = 0L;
    sha_info->local = 0;
}

/* update the SHA digest */

void sha_update(SHA_INFO *sha_info, BYTE *buffer, int count)
{
    int i;
    unsigned long clo;
    
    clo = T32(sha_info->count_lo + ((unsigned long) count << 3));
    if (clo < sha_info->count_lo)
    {
        ++sha_info->count_hi;
    }
    sha_info->count_lo = clo;
    sha_info->count_hi += (unsigned long) count >> 29;
    if (sha_info->local)
    {
        i = SHA_BLOCKSIZE - sha_info->local;
        if (i > count) 
        {
            i = count;
        }
        memcpy(((BYTE *) sha_info->data) + sha_info->local, buffer, i);
        count -= i;
        buffer += i;
        sha_info->local += i;
        if (sha_info->local == SHA_BLOCKSIZE) 
        {
            sha_transform(sha_info);
        } 
        else 
        {
            return;
        }
    }
    while (count >= SHA_BLOCKSIZE) 
    {
        memcpy(sha_info->data, buffer, SHA_BLOCKSIZE);
        buffer += SHA_BLOCKSIZE;
        count -= SHA_BLOCKSIZE;
        sha_transform(sha_info);
    }
    memcpy(sha_info->data, buffer, count);
    sha_info->local = count;
}

/* finish computing the SHA digest */

void sha_final(BYTE digest[20], SHA_INFO *sha_info)
{
    int count;
    unsigned long lo_bit_count, hi_bit_count;
    
    lo_bit_count = sha_info->count_lo;
    hi_bit_count = sha_info->count_hi;
    count = (int) ((lo_bit_count >> 3) & 0x3f);
    ((BYTE *) sha_info->data)[count++] = 0x80;
    if (count > SHA_BLOCKSIZE - 8)
    {
        memset(((BYTE *) sha_info->data) + count, 0, SHA_BLOCKSIZE - count);
        sha_transform(sha_info);
        memset((BYTE *) sha_info->data, 0, SHA_BLOCKSIZE - 8);
    }
    else
    {
        memset(((BYTE *) sha_info->data) + count, 0,
            SHA_BLOCKSIZE - 8 - count);
    }
    sha_info->data[56] = (BYTE)(hi_bit_count >> 24) & 0xff;
    sha_info->data[57] = (BYTE)(hi_bit_count >> 16) & 0xff;
    sha_info->data[58] = (BYTE)(hi_bit_count >>  8) & 0xff;
    sha_info->data[59] = (BYTE)(hi_bit_count >>  0) & 0xff;
    sha_info->data[60] = (BYTE)(lo_bit_count >> 24) & 0xff;
    sha_info->data[61] = (BYTE)(lo_bit_count >> 16) & 0xff;
    sha_info->data[62] = (BYTE)(lo_bit_count >>  8) & 0xff;
    sha_info->data[63] = (BYTE)(lo_bit_count >>  0) & 0xff;
    sha_transform(sha_info);
    digest[ 0] = (BYTE) ((sha_info->digest[0] >> 24) & 0xff);
    digest[ 1] = (BYTE) ((sha_info->digest[0] >> 16) & 0xff);
    digest[ 2] = (BYTE) ((sha_info->digest[0] >>  8) & 0xff);
    digest[ 3] = (BYTE) ((sha_info->digest[0]      ) & 0xff);
    digest[ 4] = (BYTE) ((sha_info->digest[1] >> 24) & 0xff);
    digest[ 5] = (BYTE) ((sha_info->digest[1] >> 16) & 0xff);
    digest[ 6] = (BYTE) ((sha_info->digest[1] >>  8) & 0xff);
    digest[ 7] = (BYTE) ((sha_info->digest[1]      ) & 0xff);
    digest[ 8] = (BYTE) ((sha_info->digest[2] >> 24) & 0xff);
    digest[ 9] = (BYTE) ((sha_info->digest[2] >> 16) & 0xff);
    digest[10] = (BYTE) ((sha_info->digest[2] >>  8) & 0xff);
    digest[11] = (BYTE) ((sha_info->digest[2]      ) & 0xff);
    digest[12] = (BYTE) ((sha_info->digest[3] >> 24) & 0xff);
    digest[13] = (BYTE) ((sha_info->digest[3] >> 16) & 0xff);
    digest[14] = (BYTE) ((sha_info->digest[3] >>  8) & 0xff);
    digest[15] = (BYTE) ((sha_info->digest[3]      ) & 0xff);
    digest[16] = (BYTE) ((sha_info->digest[4] >> 24) & 0xff);
    digest[17] = (BYTE) ((sha_info->digest[4] >> 16) & 0xff);
    digest[18] = (BYTE) ((sha_info->digest[4] >>  8) & 0xff);
    digest[19] = (BYTE) ((sha_info->digest[4]      ) & 0xff);
}

void Hash(BYTE* DataIn,unsigned int DataInLen,BYTE* DataOut)
{
    unsigned int i,k,m;
    SHA_INFO sha_info;

    sha_init(&sha_info);
    k=DataInLen/SHA_BLOCKSIZE;
    m=DataInLen%SHA_BLOCKSIZE;
    for(i=0;i<k;i++) {
        sha_update(&sha_info, (BYTE*)&DataIn[i*SHA_BLOCKSIZE], SHA_BLOCKSIZE);
    }
    if(m!=0)
        sha_update(&sha_info, (BYTE*)&DataIn[i*SHA_BLOCKSIZE], m);

    sha_final(DataOut, &sha_info);
}


/****************************************************************************
  ??????     :  void AddFileToBaleFile(FILE *fpBaleFile,
                          FILE *fp, int *piOutLen)
  ????       :  ?????????????
  ???????   :  1??FILE *fpBaleFile???????????
                2??FILE *fp???????????????
  ???????   :  1??int *piOutLen????????????????????????+5
  ?????     :  ??
  ??????   :
      ?????     ??????    ???·Ú??   ??????
  1?? ?????     2007-09-13   V1.0        ????
  2?? ?????     2007-05-17   V1.1        ????????????????????
****************************************************************************/
void AddFileToBaleFile(FILE *fpBaleFile, FILE *fp, int *piOutLen)
{
    int iLen, iRet;
    WORD wPiece, wTemp, i, wTailLen;
    DWORD dwCompressLen, dwCurLen;
    BYTE strTemp[10], *pSData, *pCData;

    *piOutLen = 0;
    fseek(fp, 0, SEEK_END);
    iLen = ftell(fp);
    if (iLen <= 0)
    {
        return ;
    }
    pSData = (BYTE*)malloc(iLen+10);
    if (NULL == pSData)
    {
        return ;
    }
    pCData = (BYTE*)malloc(iLen+INCISE_SIZE+2000);
    if (NULL == pCData)
    {
        free(pSData);
        return ;
    }
    fseek(fp, 0, SEEK_SET);
    iLen = fread(pSData, 1, iLen, fp);
    if (REMOTE_DOWNLOAD == g_sConfigreInfo.m_byLoadType)
    {
        wPiece = (WORD)(iLen/INCISE_SIZE);
        wTailLen = (WORD)(iLen%INCISE_SIZE);
        if (0 != wTailLen)
        {
            wPiece++;
        }
        else
        {
            wTailLen = INCISE_SIZE;
        }
        pCData[4] = wPiece/256;
        pCData[5] = wPiece%256;
        pCData[6] = INCISE_SIZE/256;
        pCData[7] = INCISE_SIZE%256;
        pCData[8] = wTailLen/256;
        pCData[9] = wTailLen%256;
        dwCurLen = 0;
        i = 0;
        while (1)
        {
            if (i >= wPiece)
            {
                pCData[0] = (BYTE)(dwCurLen>>24);
                pCData[1] = (BYTE)((dwCurLen>>16)&0xff);
                pCData[2] = (BYTE)((dwCurLen>>8)&0xff);
                pCData[3] = (BYTE)(dwCurLen%256);
                break;
            }
            else if (i == (wPiece-1))
            {
                wTemp = wTailLen;
            }
            else
            {
                wTemp = INCISE_SIZE;
            }
            Compress(&pSData[i*INCISE_SIZE], &pCData[dwCurLen+wPiece*3+10], 
                wTemp, &dwCompressLen);
            if (dwCompressLen < wTemp)
            {
                wTemp = (WORD)dwCompressLen;
                pCData[i*3+10] = IS_COMPRESS;
            }
            else
            {
                pCData[i*3+10] = NO_COMPRESS;
                memcpy(&pCData[dwCurLen+wPiece*3+10], &pSData[i*INCISE_SIZE], wTemp);
            }
            pCData[i*3+11] = (BYTE)(wTemp/256);
            pCData[i*3+12] = (BYTE)(wTemp%256);
            dwCurLen += wTemp;
            i++;
        }
        fseek(fpBaleFile, 0, SEEK_END);
        strTemp[0] = IS_COMPRESS;
        fwrite(strTemp, 1, 1, fpBaleFile);
        iRet = fwrite(pCData, 1, dwCurLen+wPiece*3+10, fpBaleFile);
        *piOutLen = dwCurLen+wPiece*3+10 + 1;
    }
    else
    {
        fseek(fpBaleFile, 0, SEEK_END);
        strTemp[0] = 0;
        fwrite(strTemp, 1, 1, fpBaleFile);
        DwordToByte((DWORD)iLen, strTemp);
        fwrite(strTemp, 1, 4, fpBaleFile);
        iRet = fwrite(pSData, 1, iLen, fpBaleFile);
        *piOutLen = iLen + 5;
    }
    free(pSData);
    free(pCData);
}


int FileSign(char *strFileName)
{
    int iFileLen, iReadLen, iExpLen, iModuleLen;
    FILE *fp;
    BYTE *pSData;
    BYTE abyTail[700], abyHash[256], abyRSAModule[256], abyRSAExp[256], abyTemp[256];

    fp = fopen(strFileName, "rb+");
    if (NULL == fp)
    {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    iFileLen = ftell(fp);
    if ((iFileLen>(2*1024*1024)) || (iFileLen<20))
    {
        fclose(fp);
        return 1;
    }
    pSData = (BYTE*)malloc(iFileLen+2000);
//    fflush(fp);
    fseek(fp, 0, SEEK_SET);
    iReadLen = 0;
    while (1)
    {
        iExpLen = fread(&pSData[iReadLen], 1, 1024, fp);
        if (iExpLen <= 0)
        {
            break;
        }
        else
        {
            iReadLen += iExpLen;
            if (iReadLen >= iFileLen)
            {
                break;
            }
        }
    }
    memset(abyHash, 0, sizeof(abyHash));
    Hash(pSData, iFileLen, &abyHash[1]);
    free(pSData);
    memcpy(&abyHash[22], "VIEWAT CO. LTD.", 15);
    memcpy(abyRSAModule, "\xDF\x2F\x04\xC7\xC6\x48\xBC\x42\x9E\x6B\x5F\x1A\x18\x5D\x7D\x51\x25\x2E\xF6\xAB\xB3\x78\x25\x00\xEF\x1C\x2C\x4B\xAE\xA3\x7E\x8F\xB5\x02\xB9\x64\x1F\x9D\x84\x76\x1D\xEB\x1C\xE2\xCB\x42\xBD\x1D\x2D\x67\x9E\x8C\xD8\xFC\xE0\xDC\xF3\x79\xE9\x6D\x9C\xB4\x33\x62\x92\x79\x15\x8B\xF6\xF4\xB9\x5E\x81\xAF\x41\xFE\x6A\x39\x2D\x49\x78\x54\xED\x94\x50\x81\xA7\x2D\x44\x43\xF5\x1F\xAF\x98\x97\xB3\x8F\xCF\x88\xF7\x89\x1F\x13\x8C\xA4\x0D\xA8\xAA\x63\xF7\x59\x74\x3B\xFD\xE5\xFB\xAE\xBD\xBD\xD1\x43\xF8\x18\xAE\x7B\xA9\x2E\x36\xFB\xF6\x6A\x06\xE6\x4F\x12\xAC\x6A\xD6\x5E\xB0\x08\x31\x23\x72\x4F\x49\x12\x22\xCB\x81\x57\x0E\xCC\x4D\x6E\xA6\xE2\xB7\xD9\xCB", 160);
    memcpy(abyRSAExp,    "\x1A\x6C\xB7\x3C\xAE\x09\xEB\x9C\x59\xA4\x6D\xD8\xA9\x9C\xDC\x1B\x10\xF3\x7C\x0A\x3D\x50\x12\xD5\x86\xFA\x07\x76\x98\x81\x4C\x19\x58\xBD\xAF\x54\x9E\xF7\xA7\xF8\xD2\x37\x05\xAF\x06\x90\xEA\xD2\x3B\x0F\x83\x32\xBA\xF8\x49\xE9\x6F\x3C\x76\x7A\x5F\x6C\xE8\x8E\x99\xB8\x13\x68\x84\x7F\x5A\x39\x49\xD2\x85\xD6\x49\x1E\x66\xF1\x8C\xDD\xB0\xE9\x32\xC1\x3D\x8B\xCE\x05\xA0\xC5\xB0\x4E\x63\xF7\x57\x09\x90\x5D\x98\xD4\x2A\x57\x4F\xCB\xC3\x10\xCF\x3F\xD1\x43\xCB\xC1\xBA\xC9\x88\xC8\x14\xE6\x1F\x96\x28\x1F\x86\xE6\xDE\x1A\x27\x44\xEE\xDE\xB3\x9B\x60\xA9\xCE\x43\x97\xE7\x72\x29\x7D\x64\x6C\x98\x77\x46\xB8\xF5\x57\xE1\x1F\x18\xF7\x4B\xC3\xE3\x02\xB9", 160);
    iExpLen = 160;
    iModuleLen = 160;
    memset(abyTail, 0, sizeof(abyTail));
    RSACompute(abyRSAModule, iModuleLen, abyRSAExp, iExpLen, abyHash, abyTail);
    abyTail[257] = 160;
    memcpy(&abyTail[258], "ViewAtSigned", 12);
    fseek(fp, 0, SEEK_END);
    fwrite(abyTail, 1, 270, fp);
    fwrite("Remote Download OK", 1, 18, fp);
   
    // for debug
    abyRSAExp[0] = 0x01;
    abyRSAExp[1] = 0x00;
    abyRSAExp[2] = 0x01;
    iExpLen = 3;
    
    RSACompute(abyRSAModule, iModuleLen, abyRSAExp, iExpLen, abyTail, abyTemp);

    fclose(fp);
    return 0;
}

