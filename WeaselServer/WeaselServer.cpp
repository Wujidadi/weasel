// WeaselServer.cpp : main source file for WeaselServer.exe
//
//	WTL MessageLoop 封装了消息循环. 实现了 getmessage/dispatchmessage....

#include "stdafx.h"
#include "resource.h"
#include "WeaselService.h"
#include <WeaselIPC.h>
#include <WeaselUI.h>
#include <RimeWithWeasel.h>
#include <WeaselUtility.h>
#include <winsparkle.h>
#include <functional>
#include <ShellScalingApi.h>
#include <WinUser.h>
#include <memory>
#include <atlstr.h>
#pragma comment(lib, "Shcore.lib")
CAppModule _Module;

int WINAPI _tWinMain(HINSTANCE hInstance,
                     HINSTANCE /*hPrevInstance*/,
                     LPTSTR lpstrCmdLine,
                     int nCmdShow) {
  LoadWeaselLogLevelFromRegistry();
  LANGID langId = get_language_id();
  SetThreadUILanguage(langId);
  SetThreadLocale(langId);

  if (!IsWindowsBlueOrLaterEx()) {
    CString info, cap;
    info.LoadStringW(IDS_STR_SYSTEM_VERSION_WARNING);
    cap.LoadStringW(IDS_STR_SYSTEM_VERSION_WARNING_CAPTION);
    MessageBoxExW(NULL, info, cap, MB_ICONERROR, langId);
    return 0;
  }
  SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

  // 防止服务进程开启输入法
  ImmDisableIME(-1);

  WCHAR user_name[20] = {0};
  DWORD size = _countof(user_name);
  GetUserName(user_name, &size);
  if (!_wcsicmp(user_name, L"SYSTEM")) {
    return 1;
  }

  HRESULT hRes = ::CoInitialize(NULL);
  // If you are running on NT 4.0 or higher you can use the following call
  // instead to make the EXE free threaded. This means that calls come in on a
  // random RPC thread.
  // HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
  ATLASSERT(SUCCEEDED(hRes));

  // this resolves ATL window thunking problem when Microsoft Layer for Unicode
  // (MSLU) is used
  ::DefWindowProc(NULL, 0, 0, 0L);

  AtlInitCommonControls(
      ICC_BAR_CLASSES);  // add flags to support other controls

  hRes = _Module.Init(NULL, hInstance);
  ATLASSERT(SUCCEEDED(hRes));

  if (!wcscmp(L"/userdir", lpstrCmdLine)) {
    CreateDirectory(WeaselUserDataPath().c_str(), NULL);
    WeaselServerApp::explore(WeaselUserDataPath());
    return 0;
  }
  if (!wcscmp(L"/weaseldir", lpstrCmdLine)) {
    WeaselServerApp::explore(WeaselServerApp::install_dir());
    return 0;
  }
  if (!wcscmp(L"/ascii", lpstrCmdLine) || !wcscmp(L"/nascii", lpstrCmdLine)) {
    weasel::Client client;
    bool ascii = !wcscmp(L"/ascii", lpstrCmdLine);
    if (client.Connect())  // try to connect to running server
    {
      if (ascii)
        client.TrayCommand(ID_WEASELTRAY_ENABLE_ASCII);
      else
        client.TrayCommand(ID_WEASELTRAY_DISABLE_ASCII);
    }
    return 0;
  }

  // command line option /q stops the running server
  bool quit = !wcscmp(L"/q", lpstrCmdLine) || !wcscmp(L"/quit", lpstrCmdLine);
  // restart if already running
  {
    weasel::Client client;
    if (client.Connect())  // try to connect to running server
    {
      client.ShutdownServer();
      if (quit)
        return 0;
      int retry = 0;
      while (client.Connect() && retry < 10) {
        client.ShutdownServer();
        retry++;
        Sleep(50);
      }
      if (retry >= 10)
        return 0;
    } else if (quit)
      return 0;
  }

  // 離線維護旗標（dotfiles 的 rime-hold-quit 於離線重建 userdb 期間放置）：
  // 伺服器結束後，TSF 用戶端連線失敗或 WER 的自動重啟都可能重新拉起本程式，
  // 與 rime_dict_manager 競逐同一 LevelDB；見旗標即退場，
  // 僅伺服器常駐模式受約束，/q 等指令動詞不受影響。
  // 旗標逾 10 分鐘視為腳本意外殘留，自動失效以免輸入法永久無法啟動
  {
    auto hold_flag = WeaselUserDataPath() / L".maintenance-hold";
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (GetFileAttributesExW(hold_flag.c_str(), GetFileExInfoStandard, &fad)) {
      FILETIME now_ft;
      GetSystemTimeAsFileTime(&now_ft);
      ULARGE_INTEGER now_u, mtime_u;
      now_u.LowPart = now_ft.dwLowDateTime;
      now_u.HighPart = now_ft.dwHighDateTime;
      mtime_u.LowPart = fad.ftLastWriteTime.dwLowDateTime;
      mtime_u.HighPart = fad.ftLastWriteTime.dwHighDateTime;
      // 10 分鐘，FILETIME 的 100ns 單位
      if (now_u.QuadPart >= mtime_u.QuadPart &&
          now_u.QuadPart - mtime_u.QuadPart < 600ULL * 10000000ULL) {
        return 0;
      }
    }
  }

  bool check_updates = !wcscmp(L"/update", lpstrCmdLine);
  if (check_updates) {
    WeaselServerApp::check_update();
  }

  CreateDirectory(WeaselUserDataPath().c_str(), NULL);

  int nRet = 0;
  try {
    LOG(INFO) << "constructing WeaselServerApp";
    WeaselServerApp app;
    RegisterApplicationRestart(NULL, 0);
    LOG(INFO) << "starting WeaselServerApp";
    nRet = app.Run();
    LOG(INFO) << "WeaselServerApp::Run returned: " << nRet;
  } catch (DWORD error) {
    LOG(ERROR) << "WeaselServer failed with Win32 error: " << error;
    nRet = -1;
  } catch (const std::exception& ex) {
    LOG(ERROR) << "WeaselServer failed with exception: " << ex.what();
    nRet = -1;
  } catch (...) {
    LOG(ERROR) << "WeaselServer failed with unknown exception";
    nRet = -1;
  }

  _Module.Term();
  ::CoUninitialize();

  return nRet;
}
