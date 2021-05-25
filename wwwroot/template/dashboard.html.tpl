<{load=parts/header.html.tpl}><div class="container-fluid">
    <h3 style="margin-top: 8px;">摘要</h3>
    <table class="table table-bordered">
        <tr>
            <td class="text-right" style="width:15%; white-space: nowrap;">CGA程序路径</td>
            <td>{{settings.cga_exepath}}</td>
        </tr>
        <tr>
            <td class="text-right" style="width:15%; white-space: nowrap;">CGA脚本目录路径</td>
            <td>{{settings.script_dirpath}}</td>
        </tr>
        <tr>
            <td class="text-right" style="width:15%; white-space: nowrap;">CGA设置文件目录路径</td>
            <td>{{settings.settings_dirpath}}</td>
        </tr>
        <tr>
            <td class="text-right" style="width:15%; white-space: nowrap;">通行证数</td>
            <td>{{accounts_count}}</td>
        </tr>
        <tr>
            <td class="text-right" style="width:15%; white-space: nowrap;">GID数</td>
            <td>{{gids_count}}</td>
        </tr>
        <tr>
            <td class="text-right" style="width:15%; white-space: nowrap;">游戏角色数</td>
            <td>{{characters_count}}</td>
        </tr>
    </table>
</div>
<{load=parts/footer.html.tpl}>
