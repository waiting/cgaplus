<{load=parts/header.html.tpl}><div class="container-fluid">
    <div class="row">
    <form class="col-lg-8 offset-lg-2" id="inputForm">
        <h3 style="margin-top: 8px;">软件设置</h3>
        <div class="form-row">
            <div class="form-group col">
                <label for="cga_exepath">CGA程序路径（CGAssistant.exe的路径）</label>
                <div class="input-group">
                    <input type="text" class="form-control" id="cga_exepath"<{if='settings.cga_exepath'}> value="{{htmlencode(settings.cga_exepath)}}"<{/if}> />
                    <div class="input-group-append">
                        <button type="button" class="btn btn-outline-dark" onclick="onDetectCgaPath();">自动获取</button>
                    </div>
                </div>
            </div>
        </div>
        <div class="form-row">
            <div class="form-group col">
                <label for="script_dirpath">脚本目录路径</label>
                <input type="text" class="form-control" id="script_dirpath"<{if='settings.script_dirpath'}> value="{{htmlencode(settings.script_dirpath)}}"<{/if}> />
            </div>
        </div>
        <div class="form-row">
            <div class="form-group col">
                <label for="settings_dirpath">设置目录路径</label>
                <input type="text" class="form-control" id="settings_dirpath"<{if='settings.settings_dirpath'}> value="{{htmlencode(settings.settings_dirpath)}}"<{/if}> />
            </div>
        </div>

        <div class="form-group">
            <button type="submit" class="btn btn-primary">设定</button>　
            <button type="reset" class="btn btn-secondary">重置</button>
        </div>
    </form>
    <script>
        function getSettings() {
            var settings = {
                'cga_exepath': $('#cga_exepath').val(),
                'script_dirpath': $('#script_dirpath').val(),
                'settings_dirpath': $('#settings_dirpath').val()
            };
            return settings;
        }
        function onDetectCgaPath() {
            $.ajax( {
                url: 'action/detectcgapath',
                data: { },
                dataType: 'json',
                success: function(result) {
                    console.log(result);
                    if ( !result.error ) {
                        
                        $('#cga_exepath').val(result.cga_exepath);
                        $('#script_dirpath').val(result.script_dirpath);
                        $('#settings_dirpath').val(result.settings_dirpath);
                    }
                    else {
                        alert(result.error);
                    }
                }
            } );
        }
        $('#inputForm').on('submit', function(e) {
            $.ajax( {
                url: 'action/cgaplussetsettings',
                data: { 'settings': JSON.stringify(getSettings()) },
                dataType: 'json',
                success: function(data) {
                    console.log(data);
                    if ( !data.error ) {
                    }
                    else {
                        alert(data.error);
                    }
                }
            } );
            return false;
        });
    </script>

    </div>
</div>
<{load=parts/footer.html.tpl}>
