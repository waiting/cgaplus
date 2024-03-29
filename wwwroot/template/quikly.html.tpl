<{load=parts/header.html.tpl}><div class="container-fluid">
    <h3 style="margin-top: 8px;">可启动角色列表</h3>
    <table>
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
      <th scope="col">脚本重启</th>
      <th scope="col">受伤保护</th>
      <th scope="col">掉魂保护</th>
      <th scope="col">加载脚本</th>
      <th scope="col">加载设置</th>
      <th scope="col">操作</th>
    </tr>
    <{loop=charas chara}><tr>
        <td>
            <input type="hidden" class="form-control form-control-sm chara-selected" id="chara{{chara.chara_id}}-chara_id" value="{{chara.chara_id}}" disabled />
            <div class="custom-control custom-checkbox">
                <input type="checkbox" class="custom-control-input chara-selected-checkbox" onchange="$(this).parent().prev().prop('disabled', !this.checked);" id="chk-chara{{chara.chara_id}}-selected" />
                <label class="custom-control-label" for="chk-chara{{chara.chara_id}}-selected">{{chara.chara_id}}</label>
            </div>
        </td>
        <td><{if=chara.chara_level}>
            <span id="chara{{chara.chara_id}}-chara_level">Lv.{{chara.chara_level}}</span><{/if}>
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
            <div class="dropdown input-group">
                <input type="text" class="form-control form-control-sm dropdown-toggle" data-toggle="dropdown" id="chara{{chara.chara_id}}-loadscript" value="{{chara.loadscript}}" />
                <select class="dropdown-menu" size="10" onclick="$(this).prev().val(this.value);">
                    <script>
                    scriptFiles.forEach( function(f) {
                        document.write('<option class="dropdown-item" value="' + f + '">' + f + '</option>');
                    } );
                    </script>
                </select>
                <div class="input-group-append">
                    <button type="button" class="btn btn-sm btn-outline-info" id="btn-chara{{chara.chara_id}}-loadscript"{{ if(!chara.cga_port,' disabled') }} onclick="onLoadScript({{chara.chara_id}})">加载</button>
                </div>
            </div>
        </td>
        <td>
            <div class="dropdown input-group">
                <input type="text" class="form-control form-control-sm dropdown-toggle" data-toggle="dropdown" id="chara{{chara.chara_id}}-loadsettings" value="{{chara.loadsettings}}" />
                <select class="dropdown-menu" size="10" onclick="$(this).prev().val(this.value);">
                    <script>
                    settingsFiles.forEach( function(f) {
                        document.write('<option class="dropdown-item" value="' + f + '">' + f + '</option>');
                    } );
                    </script>
                </select>
                <div class="input-group-append">
                    <button type="button" class="btn btn-sm btn-outline-info" id="btn-chara{{chara.chara_id}}-loadsettings"{{ if(!chara.cga_port,' disabled') }} onclick="onLoadSettings({{chara.chara_id}})">加载</button>
                </div>
            </div>
        </td>
        <td>
            <button class="btn btn-info btn-sm" id="btn-chara{{chara.chara_id}}-save" onclick="onBtnSave({{chara.chara_id}});">保存</button>
            <button class="btn{{ if(chara.cga_port,' btn-danger',' btn-success') }} btn-sm" id="btn-chara{{chara.chara_id}}-startup" onclick="onBtnStartup({{chara.chara_id}});"{{ if(chara.cga_port,' disabled') }}>{{ if(chara.cga_port,'运行中','启动') }}</button>
            <script>
                charaJudgeRunning({{chara.chara_id}});
            </script>
        </td>
    </tr><{/loop}>
    </table>
</div>
<{load=parts/footer.html.tpl}>
