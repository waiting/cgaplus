#pragma once

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

    // CGA程序路径
    String cgaExePathGBK = ctx->tpl.convFrom( settings["cga_exepath"] );

    // 账号密码
    auto rsAccountPwd = db->query( db->buildStmt( "select account_pwd from cgaplus_accounts where account_name=?", chara["account_name"] ) );
    Mixed row;
    if ( rsAccountPwd->fetchRow(&row) ) chara.merge(row);

    // 复制初始化脚本到cga脚本目录
    String scriptDirPathGBK = ctx->tpl.convFrom(settings["script_dirpath"]);
    String targetScriptFileGBK = CombinePath( scriptDirPathGBK, Format("cgaplus-scripts/CGAPLUS-Init_%u_%u.js", chara["chara_id"].toUInt(), ctx->server->config.serverPort ) );
    String sourceScriptFileGBK = "cgaplus-scripts/CGAPLUS-Init.js";
    if ( FileMTime(sourceScriptFileGBK) > FileMTime(targetScriptFileGBK) )
    {
        auto content = FileGetContents( sourceScriptFileGBK, false );
        MakeDirExists( CombinePath( scriptDirPathGBK, "cgaplus-scripts" ) );
        FilePutContents( targetScriptFileGBK, content, false );
        ColorOutput( fgGreen, "拷贝cgaplus初始化脚本文件到cga脚本库目录：", targetScriptFileGBK );
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
    cout << cgaCmdParams << endl;

    // 启动CGA和游戏
    INT rc = (INT)ShellExecute( NULL, NULL, cgaExePathGBK.c_str(), cgaCmdParams.c_str(), NULL, SW_NORMAL );

    result["rc"] = rc;
}

// 快速保存
void Action_quiklysave( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    Mixed & result = ctx->tpl.getVarContext()->set("result");
    Mixed chara;
    chara.json( ctx->get.get<String>( "chara", "{}" ) );

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

    auto db = ctx->clientCtxPtr->connectDb();
    if ( charaId > 0 )
    {
        auto rsChara = db->query( db->buildStmt( "select " + fieldNamesStr + " from cgaplus_characters where chara_id=?", charaId ) );
        if ( rsChara->fetchRow(&result) )
        {
        }
    }
    else if ( !charaName.empty() )
    {
        MixedArray params;
        params.push_back(charaName);
        params.push_back( ctx->server->gameServerId.empty() ? "4" : ctx->server->gameServerId );

        auto rsChara = db->query( db->buildStmt( "select " + fieldNamesStr + " from cgaplus_characters where chara_name=? and server_id=?", params ) );

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

    ColorOutput( fgGreen, ctx->cookies.dump() );
}

// 验证cga_gui_port是否有效
void Action_checkguiport( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");
    ushort guiPort = ctx->get.get<ushort>( "gui_port", 0 );

    if ( guiPort == 0 )
    {
        result["error"] = ctx->tpl.convTo("GUI_PORT不能为0");
        return;
    }

    HttpCUrl http;
    http.setTimeout(1);
    if ( http.get( Format("http://127.0.0.1:%u/cga/GetGameProcInfo", guiPort ) ) )
    {
        ulong len;
        char const * str = http.getResponseStr(&len);

        result.json( AnsiString( str, len ) );
    }
    else
    {
        result["error"] = ctx->tpl.convTo("GetGameProcInfo失败");
    }
}

// 验证cga_game_port是否有效
void Action_checkgameport( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");
    ushort gamePort = ctx->get.get<ushort>( "game_port", 0 );

    if ( gamePort == 0 )
    {
        result["error"] = ctx->tpl.convTo("GAME_PORT不能为0");
        return;
    }
    ip::tcp::Socket sock;
    ip::EndPoint ep{ "127.0.0.1", gamePort };
    if ( ip::tcp::ConnectAttempt( &sock, ep, 1000 ) )
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
        if ( row[0].toUInt() == 0 ) // 没有account_name的账号
        {
            // 添加账号
            if ( accountName.empty() ) // 账号为空，失败
            {
                result["error"] = ctx->tpl.convTo("账号不能为空");
                return;
            }
            if ( accountPwd.empty() ) // 密码为空，失败
            {
                result["error"] = ctx->tpl.convTo("账号不存在，需要添加账号时，账号密码不能为空");
                return;
            }
            SQLiteModifier mdf( db, "cgaplus_accounts" );
            Mixed record;
            record.addPair()( "account_name", accountName )( "account_pwd", accountPwd );
            if ( !mdf.addNew(record) )
            {
                result["error"] = ctx->tpl.convTo("添加账号失败");
                return;
            }
        }

        // 保证gid_name存在
        Mixed params;
        auto rsGidExists = db->query( db->buildStmt( "select count(*) from cgaplus_gids where gid_name=? and server_id=?", params.add()( inputChara["gid_name"] )( inputChara["server_id"] ) ) );
        rsGidExists->fetchRow( &row, 1 );
        if ( row[0].toUInt() == 0 ) // 没有gid_name的GID
        {
            String gidName = inputChara["gid_name"], serverId = inputChara["server_id"];
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
            result["error"] = ctx->tpl.convTo("添加角色失败");
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

void Action_test( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");

    result["get"] = ctx->get.getVars();
    result["post"] = ctx->post.getVars();

}