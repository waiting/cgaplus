<!doctype html>
<html lang="zh-CN">
<head>
    <!-- 必须的 meta 标签 -->
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <!-- Bootstrap 的 CSS 文件 -->
    <link rel="stylesheet" href="bootstrap-4.6.0-dist/css/bootstrap.min.css" />
    <script src="scripts/jquery-3.6.0.min.js"></script>
    <script src="bootstrap-4.6.0-dist/js/bootstrap.bundle.min.js"></script>
    <script src="scripts/main.js"></script>
    <title>{{page_title}}</title>
</head>

<body>
    <ul class="nav nav-tabs">
        <li class="nav-item">
            <a href="dashboard" class="nav-link{{ if(page_stamp=='dashboard',' active') }}">仪表板</a>
        </li>
        <li class="nav-item">
            <a href="quikly" class="nav-link{{ if(page_stamp=='quikly',' active') }}">快速启动</a>
        </li>
        <li class="nav-item">
            <a href="cgaplussettings" class="nav-link{{ if(page_stamp=='cgaplussettings',' active') }}">软件设置</a>
        </li>
        <li class="nav-item">
            <a href="accounts" class="nav-link{{ if(page_stamp=='accounts',' active') }}">通行证管理</a>
        </li>
        <li class="nav-item">
            <a href="gids" class="nav-link{{ if(page_stamp=='gids',' active') }}">GID管理</a>
        </li>
        <li class="nav-item">
            <a href="characters" class="nav-link{{ if(page_stamp=='characters',' active') }}">游戏角色管理</a>
        </li>
        <{if=page_stamp=='addchara'}><li class="nav-item">
            <span class="nav-link active">添加角色</span>
        </li><{/if}>
    </ul>
    <div class="tab-content">
        <div class="tab-pane fade show active" id="{{page_stamp}}" role="tabpanel">
