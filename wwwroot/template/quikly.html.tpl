<{load=parts/header.html.tpl}><div class="container-fluid">
    <h3 style="margin-top: 8px;">可启动角色列表</h3>
    <table class="table table-hover">
    <thead>
    <tr>
      <th scope="col">角色</th>
      <th scope="col">左右</th>
      <th scope="col">GID</th>
      <th scope="col">服务器</th>
      <th scope="col">通行证</th>
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
    <tbody>
<{loop=charas chara}><tr>
        <td>{{chara.chara_name}}</td>
        <td><select id="chara{{chara.chara_id}}-chara_lr">
            <option value="1"{{ if(chara.chara_lr==1, ' selected')}}>左</option>
            <option value="2"{{ if(chara.chara_lr==2, ' selected')}}>右</option>
        </select></td>
        <td><input type="hidden" id="chara{{chara.chara_id}}-gid_name" value="{{chara.gid_name}}" /> *</td>
        <td><select id="chara{{chara.chara_id}}-server_id">
            <option value="4"{{ if(chara.server_id==4, ' selected') }}>道具电信</option>
            <option value="40"{{ if(chara.server_id==40, ' selected') }}>道具网通</option>
        </select></td>
        <td><input type="hidden" id="chara{{chara.chara_id}}-account_name" value="{{chara.account_name}}" /> *</td>
        <td><input type="checkbox" id="chara{{chara.chara_id}}-autologin"{{ if(chara.autologin, ' checked')}} /></td>
        <td><input type="checkbox" id="chara{{chara.chara_id}}-skipupdate"{{ if(chara.skipupdate, ' checked')}} /></td>
        <td><input type="checkbox" id="chara{{chara.chara_id}}-autochangeserver"{{ if(chara.autochangeserver, ' checked')}} /></td>
        <td><input type="checkbox" id="chara{{chara.chara_id}}-scriptautorestart"{{ if(chara.scriptautorestart, ' checked')}} /></td>
        <td><input type="checkbox" id="chara{{chara.chara_id}}-injuryprotect"{{ if(chara.injuryprotect, ' checked')}} /></td>
        <td><input type="checkbox" id="chara{{chara.chara_id}}-soulprotect"{{ if(chara.soulprotect, ' checked')}} /></td>
        <td><input type="text" id="chara{{chara.chara_id}}-loadscript" value="{{chara.loadscript}}" /></td>
        <td><input type="text" id="chara{{chara.chara_id}}-loadsettings" value="{{chara.loadsettings}}" /></td>
        <td>
            <button class="btn btn-primary" onclick="onSave({{chara.chara_id}});">保存</button>
            <button class="btn btn-success" onclick="onStartup({{chara.chara_id}});">启动</button>
        </td>
    </tr><{/loop}>
    </tbody>
    </table>
    <script>
        function onSave(id) {
            var chara = {
                chara_id: id,
                chara_lr: $('#chara'+id+'-chara_lr').val(),
                gid_name: $('#chara'+id+'-gid_name').val(),
                server_id: $('#chara'+id+'-server_id').val(),
                account_name: $('#chara'+id+'-account_name').val(),
                autologin: $('#chara'+id+'-autologin')[0].checked,
                skipupdate: $('#chara'+id+'-skipupdate')[0].checked,
                autochangeserver: $('#chara'+id+'-autochangeserver')[0].checked,
                scriptautorestart: $('#chara'+id+'-scriptautorestart')[0].checked,
                injuryprotect: $('#chara'+id+'-injuryprotect')[0].checked,
                soulprotect: $('#chara'+id+'-soulprotect')[0].checked,
                loadscript: $('#chara'+id+'-loadscript').val(),
                loadsettings: $('#chara'+id+'-loadsettings').val(),
            };
            
            console.log(chara);
        }
        function onStartup(id) {
            
        }
    </script>
</div>
<{load=parts/footer.html.tpl}>
