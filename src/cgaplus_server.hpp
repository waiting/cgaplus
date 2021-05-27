#pragma once

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

    // ��Ϸ����
    String gameServerId;

    // ����ҳ�洦����
    void setPageHandler( String const & pageStamp, PageHandler handler );
    // ���ö���������
    void setActionHandler( String const & actionStamp, PageHandler handler );
    // ҳ�����
    void Page_index( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, http::Header const & reqHdr, http::Url const & url, http::Header & rspHdr, std::ostream & rspOut );
    // �������
    void Action_index( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, http::Header const & reqHdr, http::Url const & url, http::Header & rspHdr, std::ostream & rspOut );
};
