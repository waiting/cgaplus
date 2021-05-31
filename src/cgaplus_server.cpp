#include <winux.hpp>
#include <eiennet.hpp>
#include <eientpl.hpp>
#include <eiendb.hpp>

using namespace winux;
using namespace eiennet;
using namespace eientpl;
using namespace eiendb;
using namespace http;
using namespace std::placeholders;

#include "cgaplus_common.hpp"
#include "cgaplus_clientctx.hpp"
#include "cgaplus_server.hpp"

void CgaPlusHttpServer::setPageHandler( String const & pageStamp, PageHandler handler )
{
    this->setHandler( pageStamp, std::bind( &CgaPlusHttpServer::Page_index, this, _1, _2, _3, _4, _5 ) );
    this->pageHandlers[pageStamp] = handler;
}

void CgaPlusHttpServer::setActionHandler( String const & actionStamp, PageHandler handler )
{
    this->setHandler( "action/" + actionStamp, std::bind( &CgaPlusHttpServer::Action_index, this, _1, _2, _3, _4, _5 ) );
    this->pageHandlers["action/" + actionStamp] = handler;
}

void CgaPlusHttpServer::Page_index( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, http::Header const & reqHdr, http::Url const & url, http::Header & rspHdr, std::ostream & rspOut )
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

    rspHdr.setHeader( "Access-Control-Allow-Origin", "*" );
    rspOut << tpl.output( page + ".html.tpl", ctx.get.toString() );
}

void CgaPlusHttpServer::Action_index( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, http::Header const & reqHdr, http::Url const & url, http::Header & rspHdr, std::ostream & rspOut )
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

    rspHdr.setHeader( "Access-Control-Allow-Origin", "*" );
    rspOut << tpl.output( "action.json.tpl", ctx.get.toString() );
}
