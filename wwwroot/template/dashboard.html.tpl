<{load=parts/header.html.tpl}><div class="container-fluid">
   <div class="row">
        <div class="col-lg-8 offset-lg-2">
            <h3 style="margin-top: 8px;">统计摘要</h3>
            <table class="table table-striped">
                <tr>
                    <td class="text-right" style="white-space: nowrap;">CGA程序路径</td>
                    <td>{{settings.cga_exepath}}</td>
                </tr>
                <tr>
                    <td class="text-right" style="white-space: nowrap;">CGA脚本目录路径</td>
                    <td>{{settings.script_dirpath}}</td>
                </tr>
                <tr>
                    <td class="text-right" style="white-space: nowrap;">CGA设置文件目录路径</td>
                    <td>{{settings.settings_dirpath}}</td>
                </tr>
                <tr>
                    <td class="text-right" style="white-space: nowrap;">通行证数</td>
                    <td>{{accounts_count}}</td>
                </tr>
                <tr>
                    <td class="text-right" style="white-space: nowrap;">GID数</td>
                    <td>{{gids_count}}</td>
                </tr>
                <tr>
                    <td class="text-right" style="white-space: nowrap;">游戏角色数</td>
                    <td>{{characters_count}}</td>
                </tr>
            </table>
            <style>
                iframe.myframe {
                    border: none; width: 100%; height: 84px;
                }
                table.c12px {
                    border: none;
                }

            </style>
            <h3 style="margin-top: 8px;">道具电信服务器状态</h3>
            <iframe src="http://221.122.119.109/news/4.html" class="myframe"></iframe>
            </script>
            <h3 style="margin-top: 8px;">道具网通服务器状态</h3>
            <iframe src="http://221.122.119.125/news/4.html" class="myframe"></iframe>
        </div>
    </div>
</div>
<{load=parts/footer.html.tpl}>
