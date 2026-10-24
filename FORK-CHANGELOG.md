# Fork 變更日誌

本檔記錄本 fork（Wujidadi/weasel）相對上游 rime/weasel 的所有變動，依自建版版號分節。
上游自身的變更見 `CHANGELOG.md`；分支與版號規範見 `FORK-POLICY.md`。

## 0.17.4-wujidadi.7 — 2026-10-24

### 上游同步

- 合併上游 master 至 `82b50c9`（含候選視窗改以 DirectComposition 重寫、設定與部署對話框 DPI 感知、字型設定對話框、WeaselDeployer 命令列字典管理、診斷日誌改由登錄等級控管），均屬上游內容、不另記於本檔
- 衝突解法：
  - `WeaselServerApp.cpp` 取上游的啟動日誌，不採上游的 `SetTrayRefreshCallback`，托盤刷新維持 fork 的專用執行緒機制（`c7e6acb`）
  - `PipeChannel.h` 寫入失敗重連後的重試保留 fork 版：解參照新管線控制代碼並沿用逾時參數

### librime 更新

- librime 子模組 pin 自 `37e47f86` 更新至 `fa2dadb6`（librime fork `1.17.0-wujidadi.2`，基於上游 `1b3417d2`），重編 x64／Win32 產物接入：
  上游帶進 C++20 支援、依執行期條件啟用元件與 `streaming_chord` 修正；fork 的 macOS 部署目標調整不影響 Windows；
  `fa2dadb6` 修正合併後 `UserDbValue` 重複 `RIME_DLL` 標記造成的 MSVC C2487 編譯失敗

### 版號

- fork 版號後綴進位 `-wujidadi.7`
- appcast 條目更新至 `0.17.4-wujidadi.7`

## 0.17.4-wujidadi.6 — 2026-09-23

### librime 更新

- librime 子模組 pin 自 `c7d525ed` 更新至 `37e47f86`（librime fork `1.17.0-wujidadi.1`，基於上游 `ef1a16aa`），重編 x64／Win32 產物接入：
  - userdb 詞條老化丟棄門檻改為設定項 `<ns>/user_dict_forget_threshold`（double，與 `enable_user_dict` 同層）；
    上游 `de21e7d4` 的常量門檻 `1e-200` 會把落後庫級 tick 約 92103 的詞條整條丟棄，舊版合併語義蓋成 `max_tick` 的整批詞條跨過門檻時一次全部失效、候選退回字典表順序；
    fork 預設 `0` 代表不遺忘，設為 `1e-200` 即恢復上游行為，不必重編
  - opencc 子模組升 `ver.1.4.2`（上游 `2479df58`），`output\data\opencc\` 隨之更新
  - 區間另含上游 streaming_chord、calculus、prism 等功能提交，本 fork 前端未採用，僅隨庫帶入

### 版號

- fork 版號後綴進位 `-wujidadi.6`
- appcast 條目更新至 `0.17.4-wujidadi.6`

## 0.17.4-wujidadi.5 — 2026-08-25

### librime 更新

- librime 子模組 pin 自 `9b46caec` 更新至 `c7d525ed`（librime fork 2026-08-25 合併上游後的端點），重編 x64／Win32 產物接入，涵蓋四筆上游提交：
  - `8dc90354` 修正 stabledb（custom_phrase.txt）無權重詞條被 dee 門檻誤丟的回歸（`de21e7d4` 引入），custom_phrase 自此可安全啟用
  - `417db238` custom_phrase 測試改寫，不再依賴 TableTranslator
  - `4cacd155`／`4901c8a4` 新增 CandidatePreview API 並露出至 C 前端；本 fork 前端未採用，僅隨庫帶入

### 版號

- fork 版號後綴進位 `-wujidadi.5`
- appcast 條目更新至 `0.17.4-wujidadi.5`

## 0.17.4-wujidadi.4 — 2026-08-22

### 修正

- 自上游 PR rime/weasel#1914 分支揀回四筆修正，使 fork 與 PR 內容一致：
  - `_Send()` 重連後改用新控制代碼重送
  - 托盤刷新改於專用執行緒執行，補上 #1912 未涵蓋的殘餘死結窗口（管線工作執行緒跨執行緒 `SetWindowPos` 等訊息執行緒，而訊息執行緒卡在 `Shell_NotifyIcon`），並移除 `_RefreshTrayIcon()` 對 explorer.exe 的延遲繞道
  - 托盤狀態加鎖，避免托盤執行緒與訊息執行緒競態
  - 管線 I/O 錯誤處理補強：`CreateEvent` 失敗即拋出、`_WaitIo()` 保留原始錯誤碼、`_ConnectServerPipe()` 檢查等待結果；
    `_Receive()` 失敗（含逾時）即關閉客戶端連線，修正 `.2` 起逾時後遲到的回應會被當成下一筆請求回應的錯位問題

### 版號

- fork 版號後綴進位 `-wujidadi.4`
- appcast 條目更新至 `0.17.4-wujidadi.4`

## 0.17.4-wujidadi.3 — 2026-08-22

### 合併上游

- 合入 rime/weasel master 至 `d73f629`，含 #1912「托盤刷新改於伺服器訊息執行緒執行、不再持 `g_api_mutex` 呼叫 `Shell_NotifyIcon`」，
  斷開「explorer 工作列執行緒等管線回應 ↔ WeaselServer 管線工作執行緒等工作列處理 `Shell_NotifyIcon`」的死結環（#1909 伺服器端根因）；
  本 fork 的客戶端有界逾時修補保留，兩者互補
- 上游 `weasel.props.template` 已自帶版本巨集定義，移除本 fork 重複的那行

### 版號

- fork 版號後綴進位 `-wujidadi.3`
- appcast 條目更新至 `0.17.4-wujidadi.3`

## 0.17.4-wujidadi.2 — 2026-08-09

### 修正

- WeaselIPC 客戶端 IPC 改為 fail-fast 帶兩級逾時，修正 TSF 焦點切換時在宿主程式 UI 執行緒上同步等待 WeaselServer 造成整個視窗凍結 7～8 秒的問題。
  組字／按鍵路徑逾時 500ms 確保按鍵不被誤放；焦點／通知路徑逾時 25ms，漏掉一次下次自動補上，視窗切換肉眼無感；
  失敗經既有 catch(DWORD) 安靜降級（該次按鍵放行、下次自動重連），伺服器端呼叫以 timeout_ms 預設 INFINITE 維持原行為

### 版號

- fork 版號後綴進位 `-wujidadi.2`
- appcast 條目更新至 `0.17.4-wujidadi.2`（`.1` 發布時未及更新，一併補上）

## 0.17.4-wujidadi.1 — 2026-08-09

### 行為變更

- 新增離線維護旗標：`%APPDATA%\Rime\.maintenance-hold` 存在且未逾 10 分鐘時，伺服器常駐模式啟動的 WeaselServer 立即退場，`/q` 等指令動詞不受約束。
  供 dotfiles 的 `rime-hold-quit` 於離線重建 userdb 期間阻擋 TSF 用戶端連線失敗或 WER 自動重啟重新拉起的伺服器，避免與 `rime_dict_manager` 競逐同一 LevelDB（此競態 2026-08-08 於 macOS 端實測會造成 `CURRENT` 指向已刪 MANIFEST 的損壞，Windows 端機制同構）；
  旗標逾時自動失效，避免腳本意外殘留旗標時輸入法永久無法啟動

### 版號

- fork 版號後綴進位 `-wujidadi.1`

## 0.17.4-wujidadi — 2026-08-09

基於上游 master f9203ca（官方 0.17.4 之後的開發版）。首個自建版。

### 行為變更

- 更新頻道脫離官方 appcast：`WeaselServer.rc` 的四個 APPCAST 資源（FEEDURL、MANUALUPDATEFEEDURL、TESTINGFEEDURL、TESTINGMANUALUPDATEFEEDURL）改指本 fork 的 `update/appcast.xml`／`testing-appcast.xml`（raw.githubusercontent.com），
  兩個 appcast 模板改為 fork 版號條目；
  WinSparkle 對連字號版號的比較語義不可靠，改頻道來源後「檢查新版本」恆得「已是最新版本」，不再有被官方版蓋台或誤判降級的疑慮

### 版號

- fork 版號 `0.17.4-wujidadi` 進 `PRODUCT_VERSION`（`build.bat` 新增 `WEASEL_FORK_SUFFIX` 變數）：資源字串 ProductVersion 與 installer 檔名可辨識 fork；`WEASEL_VERSION`、`FILE_VERSION` 維持純數字（RC `FILEVERSION` 與 NSIS `VIProductVersion` 限制）

### 基礎設施

- librime 子模組改指自家 fork（Wujidadi/librime）並 pin 至 9b46caec（`1.17.0-wujidadi`，含 Windows 資源字串 fork 標記、MSVC `/utf-8`、`RIME_DLL` 匯出三項 Windows 建置修正）；建置採「librime 於自家倉庫編妥、手動接入產物」路線，不走 `build.bat rime`（其會清空 librime 的建置快取）
- 新增 `FORK-POLICY.md` 開發與貢獻規範
