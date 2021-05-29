<{load=parts/header.html.tpl}><div class="container-fluid">
    <h3 style="margin-top: 8px;">可启动角色列表</h3>
    <table class="">
        <tr>
            <td style="padding: 4px;" class="form-inline">
                <span>选择服务器：</span>
                <select class="custom-control custom-select" onchange="onChangeServer(this.value);"><{loop=servers server}>
                    <option value="{{server.server_id}}"{{if(cur_server_id==server.server_id, ' selected')}}>{{server.server_name}}</option><{/loop}>
                </select>
            </td>
            <td style="padding: 4px;"><a class="btn btn-link" href="addchara">添加角色</a></td>
            <td style="padding: 4px;" class="form-inline">
                <span>批量操作：</span>
                <select class="custom-control custom-select">
                    <option value="onBtnSave">保存</option>
                    <option value="onBtnStartup">启动</option>
                </select>&nbsp;
                <input type="button" class="btn btn-warning" value="执行" onclick="onMultiExec( $(this).prev().val() );" />
            </td>
        </tr>
    </table>

    <script>
        // 脚本和设置文件
        var scriptFiles = {{ script_files }};
        var settingsFiles = {{ settings_files }};
    </script>

    <table class="table table-hover table-sm">
    <thead>
    <tr>
      <th scope="col">
          <div class="custom-control custom-checkbox">
            <input type="checkbox" class="custom-control-input" onclick="$('.chara-selected-checkbox').prop('checked', this.checked); $('.chara-selected').prop('disabled', !this.checked);" id="chk-all_select" />
            <label class="custom-control-label" for="chk-all_select">全选</label>
          </div>

      </th>
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
            <input type="hidden" class="form-control form-control-sm chara-selected" id="chara{{chara.chara_id}}-selected" value="{{chara.chara_id}}" disabled />
            <div class="custom-control custom-checkbox">
                <input type="checkbox" class="custom-control-input chara-selected-checkbox" onchange="document.getElementById('chara{{chara.chara_id}}-selected').disabled = !this.checked;" id="chk-chara{{chara.chara_id}}-selected" />
                <label class="custom-control-label" for="chk-chara{{chara.chara_id}}-selected"></label>
            </div>
        </td>
        <td>
            <span id="chara{{chara.chara_id}}-chara_name">{{chara.chara_name}}</span>
            <input type="hidden" id="chara{{chara.chara_id}}-gid_name" value="{{chara.gid_name}}" />
            <input type="hidden" id="chara{{chara.chara_id}}-server_id" value="{{chara.server_id}}" />
            <input type="hidden" id="chara{{chara.chara_id}}-account_name" value="{{chara.account_name}}" />
        </td>
        <td><select class="custom-control custom-select custom-select-sm" id="chara{{chara.chara_id}}-chara_lr">
            <option value="1"{{ if(chara.chara_lr==1, ' selected')}}>左</option>
            <option value="2"{{ if(chara.chara_lr==2, ' selected')}}>右</option>
        </select></td>
        <td><select class="custom-control custom-select custom-select-sm" id="chara{{chara.chara_id}}-server_line"><{for i=1 to=10}>
            <option value="{{i}}"{{ if(chara.server_line==i, ' selected') }}>{{i}}线</option><{/for}>
        </select></td>
        <td>
            <div class="custom-control custom-switch">
                <input type="checkbox" class="custom-control-input" id="chara{{chara.chara_id}}-autologin"{{ if(chara.autologin, ' checked')}} />
                <label class="custom-control-label" for="chara{{chara.chara_id}}-autologin"></label>
            </div>
        </td>
        <td>
            <div class="custom-control custom-switch">
                <input type="checkbox" class="custom-control-input" id="chara{{chara.chara_id}}-skipupdate"{{ if(chara.skipupdate, ' checked')}} />
                <label class="custom-control-label" for="chara{{chara.chara_id}}-skipupdate"></label>
            </div>
        </td>
        <td>
            <div class="custom-control custom-switch">
                <input type="checkbox" class="custom-control-input" id="chara{{chara.chara_id}}-autochangeserver"{{ if(chara.autochangeserver, ' checked')}} />
                <label class="custom-control-label" for="chara{{chara.chara_id}}-autochangeserver"></label>
            </div>
        </td>
        <td>
            <div class="custom-control custom-switch">
                <input type="checkbox" class="custom-control-input" id="chara{{chara.chara_id}}-scriptautorestart"{{ if(chara.scriptautorestart, ' checked')}} />
                <label class="custom-control-label" for="chara{{chara.chara_id}}-scriptautorestart"></label>
            </div>
        </td>
        <td>
            <div class="custom-control custom-switch">
                <input type="checkbox" class="custom-control-input" id="chara{{chara.chara_id}}-injuryprotect"{{ if(chara.injuryprotect, ' checked')}} />
                <label class="custom-control-label" for="chara{{chara.chara_id}}-injuryprotect"></label>
            </div>
        </td>
        <td>
            <div class="custom-control custom-switch">
                <input type="checkbox" class="custom-control-input" id="chara{{chara.chara_id}}-soulprotect"{{ if(chara.soulprotect, ' checked')}} />
                <label class="custom-control-label" for="chara{{chara.chara_id}}-soulprotect"></label>
            </div>
        </td>
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
            <button class="btn btn-info btn-sm" id="btn-chara{{chara.chara_id}}-save" onclick="onBtnSave({{chara.chara_id}});">保存</button>
            <button class="btn btn-success btn-sm" id="btn-chara{{chara.chara_id}}-startup" onclick="onBtnStartup({{chara.chara_id}});">启动</button>
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
        function onBtnSave(id) {
            var elem = document.getElementById('btn-chara' + id + '-save');
            elem.disabled = true;
            $.ajax( {
                url: 'action/quiklysave',
                data: getChara(id),
                dataType: 'json',
                success: function(data) {
                    console.log(data);
                    if ( !data.error ) {
                        elem.disabled = false;
                    }
                }
            } );
        }
        function onBtnStartup(id) {
            var elem = document.getElementById('btn-chara' + id + '-startup');
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
        function onMultiExec(opr) {
            var oCharaSelecteds = $('.chara-selected');
            var charaIds = [];
            for ( var i = 0; i < oCharaSelecteds.length; i++ ) {
                if ( !oCharaSelecteds[i].disabled ) {
                    //console.log(opr, oCharaSelecteds[i].value);
                    var id = oCharaSelecteds[i].value;
                    var oprFunc = eval(opr);
                    //console.log(id);
                    oprFunc(id);
                }
                
            }
            //console.log(o.length);
        }
    </script>
</div>
<{load=parts/footer.html.tpl}>
