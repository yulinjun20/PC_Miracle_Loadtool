#include "stdafx.h"
#include "EnvJson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const char *SkipWs(const char *p)
{
    while (p && *p && ((unsigned char)*p <= 0x20))
        p++;
    return p;
}

static int JsonUnescape(const char *src, int iSrcLen, char *dst, int iDstMax)
{
    int i = 0, j = 0;
    while (i < iSrcLen)
    {
        char c = src[i++];
        if (c == '\\' && i < iSrcLen)
        {
            char e = src[i++];
            switch (e)
            {
            case '\\': c = '\\'; break;
            case '"':  c = '"';  break;
            case '/':  c = '/';  break;
            case 'b':  c = '\b'; break;
            case 'f':  c = '\f'; break;
            case 'n':  c = '\n'; break;
            case 'r':  c = '\r'; break;
            case 't':  c = '\t'; break;
            case 'u':
                /* Skip \uXXXX; store '?' (BMP not fully handled). */
                if (i + 4 > iSrcLen)
                    return -1;
                i += 4;
                c = '?';
                break;
            default:
                return -1;
            }
        }
        if (j >= iDstMax)
            return -1;
        dst[j++] = c;
    }
    dst[j] = 0;
    return j;
}

static int JsonEscapeWrite(FILE *fp, const char *s)
{
    const unsigned char *p = (const unsigned char *)s;
    if (fputc('"', fp) == EOF)
        return -1;
    while (*p)
    {
        unsigned char c = *p++;
        if (c == '\\' || c == '"')
        {
            if (fputc('\\', fp) == EOF || fputc(c, fp) == EOF)
                return -1;
        }
        else if (c == '\n')
        {
            if (fputs("\\n", fp) == EOF)
                return -1;
        }
        else if (c == '\r')
        {
            if (fputs("\\r", fp) == EOF)
                return -1;
        }
        else if (c == '\t')
        {
            if (fputs("\\t", fp) == EOF)
                return -1;
        }
        else if (c < 0x20)
        {
            if (fprintf(fp, "\\u%04x", c) < 0)
                return -1;
        }
        else
        {
            if (fputc(c, fp) == EOF)
                return -1;
        }
    }
    if (fputc('"', fp) == EOF)
        return -1;
    return 0;
}

/* Parse a JSON string starting at p (must be on '"'). Returns pointer after closing quote. */
static const char *ParseJsonString(const char *p, char *dst, int iDstMax)
{
    const char *start;
    int len;
    char tmp[ENV_JSON_VALUE_MAX * 2 + 8];

    p = SkipWs(p);
    if (!p || *p != '"')
        return NULL;
    p++;
    start = p;
    while (*p)
    {
        if (*p == '\\')
        {
            p++;
            if (!*p)
                return NULL;
            p++;
            continue;
        }
        if (*p == '"')
            break;
        p++;
    }
    if (*p != '"')
        return NULL;
    len = (int)(p - start);
    if (len >= (int)sizeof(tmp))
        return NULL;
    memcpy(tmp, start, len);
    tmp[len] = 0;
    if (JsonUnescape(tmp, len, dst, iDstMax) < 0)
        return NULL;
    return p + 1;
}

static const char *ParseOneObject(const char *p, ENV_PARAM_ITEM *item)
{
    char key[64];
    int gotName = 0, gotValue = 0;

    memset(item, 0, sizeof(ENV_PARAM_ITEM));
    p = SkipWs(p);
    if (!p || *p != '{')
        return NULL;
    p = SkipWs(p + 1);

    if (*p == '}')
        return NULL; /* empty object not allowed: need name and value */

    while (*p)
    {
        p = ParseJsonString(p, key, (int)sizeof(key) - 1);
        if (!p)
            return NULL;
        p = SkipWs(p);
        if (*p != ':')
            return NULL;
        p = SkipWs(p + 1);

        if (0 == strcmp(key, "name"))
        {
            p = ParseJsonString(p, item->strName, ENV_JSON_NAME_MAX);
            if (!p)
                return NULL;
            gotName = 1;
        }
        else if (0 == strcmp(key, "value"))
        {
            p = ParseJsonString(p, item->strContent, ENV_JSON_VALUE_MAX);
            if (!p)
                return NULL;
            gotValue = 1;
        }
        else
        {
            char discard[8];
            p = ParseJsonString(p, discard, (int)sizeof(discard) - 1);
            if (!p)
                return NULL;
        }

        p = SkipWs(p);
        if (*p == ',')
        {
            p = SkipWs(p + 1);
            continue;
        }
        if (*p == '}')
        {
            if (!gotName || !gotValue)
                return NULL;
            return p + 1;
        }
        return NULL;
    }
    return NULL;
}

static int ParseEnvJsonBuffer(const char *buf, ENV_PARAM_ITEM *items, int iMaxItems, int *piCount)
{
    const char *p;
    int n = 0;

    if (piCount)
        *piCount = 0;
    p = SkipWs(buf);
    if (!p || *p != '[')
        return -1;
    p = SkipWs(p + 1);
    if (*p == ']')
    {
        if (piCount)
            *piCount = 0;
        return 0;
    }

    while (*p)
    {
        if (n >= iMaxItems)
            return -3;
        p = ParseOneObject(p, &items[n]);
        if (!p)
            return -1;
        n++;
        p = SkipWs(p);
        if (*p == ',')
        {
            p = SkipWs(p + 1);
            continue;
        }
        if (*p == ']')
        {
            p = SkipWs(p + 1);
            if (piCount)
                *piCount = n;
            if (p && *p)
                return -1;
            return 0;
        }
        return -1;
    }
    return -1;
}

int EnvJsonLoadFile(const char *strFileName, ENV_PARAM_ITEM *items, int iMaxItems)
{
    FILE *fp;
    long iLen;
    char *buf;
    int iRet, iCount = 0;

    if (!strFileName || !items || iMaxItems <= 0)
        return -2;

    memset(items, 0, sizeof(ENV_PARAM_ITEM) * iMaxItems);

    fp = fopen(strFileName, "rb");
    if (NULL == fp)
        return -2;

    fseek(fp, 0, SEEK_END);
    iLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (iLen < 0 || iLen > ENV_JSON_MAX_FILE_SIZE)
    {
        fclose(fp);
        return -1;
    }
    if (0 == iLen)
    {
        fclose(fp);
        return 0;
    }

    buf = (char *)malloc((size_t)iLen + 1);
    if (NULL == buf)
    {
        fclose(fp);
        return -2;
    }
    if ((long)fread(buf, 1, (size_t)iLen, fp) != iLen)
    {
        free(buf);
        fclose(fp);
        return -2;
    }
    fclose(fp);
    buf[iLen] = 0;

    {
        const char *start = buf;
        if (iLen >= 3 && (unsigned char)buf[0] == 0xEF
            && (unsigned char)buf[1] == 0xBB && (unsigned char)buf[2] == 0xBF)
            start = buf + 3;
        iRet = ParseEnvJsonBuffer(start, items, iMaxItems, &iCount);
    }
    free(buf);
    if (0 != iRet)
        return iRet;
    return iCount;
}

int EnvJsonSaveFile(const char *strFileName, const ENV_PARAM_ITEM *items, int iCount)
{
    FILE *fp;
    int i;
    int iWritten = 0;
    int iLast;

    if (!strFileName || !items || iCount < 0)
        return -2;

    iLast = -1;
    for (i = 0; i < iCount; i++)
    {
        if (items[i].strName[0] != 0)
            iLast = i;
    }

    fp = fopen(strFileName, "wb");
    if (NULL == fp)
        return -1;

    if (fputs("[\r\n", fp) == EOF)
    {
        fclose(fp);
        return -1;
    }

    for (i = 0; i <= iLast; i++)
    {
        if (items[i].strName[0] == 0)
            continue;

        if (iWritten > 0)
        {
            if (fputs(",\r\n", fp) == EOF)
            {
                fclose(fp);
                return -1;
            }
        }
        if (fputs("  {", fp) == EOF)
        {
            fclose(fp);
            return -1;
        }
        if (fputs("\"name\":", fp) == EOF || JsonEscapeWrite(fp, items[i].strName) != 0)
        {
            fclose(fp);
            return -1;
        }
        if (fputs(", \"value\":", fp) == EOF || JsonEscapeWrite(fp, items[i].strContent) != 0)
        {
            fclose(fp);
            return -1;
        }
        if (fputs("}", fp) == EOF)
        {
            fclose(fp);
            return -1;
        }
        iWritten++;
    }

    if (fputs("\r\n]\r\n", fp) == EOF)
    {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

int EnvJsonCheckFile(const char *strFileName)
{
    FILE *fp;
    long iLen;
    char *buf;
    int iRet;
    ENV_PARAM_ITEM *items;
    int iCount = 0;

    if (!strFileName)
        return -2;

    fp = fopen(strFileName, "rb");
    if (NULL == fp)
        return -2;

    fseek(fp, 0, SEEK_END);
    iLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (iLen <= 0 || iLen > ENV_JSON_MAX_FILE_SIZE)
    {
        fclose(fp);
        return -1;
    }

    buf = (char *)malloc((size_t)iLen + 1);
    if (NULL == buf)
    {
        fclose(fp);
        return -2;
    }
    if ((long)fread(buf, 1, (size_t)iLen, fp) != iLen)
    {
        free(buf);
        fclose(fp);
        return -2;
    }
    fclose(fp);
    buf[iLen] = 0;

    items = (ENV_PARAM_ITEM *)malloc(sizeof(ENV_PARAM_ITEM) * ENV_JSON_MAX_ITEMS);
    if (NULL == items)
    {
        free(buf);
        return -2;
    }
    memset(items, 0, sizeof(ENV_PARAM_ITEM) * ENV_JSON_MAX_ITEMS);

    {
        const char *start = buf;
        if (iLen >= 3 && (unsigned char)buf[0] == 0xEF
            && (unsigned char)buf[1] == 0xBB && (unsigned char)buf[2] == 0xBF)
            start = buf + 3;
        iRet = ParseEnvJsonBuffer(start, items, ENV_JSON_MAX_ITEMS, &iCount);
    }

    free(items);
    free(buf);
    if (0 != iRet)
        return -1;
    return (int)iLen;
}
