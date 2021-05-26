#include <winux.hpp>
#include <eiennet.hpp>
#include <eientpl.hpp>
#include <eiendb.hpp>

using namespace winux;
using namespace eiennet;
using namespace eientpl;
using namespace eiendb;
using namespace http;

using namespace std;
using namespace std::placeholders;

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

#include "common.hpp"

class CgaPlusHttpServer : public ws::WsHttpServer<CgaPlusHttpClientCtx>
{
public:
    using ws::WsHttpServer<CgaPlusHttpClientCtx>::WsHttpServer;

    struct PageContext
    {
        CgaPlusHttpServer * server;
        SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr;
        http::Header const & reqHdr;
        http::Url const & url;
        http::Header & rspHdr;
        std::ostream & rspOut;
        Template & tpl;

        Vars get;
        Vars post;
        Cookies cookies;

        PageContext(
            CgaPlusHttpServer * server,
            SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr,
            Header const & reqHdr,
            Url const & url,
            Header & rspHdr,
            std::ostream & rspOut,
            Template & tpl
        ) :
            server(server),
            clientCtxPtr(clientCtxPtr),
            reqHdr(reqHdr),
            url(url),
            rspHdr(rspHdr),
            rspOut(rspOut),
            tpl(tpl)
        {
            ProcessData( clientCtxPtr, &get, &post, &cookies );
        }
    };

    using PageHandler = std::function< void( PageContext * ctx ) >;
    std::map< String, PageHandler > pageHandlers;
    // 设置页面处理函数
    void setPageHandler( String const & pageStamp, PageHandler handler )
    {
        this->setHandler( pageStamp, std::bind( &CgaPlusHttpServer::Page_index, this, _1, _2, _3, _4, _5 ) );
        this->pageHandlers[pageStamp] = handler;
    }
    // 设置动作处理函数
    void setActionHandler( String const & actionStamp, PageHandler handler )
    {
        this->setHandler( "action/" + actionStamp, std::bind( &CgaPlusHttpServer::Action_index, this, _1, _2, _3, _4, _5 ) );
        this->pageHandlers["action/" + actionStamp] = handler;
    }

    // 页面入口
    void Page_index( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, http::Header const & reqHdr, http::Url const & url, http::Header & rspHdr, std::ostream & rspOut )
    {
        Template tpl( CombinePath( this->config.documentRoot, "template" ), "gbk", "utf-8" );
        PageContext ctx( this, clientCtxPtr, reqHdr, url, rspHdr, rspOut, tpl );

        String pageStamp = url.getPath();
        if ( pageStamp.empty() ) pageStamp = "dashboard";
        String path = "";
        String page = pageStamp;

        tpl.assign( "page_stamp", pageStamp );

        if ( isset( this->pageHandlers, page ) )
        {
            this->pageHandlers[page](&ctx);
        }

        rspOut << tpl.output( page + ".html.tpl", ctx.get.toString() );
    }

    // 动作入口
    void Action_index( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, http::Header const & reqHdr, http::Url const & url, http::Header & rspHdr, std::ostream & rspOut )
    {
        Template tpl( CombinePath( this->config.documentRoot, "template" ), "gbk", "utf-8" );
        PageContext ctx( this, clientCtxPtr, reqHdr, url, rspHdr, rspOut, tpl );

        String actionStamp;
        String path = FilePath( url.getPath(), &actionStamp );
        String action = path + "/" + actionStamp;

        Mixed & result = tpl.getVarContext()->set("result");
        result.createCollection();

        if ( isset( this->pageHandlers, action ) )
        {
            this->pageHandlers[action](&ctx);
        }

        rspOut << tpl.output( "action.json.tpl", ctx.get.toString() );
    }
};

#include "actions.hpp"
#include "pages.hpp"

int main( int argc, const char * argv[] )
{
    SetLocale loc;
    SocketLib init;
    HttpServerConfig config( Configure("server.conf") );
    config.outputVerbose = false;
    CgaPlusHttpServer server(config);

    // 页面
    server.setPageHandler( "", Page_dashboard );
    server.setPageHandler( "dashboard", Page_dashboard );
    server.setPageHandler( "quikly", Page_quikly );
    server.setPageHandler( "cgasettings", Page_cgasettings );
    server.setPageHandler( "accounts", Page_accounts );
    server.setPageHandler( "characters", Page_characters );

    // 动作
    server.setActionHandler( "startupgame", Action_startupgame );
    server.setActionHandler( "quiklysave", Action_quiklysave );

    // 开启服务线程
    Thread serverThread( true, [&server] () {
        server.run();
    } );

    CommandLineVars cmdVars( argc, argv, "", "", "--no-open-browser" );

    if ( !cmdVars.hasFlag("--no-open-browser") )
    {
        auto hosts = ip::Resolver("", 9456).getArr();
        auto hostStr = hosts[hosts.size() - 1].toString();
        // 启动浏览器
        String mainPageUrl = Format( "http://127.0.0.1:%u/", config.serverPort );
        cout << "open " << mainPageUrl << endl;
        ShellExecute( NULL, "open", mainPageUrl.c_str(), NULL, NULL, SW_NORMAL );
    }

    serverThread.joined();
    return 0;
}
