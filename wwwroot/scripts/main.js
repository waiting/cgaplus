// 改变quikly的UI
function changeUi(charaId, canStartup) {
    if (canStartup) {
        $('#btn-chara'+charaId+'-startup').prop('disabled', false);
        $('#btn-chara'+charaId+'-loadscript').prop('disabled', true);
        $('#btn-chara'+charaId+'-loadsettings').prop('disabled', true);
        $('#btn-chara'+charaId+'-startup').text('启动');
        $('#btn-chara'+charaId+'-startup').removeClass('btn-danger');
        $('#btn-chara'+charaId+'-startup').addClass('btn-success');
    }
    else {
        $('#btn-chara'+charaId+'-startup').prop('disabled', true);
        $('#btn-chara'+charaId+'-loadscript').prop('disabled', false);
        $('#btn-chara'+charaId+'-loadsettings').prop('disabled', false);
        $('#btn-chara'+charaId+'-startup').text('运行中');
        $('#btn-chara'+charaId+'-startup').removeClass('btn-success');
        $('#btn-chara'+charaId+'-startup').addClass('btn-danger');
    }
}
// 判断是否运行中
function charaJudgeRunning(charaId) {
    $.ajax( {
        url: 'action/getchara',
        data: { chara_id: charaId },
        dataType: 'json',
        success: function(chara) {
            //console.log(chara);
            if ( chara.cga_port > 0 ) {
                $.ajax({
                    url: 'action/checkguiport',
                    data: { gui_port: chara.cga_port },
                    dataType: 'json',
                    timeout: 3000,
                    success: function(data) {
                        console.log(data);
                        if (data.error) {
                            changeUi(charaId, true);
                            $.ajax( {
                                url: 'action/quiklysave',
                                data: { 'chara': JSON.stringify({ chara_id: charaId, cga_port: 0 }) },
                                dataType: 'json',
                                success: function(data) {
                                    if ( !data.error ) {
                                        var charaname = $('#chara' + charaId + '-chara_name').text();
                                        console.log('清空【'+charaname+'】的端口号');
                                    }
                                }
                            } );
                        }
                        else {
                            
                        }
                    },
                    error: function(x,s,e) {
                        //console.log(s,e);
                        if ( s == 'timeout' ) {
                            changeUi(charaId, true);
                            $.ajax( {
                                url: 'action/quiklysave',
                                data: { 'chara': JSON.stringify({ chara_id: charaId, cga_port: 0 }) },
                                dataType: 'json',
                                success: function(data) {
                                    if ( !data.error ) {
                                        var charaname = $('#chara' + charaId + '-chara_name').text();
                                        console.log('清空【'+charaname+'】的端口号');
                                    }
                                }
                            } );
                        }
                        else {
                        }
                    },
                });
            }
        }
    } );
    
}

function getCharaInQuikly(charaId) {
    var chara = {
        chara_id: charaId,
        chara_lr: $('#chara'+charaId+'-chara_lr').val(),
        gid_name: $('#chara'+charaId+'-gid_name').val(),
        server_id: $('#chara'+charaId+'-server_id').val(),
        account_name: $('#chara'+charaId+'-account_name').val(),
        server_line: $('#chara'+charaId+'-server_line').val(),
        autologin: $('#chara'+charaId+'-autologin')[0].checked+0,
        skipupdate: $('#chara'+charaId+'-skipupdate')[0].checked+0,
        autochangeserver: $('#chara'+charaId+'-autochangeserver')[0].checked+0,
        scriptautorestart: $('#chara'+charaId+'-scriptautorestart')[0].checked+0,
        injuryprotect: $('#chara'+charaId+'-injuryprotect')[0].checked+0,
        soulprotect: $('#chara'+charaId+'-soulprotect')[0].checked+0,
        loadscript: $('#chara'+charaId+'-loadscript').val(),
        loadsettings: $('#chara'+charaId+'-loadsettings').val(),
    };
    return chara;
}
function onBtnSave(charaId) {
    var elem = document.getElementById('btn-chara' + charaId + '-save');
    if ( elem.disabled ) return;
    elem.disabled = true;
    $.ajax( {
        url: 'action/quiklysave',
        data: { 'chara': JSON.stringify(getCharaInQuikly(charaId)) },
        dataType: 'json',
        success: function(data) {
            console.log(data);
            if ( !data.error ) {
                elem.disabled = false;
            }
        }
    } );
}
function onBtnStartup(charaId) {
    var elem = document.getElementById('btn-chara' + charaId + '-startup');
    if ( elem.disabled ) return;
    elem.disabled = true;
    $.ajax( {
        url: 'action/startupgame',
        data: { 'chara': JSON.stringify(getCharaInQuikly(charaId)) },
        dataType: 'json',
        success: function(data) {
            if ( !data.error ) {
                changeUi(charaId, false);
            }
            else {
                var charaName = $('#chara' + charaId + '-chara_name').text();
                alert(data.error + '\n【'+charaName+'】启动失败');
                elem.disabled = false;
            }
        }
    } );
}
function onLoadScript(charaId) {
    $.get(
        'action/getchara',
        { 'chara_id': charaId },
        function(targetChara) {
            var chara = getCharaInQuikly(charaId);
            var params = {
                'path': chara.loadscript,
                'autorestart': chara.scriptautorestart, //自动重启脚本开启
                'injuryprot': chara.injuryprotect, //受伤保护开启
                'soulprot': chara.soulprotect, //掉魂受伤保护开启
            };
            $.post(
                'action/cgasetscript?gui_port=' + targetChara.cga_port,
                { 'params': JSON.stringify(params) },
                function(data) {
                    console.log(data);
                    if ( !data.errcode ) {
                        //alert('Success');
                    }
                    else {
                        //alert(data.message);
                    }
                },
                'json'
            );
        },
        'json'
    );
}
function onLoadSettings(charaId) {
    $.get(
        'action/getchara',
        { 'chara_id': charaId },
        function(targetChara) {
            var chara = getCharaInQuikly(charaId);
            var charaSettingsFile = chara.loadsettings;
            $.get(
                'action/cgasetsettings',
                { gui_port: targetChara.cga_port, 'chara_settings_file': charaSettingsFile },
                function(data) {
                    console.log(data);
                },
                'json'
            );
        },
        'json'
    );
}
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

function getSettingsInCgaplussettings() {
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

function onDeleteAccount(accountName) {
    if ( confirm('是否删除这个通行证`'+ accountName + '`？') ) {
        $.ajax( {
            url: 'action/delaccount',
            data: { 'account_name': accountName },
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

function onDeleteChara(charaId, charaName) {
    if ( confirm('是否删除这个角色`'+ charaName + '`？') ) {
        $.ajax( {
            url: 'action/delchara',
            data: { 'chara_id': charaId },
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
