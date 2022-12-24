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
using namespace std;

#include "cgaplus_common.hpp"
#include "cgaplus_clientctx.hpp"
#include "cgaplus_server.hpp"
#include "cgaplus_pages.hpp"


void Page_dashboard( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl )
{
    Mixed & settings = tpl.getVarContext()->set("settings");
    {
        ScopeGuard guard( server.getMutex() ); // 加锁
        settings = requestCtxPtr->getSettings();
    }
    // 如果没有设置CGA程序路径，需要到软件设置里设置
    if ( !settings["cga_exepath"] || !DetectPath( tpl.convFrom(settings["cga_exepath"]) ) )
    {
        tpl.assign( "page_stamp", "cgaplussettings" );
        Page_cgaplussettings( requestCtxPtr, rsp, server, tpl );
        return;
    }

    ScopeGuard guard( server.getMutex() ); // 加锁

    tpl.assign( "page_title", "仪表板" );

    auto db = requestCtxPtr->connectDb();

    Mixed row;

    auto rs1 = db->query("select count(*) from cgaplus_accounts");
    if ( rs1->fetchRow(&row, 1) )
        tpl.getVarContext()->set("accounts_count") = row[0];

    auto rs2 = db->query("select count(*) from cgaplus_gids");
    if ( rs2->fetchRow(&row, 1) )
        tpl.getVarContext()->set("gids_count") = row[0];

    auto rs3 = db->query("select count(*) from cgaplus_characters");
    if ( rs3->fetchRow(&row, 1) )
        tpl.getVarContext()->set("characters_count") = row[0];

}

void Page_quikly( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl )
{
    ScopeGuard guard( server.getMutex() ); // 加锁

    tpl.assign( "page_title", "快速角色启动" );

    if ( server.gameServerId.empty() )
    {
        // 从Cookies读取server_id
        server.gameServerId = requestCtxPtr->cookies.has("server_id") ? requestCtxPtr->cookies.get("server_id").value : "4";
    }
    tpl.assign( "cur_server_id", server.gameServerId, false );

    try
    {
        auto db = requestCtxPtr->connectDb();
        Mixed row;

        // 查询角色
        String sql = "select chara_id, chara_name, chara_lr, charas.gid_name, charas.server_id, account_name, server_line, chara_level, chara_job, autologin, skipupdate, autochangeserver, scriptautorestart, injuryprotect, soulprotect, loadscript, loadsettings, cga_port from cgaplus_characters as charas left join cgaplus_gids as gids on gids.gid_name = charas.gid_name and gids.server_id = charas.server_id";
        auto rs = db->query( db->buildStmt( sql + ( server.gameServerId.empty() ? "" : " where charas.server_id=?" ), server.gameServerId ) );
        Mixed & charas = tpl.getVarContext()->set("charas").createArray();
        while ( rs->fetchRow(&row) ) charas.add(row);

        // 查询设置
        Mixed & settings = tpl.getVarContext()->set("settings");
        settings = requestCtxPtr->getSettings();

        // 枚举JSON文件和JS文件
        StringArray scriptFiles, settingsFiles;
        EnumFiles( tpl.convFrom(settings["script_dirpath"]), "js", &scriptFiles );
        tpl.assign( "script_files", scriptFiles, true );
        EnumFiles( tpl.convFrom(settings["settings_dirpath"]), "json", &settingsFiles );
        tpl.assign( "settings_files", settingsFiles, true );

        // 查询ServerID
        auto rsServers = db->query("select * from cgaplus_servers");
        Mixed & servers = tpl.getVarContext()->set("servers").createArray();
        while ( rsServers->fetchRow(&row) ) servers.add(row);
    }
    catch ( SqliteDbError const & e )
    {
        cout << e.what();
    }
}

void Page_cgaplussettings( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl )
{
    ScopeGuard guard( server.getMutex() ); // 加锁

    tpl.assign( "page_title", "软件设置" );

    // 查询设置
    tpl.getVarContext()->set("settings") = requestCtxPtr->getSettings();

}

void Page_accounts( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl )
{
    ScopeGuard guard( server.getMutex() ); // 加锁

    tpl.assign( "page_title", "通行证管理" );

    auto db = requestCtxPtr->connectDb();
    Mixed row;

    // 查询通行证
    Mixed & accounts = tpl.getVarContext()->set("accounts").createArray();
    String sql = "select * from cgaplus_accounts order by account_name";
    auto rsAccounts = db->query( db->buildStmt(sql) );
    while ( rsAccounts->fetchRow(&row) ) accounts.add(row);

}

void Page_gids( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl )
{
    ScopeGuard guard( server.getMutex() ); // 加锁

    tpl.assign( "page_title", "GID管理" );

    if ( server.gameServerId.empty() )
    {
        // 从Cookies读取server_id
        server.gameServerId = requestCtxPtr->cookies.has("server_id") ? requestCtxPtr->cookies.get("server_id").value : "4";
    }
    tpl.assign( "cur_server_id", server.gameServerId, false );

    auto db = requestCtxPtr->connectDb();
    Mixed row;

    // 查询gid
    Mixed & gids = tpl.getVarContext()->set("gids").createArray();
    String sql = "select * from cgaplus_gids";
    auto rsGids = db->query( db->buildStmt(sql + ( server.gameServerId.empty() ? "" : " where server_id=?" ) + " order by account_name, gid_name", server.gameServerId ) );
    while ( rsGids->fetchRow(&row) ) gids.add(row);

    // 查询ServerID
    auto rsServers = db->query("select * from cgaplus_servers");
    Mixed & servers = tpl.getVarContext()->set("servers").createArray();
    while ( rsServers->fetchRow(&row) ) servers.add(row);
}

void Page_characters( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl )
{
    ScopeGuard guard( server.getMutex() ); // 加锁

    tpl.assign( "page_title", "游戏角色管理" );

    if ( server.gameServerId.empty() )
    {
        // 从Cookies读取server_id
        server.gameServerId = requestCtxPtr->cookies.has("server_id") ? requestCtxPtr->cookies.get("server_id").value : "4";
    }
    tpl.assign( "cur_server_id", server.gameServerId, false );

    auto db = requestCtxPtr->connectDb();
    Mixed row;

    // 查询chara
    Mixed & charas = tpl.getVarContext()->set("charas").createArray();
    String sql = "select * from cgaplus_characters";
    auto rsCharas = db->query( db->buildStmt(sql + ( server.gameServerId.empty() ? "" : " where server_id=?" ), server.gameServerId ) );
    while ( rsCharas->fetchRow(&row) ) charas.add(row);

    // 查询ServerID
    auto rsServers = db->query("select * from cgaplus_servers");
    Mixed & servers = tpl.getVarContext()->set("servers").createArray();
    while ( rsServers->fetchRow(&row) ) servers.add(row);
}

void Page_addchara( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl )
{
    ScopeGuard guard( server.getMutex() ); // 加锁

    tpl.assign( "page_title", "添加角色" );

    tpl.assign( "cur_server_id", server.gameServerId, false );

    auto db = requestCtxPtr->connectDb();
    Mixed row;

    // 查询设置
    Mixed settings = requestCtxPtr->getSettings();

    // 枚举JSON文件和JS文件
    StringArray scriptFiles, settingsFiles;
    EnumFiles( tpl.convFrom(settings["script_dirpath"]), "js", &scriptFiles );
    tpl.assign( "script_files", scriptFiles, true );
    EnumFiles( tpl.convFrom(settings["settings_dirpath"]), "json", &settingsFiles );
    tpl.assign( "settings_files", settingsFiles, true );

    // 查询ServerID
    auto rsServers = db->query("select * from cgaplus_servers");
    Mixed & servers = tpl.getVarContext()->set("servers").createArray();
    while ( rsServers->fetchRow(&row) ) servers.add(row);
}

void Page_tags( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl )
{
    ScopeGuard guard( server.getMutex() ); // 加锁

    tpl.assign( "page_title", "标签管理" );

    auto db = requestCtxPtr->connectDb();

}

void Page_tagview( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl )
{
    ScopeGuard guard( server.getMutex() ); // 加锁

    tpl.assign( "page_title", "标签视图" );

    auto db = requestCtxPtr->connectDb();

}
