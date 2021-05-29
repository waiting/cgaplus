const request = require('request');

require('./flandre').then( async () => {
    cga.gui.init();
    await ff.gui.setScript( { autorestart: false } );

    // 根据脚本名后缀获取cgaplus端口
    let re1 = /_(\d+)\.js$/;
    let re2 = /_(\d+)_(\d+)\.js$/;
    let res;
    let chara = {};
    let charaId = 0;
    let charaName = cga.GetPlayerInfo().name;
    let cgaplusPort = 9456;
    if ( res = re2.exec(__filename) ) {
        charaId = res[1];
        cgaplusPort = res[2];
        chara = await ff.httpGet('http://127.0.0.1:' + cgaplusPort + '/action/getchara?chara_id=' + charaId);
    }
    else if ( res = re1.exec(__filename) ) {
        cgaplusPort = res[1];
        chara = await ff.httpGet('http://127.0.0.1:' + cgaplusPort + '/action/getchara?chara_name=' + encodeURIComponent(charaName));
    }

    console.log(chara);

    if (chara.chara_id) {
        // 更新cga_port
        let r;
        r = await ff.httpGet('http://127.0.0.1:' + cgaplusPort + '/action/quiklysave?chara_id=' + chara.chara_id + '&cga_port=' + cga.gui.port + ( charaId > 0 ? '&chara_name=' + encodeURIComponent(charaName) : '' ) );
        if (!r.error) {
            console.log('更新cga_port成功', cga.gui.port );
        }
        else {
            console.log('更新cga_port失败');
        }

        // 加载想要运行的脚本
        if ( chara.loadscript != '' ) {
            r = await ff.gui.setScript( { path: __dirname + '/' + chara.loadscript, autorestart: (chara.scriptautorestart ? true : false) } );
            console.log(r);
        }
    }
    else {
        console.log('没有该角色');
    }
} );
