#pragma once

// 启动游戏
void Action_startupgame( CgaPlusHttpServer::PageContext * ctx );

// 快速保存
void Action_quiklysave( CgaPlusHttpServer::PageContext * ctx );

// 获取角色信息
void Action_getchara( CgaPlusHttpServer::PageContext * ctx );

// 改变当前服务器ID
void Action_changeserver( CgaPlusHttpServer::PageContext * ctx );

// 验证cga_gui_port是否有效
void Action_checkguiport( CgaPlusHttpServer::PageContext * ctx );

// 验证cga_game_port是否有效
void Action_checkgameport( CgaPlusHttpServer::PageContext * ctx );

// CGA设置脚本
void Action_cgasetscript( CgaPlusHttpServer::PageContext * ctx );

// CGA设置玩家配置
void Action_cgasetsettings( CgaPlusHttpServer::PageContext * ctx );

// 添加角色
void Action_addchara( CgaPlusHttpServer::PageContext * ctx );

// 修改cgaplus设置
void Action_cgaplussetsettings( CgaPlusHttpServer::PageContext * ctx );

// 获取cgaplus设置
void Action_cgaplusgetsettings( CgaPlusHttpServer::PageContext * ctx );

// 检测CGAssistant.exe的路径
void Action_detectcgapath( CgaPlusHttpServer::PageContext * ctx );

void Action_delaccount( CgaPlusHttpServer::PageContext * ctx );

void Action_delgid( CgaPlusHttpServer::PageContext * ctx );

void Action_delchara( CgaPlusHttpServer::PageContext * ctx );

void Action_test( CgaPlusHttpServer::PageContext * ctx );
