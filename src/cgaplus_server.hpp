#pragma once

class CgaPlusHttpServer : public HttpApp
{
public:
    using HttpApp::HttpApp;

    using CgaPlusPageHandlerFunction = std::function< void ( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl ) >;
    using CgaPlusActionHandlerFunction = std::function< void ( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result ) >;

    // 服务器互斥锁
    Mutex & getMutex()
    {
        return this->_mtxServer;
    }

    // 游戏区服
    String gameServerId;

    // 设置页面处理函数
    void setPageHandler( String const & pageStamp, CgaPlusPageHandlerFunction handler );
    // 设置动作处理函数
    void setActionHandler( String const & actionStamp, CgaPlusActionHandlerFunction handler );

protected:
    virtual ClientCtx * onCreateClient( winux::uint64 clientId, winux::String const & clientEpStr, winux::SharedPointer<eiennet::ip::tcp::Socket> clientSockPtr ) override;
};
