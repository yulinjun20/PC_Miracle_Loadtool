# 设备端下载协议实施说明（v1）

> **对应协议**：[`DownloadProtocol_v1.md`](./DownloadProtocol_v1.md)  
> **关联决策**：协议 **D23 / D42～D70**（OS 过渡仍旧协议；设备未对接 `0xB200`；非 OS 新建 `0xB210+`；宏 ON 时 Font/Logo 走新协议；SC/WIFI 先收文件再安装；**多文件先收齐再逐个安装、OS 传完不立即重启（D47）**；**各类型整次任务结束后生效（D49）**；**临时文件：成功删 / 失败立即删（D48，不保留待下次）**；**工厂每台收完就装（D50）**；**安装顺序 D53**；**Bale 仅 PC 解包（D54）**；**Bale 缺项跳过（D56）**；**SN≤32（D57）**；**残留临时下次进菜单清（D62）**；链路 **USB VCP**）  
> **设备 SDK 参考路径**：`E:\MyProject\UN650\ML160T\UN650-ML160T_SDK\haoming\master\UN650-SDK_RTOS\UN650-ML160T-SDK_RTOS`  
> **状态**：文档草案；**本文不改代码**，仅说明如何在设备端落地。

---

## 1. 目的

说明设备端如何用宏区分「原有 `UpdateFirmwareDemo` 路径」与「新下载协议路径」，并在开启新协议时仍保证 **OS 可更新**（过渡期走旧链路）。

与 PC 侧 `DownloadProtocol_v1.md` 对齐：统一 Start/Data/End 会话负责非 OS 文件；OS 在设备未实现统一会话前继续旧协议。

**物理链路**：与旧通讯下载相同，走 **USB VCP**（虚拟串口）。新/旧协议会话都经同一 VCP 通道；不另开 USB 自定义传输。

---

## 2. 宏开关：`USE_NEW_DOWNLOAD_PROTOCOL`

建议在设备工程公共头（例如 `firmware_config.h` 或等价配置头）增加：

```c
/* 0 = 保持原有 UpdateFirmwareDemo / 下载行为
 * 1 = 启用新下载协议路径（非 OS）；OS 仍走旧协议（见 §3）
 */
#ifndef USE_NEW_DOWNLOAD_PROTOCOL
#define USE_NEW_DOWNLOAD_PROTOCOL  0
#endif
```

### 2.1 宏 OFF（`USE_NEW_DOWNLOAD_PROTOCOL == 0`）

- 保持 **原有** `UpdateFirmwareDemo` 及现有下载/安装行为不变。
- 菜单项、`DownloadAppFromPC` / `UsbDownloadApp`、Boot LOGO、Font、公钥、Install、Delete、Reset SC 等逻辑不因本文而改。

### 2.2 宏 ON（`USE_NEW_DOWNLOAD_PROTOCOL == 1`）

| 类型 | 握手 / 命令 | 说明 |
|------|-------------|------|
| **非 OS**（App / Font / Param / Data / SC App / WIFI / Logo 等） | 新握手 `'L'`/`'A'`（可选 SN）+ Start/Data/End（`0xB210` / `0xB212` / `0xB214`，Abort `0xB216`） | 新 `UpdateFirmwareDemo`（或旁路会话）按 `file_type` 落盘/安装 |
| **OS** | **仍用旧协议**：握手 `'T'`/`'R'`/`'S'`（及现有 `'Q'`/`'P'` 等）+ `0xB0`/`0xB1` | **与协议 D23/D46 一致**；不因宏开启而改走 `0xB210+` |

要点：

- 宏打开 **不等于** OS 立刻切到新协议；OS 迁入统一会话是后续阶段（设备实现并验证后再做）。
- 新旧路径 **不得抢同一串口/USB 状态机**：同一时刻只处于一种会话；更新菜单用握手字节区分旧 OS 流 vs 新文件流（协议 D25）。

---

## 3. 与协议 D23 的对应关系

摘自 `DownloadProtocol_v1.md` **D23**：

> 设备未支持统一会话前，**仅 OS** 可继续使用旧路径：`'T'`/`'R'`/`'S'` + `0xB0`/`0xB1`；开发者与工厂均适用；同一任务可混跑「旧协议 OS + 新协议其它文件」。

本实施说明要求：

1. `USE_NEW_DOWNLOAD_PROTOCOL=1` 时，设备对 **OS** 仍应答旧握手与 `0xB0`/`0xB1`（**D46**，同协议 D23）。  
2. 其它 `file_type`（含 **Font / Logo，D44**）走新会话；**不要**再用旧 `0xB0` 下 Font/Logo。  
3. **SC App / WIFI（D45）**：新会话只收文件，再调现有安装/烧录。  
4. **设备从未对接 `0xB200`（D42）**：非 OS 按 `0xB210+` **新建**；PC 同步废除 `0xB200`（D43）。  
5. **多文件先收后装（D47） + 生效时机（D49） + 临时文件（D48） + 工厂独立安装（D50）**：同一任务勾选多项时，**OS 传完后不要立即重启**；继续接收后续文件，**全部收齐后再按 D53 顺序逐个安装**（OS → Font → Logo → Param → Data → SC App → WIFI → App；无 Bale）（App/Font/Param/Data/Logo 落盘或安装；SC/WIFI 收完再调现有刷写）。**各类型均仅在整次任务结束后生效（D49）**。仅勾选 OS 一项时（开发者与工厂）：收完即装并重启。收文件阶段 End.`reboot`=0。链路 **USB VCP**。**整次任务成功**：删除本任务临时下载文件（D48）。**下载过程中失败**：设备**立即删除当前临时下载文件**；**不**保留待下次（D48）。**工厂一拖多**：每台**自己收完就自己装**（每台收完即装），**互不等待**其它台（D50；PC 编排，设备仍是单台会话）。  
6. 稳定并验证后，再把 OS 迁到 `file_type=1` 的 Start/Data/End，并计划废弃旧 OS 路径。

---

## 4. 当前设备端现状（只读调研摘要）

### 4.1 入口与菜单

| 文件 | 作用 |
|------|------|
| `device/OS/factory_test/Factory_Demo.c` | `UpdateFirmwareDemo()`：更新菜单；入口调用 `DownloadAppFromPC()` 等 |
| `device/OS/factory_test/Factory_Demo.h` | `UpdateFirmwareDemo` 声明 |
| `device/OS/factory_test/factory_test.c` | 工厂/测试菜单中调用 `UpdateFirmwareDemo()` |

现有菜单能力（节选）：从 PC 下载、Boot LOGO、Font、公钥、Install App/Firmware、删文件、Reset SC。

### 4.2 旧 OS / 文件 USB 下载（过渡期应保留）

| 文件 | 作用 |
|------|------|
| `services/un650_lib/src/usb/usb_download.c` | USB 下载实现（与 PC 旧 OS 链路对接的核心） |
| `services/un650_lib/src/usb/usb_firmware.c` | 下载后固件分类 / 提交类型等 |
| `services/un650_lib/src/usb/usb.c` / `usb.h` / `usb_fs.h` | USB 传输与文件系统辅助 |
| `services/un650_lib/inc/firmware_config.h` | 固件相关宏与配置 |

`DownloadAppFromPC()` → `UsbDownloadApp()`；Boot LOGO 注释写明与 **T/S、Q/P、0xB0/0xB1** 同类握手。**开启新宏后，OS 仍应走此旧链路。**

### 4.3 SC / WIFI 安装（文件到达后的二次安装）

| 文件 | 作用 |
|------|------|
| `services/un650_lib/src/mh2101_upgrade/MH2101_Upgrade.*`、`mh2101_recovery.*` | 安全芯片（SC）升级 |
| `services/un650_lib/src/esp-serial-flasher/esp_flashing_api.*`、`mh2101_esp_control.*` | ESP / WIFI 固件烧录 |

**已确认（D45）**：新协议只负责把 SC App / WIFI 文件收到设备；安装仍复用上述现有模块（`file_type` 分支到对应安装 API）。

### 4.4 已确认（D42）：设备从未对接 `0xB200`，须按新协议新建

**已核对确认（协议 D42）**：设备端 **从未对接** PC 旧 App/Font/Param/Data 的 **`0xB200` / `0xB202` / `0xB204`** 管理/下载协议。本 SDK 源码树中亦无对应业务实现。

**实施要求**：

- **不要**补齐旧 `0xB200` 族。  
- 非 OS 一律按协议 **新建** `'L'`/`'A'` + Start/Data/End（`0xB210+`）（协议 **D43**：PC 侧同步废除 `0xB200`，只发新会话）。  
- `Factory_Demo.c` 等处的旧头文件引用若存在，不视为已具备 `0xB200` 能力。

---

## 5. 推荐分期实施计划

### 阶段 A — 宏与双路径骨架（不破坏现网 OS）

1. 增加 `USE_NEW_DOWNLOAD_PROTOCOL`，默认 **0**。  
2. 宏为 0：行为与现网完全一致。  
3. 宏为 1：  
   - 保留现有 OS / `UsbDownloadApp`（或等价）旧路径；  
   - **旁路**增加新会话状态机（握手 `'L'`/`'A'` → Start/Data/End）；  
   - 更新菜单：收到 `'T'`… 走旧 OS；收到 `'L'`… 走进新会话并读 `file_type`。

### 阶段 B — 非 OS 类型落盘

按协议类型表实现（name 以类型为准，Data 建议带本机名）：

| file_type | 名称 | 设备侧建议 |
|-----------|------|------------|
| 2 | App | 落盘后走现有 Install / 自动安装策略 |
| 3 | Param | 按类型落盘（不必定死 `EnvFile` 文件名，以协议为准） |
| 4 | Data | 按本机文件名；同名覆盖（**安装阶段**成功后生效，D10/D49） |
| 5 | Font | **宏 ON 走新协议（D44）**；落盘 / 字体库安装；不再用旧 `0xB0` 下 Font |
| 6 | SC App | **新协议只收文件（D45）** → 再调现有 MH2101 升级流程 |
| 7 | WIFI | **新协议只收文件（D45）** → 再调现有 ESP flasher 流程 |
| 8 | Logo | **宏 ON 走新协议（D44）**；落盘 / 显示预览（可对照现有 Boot LOGO 流程，但走新会话；不用旧 `0xB0`） |

> **无 Bale 类型（D54）**：类型表无 Bale。Bale 仅由 PC 解包；设备只收到解包后的子项 `file_type`。

v1 规则（与协议一致）：整文件 CRC32；默认 chunk 4096（设备可在 Start 应答下调）；强制 chunk CRC；无压缩、无断点；认证**永远关闭、无 UI 开关**（D5）；重启以 End 为准。

**多文件安装节奏（D47）+ 安装顺序（D53）+ Bale（D54）**：

```text
接收阶段（保持 USB VCP 在线、不重启）：
  按任务勾选项分别接收（OS=旧协议；其它=新会话）。
  若勾选 Bale：仅 PC 解包后按子项接收——设备无 Bale 类型、无 Bale 整包。

安装阶段（该台全部接收并校验成功之后；工厂每台自装 D50）：
  OS → Font → Logo → Param → Data → SC App → WIFI → App
  （只装本任务实际收到的类型；缺项跳过；顺序中无 Bale）
  → 需要时再 reboot（可一项一重启，或若干项后重启）
```

**仅勾选 OS 一项**（开发者与工厂，D47）：该台收完即安装并重启。

过渡期旧 OS 路径若 API 在收完后默认重启：宏 ON 且任务还有后续文件时，须 **抑制立即重启**（例如缓冲到临时区 / 延迟 commit），等接收阶段结束后再安装 OS 并重启。

**生效（D49）**：App/Param/Data/Font/SC/WIFI/Logo（及多文件 OS）在安装阶段完成前**不对运行中系统生效**。
**临时文件（D48）**：整次任务**成功**后删除临时下载文件；下载过程中**失败**则**立即删除当前临时下载文件**；**不**保留待下次。

### 阶段 C — 握手带可选 SN

工厂模式需要：`'L'`/`'A'` 成功应答中 **可选 SN**（无 SN 则长度为 0）。设备从 `DLL_SysReadSN`（或现有 SN API）填充；与 PC 工厂状态列表（COM + SN + 进度 + 状态）对齐。PC 侧进度为**该台整次任务总进度**并显示当前文件名（D51）；UI 文案英文（D52）。设备只要保证单台会话正确；**何时开始安装**由 PC 在「该台收齐」后驱动，设备不跨台同步（D50）。

### 阶段 D — OS 迁入统一会话（宏仍为 ON 之后的后续工作）

1. 设备对 `file_type=1`（OS）实现 Start/Data/End。  
2. 与 PC 约定切换窗口后，PC 才把 OS 从旧路径改为 `0xB210+`。  
3. 验证通过后废弃设备旧 OS 下载路径；宏可简化或删除「OS 例外」分支。

**在阶段 D 完成前**：即使用户开启 `USE_NEW_DOWNLOAD_PROTOCOL`，**OS 也必须继续旧协议**（本文 §2.2 / 协议 D23）。

---

## 6. 更新菜单识别逻辑（建议）

```
等待握手字节
  ├─ 旧 OS 流（'T' / 'R' / 'S' 等） → 0xB0/0xB1 接收 → OS 升级（过渡期，宏 ON/OFF 均保留此能力）
  └─ 新流（'L' / 'A'）
        └─ 仅当 USE_NEW_DOWNLOAD_PROTOCOL==1
              → Start(file_type, …) → Data… → End
              → 按 file_type 分支落盘/安装
```

宏为 0 时：可不实现 `'L'`/`'A'` 新会话，或直接拒绝，避免半成品协议干扰产线。

多文件（D47）：菜单/下载状态机应区分 **接收中** 与 **安装中**；接收中禁止因 OS 收完而复位 USB VCP。

---

## 7. 实施注意点

1. **不要**在宏 ON 时删掉旧 OS 代码路径。  
2. **不要**让新会话与 `UsbDownloadApp` 同时占用同一端口接收循环。  
3. SC/WIFI：先收文件，再调现有升级 API；失败码映射到协议 End/Abort 语义。  
4. Finish：`0xB400` 使用正确拼写 Finish（协议已定；不再兼容错误 `FINSH`）。  
5. 工厂一拖多由 **PC 多会话**完成；设备仍是单台、单会话，无需设备侧「一拖多」状态机。  
6. **D42 已确认**：设备从未对接 `0xB200`；实施时 **新建** `0xB210+`，不补齐旧协议。PC 非 OS 同步废除 `0xB200`（D43）。  
7. **D47/D48/D49/D50/D53/D54**：安装顺序 **OS → Font → Logo → Param → Data → SC App → WIFI → App**（D53）；Bale 仅 PC 解包、设备无该类型（D54）。Bale 缺项跳过（D56）；SN≤32（D57）；残留临时下次进更新菜单清（D62）。其余同前——多文件任务 OS 传完不重启；先收齐再逐个安装；各类型整次任务结束后生效（D49）；**成功删临时 / 失败立即删当前临时（D48，不保留待下次）**；工厂多台时每台收完就装、互不等待（D50，PC 编排）；链路 **USB VCP**。  
8. 收文件阶段对 PC 的 End（及旧 OS 收完应答）应表明 **不立即 reboot**，避免 PC 误等重连而中断后续传输。  
9. **认证（D5）**：**永远关闭**；设备不要求 DES；PC 不提供开关。  
10. **Sync time（D34/D55）**：校时命令 `0xB30A` 仍由设备按旧控制命令处理即可。D55 为 **LoadTool UI** 约束（开发者独立选项、与下载勾选互斥）；设备端**无**新线协议、无互斥状态机要求。  
11. **D66/D67（PC）**：Param JSON / OS 大小校验失败则提示且不开始——设备无感。  
12. **D68/D69**：传输或安装单项失败时 PC 继续下一项；设备按单会话失败处理（删当前临时文件 D48），准备接受下一会话/下一安装。

---

## 8. 验收建议（设备侧）

| 项 | 宏 | 期望 |
|----|----|------|
| 回归 OS 升级 | 0 与 1 | PC 旧 OS 流程均可刷入 |
| 新协议 App（或 Data） | 1 | `'L'`/`'A'` + Start/Data/End 成功落盘 |
| 宏关闭时发新握手 | 0 | 不误入半实现；行为与现网一致 |
| SC / WIFI | 1 | 文件收完后安装成功（若硬件可用） |
| 可选 SN | 1 | 有 SN 则握手带回；无则长度 0 |
| 多文件先收后装（D47） | 1 | OS+其它文件同任务：OS 传完 VCP 不断、不重启；全部收齐后再按 D53 顺序逐个安装 |
| 生效时机（D49） | 1 | App/Param/Data/Font/SC/WIFI/Logo（及多文件 OS）仅整次任务结束后生效 |
| 临时文件清理（D48） | 1 | 成功删临时；失败立即删当前临时（不保留待下次） |
| 工厂独立安装（D50） | 1 | 多台并行时，单台收齐后即可进入该台安装，不等其它台（由 PC 验证编排；设备单会话正确即可） |
| 安装顺序（D53） | 1 | 安装阶段顺序为 OS→Font→Logo→Param→Data→SC App→WIFI→App；无 Bale 步 |
| 残留临时（D62） | 1 | 异常中断后下次进入更新菜单清理残留临时文件 |
| Bale（D54） | 1 | 设备从不收到 file_type=Bale；仅收到解包子项 |
| 打开口先于握手（D64） | — | 主要由 PC 保证；设备只在口已打开后才可能收到握手；PC：开发者开口失败提示停止，工厂静默跳过 |
| 开发者握手重试（D65） | — | **PC 侧**：紧循环约 10000×20ms（沿用现网）；非 3×2s；设备无额外状态 |
| 认证（D5） | 0 与 1 | **永不认证**；设备不要求 DES；PC 无认证开关 |
| 传输/安装单项失败（D68/D69） | 1 | 单项失败不阻断后续项；PC 记录并继续 |
| 无日志文件（D70） | — | 主要为 PC：不落盘日志，仅 UI |
| USB VCP | 0 与 1 | 与旧下载同一虚拟串口通道 |

---

## 9. 文档关系

| 文档 | 内容 |
|------|------|
| [`DownloadProtocol_v1.md`](./DownloadProtocol_v1.md) | PC↔设备线协议、D1～D70；D23/D46 OS 过渡；D42～D54 设备落地；**D47 多文件+USB VCP；D48 成功删/失败立即删临时；D49 任务结束后生效；D50 工厂每台收完就装；D51/D52 进度与英文 UI（PC）；D53 安装顺序；D54 Bale 仅 PC 解包；D55 Sync time 与下载互斥（PC UI）；**D56 Bale 缺项跳过；D57 SN≤32；D58 等待 20s/Stop；D59 Slots full+活动路数（PC）；D60/D61 Start 前校验（PC）；D62 残留临时下次进菜单清；D63 握手失败 2s 再试（PC）；**D64 先打开口再握手（PC，开发者提示停止 / 工厂静默跳过）**；**D65 开发者握手紧循环约 10000×20ms（PC，非 3×2s）**；**D5 认证永远关无 UI**；**D66/D67 Param JSON / OS 大小失败则提示不开始（PC）**；**D68 传输单项失败记录并继续 / D69 安装单项失败记录并继续**；**D70 无日志文件仅 UI** |
| **本文** `DeviceDownloadImpl_v1.md` | 设备端宏切换、分期、关键文件；D42～D54 落地约束；D55/D56/D59/D60/D61/D63/D64/D65 等 PC UI 交叉引用；**D57/D62 设备侧落地** |

---

## 10. 修订记录

| 版本 | 日期 | 说明 |
|------|------|------|
| v1 Draft | 2026-10-15 | 初稿：`USE_NEW_DOWNLOAD_PROTOCOL`；宏 ON 时非 OS 新协议、OS 仍旧协议（D23）；分期计划；关键 SDK 文件；`0xB200` 缺失风险 |
| v1 Draft | 2026-10-15 | **核对写入（D42～D46）**：确认设备从未对接 `0xB200`、须新建 `0xB210+`；PC 非 OS 废除 `0xB200`；宏 ON 时 Font/Logo 走新协议；SC/WIFI 只收文件再现有安装；宏 ON 时 OS 仍旧协议 |
| v1 Draft | 2026-10-15 | **D47 + USB VCP**：多文件时 OS 传完不立即重启，全部接收后再逐个安装/按需重启；物理链路与旧下载同为 USB VCP；收文件阶段 End.`reboot`=0。同步协议 D9/D47 |
| v1 Draft | 2026-10-15 | **D48/D49 纠正**：失败时**立即删除当前临时下载文件**（不保留待下次）；整次任务成功后删除临时文件（D48）。App/Param/Data/SC/WIFI/Logo **整次任务结束后**再安装/生效（D49）。同步协议文档 |
| v1 Draft | 2026-10-15 | **D50/D51/D52**：工厂每台收完就装、互不等待（D50）；进度语义与英文 UI 以协议为准（D51/D52，主要约束 PC）；重申 D48 成功删/失败立即删临时。同步协议文档 |
| v1 Draft | 2026-10-15 | **D53/D54**：安装顺序 OS→Font→Logo→Param→Data→SC App→WIFI→App；Bale 仅 PC 解包、设备无 Bale 类型/无安装步；仅勾 OS 时开发者/工厂收完即装并重启。同步协议文档 |
| v1 Draft | 2026-10-15 | **D55**：协议增加 Sync time 与下载勾选互斥（LoadTool/开发者）；设备侧仍只处理 `0xB30A`，无新线协议。同步 `DownloadProtocol_v1.md` |
| v1 Draft | 2026-10-15 | **D64**：同步协议——LoadTool **先成功打开 COM/USB-VCP 再发握手**；打开失败不握手。设备侧无额外状态机，仅交叉引用。同步 `DownloadProtocol_v1.md`。 |
| v1 Draft | 2026-10-15 | **D56～D63**：同步协议——Bale 缺项跳过（D56）；SN≤32（D57）；OS 等待 20s/Stop（D58，主要 PC）；工厂槽位提示（D59，PC）；Start 前校验（D60/D61，PC）；**残留临时下次进更新菜单清理（D62）**；握手失败 2s 再试（D63，PC）。同步 `DownloadProtocol_v1.md`。 |
| v1 Draft | 2026-10-15 | **D64 补强 + D65**：同步协议——开口失败：开发者**提示并停止**，工厂**静默跳过**；开发者握手沿用紧循环约 **10000×20ms**（非 3×2s）。主要为 PC 行为；设备交叉引用。同步 `DownloadProtocol_v1.md`。 |
| v1 Draft | 2026-10-15 | **D5 修订 + D66～D69**：同步协议——认证**永远关、无 UI**（开发者/工厂）；Param JSON / OS 大小失败提示不开始（PC，D66/D67）；传输/安装单项失败记录并继续（D68/D69）；重申 D64/D65。同步 `DownloadProtocol_v1.md`。 |
| v1 Draft | 2026-10-15 | **D70 + 本轮对齐收口**：同步协议——**不写下载日志文件**，仅 UI（D70）；**本轮需求核对已对齐（D1～D70）**（D5 认证永远关；D66～D69 校验/Continue-on-fail）。同步 `DownloadProtocol_v1.md`。 |
