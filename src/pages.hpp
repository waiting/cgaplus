#pragma once

void Page_dashboard( CgaPlusHttpServer::PageContext * ctx )
{
    ctx->tpl.assign( "page_title", "仪表板" );
}

void Page_quikly( CgaPlusHttpServer::PageContext * ctx )
{
    ctx->tpl.assign( "page_title", "快速启动" );
    try
    {
        String sql = "select chara_id, chara_name, chara_lr, charas.gid_name, charas.server_id, account_name, server_line, autologin, skipupdate, autochangeserver, scriptautorestart, injuryprotect, soulprotect, loadscript, loadsettings from cgaplus_characters as charas left join cgaplus_gids as gids on gids.gid_name = charas.gid_name and gids.server_id = charas.server_id";
        auto rs = ctx->clientCtxPtr->getDb()->query(sql);
        Mixed & charas = ctx->tpl.getVarContext()->set("charas").createArray();
        Mixed row;
        while ( rs->fetchRow(&row) )
        {
            charas.add(row);
        }
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
