# Fork 變更日誌

本檔記錄本 fork（Wujidadi/weasel）相對上游 rime/weasel 的所有變動，依自建版版號分節。
上游自身的變更見 `CHANGELOG.md`；分支與版號規範見 `FORK-POLICY.md`。

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
