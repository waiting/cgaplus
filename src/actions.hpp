#pragma once

// 启动游戏
void Action_startupgame( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");
    Mixed chara;
    chara.json( ctx->get.get<String>("chara") );

    auto db = ctx->clientCtxPtr->getDb();

    // 取得CGA软件设置数据
    Mixed settings = ctx->clientCtxPtr->getSettings();

    // CGA程序路径
    String cgaExePath = ctx->tpl.convFrom( settings["cga_exepath"] );

    // 账号密码
    auto rsAccountPwd = db->query( db->buildStmt( "select account_pwd from cgaplus_accounts where account_name=?", chara["account_name"] ) );
    Mixed row;
    if ( rsAccountPwd->fetchRow(&row) )
    {
        chara.merge(row);
    }

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
        ( chara["autologin"] ? " -autologin" : "" ) +
        ( chara["skipupdate"] ? " -skipupdate" : "" ) +
        ( chara["autochangeserver"] ? " -autochangeserver" : "" ) +
        ( chara["scriptautorestart"] ? " -scriptautorestart" : "" ) +
        ( chara["injuryprotect"] ? " -injuryprotect" : "" ) +
        ( chara["soulprotect"] ? " -soulprotect" : "" )
        ;

    cgaCmdParams = ctx->tpl.convFrom(cgaCmdParams);
    cout << cgaCmdParams << endl;

    // 启动CGA和游戏
    ShellExecute( NULL, NULL, cgaExePath.c_str(), cgaCmdParams.c_str(), NULL, SW_NORMAL );

    //result;
}

// 快速保存
void Action_quiklysave( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & result = ctx->tpl.getVarContext()->set("result");
    Mixed chara;
    chara.json( ctx->get.get<String>("chara") );

    cout << chara << endl;
}
