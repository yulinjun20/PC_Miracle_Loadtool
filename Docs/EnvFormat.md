# Miracle ENV (.env) format

The param / ENV file used by Edit Param, App manage Param download, and Task bale Param is **UTF-8 JSON** (you can open it in Notepad).

## Schema

An array of objects. Each object has two string fields: `name` and `value`.

```json
[
  {"name": "Merchant", "value": "ABC Store"},
  {"name": "Timeout", "value": "30"}
]
```

Empty array `[]` is valid (no parameters).

## Limits

| Item | Limit |
|------|--------|
| `name` length | <= 64 characters |
| `value` length | <= 1024 characters |
| Number of entries | <= 600 |
| File size | <= 128 KB |

## Notes

- Extension remains `*.env`; content is JSON.
- Download sends the raw file bytes as type Param / device name `EnvFile`.
- Old binary ENV (128 bytes per record) is no longer supported.
