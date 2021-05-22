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
};

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
            this->processData();
        }

        void processData()
        {
            this->get.parse( clientCtxPtr->url.getRawQueryStr() );
            this->post.parse( clientCtxPtr->requestBody.toAnsi() );
            auto arr = clientCtxPtr->requestHeader.getHeaders("Cookie");
            for ( auto &fieldvalue : arr )
            {
                this->cookies.loadCookies(fieldvalue);
            }
        }
    };

    using PageHandler = std::function< void( PageContext * ctx ) >;
    std::map< String, PageHandler > pageHandlers;
    void setPageHandler( String const & pageStamp, PageHandler handler )
    {
        this->setHandler( pageStamp, std::bind( &CgaPlusHttpServer::Page_index, this, _1, _2, _3, _4, _5 ) );
        this->pageHandlers[pageStamp] = handler;
    }

    void Page_index( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, http::Header const & reqHdr, http::Url const & url, http::Header & rspHdr, std::ostream & rspOut )
    {
        Template tpl( CombinePath( this->config.documentRoot, "template" ), "gbk", "utf-8" );
        PageContext ctx( this, clientCtxPtr, reqHdr, url, rspHdr, rspOut, tpl );

        String pageStamp = url.getPath();
        if ( pageStamp.empty() ) pageStamp = "dashboard";
        tpl.assign( "page_stamp", pageStamp );

        if ( isset( this->pageHandlers, pageStamp ) )
        {
            this->pageHandlers[pageStamp](&ctx);
        }

        rspOut << tpl.output( pageStamp + ".html.tpl", ctx.get.toString() );
    }

};

int main()
{
    SetLocale loc;
    SocketLib init;
    HttpServerConfig config( Configure("server.conf") );
    CgaPlusHttpServer server(config);

    server.setPageHandler( "", [] ( CgaPlusHttpServer::PageContext * ctx ) {} );
    server.setPageHandler( "dashboard", [] ( CgaPlusHttpServer::PageContext * ctx ) {
        ctx->tpl.assign( "page_title", "仪表板" );
    } );
    server.setPageHandler( "quikly", [] ( CgaPlusHttpServer::PageContext * ctx ) {
        ctx->tpl.assign( "page_title", "快速启动" );
        try
        {
            String sql = "select chara_id, chara_name, chara_lr, charas.gid_name, charas.server_id, account_name, autologin, skipupdate, autochangeserver, scriptautorestart, injuryprotect, soulprotect, loadscript, loadsettings from cgaplus_characters as charas left join cgaplus_gids as gids on gids.gid_name = charas.gid_name and gids.server_id = charas.server_id";
            SQLiteConnection db( CombinePath( ctx->server->config.documentRoot, "data/cgaplus.sqlite" ), "", "utf-8" );
            auto rs = db.query(sql);
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
        catch ( ... )
        {
        }

    } );
    server.setPageHandler( "cgasettings", [] ( CgaPlusHttpServer::PageContext * ctx ) {
    } );
    server.setPageHandler( "accounts", [] ( CgaPlusHttpServer::PageContext * ctx ) {
    } );
    server.setPageHandler( "characters", [] ( CgaPlusHttpServer::PageContext * ctx ) {
    } );


    Thread serverThread( true, [&server] () {
        server.run();
    } );

    auto hosts = ip::Resolver("", 9456).getArr();
    auto hostStr = hosts[hosts.size() - 1].toString();
    // 启动浏览器
    String mainPageUrl = Format( "http://127.0.0.1:%u/", config.serverPort );
    cout << "open " << mainPageUrl << endl;
    ShellExecute( NULL, "open", mainPageUrl.c_str(), NULL, NULL, SW_NORMAL );


    serverThread.joined();
    return 0;
}
