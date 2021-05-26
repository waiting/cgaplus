<{load=parts/header.html.tpl}><div class="container-fluid">
    <h3 style="margin-top: 8px;">可启动角色列表</h3>
    <table class="">
        <tr>
            <td><a class="btn btn-primary">快速添加角色</a></td>
            <td>
                <div class="dropdown">
                    <input type="text" class="form-control dropdown-toggle" data-toggle="dropdown" value="" />
                    <select class="form-control dropdown-menu" size="10" onclick="$(this).prev().val(this.value);">
                        <option value="Action 2">Action 2</option>
                        <option value="Action 4">Action 4</option>
                        <option value="Action 6">Action 6</option>
                        <option value="Action 7 6 5 4 3 2 1">Action 7 6 5 4 3 2 1</option>
                    </select>
                </div>

            </td>
        </tr>
    </table>

    <hr />
    <script>
        // 脚本和设置文件
        var scriptFiles = {{ script_files }};
        var settingsFiles = {{ settings_files }};
    </script>
    <table class="table table-hover table-sm">
    <thead>
    <tr>
      <th scope="col">角色</th>
      <th scope="col">左右</th>
      <th scope="col">服务器</th>
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
            {{chara.chara_name}}
            <input type="hidden" id="chara{{chara.chara_id}}-gid_name" value="{{chara.gid_name}}" />
            <input type="hidden" id="chara{{chara.chara_id}}-account_name" value="{{chara.account_name}}" />
        </td>
        <td><select class="form-control form-control-sm" id="chara{{chara.chara_id}}-chara_lr">
            <option value="1"{{ if(chara.chara_lr==1, ' selected')}}>左</option>
            <option value="2"{{ if(chara.chara_lr==2, ' selected')}}>右</option>
        </select></td>
        <td><select class="form-control form-control-sm" id="chara{{chara.chara_id}}-server_id">
            <option value="4"{{ if(chara.server_id==4, ' selected') }}>道具电信</option>
            <option value="40"{{ if(chara.server_id==40, ' selected') }}>道具网通</option>
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
                <select class="form-control dropdown-menu" size="10" onclick="$(this).prev().val(this.value);"><{loop=script_files script_file}>
                    <option value="{{script_file}}">{{script_file}}</option><{/loop}>
                </select>
            </div>
        </td>
        <td>
            <div class="dropdown dropdown-settings_files">
                <input type="text" class="form-control form-control-sm dropdown-toggle" data-toggle="dropdown" id="chara{{chara.chara_id}}-loadsettings" value="{{chara.loadsettings}}" />
                <select class="form-control dropdown-menu" size="10" onclick="$(this).prev().val(this.value);"><{loop=settings_files settings_file}>
                    <option value="{{settings_file}}">{{settings_file}}</option><{/loop}>
                </select>
            </div>
        </td>
        <td>
            <button class="btn btn-info btn-sm" onclick="onSave({{chara.chara_id}});">保存</button>
            <button class="btn btn-success btn-sm" onclick="onStartup({{chara.chara_id}});">启动</button>
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
        function onSave(id) {
            $.ajax( {
                url: 'action/quiklysave',
                data: { 'chara': JSON.stringify( getChara(id) ) },
                dataType: 'json',
                success: function(data) {
                    console.log(data);
                    //console.log( eval('('+data+')') );
                }
            } );
        }
        function onStartup(id) {
            $.ajax( {
                url: 'action/startupgame',
                data: { 'chara': JSON.stringify( getChara(id) ) },
                dataType: 'json',
                success: function(data) {
                    console.log(data);
                    //console.log( eval('('+data+')') );
                }
            } );
        }
    </script>
</div>
<{load=parts/footer.html.tpl}>
