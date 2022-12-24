#pragma once

class CgaPlusHttpClientCtx : public HttpRequestCtx
{
public:
    using HttpRequestCtx::HttpRequestCtx;

    // 连接数据库
    SqliteConnection * connectDb();

    // 取得cgaplus软件相关设置
    Mixed getSettings();
private:
    SimplePointer<SqliteConnection> _db;
};
