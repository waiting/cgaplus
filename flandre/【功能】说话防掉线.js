var cga = require('../cgaapi')( () => {
    let startTime = new Date().valueOf();
    cga.waitSysMsg((msg)=>{
        if (msg.indexOf('您处于＂离开＂状态') != -1) {
            console.log('间隔了', (new Date().valueOf() - startTime) / 1000, '秒');
            cga.SayWords('', 0, 1, 1);
            startTime = new Date().valueOf();
        }
        return true;
    });
} );
