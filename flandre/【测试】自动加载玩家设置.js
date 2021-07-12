let fs = require('fs');
require('./flandre').then( async (cga) => {
    // 加载自动登录信息
    let accountInfo = {
        user: '', // 通行证
        pwd: '', // 密码
        gid: '', // 子账号
        game: 4, // 4道具服 区服
        bigserver: 1, // 1电信 or 2网通
        server: 3, // 线路1~10
        character: 1, // 1左边 or 2右边
        autologin: true, // 自动登录是否开启
        skipupdate: false, // 禁用登录器更新是否开启
    };
    cga.gui.LoadAccount( accountInfo, (err,r)=>{
        console.log(err,r);
    } );
    // 加载玩家设置
    var settingsConfigFile = './' + cga.FileNameEscape(cga.GetPlayerInfo().name) + '.json';
    let settingsStr = fs.readFileSync(settingsConfigFile);
    let settings = JSON.parse(settingsStr);
    //console.log(settings);
    cga.gui.LoadSettings( settings, (err,r)=>{
        console.log(err,r);
    } );

} );
