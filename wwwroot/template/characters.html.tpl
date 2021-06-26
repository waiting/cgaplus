<{load=parts/header.html.tpl}><div class="container-fluid">
    <h3 style="margin-top: 8px;">游戏角色列表</h3>
    <table>
        <tr>
            <td style="padding: 4px;" class="form-inline">
                <span>选择服务器：</span>
                <select class="custom-control custom-select" onchange="onChangeServer(this.value);"><{loop=servers server}>
                    <option value="{{server.server_id}}"{{if(cur_server_id==server.server_id, ' selected')}}>{{server.server_name}}</option><{/loop}>
                </select>
            </td>
        </tr>
    </table>
    
    <div class="row"><{loop=charas chara}>
        <div class="col-lg-2 col-md-3 col-sm-4" style="margin:1rem 0;">
            <div class="card">
                <div class="card-body">
                    <h5 class="card-title">{{chara.chara_name}}</h5>
                    <h6 class="card-subtitle">{{chara.gid_name}}</h6>
                    <a class="card-link" onclick="onDeleteChara($(this).prev().prev().text());">删除</a>
                </div>
            </div>
        </div><{/loop}>
        <script>
        function onChangeServer(serverId) {
            $.ajax( {
                url: 'action/changeserver',
                data: { server_id: serverId },
                dataType: 'json',
                success: function(data) {
                    console.log(data);

                    if ( !data.error ) {
                        window.location.reload(true);
                    }
                }
            } );
        }
        function onDeleteChara(charaName) {
            if ( confirm('是否删除这个角色`'+ charaName + '`？') ) {
                $.ajax( {
                    url: 'action/delchara',
                    data: { 'chara_name': charaName },
                    dataType: 'json',
                    success: function(data) {
                        console.log(data);
                        if ( !data.error ) {
                            window.location.reload(true);
                        }
                        else {
                            alert(data.error);
                        }
                    }
                } );
            }
        }
        </script>
    </div>
</div>
<{load=parts/footer.html.tpl}>
