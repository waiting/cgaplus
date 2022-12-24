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

#include "cgaplus_clientctx.hpp"

SqliteConnection * CgaPlusHttpClientCtx::connectDb()
{
    if ( !_db )
    {
        String dbPath;
        HttpApp * httpApp = static_cast<HttpApp *>( this->app );
        HttpServerConfig & httpConfig = httpApp->httpConfig;
        if ( httpConfig.confSettings )
        {
            dbPath = CombinePath(httpConfig.documentRoot, httpConfig.confSettings->execVal("site.db_path"));
        }
        else
        {
            dbPath = CombinePath(httpConfig.documentRoot, "data/cgaplus.sqlite");
        }
        _db.attachNew(new SqliteConnection(dbPath, "", "utf-8"));
    }
    return _db.get();
}

winux::Mixed CgaPlusHttpClientCtx::getSettings()
{
    auto db = connectDb();
    auto rsSettings = db->query("select * from cgaplus_settings");
    Mixed settings;
    settings.createCollection();
    Mixed row;
    while ( rsSettings->fetchRow(&row, 1) )
    {
        settings[row[0].toAnsi()] = row[1];
    }
    return settings;
}
