#pragma once
class CgaPlusHttpClientCtx;

// 处理Web相关GET,POST,COOKIES数据
void ProcessData( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, Vars * get = nullptr, Vars * post = nullptr, Cookies * cookies = nullptr );
// 获取指定PID的主窗口
HWND GetMainWindowByProcessId( DWORD processId );
// Windows错误代码转错误串
winux::SimpleHandle<char *> GetErrorStr( int err );
