#pragma once

// 获取指定PID的主窗口
HWND GetMainWindowByProcessId( DWORD processId );
// Windows错误代码转错误串
winux::SimpleHandle<char *> GetErrorStr( int err );
