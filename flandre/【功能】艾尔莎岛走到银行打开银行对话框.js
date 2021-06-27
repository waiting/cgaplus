require('./flandre').then( async () => {
    let stop = false;
    while (!stop) {
        if ( cga.GetMapName() == '艾尔莎岛' ) {
            await ff.walk([ [158, 94] ]);
            await ff.turnDirWarpMap(6, { map:'艾夏岛', delay:1000, timeout:5000 } );
        }
        else if ( cga.GetMapName() == '艾夏岛' ) {
            await ff.walk([ [114, 104, '银行'] ]);
        }
        else if ( cga.GetMapName() == '银行' ) {
            try {
                await ff.walk([ [49, 30] ]);
            }
            catch (e) {
                await ff.walk([ [11, 8] ]);
            }
            console.log('走到柜台位置');
            let dlg = await ff.turnDirOpenDialog(0);
            console.log('银行对话框打开', dlg.type, dlg.dialog_id);
            let items = cga.GetBankItemsInfo();
            console.log(items);
            //let pets = cga.GetBankPetsInfo();
            //console.log(pets);

            stop = true;
        }
        else if ( cga.GetMapName() == '里谢里雅堡' ) {
            await ff.walk([ [41, 98, '法兰城'] ]);
        }
        else if ( cga.GetMapName() == '法兰城' ) {
            if ( ff.utils.coordInPointRange( cga.GetMapXY(), {x: 238, y: 111}, 58 ) ) {
                await ff.walk([ [238, 111, '银行'] ]);
            }
            else if (
                ff.utils.coordInPointRange( cga.GetMapXY(), {x: 141, y: 148}, 10 ) ||
                ff.utils.coordInPointRange( cga.GetMapXY(), {x: 153, y: 103}, 10 ) ||
                ff.utils.coordInPointRange( cga.GetMapXY(), {x: 162, y: 130}, 10 )
            ) {
                await ff.walk([ [141, 148] ]);
                await ff.turnDirWarpMap(6, {x:63, y:79, delay:1000, timeout:5000});
            }
            else if (
                ff.utils.coordInPointRange( cga.GetMapXY(), {x: 63, y: 79}, 10 ) ||
                ff.utils.coordInPointRange( cga.GetMapXY(), {x: 72, y: 123}, 10 )
            ) {
                await ff.walk([ [63, 79] ]);
                await ff.turnDirWarpMap(6, {x:242, y:100, delay:1000, timeout:5000});
            }
            else {
                console.log('人在法兰城未知坐标，请先走到W1、E1、E2、M2传送石附近');
                await ff.delay(1000);
            }
        }
        else {
            console.log('人在未知地图，请登出！');
            stop = true;
        }
    }
} ).catch((reason)=>{
    console.log(reason);
});
