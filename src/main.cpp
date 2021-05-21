
#include <winux.hpp>
#include <eiennet.hpp>

using namespace winux;
using namespace eiennet;
using namespace http;
using namespace std;

class CgaPlusHttpClientCtx : public ws::WsHttpClientCtx
{
public:
    using WsHttpClientCtx::WsHttpClientCtx;
};

class CgaPlusHttpServer : public ws::WsHttpServer<CgaPlusHttpClientCtx>
{
public:
    using ws::WsHttpServer<CgaPlusHttpClientCtx>::WsHttpServer;

protected:

};

int main()
{
    SocketLib init;
    HttpServerConfig config( Configure("server.conf") );
    CgaPlusHttpServer server(config);

    server.setHandler( "", [] ( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, http::Header const & reqHdr, http::Url const & url, http::Header & rspHdr, std::ostream & rspOut ) {
        rspOut << "Hello world!";
    } );

    Thread serverThread( true, [&server] () {
        server.run();
    } );

    // 启动浏览器
    String mainPageUrl = Format( "http://127.0.0.1:%u/index.html", config.serverPort );
    ShellExecute( NULL, "open", mainPageUrl.c_str(), NULL, NULL, SW_NORMAL );


    serverThread.joined();
    return 0;
}
