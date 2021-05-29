#pragma once

void Page_dashboard( CgaPlusHttpServer::PageContext * ctx )
{
    ctx->tpl.assign( "page_title", "仪表板" );

    ctx->tpl.getVarContext()->set("settings") = ctx->clientCtxPtr->getSettings();

    auto db = ctx->clientCtxPtr->connectDb();
    Mixed row;
    auto rs1 = db->query("select count(*) from cgaplus_accounts");
    if ( rs1->fetchRow(&row, 1) )
        ctx->tpl.getVarContext()->set("accounts_count") = row[0];
    auto rs2 = db->query("select count(*) from cgaplus_gids");
    if ( rs2->fetchRow(&row, 1) )
        ctx->tpl.getVarContext()->set("gids_count") = row[0];
    auto rs3 = db->query("select count(*) from cgaplus_characters");
    if ( rs3->fetchRow(&row, 1) )
        ctx->tpl.getVarContext()->set("characters_count") = row[0];

}

void Page_quikly( CgaPlusHttpServer::PageContext * ctx )
{
    ctx->tpl.assign( "page_title", "快速角色启动" );

    // 从Cookies读取server_id
    ctx->server->gameServerId = ctx->cookies.has("server_id") ? ctx->cookies.get("server_id").value : "4";
    ctx->tpl.assign( "cur_server_id", ctx->server->gameServerId, false );

    try
    {
        // 查询角色
        String sql = "select chara_id, chara_name, chara_lr, charas.gid_name, charas.server_id, account_name, server_line, autologin, skipupdate, autochangeserver, scriptautorestart, injuryprotect, soulprotect, loadscript, loadsettings, cga_port from cgaplus_characters as charas left join cgaplus_gids as gids on gids.gid_name = charas.gid_name and gids.server_id = charas.server_id";

        auto db = ctx->clientCtxPtr->connectDb();
        auto rs = db->query( db->buildStmt( sql + ( ctx->server->gameServerId.empty() ? "" : " where charas.server_id=?" ), ctx->server->gameServerId ) );
        Mixed & charas = ctx->tpl.getVarContext()->set("charas").createArray();
        Mixed row;
        while ( rs->fetchRow(&row) )
            charas.add(row);

        // 查询设置
        Mixed settings = ctx->clientCtxPtr->getSettings();

        // 枚举JSON文件和JS文件
        StringArray scriptFiles, settingsFiles;
        EnumFiles( ctx->tpl.convFrom(settings["script_dirpath"]), "js", &scriptFiles );
        ctx->tpl.assign( "script_files", scriptFiles, true );
        EnumFiles( ctx->tpl.convFrom(settings["settings_dirpath"]), "json", &settingsFiles );
        ctx->tpl.assign( "settings_files", settingsFiles, true );

        // 查询ServerID
        auto rsServers = db->query("select * from cgaplus_servers");
        Mixed & servers = ctx->tpl.getVarContext()->set("servers").createArray();
        while ( rsServers->fetchRow(&row) )
            servers.add(row);
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

void Page_addchara( CgaPlusHttpServer::PageContext * ctx )
{
    ctx->tpl.assign( "page_title", "添加角色" );
}
