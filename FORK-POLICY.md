# 本倉庫開發與貢獻規範

2026-11-24T14:34:45+08:00

本倉庫（Wujidadi/weasel）是 rime/weasel（小狼毫）的 fork。
目標是基於原作（上游），逐步將小狼毫打造為最適配倉庫擁有者個人的完全客製化輸入法。

## 分支原則

- 本倉庫的 master 分支**永不直接合併回上游**。
- 當上游 master 有更新，或其他分支有值得吸收的變化時，本地 master 可合併它們（方向永遠是「上游→本地」）。
- 吸收上游一律用 merge，**禁止 rebase**：rebase 會改寫 fork commit 的 hash，使歷代子模組 pin 與 FORK-CHANGELOG 標注的 hash 懸空，force push 後從遠端 clone 的舊版本將無法還原子模組。
- 發現上游主專案有 bug 應修，或本地開發遇到值得貢獻回去的東西，一律從 `upstream/master` 開獨立分支，以 cherry-pick 擷取或獨立重寫的方式製作變更，再向上游提 PR；不可拿本地 master 或混有本地客製內容的分支直接開 PR。

## 語言規範

- 本地 master 及其他暫不考慮合回上游的分支：程式碼異動、註解、commit message、AI agent 指引檔（如 `.claude`、`.codex`、`CLAUDE.local.md`）等，一律使用繁體中文（台灣）；批次檔（`.bat`）內的註解為避免 cmd 字碼頁亂碼，得沿用英文。
- 合回上游的內容：commit message 一律以英文撰寫，與上游慣例一致；PR 與 Issue 的標題及內文得以繁體中文（台灣）撰寫（上游討論中英混用）；程式碼識別字沿用英文，註解語言以與所改動檔案的既有風格協調為準。
- 本地分支中確定（或幾乎確定）不合回上游、但有部分內容值得貢獻的，以「從 `upstream/master` 開新分支，cherry-pick 或參考原異動獨立修改」為主，減少本地與上游互相合併的衝突與污染風險。
- 從上游 PR 分支 cherry-pick 回本地 master 的內容，程式碼與註解保持與 PR 逐字一致，commit message 依本地慣例以繁體中文撰寫；如此上游合併後再同步時不會產生內容衝突。

## 變更紀錄

- 本 fork 相對上游的變動，一律記錄於 `FORK-CHANGELOG.md`（依自建版版號分節）；上游自身的變更維持在 `CHANGELOG.md`，兩者不混寫。
- librime、squirrel 等其他 fork 倉庫比照辦理，同樣依 fork 版號分節（慣例相同：核心＝上游當前正式版）。

## 版號慣例

- 自建版版號採 SemVer 2.0.0 相容格式：`<官方當前正式版>-wujidadi[.流水號]`。
  第零版用裸後綴（如 `0.17.4-wujidadi`），同一官方基底上的後續改版依序為 `.1`、`.2`…；
  官方發布新正式版並跟上後，核心版號隨之更換、流水號歸零（回到裸後綴）。
- fork 後綴僅進 `PRODUCT_VERSION`（資源字串 ProductVersion 與 installer 檔名），
  由 `build.bat` 的 `WEASEL_FORK_SUFFIX` 變數控制；
  `WEASEL_VERSION`、`FILE_VERSION`、`VERSION_MAJOR/MINOR/PATCH` 必須維持純數字（RC `FILEVERSION` 與 NSIS `VIProductVersion` 的硬性格式限制）。
- 更新頻道與版號比較脫鉤：macOS 端 Sparkle 比較器遇連字號即截斷後綴、同版判相等，可安全沿用官方 appcast；
  Windows 端 WinSparkle 對連字號版號的比較語義不可靠（可能把 fork 後綴判為 pre-release 而視同舊版），
  故 `WeaselServer.rc` 的四個 APPCAST 資源一律指向本 fork 自控的 `update/appcast.xml`／`testing-appcast.xml`（raw.githubusercontent.com），
  appcast 條目版號與自建版一致，「檢查新版本」恆得「已是最新版本」；官方新版改以 git 上游追蹤。
