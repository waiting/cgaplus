#pragma once
class CgaPlusHttpClientCtx : public old_v1::ws::WsHttpClientCtx
{
public:
    using WsHttpClientCtx::WsHttpClientCtx;

    CgaPlusHttpClientCtx( winux::uint64 clientId, winux::String clientEpStr, winux::SharedPointer<ip::tcp::Socket> clientSockPtr )
        : WsHttpClientCtx( clientId, clientEpStr, clientSockPtr )
    {

    }

    // 连接数据库
    SqliteConnection * connectDb()
    {
        if ( !_db )
        {
            String dbPath;
            if ( this->config->constConfObj )
            {
                dbPath = CombinePath( this->config->documentRoot, this->config->constConfObj->operator()("db_path") );
            }
            else
            {
                dbPath = CombinePath( this->config->documentRoot, "data/cgaplus.sqlite" );
            }
            _db.attachNew( new SqliteConnection( dbPath, "", "utf-8" ) );
        }
        return _db.get();
    }

    // 取得cgaplus软件相关设置
    Mixed getSettings()
    {
        auto db = connectDb();
        auto rsSettings = db->query("select * from cgaplus_settings");
        Mixed settings;
        settings.createCollection();
        Mixed row;
        while ( rsSettings->fetchRow( &row, 1 ) )
        {
            settings[ row[0].toAnsi() ] = row[1];
        }
        return std::move(settings);
    }
private:
    SimplePointer<SqliteConnection> _db;
};
