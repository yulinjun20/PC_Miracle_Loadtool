#ifndef _ENV_JSON_H_
#define _ENV_JSON_H_

/*
 * Miracle ENV (.env) file format: UTF-8 JSON (readable in Notepad).
 *
 * Schema (array of name/value objects):
 * [
 *   {"name": "Merchant", "value": "ABC Store"},
 *   {"name": "Timeout",  "value": "30"}
 * ]
 *
 * Limits:
 *   - name  <= 64 chars
 *   - value <= 1024 chars
 *   - at most 600 entries
 *   - file size <= 128 KB
 *
 * Download still sends the raw .env bytes as FILE_TYPE_PARAM / EnvFile.
 */

#define ENV_JSON_NAME_MAX       64
#define ENV_JSON_VALUE_MAX      1024
#define ENV_JSON_MAX_ITEMS      600
#define ENV_JSON_MAX_FILE_SIZE  (128 * 1024)

typedef struct {
    char strName[ENV_JSON_NAME_MAX + 1];
    char strContent[ENV_JSON_VALUE_MAX + 1];
} ENV_PARAM_ITEM;

/* Load JSON .env into items[]. Returns entry count (>=0), or
 * -1 invalid JSON/structure, -2 cannot open/read, -3 too many entries / field too long. */
int EnvJsonLoadFile(const char *strFileName, ENV_PARAM_ITEM *items, int iMaxItems);

/* Save items that have non-empty name. Returns 0 ok, -1 write fault, -2 bad args. */
int EnvJsonSaveFile(const char *strFileName, const ENV_PARAM_ITEM *items, int iCount);

/* Validate file is Miracle JSON env. Same return codes as CheckParam:
 * -1 not a valid env file, -2 cannot open, >0 file size in bytes. */
int EnvJsonCheckFile(const char *strFileName);

#endif
