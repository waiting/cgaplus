﻿#pragma once

void Page_dashboard( CgaPlusHttpServer::PageContext * ctx )
{
    ctx->tpl.assign( "page_title", "仪表板" );

    ctx->tpl.getVarContext()->set("settings") = ctx->clientCtxPtr->getSettings();

    Mixed row;
    auto rs1 = ctx->clientCtxPtr->getDb()->query("select count(*) from cgaplus_accounts");
    if ( rs1->fetchRow(&row, 1) )
        ctx->tpl.getVarContext()->set("accounts_count") = row[0];
    auto rs2 = ctx->clientCtxPtr->getDb()->query("select count(*) from cgaplus_gids");
    if ( rs2->fetchRow(&row, 1) )
        ctx->tpl.getVarContext()->set("gids_count") = row[0];
    auto rs3 = ctx->clientCtxPtr->getDb()->query("select count(*) from cgaplus_characters");
    if ( rs3->fetchRow(&row, 1) )
        ctx->tpl.getVarContext()->set("characters_count") = row[0];

}

void Page_quikly( CgaPlusHttpServer::PageContext * ctx )
{
    ctx->tpl.assign( "page_title", "快速启动" );
    try
    {
        // 查询角色
        String sql = "select chara_id, chara_name, chara_lr, charas.gid_name, charas.server_id, account_name, server_line, autologin, skipupdate, autochangeserver, scriptautorestart, injuryprotect, soulprotect, loadscript, loadsettings from cgaplus_characters as charas left join cgaplus_gids as gids on gids.gid_name = charas.gid_name and gids.server_id = charas.server_id";
        auto rs = ctx->clientCtxPtr->getDb()->query(sql);
        Mixed & charas = ctx->tpl.getVarContext()->set("charas").createArray();
        Mixed row;
        while ( rs->fetchRow(&row) )
            charas.add(row);

        Mixed settings = ctx->clientCtxPtr->getSettings();

        // 枚举设置文件和脚本文件
        StringArray scriptFiles, settingsFiles;
        EnumFiles( ctx->tpl.convFrom(settings["script_dirpath"]), "js", &scriptFiles );
        ctx->tpl.assign( "script_files", scriptFiles, true );
        EnumFiles( ctx->tpl.convFrom(settings["settings_dirpath"]), "json", &settingsFiles );
        ctx->tpl.assign( "settings_files", settingsFiles, true );
    }
    catch ( SQLiteDbError const & e )
    {
        cout << e.what();
    }
}

void Page_cgasettings( CgaPlusHttpServer::PageContext * ctx )
{
}

void Page_accounts( CgaPlusHttpServer::PageContext * ctx )
{
}

void Page_characters( CgaPlusHttpServer::PageContext * ctx )
{
}
