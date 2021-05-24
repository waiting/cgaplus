#pragma once
// ����Web���GET,POST,COOKIES����
void ProcessData( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, Vars * get = nullptr, Vars * post = nullptr, Cookies * cookies = nullptr )
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
