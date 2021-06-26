<{load=parts/header.html.tpl}><div class="container-fluid">
    <h3 style="margin-top: 8px;">GID列表</h3>
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

    <div class="row"><{loop=gids gid}>
        <div class="col-lg-2 col-md-3 col-sm-4" style="margin:1rem 0;">
            <div class="card">
                <div class="card-body">
                    <h5 class="card-title">{{gid.gid_name}}</h5>
                    <h6 class="card-subtitle">{{gid.account_name}}</h6>
                    <a class="card-link" onclick="onDeleteGid($(this).prev().prev().text());">删除</a>
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
        function onDeleteGid(gidName) {
            if ( confirm('是否删除这个Gid`'+ gidName + '`？') ) {
                $.ajax( {
                    url: 'action/delgid',
                    data: { 'gid_name': gidName },
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
