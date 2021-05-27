#include <winux.hpp>
#include <eiennet.hpp>
#include <eientpl.hpp>
#include <eiendb.hpp>

using namespace winux;
using namespace eiennet;
using namespace eientpl;
using namespace eiendb;
using namespace http;

#include "cgaplus_common.hpp"
#include "cgaplus_clientctx.hpp"
#include "cgaplus_server.hpp"

// 处理Web相关GET,POST,COOKIES数据
void ProcessData( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, Vars * get, Vars * post, Cookies * cookies )
{
    if ( get ) get->parse( clientCtxPtr->url.getRawQueryStr() );
    if ( post ) post->parse( clientCtxPtr->requestBody.toAnsi() );
    if ( cookies )
    {
        auto arr = clientCtxPtr->requestHeader.getHeaders("Cookie");
        for ( auto &fieldvalue : arr )
        {
            cookies->loadCookies(fieldvalue);
        }
    }
}
