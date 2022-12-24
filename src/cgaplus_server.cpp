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
using namespace std::placeholders;

#include "cgaplus_common.hpp"
#include "cgaplus_clientctx.hpp"
#include "cgaplus_server.hpp"

void CgaPlusHttpServer::setPageHandler( String const & pageStamp, CgaPlusPageHandlerFunction handler )
{
    this->route( "*", "/" + pageStamp, [=] ( SharedPointer<HttpRequestCtx> requestCtxPtr, Response & rsp ) {
        Template tpl( CombinePath( this->httpConfig.documentRoot, "template" ), "gbk", "utf-8" );

        String page_stamp = pageStamp;
        if ( page_stamp.empty() ) page_stamp = "dashboard";
        tpl.assign( "page_stamp", page_stamp );

        handler( requestCtxPtr.ensureCast<CgaPlusHttpClientCtx>(), rsp, *this, tpl );

        rsp.header.setHeader( "Access-Control-Allow-Origin", "*" );
        rsp << tpl.output( page_stamp + ".html.tpl", requestCtxPtr->get.toString() );
    } );
}

void CgaPlusHttpServer::setActionHandler( String const & actionStamp, CgaPlusActionHandlerFunction handler )
{
    this->route( "*", "/action/" + actionStamp, [=] ( SharedPointer<HttpRequestCtx> requestCtxPtr, Response & rsp ) {
        Template tpl( CombinePath( this->httpConfig.documentRoot, "template" ), "gbk", "utf-8" );
        Mixed & result = tpl.getVarContext()->set("result");
        result.createCollection();

        handler( requestCtxPtr.ensureCast<CgaPlusHttpClientCtx>(), rsp, *this, tpl, result );

        rsp.header.setHeader( "Access-Control-Allow-Origin", "*" );
        rsp << tpl.output( "action.json.tpl", requestCtxPtr->get.toString() );
    } );
}

ClientCtx * CgaPlusHttpServer::onCreateClient( uint64 clientId, String const & clientEpStr, SharedPointer<ip::tcp::Socket> clientSockPtr )
{
    return new CgaPlusHttpClientCtx( this, clientId, clientEpStr, clientSockPtr );
}
