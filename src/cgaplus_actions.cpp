#include <winux.hpp>
#include <eiennet.hpp>
#include <eientpl.hpp>
#include <eiendb.hpp>

using namespace winux;
using namespace eiennet;
using namespace eientpl;
using namespace eiendb;
using namespace http;
using namespace std;

#include "cgaplus_common.hpp"
#include "cgaplus_clientctx.hpp"
#include "cgaplus_server.hpp"
#include "cgaplus_actions.hpp"


// 启动游戏
void Action_startupgame( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    Mixed & result = ctx->tpl.getVarContext()->set("result");

    // 从GET取得chara对象
    Mixed chara;
    chara.json( ctx->get.get<String>( "chara", "{}" ) );

    auto db = ctx->clientCtxPtr->connectDb();

    // 取得CGA软件设置数据
    Mixed settings = ctx->clientCtxPtr->getSettings();

    // 查询通行证和密码
    auto rsAccountPwd = db->query( db->buildStmt( "select account_pwd from cgaplus_accounts where account_name=?", chara["account_name"] ) );
    Mixed row;
    if ( rsAccountPwd->fetchRow(&row) ) chara.merge(row);

    // 复制初始化脚本到cga脚本目录
    String scriptDirPathGBK = ctx->tpl.convFrom( FilePath(settings["cga_exepath"]) + DirSep + "flandre" );
    String targetScriptFileGBK = CombinePath( scriptDirPathGBK, Format("cgaplus-misc/cgaplus-init_%u_%u.js", chara["chara_id"].toUInt(), ctx->server->config.serverPort ) );
    String sourceScriptFileGBK = "cgaplus-scripts/cgaplus-init.js";
    if ( FileMTime(sourceScriptFileGBK) > FileMTime(targetScriptFileGBK) )
    {
        auto content = FileGetContents( sourceScriptFileGBK, false );
        MakeDirExists( CombinePath( scriptDirPathGBK, "cgaplus-misc" ) );
        FilePutContents( targetScriptFileGBK, content, false );
        ColorOutput( fgGreen, "拷贝cgaplus初始化脚本文件到cga脚本库目录：", targetScriptFileGBK );
    }

    // 保存部分参数以便cgaplus-init.js脚本使用
    {
        Mixed chara2 = chara;
        chara2.del("account_name");
        chara2.del("account_pwd");
        chara2.del("gid_name");
        String targetStartupFile = FilePath(targetScriptFileGBK) + DirSep + Format( "cgaplus-startup_%d.json", chara2["chara_id"].toInt() );
        FilePutContents( targetStartupFile, chara2.json(), false );
        ColorOutput( fgGreen, "拷贝cgaplus启动参数文件到cga脚本库目录：", targetStartupFile );
    }

    // 生成命令参数
    //String loadscript = chara["loadscript"].toAnsi();
    //loadscript = IsAbsPath(loadscript) ? loadscript : CombinePath( settings["script_dirpath"], loadscript );
    String loadscript = ctx->tpl.convTo(targetScriptFileGBK);

    String loadsettings = chara["loadsettings"].toAnsi();
    loadsettings = IsAbsPath(loadsettings) ? loadsettings : CombinePath( settings["settings_dirpath"], loadsettings );

    String cgaCmdParams =
        "-gametype=" + chara["server_id"].toAnsi() + " " +
        "-loginuser=" + chara["account_name"].toAnsi() + " " +
        "-loginpwd=" + chara["account_pwd"].toAnsi() + " " +
        "-gid=" + chara["gid_name"].toAnsi() + " " +
        "-server=" + chara["server_line"].toAnsi() + " " +
        "-character=" + chara["chara_lr"].toAnsi() +
        //( chara["loadscript"] ? " -loadscript=\"" + loadscript + "\"" : "" ) +
        ( !loadscript.empty() ? " -loadscript=\"" + loadscript + "\"" : "" ) +
        ( chara["loadsettings"] ? " -loadsettings=\"" + loadsettings + "\"" : "" ) +
        ( chara["autologin"].toInt() ? " -autologin" : "" ) +
        ( chara["skipupdate"].toInt() ? " -skipupdate" : "" ) +
        ( chara["autochangeserver"].toInt() ? " -autochangeserver" : "" ) +
        //( chara["scriptautorestart"].toInt() ? " -scriptautorestart" : "" ) +
        " -scriptautorestart" +
        ( chara["injuryprotect"].toInt() ? " -injuryprotect" : "" ) +
        ( chara["soulprotect"].toInt() ? " -soulprotect" : "" )
        ;

    cgaCmdParams = ctx->tpl.convFrom(cgaCmdParams);
    // CGA程序路径
    String cgaExePathGBK = ctx->tpl.convFrom( settings["cga_exepath"] );

    cgaCmdParams = "\"" + cgaExePathGBK + "\" " + cgaCmdParams;
    cout << cgaCmdParams << endl;

    // 设置工作目录
    String oldCurDir = GetCurrentDir();
    SetCurrentDir( FilePath(cgaExePathGBK) );

    // 启动CGA和游戏
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = 0;
    BOOL rc = CreateProcess( NULL, ( cgaCmdParams.empty() ? (LPSTR)"": &cgaCmdParams[0] ), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );
    if ( rc )
    {
        DWORD processId = GetProcessId(pi.hProcess);
        cout << "CGAssistant ProcessId:" << processId;
        HWND hwnd = nullptr;
        // 等待CGA进程的主窗口有效
        int retryCount = 5000; // 5000*1 = 5000ms = 5秒
        while ( --retryCount > 0 && !( hwnd = GetMainWindowByProcessId(processId) ) ) Sleep(1);
        cout << ", CGAssistant MainHwnd:" << hwnd << endl;

        if ( hwnd )
        {
            // 设置为前台窗口
            if ( hwnd != GetForegroundWindow() )
            {
                //SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );
                BringWindowToTop(hwnd);
                SetForegroundWindow(hwnd);
                SwitchToThisWindow(hwnd, TRUE);
            }

            // 设置窗口标题
            String title = "CGAssistant [" + chara["chara_id"].toAnsi() + ":" + ctx->tpl.convFrom(chara["chara_name"]) + "] 尚未登录 (" + chara["server_line"].toAnsi() + "线)";
            SetWindowText( hwnd, title.c_str() );
        }
        else
        {
            result["error"] = ctx->tpl.convTo("CGA[" + Mixed(processId).toAnsi() + "]主窗口未启动");
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
    {
        auto errstr = GetErrorStr( GetLastError());
        result["error"] = ctx->tpl.convTo( errstr.get() + cgaExePathGBK );
    }

    SetCurrentDir(oldCurDir);
}

// 快速保存
void Action_quiklysave( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    Mixed & result = ctx->tpl.getVarContext()->set("result");
    Mixed chara;
    chara.json( ctx->get.get<String>( "chara", "{}" ) );

    cout << ctx->tpl.convFrom(chara) << endl;

    if ( chara.has("account_name") ) chara.del("account_name");

    try
    {
        auto db = ctx->clientCtxPtr->connectDb();
        SQLiteModifier mdf( db, "cgaplus_characters" );

        if ( mdf.modify(chara, chara["chara_id"] ) )
        {
            result["error"];
        }
    }
    catch ( winux::Error const & e )
    {
        result["error"] = e.what();
    }

}

// 获取角色信息
void Action_getchara( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    Mixed & result = ctx->tpl.getVarContext()->set("result");

    int charaId = ctx->get.get<int>( "chara_id", 0 );
    String charaName = ctx->get.get<String>( "chara_name", "" );
    String fieldNamesStr = ctx->get.get<String>( "fields", "*" );
    bool incAccountInfo = ctx->get.has("account");

    auto db = ctx->clientCtxPtr->connectDb();
    if ( charaId > 0 )
    {
        ColorOutput( fgYellow, "chara_id:", charaId, ", fields:", fieldNamesStr );

        String sql = "select " + fieldNamesStr + " from cgaplus_characters where chara_id=?";
        if ( incAccountInfo )
        {
            sql = "select charas.*, accounts.* from cgaplus_characters as charas left join cgaplus_gids as gids on gids.gid_name = charas.gid_name and gids.server_id = charas.server_id left join cgaplus_accounts as accounts on accounts.account_name = gids.account_name where charas.chara_id=?";
        }
        auto rsChara = db->query( db->buildStmt( sql, charaId ) );
        if ( rsChara->fetchRow(&result) )
        {
        }
    }
    else if ( !charaName.empty() )
    {
        ColorOutput( fgYellow, "chara_name:", ctx->tpl.convFrom(charaName), ", server_id:", ctx->server->gameServerId.empty() ? "4" : ctx->server->gameServerId, ", fields:", fieldNamesStr );

        MixedArray params;
        params.push_back(charaName);
        params.push_back( ctx->server->gameServerId.empty() ? "4" : ctx->server->gameServerId );

        String sql = "select " + fieldNamesStr + " from cgaplus_characters where chara_name=? and server_id=?";
        if ( incAccountInfo )
        {
            sql = "select charas.*, accounts.* from cgaplus_characters as charas left join cgaplus_gids as gids on gids.gid_name = charas.gid_name and gids.server_id = charas.server_id left join cgaplus_accounts as accounts on accounts.account_name = gids.account_name where charas.chara_name=? and charas.server_id=?";
        }
        auto rsChara = db->query( db->buildStmt( sql, params ) );
        if ( rsChara->fetchRow(&result) )
        {
        }
    }
}

// 改变当前服务器ID
void Action_changeserver( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    Mixed & result = ctx->tpl.getVarContext()->set("result");
    String serverId = ctx->get.get<String>( "server_id", "4" );

    ctx->server->gameServerId = serverId; // 设置当前服务器ID

    ctx->cookies.set( "server_id", serverId, GetUtcTime() + 30*24*3600, "", "/" );
    ctx->cookies.commitTo(&ctx->rspHdr);

    result["error"];

    ColorOutput( fgGreen, "cookies:", ctx->cookies.dump() );
}

// 验证cga_gui_port是否有效
void Action_checkguiport( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");
    ushort guiPort = ctx->get.get<ushort>( "gui_port", 0 );

    ColorOutput( fgYellow, "gui_port=", guiPort );

    if ( guiPort == 0 )
    {
        result["error"] = ctx->tpl.convTo("GUI_PORT不能为0");
        return;
    }

    HttpCUrl http;
    http.setTimeout(3);
    if ( http.get( Format("http://127.0.0.1:%u/cga/GetGameProcInfo", guiPort ) ) )
    {
        ulong len;
        char const * str = http.getResponseStr(&len);

        result.json( AnsiString( str, len ) );
    }
    else
    {
        result["error"] = ctx->tpl.convTo( Format( "[gui_port:%u]GetGameProcInfo失败", guiPort ) );
    }
}

// 验证cga_game_port是否有效
void Action_checkgameport( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");
    ushort gamePort = ctx->get.get<ushort>( "game_port", 0 );

    ColorOutput( fgYellow, "game_port=", gamePort );
    if ( gamePort == 0 )
    {
        result["error"] = ctx->tpl.convTo("GAME_PORT不能为0");
        return;
    }
    ip::tcp::Socket sock;
    ip::EndPoint ep{ "127.0.0.1", gamePort };
    if ( ip::tcp::ConnectAttempt( &sock, ep, 3000 ) )
    {
        result["error"];
        sock.shutdown();
        sock.close();
    }
    else
    {
        result["error"] = ctx->tpl.convTo("连接GamePort失败");
    }
}

// CGA设置脚本
void Action_cgasetscript( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");

    ushort guiPort = ctx->get.get<ushort>( "gui_port", 0 );

    if ( guiPort == 0 )
    {
        result["error"] = ctx->tpl.convTo("GUI_PORT不能为0");
        return;
    }

    Mixed params;
    params.json( ctx->post.get<String>( "params", "{}" ) );
    // 取得设置
    Mixed settings = ctx->clientCtxPtr->getSettings();
    if ( !IsAbsPath(params["path"]) )
    {
        params["path"] = CombinePath( settings["script_dirpath"], params["path"] );
    }

    if ( params.has("autorestart") )
        params["autorestart"] = params["autorestart"].toInt() ? true : false;
    if ( params.has("injuryprot") )
        params["injuryprot"] = params["injuryprot"].toInt() ? true : false;
    if ( params.has("soulprot") )
        params["soulprot"] = params["soulprot"].toInt() ? true : false;

    ColorOutput( fgYellow, ctx->tpl.convFrom(params) );

    HttpCUrl http;
    http.setTimeout(1);
    if ( http.post( Format("http://127.0.0.1:%u/cga/LoadScript", guiPort ), "application/json", params ) )
    {
        ulong len;
        char const * str = http.getResponseStr(&len);

        result.json( AnsiString( str, len ) );
    }
    else
    {
        result["error"] = ctx->tpl.convTo("LoadScript失败");
    }
}

// CGA设置玩家配置
void Action_cgasetsettings( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");
    ushort guiPort = ctx->get.get<ushort>( "gui_port", 0 ); // cga GUI端口
    String cgaCharaSettingsFile = ctx->get.get<String>("chara_settings_file"); // cga角色设置文件
    String cgaCharaSettings = ctx->get.get<String>("chara_settings"); // cga角色设置JSON
    if ( guiPort == 0 )
    {
        result["error"] = ctx->tpl.convTo("GUI_PORT不能为0");
        return;
    }
    Mixed settings = ctx->clientCtxPtr->getSettings(); // 读取cgaplus设置
    Mixed params;
    if ( cgaCharaSettingsFile.empty() )
    {
        if ( cgaCharaSettings.empty() )
        {
            params.json("{}");
        }
        else
        {
            params.json(cgaCharaSettings);
        }
    }
    else
    {
        cgaCharaSettingsFile = IsAbsPath(cgaCharaSettingsFile) ? cgaCharaSettingsFile : CombinePath( settings["settings_dirpath"], cgaCharaSettingsFile );
        AnsiString content = FileGetContents( ctx->tpl.convFrom(cgaCharaSettingsFile), false );
        if ( content.empty() )
        {
            content = "{}";
        }
        params.json(content);
    }

    ColorOutput( fgYellow, ctx->tpl.convFrom(params) );

    HttpCUrl http;
    http.setTimeout(1);
    if ( http.post( Format("http://127.0.0.1:%u/cga/LoadSettings", guiPort ), "application/json", params ) )
    {
        ulong len;
        char const * str = http.getResponseStr(&len);
        result.json( AnsiString( str, len ) );
    }
    else
    {
        result["error"] = ctx->tpl.convTo("LoadSettings失败");
    }
}

// 添加角色
void Action_addchara( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    Mixed & result = ctx->tpl.getVarContext()->set("result");

    Mixed inputChara;
    inputChara.json( ctx->get.get<String>( "chara", "{}" ) );

    ColorOutput( fgYellow, ctx->tpl.convFrom(inputChara) );

    try
    {
        auto db = ctx->clientCtxPtr->connectDb();
        // 保证account_name存在
        String accountName = inputChara["account_name"], accountPwd = inputChara["account_pwd"];
        inputChara.del("account_name");
        inputChara.del("account_pwd");
        auto rsAccountExists = db->query( db->buildStmt( "select count(*) from cgaplus_accounts where account_name=?", accountName ) );
        Mixed row;
        rsAccountExists->fetchRow( &row, 1 );
        if ( row[0].toUInt() == 0 ) // 没有account_name的通行证
        {
            // 添加通行证
            if ( accountName.empty() ) // 通行证为空，失败
            {
                result["error"] = ctx->tpl.convTo("通行证不能为空");
                return;
            }
            if ( accountPwd.empty() ) // 密码为空，失败
            {
                result["error"] = ctx->tpl.convTo("需要添加通行证时，通行证密码不能为空");
                return;
            }
            SQLiteModifier mdf( db, "cgaplus_accounts" );
            Mixed record;
            record.addPair()( "account_name", accountName )( "account_pwd", accountPwd );
            if ( !mdf.addNew(record) )
            {
                result["error"] = ctx->tpl.convTo("添加通行证失败");
                return;
            }
        }

        // 保证gid_name存在
        Mixed params;
        auto rsGidExists = db->query( db->buildStmt( "select count(*) from cgaplus_gids where gid_name=? and server_id=?", params.add()( inputChara["gid_name"] )( inputChara["server_id"] ) ) );
        rsGidExists->fetchRow( &row, 1 );
        if ( row[0].toUInt() == 0 ) // 没有gid_name的GID
        {
            String gidName = inputChara["gid_name"];
            String serverId = inputChara["server_id"];
            if ( gidName.empty() ) // GID为空，失败
            {
                result["error"] = ctx->tpl.convTo("GID不能为空");
                return;
            }
            if ( serverId.empty() ) // ServerId为空，失败
            {
                result["error"] = ctx->tpl.convTo("ServerId不能为空");
                return;
            }
            // 添加GID
            SQLiteModifier mdf( db, "cgaplus_gids" );
            Mixed record;
            record.addPair()("gid_name", inputChara["gid_name"])("server_id",inputChara["server_id"])("account_name",accountName);
            if ( !mdf.addNew(record) )
            {
                result["error"] = ctx->tpl.convTo("添加GID失败");
                return;
            }
        }

        // 添加角色
        SQLiteModifier mdf( db, "cgaplus_characters" );

        inputChara["chara_lr"] = inputChara["chara_lr"].toInt();
        inputChara["server_id"] = inputChara["server_id"].toInt();
        inputChara["server_line"] = inputChara["server_line"].toInt();
        inputChara["autologin"] = inputChara["autologin"].toInt();
        inputChara["skipupdate"] = inputChara["skipupdate"].toInt();
        inputChara["autochangeserver"] = inputChara["autochangeserver"].toInt();
        inputChara["scriptautorestart"] = inputChara["scriptautorestart"].toInt();
        inputChara["injuryprotect"] = inputChara["injuryprotect"].toInt();
        inputChara["soulprotect"] = inputChara["soulprotect"].toInt();

        if ( !mdf.addNew(inputChara) )
        {
            result["error"] = ctx->tpl.convTo("添加角色失败：") + db->error();
            return;
        }

        result["error"];
        ColorOutput( fgGreen, ctx->tpl.convFrom(inputChara) );
    }
    catch ( winux::Error const & e )
    {
        result["error"] = e.what();
    }

}

// 修改cgaplus设置
void Action_cgaplussetsettings( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁
    Mixed & result = ctx->tpl.getVarContext()->set("result");

    Mixed inputSettings;
    inputSettings.json( ctx->get.get<String>( "settings", "{}" ) );

    ColorOutput( fgYellow, ctx->tpl.convFrom(inputSettings) );

    auto db = ctx->clientCtxPtr->connectDb();
    SQLiteModifier mdf( db, "cgaplus_settings" );
    // 修改设置
    int n = inputSettings.getCount();
    for ( int i = 0; i < n; ++i )
    {
        auto & pr = inputSettings.getPair(i);
        Mixed fields;
        fields.addPair()
            ( "setting_name", pr.first )
            ( "setting_value", pr.second )
        ;

        if ( mdf.modify(fields, pr.first) )
        {
            ColorOutput( fgGreen, "修改 ", ctx->tpl.convFrom(fields) );
        }
        else
        {
            result["error"] = db->error();
            return;
        }
    }

    // 复制flandre脚本到cga目录下
    String cgaExePath = ctx->tpl.convFrom(inputSettings["cga_exepath"]);
    if ( DetectPath(cgaExePath) )
    {
        String cgaExeDir = FilePath(cgaExePath);
        String cmdParams = "/D /Y /I flandre \"" + cgaExeDir + DirSep + "flandre\"";
        int rc = (int)ShellExecute( NULL, NULL, "xcopy", cmdParams.c_str(), NULL, SW_NORMAL );
        cout << "xcopy " << cmdParams << "\n" << rc << endl;
    }

    result["error"];
}

// 获取cgaplus设置
void Action_cgaplusgetsettings( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁
    Mixed & result = ctx->tpl.getVarContext()->set("result");

    result = ctx->clientCtxPtr->getSettings();

    cout << ctx->tpl.convFrom(result) << endl;
}

// 检测CGAssistant.exe的路径
void Action_detectcgapath( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");

    String path = FilePath( GetExecutablePath() );
    while ( path.substr( path.length() - 1 ) != "." ) // 不是根目录
    {
        String detectPath = CombinePath( path, "CGAssistant.exe" );

        ColorOutput( fgYellow, detectPath );
        if ( DetectPath(detectPath) ) // 找到CGAssistant.exe
        {
            result["cga_exepath"] = ctx->tpl.convTo(detectPath);
            break;
        }
        path = RealPath(path + "\\..");
    }

    if ( result["cga_exepath"] )
    {
        result["script_dirpath"] = FilePath(result["cga_exepath"]) + "\\flandre";
        result["settings_dirpath"] = result["script_dirpath"];
        result["error"];
    }
    else
    {
        result["error"] = ctx->tpl.convTo("请把cgaplus放在CGAssistant的目录里");
    }
}

void Action_delaccount( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁
    Mixed & result = ctx->tpl.getVarContext()->set("result");

    String accountName = ctx->get.get<String>("account_name");

    auto db = ctx->clientCtxPtr->connectDb();
    SQLiteModifier mdf( db, "cgaplus_accounts" );
    // 删除通行证
    try
    {
        if ( mdf.deleteOne(accountName) )
        {
            result["error"];
        }
        else
        {
            result["error"] = db->error();
        }
    }
    catch ( SQLiteDbError const & e )
    {
        result["error"] = e.what();
    }
}

void Action_delgid( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁
    Mixed & result = ctx->tpl.getVarContext()->set("result");

    String gidName = ctx->get.get<String>("gid_name");

    auto db = ctx->clientCtxPtr->connectDb();
    SQLiteModifier mdf( db, "cgaplus_gids" );
    // 删除GID
    try
    {
        if ( mdf.deleteOne(gidName) )
        {
            result["error"];
        }
        else
        {
            result["error"] = db->error();
        }
    }
    catch ( SQLiteDbError const & e )
    {
        result["error"] = e.what();
    }
}

void Action_delchara( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁
    Mixed & result = ctx->tpl.getVarContext()->set("result");

    int charaId = ctx->get.get<int>("chara_id");

    auto db = ctx->clientCtxPtr->connectDb();
    SQLiteModifier mdf( db, "cgaplus_characters" );
    // 删除角色
    try
    {
        if ( mdf.deleteOne(charaId) )
        {
            result["error"];
        }
        else
        {
            result["error"] = db->error();
        }
    }
    catch ( SQLiteDbError const & e )
    {
        result["error"] = e.what();
    }
}

void Action_test( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");

    result["get"] = ctx->get.getVars();
    result["post"] = ctx->post.getVars();

}
