#include <winux.hpp>
#include <eiennet.hpp>
#include <eienwebx.hpp>
#include <eientpl.hpp>
#include <eiendb.hpp>

using namespace winux;
using namespace eiennet;
using namespace eienwebx;
using namespace eientpl;
using namespace eiendb;
using namespace http;

#include "cgaplus_common.hpp"
#include "cgaplus_clientctx.hpp"
#include "cgaplus_server.hpp"

struct EnumWindowsParam
{
    HWND hwndResult;
    DWORD processId;
};

BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
    EnumWindowsParam * param = (EnumWindowsParam*)lParam;

    DWORD dwPid = 0;
    GetWindowThreadProcessId( hwnd, &dwPid );

    if ( dwPid == param->processId )
    {
        param->hwndResult = hwnd;
        return FALSE;
    }
    return TRUE;
}

// 获取指定PID的主窗口
HWND GetMainWindowByProcessId( DWORD processId )
{
    EnumWindowsParam param = { nullptr, processId };
    EnumWindows( EnumWindowsProc, (LPARAM)&param );
    return param.hwndResult;
}

// Windows错误代码转错误串
winux::SimpleHandle<char *> GetErrorStr( int err )
{
    char * buf = NULL;
    DWORD dw = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,
        err,
        0,
        (LPSTR)&buf,
        256,
        NULL
    );

    return winux::SimpleHandle<char *>( buf, NULL, LocalFree );
}
