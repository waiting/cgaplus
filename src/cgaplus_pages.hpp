#pragma once
void Page_cgaplussettings( CgaPlusHttpServer::PageContext * ctx );

void Page_dashboard( CgaPlusHttpServer::PageContext * ctx )
{
    Mixed & settings = ctx->tpl.getVarContext()->set("settings");
    {
        ScopeGuard guard( ctx->server->getMutex() ); // 加锁
        settings = ctx->clientCtxPtr->getSettings();
    }
    // 如果没有设置CGA程序路径，需要到软件设置里设置
    if ( !settings["cga_exepath"] || !DetectPath( ctx->tpl.convFrom(settings["cga_exepath"]) ) )
    {
        ctx->tpl.assign( "page_stamp", "cgaplussettings" );
        Page_cgaplussettings(ctx);
        return;
    }

    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    ctx->tpl.assign( "page_title", "仪表板" );

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
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    ctx->tpl.assign( "page_title", "快速角色启动" );

    if ( ctx->server->gameServerId.empty() )
    {
        // 从Cookies读取server_id
        ctx->server->gameServerId = ctx->cookies.has("server_id") ? ctx->cookies.get("server_id").value : "4";
    }
    ctx->tpl.assign( "cur_server_id", ctx->server->gameServerId, false );

    try
    {
        auto db = ctx->clientCtxPtr->connectDb();
        Mixed row;

        // 查询角色
        String sql = "select chara_id, chara_name, chara_lr, charas.gid_name, charas.server_id, account_name, server_line, chara_level, chara_job, autologin, skipupdate, autochangeserver, scriptautorestart, injuryprotect, soulprotect, loadscript, loadsettings, cga_port from cgaplus_characters as charas left join cgaplus_gids as gids on gids.gid_name = charas.gid_name and gids.server_id = charas.server_id";
        auto rs = db->query( db->buildStmt( sql + ( ctx->server->gameServerId.empty() ? "" : " where charas.server_id=?" ), ctx->server->gameServerId ) );
        Mixed & charas = ctx->tpl.getVarContext()->set("charas").createArray();
        while ( rs->fetchRow(&row) ) charas.add(row);

        // 查询设置
        Mixed & settings = ctx->tpl.getVarContext()->set("settings");
        settings = ctx->clientCtxPtr->getSettings();

        // 枚举JSON文件和JS文件
        StringArray scriptFiles, settingsFiles;
        EnumFiles( ctx->tpl.convFrom(settings["script_dirpath"]), "js", &scriptFiles );
        ctx->tpl.assign( "script_files", scriptFiles, true );
        EnumFiles( ctx->tpl.convFrom(settings["settings_dirpath"]), "json", &settingsFiles );
        ctx->tpl.assign( "settings_files", settingsFiles, true );

        // 查询ServerID
        auto rsServers = db->query("select * from cgaplus_servers");
        Mixed & servers = ctx->tpl.getVarContext()->set("servers").createArray();
        while ( rsServers->fetchRow(&row) ) servers.add(row);
    }
    catch ( SQLiteDbError const & e )
    {
        cout << e.what();
    }
}

void Page_cgaplussettings( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    ctx->tpl.assign( "page_title", "软件设置" );

    // 查询设置
    ctx->tpl.getVarContext()->set("settings") = ctx->clientCtxPtr->getSettings();

}

void Page_accounts( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    ctx->tpl.assign( "page_title", "通行证管理" );

    auto db = ctx->clientCtxPtr->connectDb();
    Mixed row;

    // 查询通行证
    Mixed & accounts = ctx->tpl.getVarContext()->set("accounts").createArray();
    String sql = "select * from cgaplus_accounts order by account_name";
    auto rsAccounts = db->query( db->buildStmt(sql) );
    while ( rsAccounts->fetchRow(&row) ) accounts.add(row);

}

void Page_gids( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    ctx->tpl.assign( "page_title", "GID管理" );

    if ( ctx->server->gameServerId.empty() )
    {
        // 从Cookies读取server_id
        ctx->server->gameServerId = ctx->cookies.has("server_id") ? ctx->cookies.get("server_id").value : "4";
    }
    ctx->tpl.assign( "cur_server_id", ctx->server->gameServerId, false );

    auto db = ctx->clientCtxPtr->connectDb();
    Mixed row;

    // 查询gid
    Mixed & gids = ctx->tpl.getVarContext()->set("gids").createArray();
    String sql = "select * from cgaplus_gids";
    auto rsGids = db->query( db->buildStmt(sql + ( ctx->server->gameServerId.empty() ? "" : " where server_id=?" ) + " order by account_name, gid_name", ctx->server->gameServerId ) );
    while ( rsGids->fetchRow(&row) ) gids.add(row);

    // 查询ServerID
    auto rsServers = db->query("select * from cgaplus_servers");
    Mixed & servers = ctx->tpl.getVarContext()->set("servers").createArray();
    while ( rsServers->fetchRow(&row) ) servers.add(row);
}

void Page_characters( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    ctx->tpl.assign( "page_title", "游戏角色管理" );

    if ( ctx->server->gameServerId.empty() )
    {
        // 从Cookies读取server_id
        ctx->server->gameServerId = ctx->cookies.has("server_id") ? ctx->cookies.get("server_id").value : "4";
    }
    ctx->tpl.assign( "cur_server_id", ctx->server->gameServerId, false );

    auto db = ctx->clientCtxPtr->connectDb();
    Mixed row;

    // 查询chara
    Mixed & charas = ctx->tpl.getVarContext()->set("charas").createArray();
    String sql = "select * from cgaplus_characters";
    auto rsCharas = db->query( db->buildStmt(sql + ( ctx->server->gameServerId.empty() ? "" : " where server_id=?" ), ctx->server->gameServerId ) );
    while ( rsCharas->fetchRow(&row) ) charas.add(row);

    // 查询ServerID
    auto rsServers = db->query("select * from cgaplus_servers");
    Mixed & servers = ctx->tpl.getVarContext()->set("servers").createArray();
    while ( rsServers->fetchRow(&row) ) servers.add(row);
}

void Page_addchara( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    ctx->tpl.assign( "page_title", "添加角色" );

    ctx->tpl.assign( "cur_server_id", ctx->server->gameServerId, false );

    auto db = ctx->clientCtxPtr->connectDb();
    Mixed row;

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
    while ( rsServers->fetchRow(&row) ) servers.add(row);
}

void Page_tags( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    ctx->tpl.assign( "page_title", "标签管理" );

    auto db = ctx->clientCtxPtr->connectDb();

}

void Page_tagview( CgaPlusHttpServer::PageContext * ctx )
{
    ScopeGuard guard( ctx->server->getMutex() ); // 加锁

    ctx->tpl.assign( "page_title", "标签视图" );

    auto db = ctx->clientCtxPtr->connectDb();

}
