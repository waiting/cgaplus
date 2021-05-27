#pragma once
class CgaPlusHttpClientCtx;

// 处理Web相关GET,POST,COOKIES数据
void ProcessData( SharedPointer<CgaPlusHttpClientCtx> & clientCtxPtr, Vars * get = nullptr, Vars * post = nullptr, Cookies * cookies = nullptr );
