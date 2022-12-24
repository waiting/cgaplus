#pragma once

void Page_dashboard( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl );

void Page_quikly( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl );

void Page_cgaplussettings( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl );

void Page_accounts( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl );

void Page_gids( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl );

void Page_characters( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl );

void Page_addchara( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl );

void Page_tags( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl );

void Page_tagview( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl );
