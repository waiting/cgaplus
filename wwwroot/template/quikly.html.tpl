<{load=parts/header.html.tpl}><div class="container-fluid">
    <h3 style="margin-top: 8px;">可启动角色列表</h3>
    <table class="">
        <tr>
            <td style="padding: 4px;"><a class="btn btn-primary">快速添加角色</a></td>
            <td style="padding: 4px;">
                <select class="form-control" onchange="onChangeServer(this.value);"><{loop=servers server}>
                    <option value="{{server.server_id}}"{{if(cur_server_id==server.server_id, ' selected')}}>{{server.server_name}}</option><{/loop}>
                </select>
            </td>
        </tr>
    </table>

    <script>
        var charaIds = [ <{loop=charas chara}>{{ chara.chara_id }}, <{/loop}> ];
        // 脚本和设置文件
        var scriptFiles = {{ script_files }};
        var settingsFiles = {{ settings_files }};
    </script>
    <table class="table table-hover table-sm">
    <thead>
    <tr>
      <th scope="col">角色</th>
      <th scope="col">左右</th>
      <th scope="col">线路</th>
      <th scope="col">自动登录</th>
      <th scope="col">跳过更新</th>
      <th scope="col">自动换线</th>
      <th scope="col">脚本自动重启</th>
      <th scope="col">受伤保护</th>
      <th scope="col">掉魂保护</th>
      <th scope="col">加载脚本</th>
      <th scope="col">加载设置</th>
      <th scope="col">操作</th>
    </tr>
    </thead>
    <tbody><{loop=charas chara}>
    <tr>
        <td>
            <span id="chara{{chara.chara_id}}-chara_name">{{chara.chara_name}}</span>
            <input type="hidden" id="chara{{chara.chara_id}}-gid_name" value="{{chara.gid_name}}" />
            <input type="hidden" id="chara{{chara.chara_id}}-server_id" value="{{chara.server_id}}" />
            <input type="hidden" id="chara{{chara.chara_id}}-account_name" value="{{chara.account_name}}" />
        </td>
        <td><select class="form-control form-control-sm" id="chara{{chara.chara_id}}-chara_lr">
            <option value="1"{{ if(chara.chara_lr==1, ' selected')}}>左</option>
            <option value="2"{{ if(chara.chara_lr==2, ' selected')}}>右</option>
        </select></td>
        <td><select class="form-control form-control-sm" id="chara{{chara.chara_id}}-server_line"><{for i=1 to=10}>
            <option value="{{i}}"{{ if(chara.server_line==i, ' selected') }}>{{i}}线</option><{/for}>
        </select></td>
        <td><input type="checkbox" class="form-control form-control-sm" id="chara{{chara.chara_id}}-autologin"{{ if(chara.autologin, ' checked')}} /></td>
        <td><input type="checkbox" class="form-control form-control-sm" id="chara{{chara.chara_id}}-skipupdate"{{ if(chara.skipupdate, ' checked')}} /></td>
        <td><input type="checkbox" class="form-control form-control-sm" id="chara{{chara.chara_id}}-autochangeserver"{{ if(chara.autochangeserver, ' checked')}} /></td>
        <td><input type="checkbox" class="form-control form-control-sm" id="chara{{chara.chara_id}}-scriptautorestart"{{ if(chara.scriptautorestart, ' checked')}} /></td>
        <td><input type="checkbox" class="form-control form-control-sm" id="chara{{chara.chara_id}}-injuryprotect"{{ if(chara.injuryprotect, ' checked')}} /></td>
        <td><input type="checkbox" class="form-control form-control-sm" id="chara{{chara.chara_id}}-soulprotect"{{ if(chara.soulprotect, ' checked')}} /></td>
        <td>
            <div class="dropdown dropdown-script_files">
                <input type="text" class="form-control form-control-sm dropdown-toggle" data-toggle="dropdown" id="chara{{chara.chara_id}}-loadscript" value="{{chara.loadscript}}" />
                <select class="form-control dropdown-menu" size="10" onclick="$(this).prev().val(this.value);">
                    <script>
                    scriptFiles.forEach( function(f) {
                        document.write('<option value="' + f + '">' + f + '</option>');
                    } );
                    </script>
                </select>
            </div>
        </td>
        <td>
            <div class="dropdown dropdown-settings_files">
                <input type="text" class="form-control form-control-sm dropdown-toggle" data-toggle="dropdown" id="chara{{chara.chara_id}}-loadsettings" value="{{chara.loadsettings}}" />
                <select class="form-control dropdown-menu" size="10" onclick="$(this).prev().val(this.value);">
                    <script>
                    settingsFiles.forEach( function(f) {
                        document.write('<option value="' + f + '">' + f + '</option>');
                    } );
                    </script>
                </select>
            </div>
        </td>
        <td>
            <button class="btn btn-info btn-sm" id="btn-chara{{chara.chara_id}}-save" onclick="onBtnSave(this,{{chara.chara_id}});">保存</button>
            <button class="btn btn-success btn-sm" id="btn-chara{{chara.chara_id}}-startup" onclick="onBtnStartup(this,{{chara.chara_id}});">启动</button>
        </td>
    </tr><{/loop}>
    </tbody>
    </table>
    <script>
        function getChara(id) {
            var chara = {
                chara_id: id,
                chara_lr: $('#chara'+id+'-chara_lr').val(),
                gid_name: $('#chara'+id+'-gid_name').val(),
                server_id: $('#chara'+id+'-server_id').val(),
                account_name: $('#chara'+id+'-account_name').val(),
                server_line: $('#chara'+id+'-server_line').val(),
                autologin: $('#chara'+id+'-autologin')[0].checked+0,
                skipupdate: $('#chara'+id+'-skipupdate')[0].checked+0,
                autochangeserver: $('#chara'+id+'-autochangeserver')[0].checked+0,
                scriptautorestart: $('#chara'+id+'-scriptautorestart')[0].checked+0,
                injuryprotect: $('#chara'+id+'-injuryprotect')[0].checked+0,
                soulprotect: $('#chara'+id+'-soulprotect')[0].checked+0,
                loadscript: $('#chara'+id+'-loadscript').val(),
                loadsettings: $('#chara'+id+'-loadsettings').val(),
            };
            return chara;
        }
        function onBtnSave(elem, id) {
            elem.disabled = true;
            $.ajax( {
                url: 'action/quiklysave',
                data: getChara(id),
                dataType: 'json',
                success: function(data) {
                    //console.log(data);
                    if ( !data.error ) {
                        elem.disabled = false;
                    }
                }
            } );
        }
        function onBtnStartup(elem, id) {
            elem.disabled = true;
            $.ajax( {
                url: 'action/startupgame',
                data: getChara(id),
                dataType: 'json',
                success: function(data) {
                    console.log(data);
                }
            } );
        }
        function onChangeServer(serverId) {
            $.ajax( {
                url: 'action/changeserver',
                data: { server_id: serverId },
                dataType: 'json',
                success: function(data) {
                    if ( !data.error ) {
                        window.location.reload();
                    }
                }
            } );
        }
    </script>
</div>
<{load=parts/footer.html.tpl}>
