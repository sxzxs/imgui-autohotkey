#ifndef exports_h
#define exports_h
#include <windows.h>
#include <windows.h>
#include <tchar.h>
//_T("")
//L""
//utf8
#define EXPORT extern "C" __declspec(dllexport)

EXPORT DWORD NewThread(const wchar_t *aScript, const wchar_t *aCmdLine = L"", const wchar_t *aTitle = L"AutoHotkey");
EXPORT UINT_PTR addScript(const wchar_t *script, int waitexecute = 0, DWORD aThreadID = 0);
EXPORT UINT_PTR ahkExecuteLine(UINT_PTR line, int aMode, int wait, DWORD aThreadID = 0);
EXPORT UINT_PTR ahkFindFunc(LPTSTR aFuncName, DWORD aThreadID = 0);
EXPORT UINT_PTR ahkFindLabel(LPTSTR aLabelName, DWORD aThreadID = 0);
EXPORT LPTSTR ahkFunction(LPTSTR func, LPTSTR param1 = NULL, LPTSTR param2 = NULL, LPTSTR param3 = NULL, LPTSTR param4 = NULL, LPTSTR param5 = NULL, LPTSTR param6 = NULL, LPTSTR param7 = NULL, LPTSTR param8 = NULL, LPTSTR param9 = NULL, LPTSTR param10 = NULL, DWORD aThreadID = 0);
EXPORT LPTSTR ahkGetVar(const wchar_t *name, int getVar = 0, DWORD aThreadID = 0);
EXPORT int ahkAssign(const wchar_t *name, LPTSTR value, DWORD aThreadID = 0);
EXPORT int ahkExec(const wchar_t *script, DWORD aThreadID = 0);
EXPORT int ahkLabel(LPTSTR aLabelName, int nowait = 0, DWORD aThreadID = 0);
EXPORT int ahkPause(LPTSTR aChangeTo, DWORD aThreadID = 0);
EXPORT int ahkPostFunction(LPTSTR func, LPTSTR param1 = NULL, LPTSTR param2 = NULL, LPTSTR param3 = NULL, LPTSTR param4 = NULL, LPTSTR param5 = NULL, LPTSTR param6 = NULL, LPTSTR param7 = NULL, LPTSTR param8 = NULL, LPTSTR param9 = NULL, LPTSTR param10 = NULL, DWORD aThreadID = 0);
EXPORT int ahkReady(DWORD aThreadID = 0);

//void callFuncDll(FuncAndToken *aFuncAndToken, bool throwerr = true);
//void callPromise(Promise *aPromise, HWND replyHwnd);
#ifdef _USRDLL
HRESULT ahkFunctionVariant(LPTSTR func, VARIANT& ret, VARIANT param1,/*[in,optional]*/ VARIANT param2,/*[in,optional]*/ VARIANT param3,/*[in,optional]*/ VARIANT param4,/*[in,optional]*/ VARIANT param5,/*[in,optional]*/ VARIANT param6,/*[in,optional]*/ VARIANT param7,/*[in,optional]*/ VARIANT param8,/*[in,optional]*/ VARIANT param9,/*[in,optional]*/ VARIANT param10, DWORD aThreadID, int sendOrPost);
#endif
#endif
