#pragma once

class CgaPlusHttpServer : public ws::WsHttpServer<CgaPlusHttpClientCtx>
{
public:
    using ws::WsHttpServer<CgaPlusHttpClientCtx>::WsHttpServer;

    // 服务器互斥锁
    Mutex & getMutex()
    {
        return this->_mtxServer;
    }

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

    // 游戏区服
    String gameServerId;

    // 设置页面处理函数
    void setPageHandler( String const & pageStamp, PageHandler handler );
    // 设置动作处理函数
    void setActionHandler( String const & actionStamp, PageHandler handler );
    // 页面入口
    void Page_index( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, http::Header const & reqHdr, http::Url const & url, http::Header & rspHdr, std::ostream & rspOut );
    // 动作入口
    void Action_index( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, http::Header const & reqHdr, http::Url const & url, http::Header & rspHdr, std::ostream & rspOut );
};
