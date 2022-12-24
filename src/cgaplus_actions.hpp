#pragma once

// 启动游戏
void Action_startupgame( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

// 快速保存
void Action_quiklysave( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

// 获取角色信息
void Action_getchara( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

// 改变当前服务器ID
void Action_changeserver( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

// 验证cga_gui_port是否有效
void Action_checkguiport( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

// 验证cga_game_port是否有效
void Action_checkgameport( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

// CGA设置脚本
void Action_cgasetscript( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

// CGA设置玩家配置
void Action_cgasetsettings( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

// 添加角色
void Action_addchara( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

// 修改cgaplus设置
void Action_cgaplussetsettings( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

// 获取cgaplus设置
void Action_cgaplusgetsettings( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

// 检测CGAssistant.exe的路径
void Action_detectcgapath( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

void Action_delaccount( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

void Action_delgid( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

void Action_delchara( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );

void Action_test( SharedPointer<CgaPlusHttpClientCtx> requestCtxPtr, Response & rsp, CgaPlusHttpServer & server, Template & tpl, Mixed & result );
