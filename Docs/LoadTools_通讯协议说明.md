# LoadTools[v3.1]_En 通讯协议说明

> 本文档依据工程源码整理，仅描述代码中已实现的行为，不推测设备端未在 PC 工具中体现的字段语义。  
> 主要依据：`LoadToolsDefine.h`、`Communication.cpp` / `Communication.h`、`PageApplication.cpp`、`PageFirmware.cpp`、`PageCommSet.cpp`。

---

## 1. 概述

LoadTools 与终端之间存在 **两套独立协议**，分别服务两个功能页：

| 协议 | 用途界面 | 握手字符 | 帧校验 | 认证 | 默认串口参数 |
|------|----------|----------|--------|------|--------------|
| **应用管理协议（App manage）** | `PageApplication`（应用/字库/参数/数据包下载、刷新、删除等） | PC 发 `'L'`，终端回 `'A'` | **CRC16-CCITT** | 有（DES） | 握手固定 **115200 8N1**，之后可按配置改波特率 |
| **固件下载协议（Firmware）** | `PageFirmware`（Boot / VOS） | PC 发 `'T'`，终端回 `'R'` 或 `'S'` | `'R'`→**LRC**；`'S'`→**CRC32** | **无** | 全程固定 **115200 8N1** |

### 1.1 串口默认与可选波特率

- 串口打开参数（`SerOpen` → `OpenSerialPort`）：**8 数据位、无校验、1 停止位（8N1）**。
- 应用管理握手时 **始终先以 115200 打开**；若配置项 `m_iSerialPortBaudsSelect == 0`（即 115200），握手成功后不再改波特率。
- 可选波特率表（`g_adwBauds[]`，下标对应通讯设置页 Combo）：

| 下标 | 波特率 |
|------|--------|
| 0 | 115200（默认） |
| 1 | 57600 |
| 2 | 38400 |
| 3 | 28800 |
| 4 | 19200 |
| 5 | 14400 |
| 6 | 9600 |
| 7 | 230400（界面会二次确认：仅 PCI 插槽串口卡才支持） |

- 固件下载：`Handshack()` 中 `SerOpen(..., 115200)`，**不再发送改波特率命令**。

### 1.2 USB 说明（界面已禁用）

- 通讯设置页 `PageCommSet::OnInitDialog` 中：  
  `GetDlgItem(IDC_RADIO_USE_USB)->EnableWindow(FALSE);`  
  即 **USB 单选按钮在 UI 上被禁用**，用户无法通过界面切换到 USB。
- `m_byCommType == 0` 表示串口，`== 1` 表示 USB；应用管理仍保留 `UsbHandshack()`（同样发 `'L'`、收 `'A'`），但正常使用路径为串口。
- 固件下载路径 **只走串口**，无 USB 握手分支。

### 1.3 分块长度常量

| 宏 | 值 | 用途 |
|----|----|------|
| `DOWNLOAD_STEP_LEN` | **4096** | 应用管理文件数据块最大长度 |
| `DOWNLOAD_LEN_FIRM` | **3064** | 固件文件数据块最大长度 |

---

## 2. 应用管理协议（App manage）

### 2.1 总体流程

典型下载线程（`ThreadDownload`）：

1. `DownloadStart()` = `Handshack()` + `Authentication()`
2. `DownloadProcess()`（按界面勾选项执行：校时 / 打包文件 / 建文件系统 / 写序列号 / 下字库 / 下应用 / 下参数 / 下数据文件）
3. 成功则 `UpdateTreeList()`（刷新树）
4. `DownloadEnd()`：发送 `CMD_FINSH_ALL (0xB400)`，延时 50ms，关闭串口

刷新按钮（界面控件绑定 `m_btnReflush`，消息 `OnButtonRefurbish`）：

1. `DownloadStart()`
2. `UpdateTreeList()`
3. `DownloadEnd()`（发 `0xB400`）

删除应用 / 附属文件：同样先握手+认证，再发删除命令，再刷新树，最后 `0xB400`。

### 2.2 握手（L / A）

**串口（`SerialHandshack`）**

1. `SerOpen(port+1, 115200)`
2. 循环：`SerWrite("L", 1)`，`SerRead` 1 字节、超时 20ms；收到 `'A'` 成功；循环超过 10000 次失败；用户可停止。
3. 若配置波特率不是 115200：发送 `CMD_CHANGE_BAUD_REQUEST (0xB000)`，载荷 3 字节为波特率数值（见下表）；应答整包长度 `wRecvLen==8` 且 `abyRecvBuff[5]==0` 视为成功；然后关串口再以目标波特率重开。

**改波特率请求载荷（3 字节）**（`dwBaud = g_adwBauds[select]`）：

| 偏移 | 内容 |
|------|------|
| 0 | `(dwBaud/65536) % 256` |
| 1 | `(dwBaud%65536) / 256` |
| 2 | `dwBaud % 256` |

**USB（`UsbHandshack`，UI 已禁用但仍存在代码）**：`OpenUsbDevice()` 后同样发 `'L'`、收 `'A'`，超时逻辑类似。

### 2.3 DES 双向认证（`Authentication`）

认证密钥（源码常量，**非运行时协商**）：

```text
g_abyAuthenKey[8] = { 0x33, 0x00, 0x22, 0xF1, 0xC4, 0x3D, 0x25, 0x5E }
```

使用 `des(..., ENCRYPT)`。

| 步骤 | 命令 | 方向 | 数据载荷（不含帧头/CRC） | 说明 |
|------|------|------|--------------------------|------|
| 1 | `0xB100` `CMD_AUTHEN1_REQUEST` | PC→终端 | 8 字节：本地随机数 `LRand` | 最多重试 3 次 |
| 2 | `0xB101` `CMD_AUTHEN1_RESPONSE` | 终端→PC | 期望整包长度 **39**（即数据区 32 字节） | 见下表字段 |
| 3 | `0xB102` `CMD_AUTHEN2_REQUEST` | PC→终端 | 9 字节：见下 | 校验失败时也会发失败包 |
| 4 | `0xB103` `CMD_AUTHEN2_RESPONSE` | 终端→PC | 期望整包长度 **8**（数据区 1 字节状态） | `abyRecvBuff[5]==0` 成功 |

**AUTHEN1 应答数据区布局**（相对整包缓冲，数据从下标 5 开始；与代码访问一致）：

| 整包下标 | 长度 | 含义（由 PC 代码用法确定） |
|----------|------|----------------------------|
| 5..12 | 8 | 终端对 `LRand` 的 DES 密文；PC 用同一密钥加密 `LRand` 后 `memcmp` 比对 |
| 13..20 | 8 | 终端随机数 `RRand`；PC 对其 DES 加密后放入 AUTHEN2 |
| 21..36 | 16 | 机型字符串，拷贝到 `m_abyVposType` |

**AUTHEN2 请求数据区**：

| 偏移 | 长度 | 含义 |
|------|------|------|
| 0 | 1 | `0`=PC 侧校验 AUTHEN1 成功；`1`=失败（失败时直接返回，仍尝试发送） |
| 1..8 | 8 | `DES_Encrypt(RRand, key)`（成功路径） |

### 2.4 CRC16 帧格式

由 `Crc16SendPacket` / `Crc16RecvPacket` 实现。

**发送帧**

| 字段 | 长度 | 说明 |
|------|------|------|
| STX | 1 | 固定 `0x02` |
| CMD | 2 | 大端：`CMD/256`，`CMD%256` |
| LEN | 2 | 数据区长度，大端 |
| DATA | LEN | 载荷 |
| CRC16 | 2 | CRC16-CCITT，覆盖 **CMD+LEN+DATA**（从帧内下标 1 起共 `LEN+4` 字节） |

总长 = `LEN + 7`。数据区最大发送限制：代码中 `wDataLen > 8900` 则拒绝。

**接收**：同样以 `0x02` 同步；`*pwPacketetLen` 返回 **整包长度**（`LEN+7`）。校验失败返回 `CRC_CHECK_ERROR` 等错误码。

> 下文“整包长度”均指 `Crc16RecvPacket` 输出的 `wRecvLen`（含 STX/CMD/LEN/DATA/CRC）。  
> 应答状态字节一般在整包下标 **5**（即 DATA[0]）。

### 2.5 全部 CMD 操作码与载荷

定义见 `LoadToolsDefine.h`。下表“请求载荷 / 应答”以 **PC 工具实际收发** 为准；未在 `.cpp` 中调用的命令单独标注。

#### 2.5.1 链路与认证

| 请求 Opcode | 宏 | 请求载荷 | 应答 Opcode | PC 侧期望 |
|-------------|-----|----------|-------------|-----------|
| `0xB000` | `CMD_CHANGE_BAUD_REQUEST` | 3 字节波特率 | `0xB001` | 整包长 8，DATA[0]==0 |
| `0xB100` | `CMD_AUTHEN1_REQUEST` | 8 字节 LRand | `0xB101` | 整包长 39 |
| `0xB102` | `CMD_AUTHEN2_REQUEST` | 9 字节（状态+密文） | `0xB103` | 整包长 8，DATA[0]==0 |

#### 2.5.2 文件下载三步

| 请求 Opcode | 宏 | 请求载荷 | 应答 | 说明 |
|-------------|-----|----------|------|------|
| `0xB200` | `CMD_LOAD_FILE_REQUEST` | **38 字节**（见下） | `0xB201`；要求 DATA[0]==0 | 开始下载 |
| `0xB202` | `CMD_LOAD_FILE_DATA_REQUEST` | 见数据块格式 | `0xB203`；DATA[0]==0 | 循环发送，失败最多再试 3 次 |
| `0xB204` | `CMD_SAVE_FILE_REQUEST` | **空（LEN=0）** | `0xB205`；DATA[0]==0；超时 30s | 落盘；若类型为应用，DATA[1] 作为新应用号 `m_iCurrentAppNo` |

**`0xB200` 请求 38 字节布局**（先 `memset` 为 0）：

| 偏移 | 长度 | 内容 |
|------|------|------|
| 0 | 1 | 文件类型：`FILE_TYPE_APPLICATION=2` / `PARAM=3` / `DATA=4` / `FONT=5` |
| 1 | 1 | 固定写 `0` |
| 2..4 | 3 | 文件总长度（24 位，与改波特率相同的字节序） |
| 5 | 1 | 应用号：仅 **参数/数据文件** 填写；应用/字库为 0 |
| 6..21 | ≤16+NUL | 数据文件：源文件名（去路径）；参数文件：固定字符串 `"EnvFile"`；其它类型保持 0 |
| 22..37 | — | 保持 0 |

**`0xB202` 数据块载荷**：

| 偏移 | 内容 |
|------|------|
| 0 | 固定 `0` |
| 1..3 | 当前文件偏移（24 位，同上字节序） |
| 4 | `0`=未压缩；`1`=压缩（`Compress` 后长度更短才置 1） |
| 未压缩 | 从偏移 5 起为原始数据，`LEN = 读出长度 + 5` |
| 压缩 | 偏移 5..6 = 原始长度（16 位大端）；偏移 7 起为压缩数据；`LEN = 压缩长度 + 7` |

每块读取上限 `DOWNLOAD_STEP_LEN`（4096）。

#### 2.5.3 查询 / 删除 / 系统维护

| 请求 Opcode | 宏 | 请求载荷 | PC 侧应答处理 |
|-------------|-----|----------|---------------|
| `0xB300` | `CMD_GET_APP_LIST_REQUEST` | 空 | 整包长满足 `2==(wLen%6)` 的判断逻辑；应用个数 `iAppNum = wLen/6 - 1`；从下标 6 起每 6 字节一项：`AppNum(1)+AppStatus(1)+FileSize(4)` |
| `0xB302` | `CMD_GET_APP_APPEND_REQUEST` | 1 字节：应用号 | DATA[0]==0；附属项按 37 字节步进解析（类型在 `37*j+6`：`4` 视为数据文件，否则参数文件；随后 4 字节大小、文件名等） |
| `0xB304` | `CMD_GET_APP_INFO_REQUEST` | 1 字节：应用号 | 整包长 **264**；DATA[0]==0 时，其后 **256** 字节为 `APPLICATION_INFO` |
| `0xB306` | `CMD_GET_FONT_INFO_REQUEST` | （头文件定义） | **本工程 PageApplication 等通讯路径中未调用** |
| `0xB308` | `CMD_DEL_FILE_REQUEST` | **2 字节**：`[0]=应用号`，`[1]=0` 删应用 / `=1` 删附属文件（UI：删除应用按钮 / 删除附属文件按钮） | 等待应答成功即可（不严格解析状态） |
| `0xB30A` | `CMD_SET_TIME_REQUEST` | 发送长度 **7**；前 6 字节为本地时间 BCD：`YY MM DD HH MM SS`（年=`wYear-2000` 再转 BCD） | 应答成功即返回 |
| `0xB30C` | `CMD_CREATE_FILE_SYSTEM_REQUEST` | 空 | 超时可达 60s（普通下载）或 30s（打包任务）；要求 DATA[0]==0；成功后会 `DownloadEnd` 再重新握手认证 |
| `0xB30E` | `CMD_GET_SERIAL_NUMBER_REQUEST` | 空 | 整包长 **40**；DATA[0]==0 时 DATA[1..32] 为序列号 |
| `0xB310` | `CMD_SET_SERIAL_NUMBER_REQUEST` | **32** 字节序列号字符串 | DATA[0]==0 时，本地序列号字符串 `StringAddOne` 自增并回写界面 |
| `0xB312` | `CMD_DELETE_FILE_REQUEST` | **33** 字节：前 32 字节来自打包文件中的应用名/标识，最后一字节写 `0` | 打包下载里 `BALE_TASK_DEL_APP` 使用；与 `0xB308` 不同 |
| `0xB400` | `CMD_FINSH_ALL` | 空 | **不读应答**；`DownloadEnd` 发送后 `Sleep(50)` 并 `SerClose()` |

**`APPLICATION_INFO`（256 字节）结构**（`LoadToolsDefine.h`）：

| 字段 | 长度 |
|------|------|
| AppName | 32 |
| AID | 16 |
| AppVer | 16 |
| AppProvider | 32 |
| Descript | 64 |
| DownloadTime | 14 |
| MainAddress | 4 |
| EventAddress | 4 |
| AppNum | 1 |
| RFU | 73 |

#### 2.5.4 文件类型常量

| 值 | 宏 | 备注 |
|----|-----|------|
| 0 | `FILE_TYPE_BOOT` | 固件页使用 |
| 1 | `FILE_TYPE_VOS` | 固件页使用 |
| 2 | `FILE_TYPE_APPLICATION` | 应用管理下载 |
| 3 | `FILE_TYPE_PARAM` | ENV 参数，大小须为 128 倍数且 ≤128KB |
| 4 | `FILE_TYPE_DATA` | ≤1560KB |
| 5 | `FILE_TYPE_FONT` | 头须为 `"ViewAt Font"` 或 `"VIEWAT_COMPRESS_FONT"` |

### 2.6 下载顺序（单文件）

```text
握手 L/A →（可选改波特率）→ DES 认证
  → CMD_LOAD_FILE_REQUEST (0xB200, 38B)
  → 循环 CMD_LOAD_FILE_DATA_REQUEST (0xB202, 每块≤4096)
  → CMD_SAVE_FILE_REQUEST (0xB204)
  → …其它任务…
  → CMD_FINSH_ALL (0xB400) → 关串口
```

`DownloadProcess` 在非打包模式下大致顺序：

1. 可选：`SET_TIME`
2. 若勾选打包文件：走 `DownloadBaleFile()`（见下）并返回
3. 可选：重建文件系统 → 结束会话并重新握手认证
4. 可选：写序列号
5. 可选：字库 → 应用 →（若未下应用则需树中选中应用）参数 → 数据文件

### 2.7 Reflush（刷新）/ Delete / Bale 说明

**Reflush（刷新）**

- 按钮变量名 `m_btnReflush`，处理函数 `OnButtonRefurbish` → 线程 `ThreadRefurbish`。
- 作用：握手+认证后调用 `UpdateTreeList()`：读序列号、应用列表、每应用信息与附属文件，刷新左侧树；最后 `0xB400`。
- **不下载文件**。

**Delete**

| UI 操作 | 命令 | 载荷 |
|---------|------|------|
| 删除应用 | `0xB308` | `[AppNum, 0]` |
| 删除附属文件 | `0xB308` | `[AppNum, 1]`（代码用选中应用的 `m_sAppInfo.m_byAppNum`） |
| 打包任务删应用 | `0xB312` | 33 字节（32 字节标识 + 0） |

**Bale（打包文件）**

- 文件头 ASCII：`"ViewAt BaleFile"`（16 字节比较用 `strcmp`）。
- 文件末尾 1 字节为任务数 `byTaskNum`；再向前 `byTaskNum*4` 字节为各任务偏移表（每项 4 字节大端偏移）。
- `DownloadBaleFile` 按任务类型处理：

| 任务值 | 宏 | PC 行为 |
|--------|-----|---------|
| `0x8F` | `BALE_TASK_CREATE_FILE` | 再读 8 字节，若为 `"81921170"` 则发 `0xB30C`，然后结束并重握手 |
| `0x81` | `BALE_TASK_DEL_APP` | 若本包尚未因建文件系统而跳过，则读 32 字节名，发 `0xB312`（33 字节） |
| `0x01` | `BALE_TASK_APPLICATION` | `ParseBaleFile` → 走与单文件相同的 `0xB200/0xB202/0xB204` |
| `0x02` | `BALE_TASK_PARAM` | 同上，类型 PARAM |
| `0x07` | `BALE_TASK_DATA` | 同上，类型 DATA |
| `0x04` | `BALE_TASK_FONT` | 同上，类型 FONT |

- 头文件另有 `BALE_TASK_DEL_ALL_APP (0x82)` 等，用于**制作**打包文件（`PageFileBale.cpp`）；**`DownloadBaleFile` 的 switch 未处理 `0x82`**。
- UI 约束：勾选 Bale 时，不能同时勾选单独下载应用/字库/参数/数据或重建文件系统（会弹框取消勾选）。

### 2.8 结束命令 `0xB400`

- 宏名拼写为 `CMD_FINSH_ALL`（源码如此，少一个 I）。
- 仅发送，不等待响应；之后关闭串口（`DownloadEnd`）。

---

## 3. 固件下载协议（Firmware）

### 3.1 握手（T / R|S）

`CPageFirmware::Handshack`：

1. `SerOpen(port+1, 115200)`
2. 循环发送 `'T'`，读 1 字节（超时 20ms）  
   - 收到 **`'R'`**：`g_iCommCheckType = LRC_CHECK_TYPE (0)`  
   - 收到 **`'S'`**：`g_iCommCheckType = CRC32_CHECK_TYPE (2)`  
3. **无认证、无改波特率**。

后续收发统一走 `CmdSendPacket` / `CmdRecvPacket`，按 `g_iCommCheckType` 选择 LRC 或 CRC32。

### 3.2 LRC / CRC32 帧格式

两种帧的公共头（与 CRC16 应用帧不同：命令为 **1 字节**）：

| 字段 | 长度 | 说明 |
|------|------|------|
| STX | 1 | `0x02` |
| CMD | 1 | 单字节命令 |
| LEN | 2 | 数据长度，大端 |
| DATA | LEN | 载荷 |

**LRC（`LrcSendPacket`）**

- LRC：对下标 1..(LEN+3) 各字节异或，结果放在 DATA 后 1 字节。  
- 总长 = `LEN + 5`。  
- 接收时 `*pwPacketetLen = LEN + 5`。  
- 载荷最大 4096。

**CRC32（`Crc32SendPacket`）**

- CRC32：`QuickCrc32`，初值/终值异或 `0xFFFFFFFF`，查表；覆盖 **CMD+LEN+DATA**（`LEN+3` 字节），输出 4 字节大端追加在 DATA 后。  
- 总长 = `LEN + 8`。  
- 接收时 `*pwPacketetLen = LEN + 8`。

> 固件应答状态字节在整包下标 **4**（DATA[0]），因 CMD 仅 1 字节。这与应用管理 CRC16 帧状态在下标 5 不同。

### 3.3 命令 `0xC0` / `0xC1` / `0xB0` / `0xB1` / `0xD2`

| Opcode | 方向 | 含义（由 PC 代码） | 载荷 |
|--------|------|-------------------|------|
| `0xC0` | PC→终端 | Boot 数据块 | 见下 |
| `0xC1` | PC→终端 | Boot 保存（代码中对 `byCmd++`，由 `0xC0` 得到） | **空**；随后循环收进度包，超时 30s |
| `0xB0` | PC→终端 | VOS 数据块 | 同 Boot 数据块格式 |
| `0xB1` | PC→终端 | VOS 保存（`0xB0` 自增） | **空**；同上进度循环 |
| `0xD2` | PC→终端 | 全部结束（`DownloadEnd`） | 空；`Sleep(50)` 后 `SerClose()`，**不读应答** |

**数据块载荷**（`DOWNLOAD_LEN_FIRM = 3064`）：

| 偏移 | 内容 |
|------|------|
| 0 | `0` |
| 1..3 | 文件偏移（24 位，字节序同应用协议） |
| 4 | `0` 未压缩 / `1` 压缩 |
| 5.. | 数据；若压缩则整段替换为压缩结果，`LEN = 压缩长度 + 5`（**无**应用协议里那 2 字节原始长度字段） |

发送后 `CmdRecvPacket`，要求返回成功且 **`abyRecv[4]==0`**；失败最多再试 8 次。

**保存阶段进度包**（DATA[0]=`abyTemp[4]`，进度=`abyTemp[5]`）：

| DATA[0] | 含义（界面文案） |
|---------|------------------|
| 0 | Erase flash… 百分比；>100 视为擦除故障 |
| 1 | Write flash…；`==100` 下载成功返回 |
| 3 | Check…；`==100` 校验成功 |
| 其它 | 失败返回 |

### 3.4 文件校验（下载前本地检查）

| 类型 | 命令起点 | 大小约束 | 其它 |
|------|----------|----------|------|
| Boot | `0xC0` | 4096 ≤ size ≤ 64KB | — |
| VOS | `0xB0` | 50KB ≤ size ≤ 960KB | 文件尾需匹配 `"VPP202-VOS"`（末 10 字节）或 `"VOS306"`/`"VOS307"`（末 36 字节处起 6 字节） |

### 3.5 流程小结

```text
打开串口 115200 → 发 'T'，收 'R'(LRC) 或 'S'(CRC32)
  → 可选下载 Boot：循环 0xC0 块 → 0xC1 保存/擦写进度
  → 可选下载 VOS：循环 0xB0 块 → 0xB1 保存/擦写进度
  → 发 0xD2 → 关串口
```

---

## 4. 对比表

| 项目 | 应用管理协议 | 固件协议 |
|------|--------------|----------|
| 源码入口 | `PageApplication.cpp` | `PageFirmware.cpp` |
| 握手 | `'L'` → `'A'` | `'T'` → `'R'` 或 `'S'` |
| 校验 | CRC16-CCITT（2 字节） | LRC（1）或 CRC32（4） |
| 命令字段宽度 | **16 位**（如 `0xB200`） | **8 位**（如 `0xC0`） |
| 帧头 | `02 \| CMD16 \| LEN16 \| DATA \| CRC16` | `02 \| CMD8 \| LEN16 \| DATA \| LRC/CRC32` |
| 认证 | DES 双向（`0xB100`~`0xB103`） | 无 |
| 波特率 | 握手 115200，可 `0xB000` 切换 | 固定 115200 |
| 数据块大小 | 4096 | 3064 |
| 压缩标志后是否带原长 | 压缩时带 2 字节原长 | 不带 |
| 结束命令 | `0xB400`（宏 `CMD_FINSH_ALL`） | `0xD2` |
| USB | 代码有路径，UI 禁用 | 无 |
| 重试 | 通讯类多用 `COMM_RETRY_TIMES=3`；数据块最多 3 次 | 数据块最多 8 次 |

---

## 5. 关键源文件

| 文件 | 作用 |
|------|------|
| `LoadToolsDefine.h` | CMD 操作码、文件/打包任务类型、`DOWNLOAD_*` 长度、`APPLICATION_INFO`、重试次数等 |
| `Communication.h` / `Communication.cpp` | 串口/USB 底层；`Crc16*` / `Lrc*` / `Crc32*` 组帧；`CmdSendPacket`/`CmdRecvPacket` 分发 |
| `PageApplication.cpp` | 应用管理：L/A 握手、改波特率、DES 认证、全部 `0xBxxx` 业务、打包下载、刷新/删除、`0xB400` |
| `PageFirmware.cpp` | 固件：T/R\|S 握手、`0xC0/0xC1/0xB0/0xB1/0xD2`、3064 分块 |
| `PageCommSet.cpp` | 串口/USB 选择与波特率；**USB 控件 `EnableWindow(FALSE)`** |
| `PageFileBale.cpp` | 打包文件制作（含 `BALE_TASK_DEL_ALL_APP` 等任务写入；下载解析见 `PageApplication::DownloadBaleFile`） |

---

## 6. 维护提示（基于代码事实）

1. 改应用管理协议时，应答长度判断多用 **整包长度**（含 7 字节头尾），不要误当成纯 DATA 长度。  
2. 固件与应用的“状态字节下标”不同（4 vs 5），混用组帧函数会错位。  
3. `CMD_GET_FONT_INFO_* (0xB306/0xB307)` 仅有宏定义，当前工具侧未发送。  
4. 结束命令宏名为 `CMD_FINSH_ALL`，与英文 finish 拼写不一致，搜索时注意。  
5. USB 传输与 GUID/管道代码仍在 `Communication.cpp`，但通讯设置页已禁止选择 USB。

---

*文档生成依据源码版本：LoadTools[v3.1]_En（文件时间戳约 2008–2010）。*
