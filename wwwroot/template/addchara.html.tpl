<{load=parts/header.html.tpl}><div class="container-fluid">
<div class="row">
    <script>
        // 脚本和设置文件
        var scriptFiles = {{ script_files }};
        var settingsFiles = {{ settings_files }};
    </script>
    <form class="col-lg-8 offset-lg-2" id="inputForm">
        <h3 style="margin-top: 8px;">添加角色</h3>
        <div class="form-row">
            <div class="form-group col-md-4 col-lg-3">
                <label for="inputAccountName">通行证</label>
                <input type="text" class="form-control" id="inputAccountName">
            </div>
            <div class="form-group col-md-4 col-lg-3">
                <label for="inputAccountPwd">密码</label>
                <input type="password" class="form-control" id="inputAccountPwd">
            </div>
            <div class="form-group col-md-4 col-lg-3">
                <label for="inputGidName">GID</label>
                <input type="text" class="form-control" id="inputGidName">
            </div>
        </div>
    
        <div class="form-row">
            <div class="form-group col-md-3 col-lg-3">
                <label for="inputServerId">服务器</label>
                <select class="custom-control custom-select" id="inputServerId">
                    <option value="4">道具电信</option>
                    <option value="40">道具网通</option>
                </select>
            </div>
            <div class="form-group col-md-3 col-lg-3">
                <label for="inputServerLine">路线</label>
                <select class="custom-control custom-select" id="inputServerLine">
                    <option value="1">一线</option>
                    <option value="2">二线</option>
                    <option value="3">三线</option>
                    <option value="4">四线</option>
                    <option value="5">五线</option>
                    <option value="6">六线</option>
                    <option value="7">七线</option>
                    <option value="8">八线</option>
                    <option value="9">九线</option>
                    <option value="10">十线</option>
                </select>
            </div>
        </div>
    
        <div class="form-row">
            <div class="form-group col-md-3 col-lg-3">
                <label for="inputCharaLr">左右</label>
                <select class="custom-control custom-select" id="inputCharaLr">
                    <option value="1">左</option>
                    <option value="2">右</option>
                </select>
            </div>
            <div class="form-group col-md-3 col-lg-3">
                <label for="inputCharaName">角色名</label>
                <input type="text" class="form-control" id="inputCharaName">
            </div>
        </div>
        
        <div class="form-row">
            <div class="form-group col-md-4 col-lg-3">
                <div class="custom-control custom-switch">
                  <input class="custom-control-input" type="checkbox" id="inputAutoLogin">
                  <label class="custom-control-label" for="inputAutoLogin">自动登录</label>
                </div>
            </div>
            <div class="form-group col-md-4 col-lg-3">
                <div class="custom-control custom-switch">
                  <input class="custom-control-input" type="checkbox" id="inputSkipUpdate">
                  <label class="custom-control-label" for="inputSkipUpdate">跳过更新</label>
                </div>
            </div>
            <div class="form-group col-md-4 col-lg-3">
                <div class="custom-control custom-switch">
                  <input class="custom-control-input" type="checkbox" id="intputAutoChangeServer">
                  <label class="custom-control-label" for="intputAutoChangeServer">自动换线</label>
                </div>
            </div>
            <div class="form-group col-md-4 col-lg-3">
                <div class="custom-control custom-switch">
                  <input class="custom-control-input" type="checkbox" id="inputScriptAutoRestart">
                  <label class="custom-control-label" for="inputScriptAutoRestart">脚本自动重启</label>
                </div>
            </div>
            <div class="form-group col-md-4 col-lg-3">
                <div class="custom-control custom-switch">
                  <input class="custom-control-input" type="checkbox" id="inputInjuryProtect">
                  <label class="custom-control-label" for="inputInjuryProtect">受伤保护</label>
                </div>
            </div>
            <div class="form-group col-md-4 col-lg-3">
                <div class="custom-control custom-switch">
                  <input class="custom-control-input" type="checkbox" id="inputSoulProtect">
                  <label class="custom-control-label" for="inputSoulProtect">掉魂保护</label>
                </div>
            </div>
    
        </div>
    
        <div class="form-row">
            <div class="form-group col-md-8">
                <label for="inputLoadScript">载入脚本</label>
                <div class="dropdown">
                    <input type="text" class="form-control dropdown-toggle" data-toggle="dropdown" id="inputLoadScript">
                    <select class="form-control dropdown-menu" size="10" onclick="$(this).prev().val(this.value);">
                        <script>
                        scriptFiles.forEach( function(f) {
                            document.write('<option class="dropdown-item" value="' + f + '">' + f + '</option>');
                        } );
                        </script>
                    </select>
                </div>
            </div>
        </div>
    
        <div class="form-row">
            <div class="form-group col-md-8">
                <label for="inputLoadSettings">载入设置</label>
                <div class="dropdown">
                    <input type="text" class="form-control dropdown-toggle" data-toggle="dropdown" id="inputLoadSettings">
                    <select class="form-control dropdown-menu" size="10" onclick="$(this).prev().val(this.value);">
                        <script>
                        settingsFiles.forEach( function(f) {
                            document.write('<option class="dropdown-item" value="' + f + '">' + f + '</option>');
                        } );
                        </script>
                    </select>
                </div>
            </div>
        </div>
    
        <div class="form-group">
            <button type="submit" class="btn btn-primary">添加</button>
            <button type="reset" class="btn btn-secondary">重置</button>
        </div>
    </form>
    <script>
        $('#inputForm').on('submit', function(e) {
            
            return false;
        });
    </script>
</div>
</div>
<{load=parts/footer.html.tpl}>
