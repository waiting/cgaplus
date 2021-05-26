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
            <a href="cgasettings" class="nav-link{{ if(page_stamp=='cgasettings',' active') }}">CGA软件设置</a>
        </li>
        <!--<li class="nav-item">
            <a href="accounts" class="nav-link{{ if(page_stamp=='accounts',' active') }}">账号管理</a>
        </li>
        <li class="nav-item">
            <a href="characters" class="nav-link{{ if(page_stamp=='characters',' active') }}">游戏角色管理</a>
        </li>-->
    </ul>
    <div class="tab-content">
        <div class="tab-pane fade show active" id="{{page_stamp}}" role="tabpanel">
