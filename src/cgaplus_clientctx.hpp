#pragma once
class CgaPlusHttpClientCtx : public ws::WsHttpClientCtx
{
public:
    using WsHttpClientCtx::WsHttpClientCtx;

    CgaPlusHttpClientCtx( winux::uint64 clientId, winux::String clientEpStr, winux::SharedPointer<ip::tcp::Socket> clientSockPtr )
        : WsHttpClientCtx( clientId, clientEpStr, clientSockPtr )
    {

    }

    // 数据库
    SQLiteConnection * getDb()
    {
        if ( !_db ) _db.attachNew( new SQLiteConnection( CombinePath( this->config->documentRoot, "data/cgaplus.sqlite" ), "", "utf-8" ) );
        return _db.get();
    }

    // 取得cgaplus软件相关设置
    Mixed getSettings()
    {
        auto rsSettings = getDb()->query("select * from cgaplus_settings");
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
    SimplePointer<SQLiteConnection> _db;
};
