#pragma once

// 启动游戏
void Action_startupgame( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");
    // 从GET取得chara对象
    Mixed chara = ctx->get.getVars();

    auto db = ctx->clientCtxPtr->connectDb();

    // 取得CGA软件设置数据
    Mixed settings = ctx->clientCtxPtr->getSettings();

    // CGA程序路径
    String cgaExePath = ctx->tpl.convFrom( settings["cga_exepath"] );

    // 账号密码
    auto rsAccountPwd = db->query( db->buildStmt( "select account_pwd from cgaplus_accounts where account_name=?", chara["account_name"] ) );
    Mixed row;
    if ( rsAccountPwd->fetchRow(&row) )
        chara.merge(row);

    // 复制初始化脚本到cga脚本目录

    // 生成命令参数
    String loadscript = chara["loadscript"].toAnsi();
    loadscript = IsAbsPath(loadscript) ? loadscript : CombinePath( settings["script_dirpath"], loadscript );
    String loadsettings = chara["loadsettings"].toAnsi();
    loadsettings = IsAbsPath(loadsettings) ? loadsettings : CombinePath( settings["settings_dirpath"], loadsettings );

    String cgaCmdParams =
        "-gametype=" + chara["server_id"].toAnsi() + " " +
        "-loginuser=" + chara["account_name"].toAnsi() + " " +
        "-loginpwd=" + chara["account_pwd"].toAnsi() + " " +
        "-gid=" + chara["gid_name"].toAnsi() + " " +
        "-server=" + chara["server_line"].toAnsi() + " " +
        "-character=" + chara["chara_lr"].toAnsi() +
        ( chara["loadscript"] ? " -loadscript=\"" + loadscript + "\"" : "" ) +
        ( chara["loadsettings"] ? " -loadsettings=\"" + loadsettings + "\"" : "" ) +
        ( chara["autologin"].toInt() ? " -autologin" : "" ) +
        ( chara["skipupdate"].toInt() ? " -skipupdate" : "" ) +
        ( chara["autochangeserver"].toInt() ? " -autochangeserver" : "" ) +
        ( chara["scriptautorestart"].toInt() ? " -scriptautorestart" : "" ) +
        ( chara["injuryprotect"].toInt() ? " -injuryprotect" : "" ) +
        ( chara["soulprotect"].toInt() ? " -soulprotect" : "" )
        ;

    cgaCmdParams = ctx->tpl.convFrom(cgaCmdParams);
    cout << cgaCmdParams << endl;

    // 启动CGA和游戏
    HINSTANCE hInst = ShellExecute( NULL, NULL, cgaExePath.c_str(), cgaCmdParams.c_str(), NULL, SW_NORMAL );

    result["hInst"] = (uint64)hInst;
}

// 快速保存
void Action_quiklysave( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");
    Mixed chara = ctx->get.getVars();

    if ( chara.has("account_name") ) chara.del("account_name");

    try
    {
        ScopeGuard guard( ctx->server->getMutex() );

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

void Action_getchara( CgaPlusHttpServer::PageContext * ctx )
{
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

void Action_changeserver( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");

    String serverId = ctx->get.get<String>( "server_id", "4" );

    ctx->server->gameServerId = serverId; // 设置当前全局服务器

    ctx->cookies.set("server_id", serverId, GetUtcTime() + 30*24*3600, "", "/" );
    ctx->cookies.commitTo(&ctx->rspHdr);

    result["error"];
}
