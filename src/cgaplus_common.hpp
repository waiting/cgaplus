#pragma once
class CgaPlusHttpClientCtx;

// ����Web���GET,POST,COOKIES����
void ProcessData( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, Vars * get = nullptr, Vars * post = nullptr, Cookies * cookies = nullptr );
