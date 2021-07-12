require('./flandre').then( async () => {
    // 动作：练级、回补、卖石
    let curAction = ff.ActGoBattle;

    // 练级地：芙蕾雅
    let placeCoord = [502, 215];

    // 结束遇敌条件
    let cond = {
        playerMinHp: 100,           // 人物最小生命值
        playerMinHpPercent: 0.5,    // 人物最低生命值比率
        playerMinMp: 15,           // 人物最小魔力值
        playerMinMpPercent: 0.5,    // 人物最低魔力值比率
        petMinHp: 100,              // 宠物最小生命值
        petMinHpPercent: 0.5,       // 宠物最低生命值比率
        petMinMp: 20,              // 宠物最小魔力值
        petMinMpPercent: 0.5,       // 宠物最低魔力值比率
        //playerHp: 0,
        //playerMp: 0,
        //petHp: 0,
        //petMp: 0,
    };

    let endcond = ff.createEndCondFunc(cond);
    console.log('回补条件', cond);

    // 检查条件并取得下一步执行的动作
    let CheckCondGetAction = (endcond, sellStone = true) => {
        let act = curAction;
        if ( endcond() ) { // 退出条件达成，回补
            act = ff.ActGoRecovery;
            if (sellStone && cga.getSellStoneItem().length > 0) { // 背包里有可卖的东西
                act = ff.ActGoSell;
            }
        }
        else if ( sellStone && ff.getInventoryEmptySlotCount() < 1 ) { // 背包满了，卖东西
            act = ff.ActGoSell;
        }
        return act;
    }
    curAction = CheckCondGetAction(endcond);
    ff.actOutput(curAction);

    

    // 脚本循环
    while (true) {
        console.log( 'Map:', cga.GetMapName() + ',', 'Action:', ff.actString(curAction) );

        if (ff.curMapIs('芙蕾雅')) {
            if (ff.curInPtRange( placeCoord, 1 )) { // 练级地附近
                switch (curAction) {
                case ff.ActGoBattle:
                    try {
                        let coord = cga.GetMapXY();
                        let dir = cga.getRandomSpaceDir(coord.x, coord.y);
                        console.log('开始遇敌');
                        let curXp = cga.GetPlayerInfo().xp;
                        let cnt = 0;
                        while (await ff.encounterEnemy(coord, dir, 300, endcond, '芙蕾雅')) {
                            cnt++;
                            console.log('进入第', cnt, '次战斗');
                            await ff.waitBattleEnd(0); // 等待战斗结束
                            console.log('结束第', cnt, '次战斗');
                            if ( ff.getInventoryEmptySlotCount() < 1 ) break;
                        }
                        console.log('本轮取得经验:', cga.GetPlayerInfo().xp - curXp);
                        console.log('结束遇敌');
                        curAction = CheckCondGetAction(endcond);
                        ff.actOutput(curAction);
                    }
                    catch (e) {
                        console.log(e);
                    }
                    break;
                case ff.ActGoRecovery:
                case ff.ActGoSell:
                    await ff.autoWalk(470, 196, '法兰城');
                    break;
                }
            }
            else { // 芙蕾雅其他位置
                switch (curAction) {
                case ff.ActGoBattle:
                    await ff.autoWalk(placeCoord);
                    break;
                }
            }
        }
        else if (ff.curMapIs('法兰城')) {
            if (ff.curInPtRange([162, 130], 10)) {         // 法兰M1传送石附近
                switch (curAction) {
                case ff.ActGoSell:
                    await ff.walk(156, 123);
                    await ff.sellMisc(6); // 卖石头
                    if ( cga.getSellStoneItem().length == 0 ) {
                        curAction = ff.ActGoRecovery;
                        ff.actOutput(curAction);
                    }
                    break;
                case ff.ActGoRecovery:
                default:
                    await ff.walk(162, 130);
                    await ff.turnDirWarpMap(0, '法兰城', 72, 123);
                    break;
                }
            }
            else if (ff.curInPtRange([153, 103], 10)) {    // 从里堡出来
                switch (curAction) {
                case ff.ActGoSell:
                    await ff.autoWalk(156, 123);
                    await ff.sellMisc(6); // 卖石头
                    curAction = ff.ActGoRecovery;
                    ff.actOutput(curAction);
                default:
                    await ff.autoWalk(162, 130);
                    break;
                }
            }
            else if (ff.curInPtRange([72, 123], 10)) {     // 法兰W2传送石附近
                switch (curAction) {
                default:
                    await ff.walk(72, 123);
                    await ff.turnDirWarpMap(0, '法兰城', 233, 78);
                    break;
                }
            }
            else {                                                  // 在法兰城其他地方
                switch (curAction) {
                case ff.ActGoBattle:
                    await ff.autoWalk(281, 88, '芙蕾雅');
                    break;
                case ff.ActGoRecovery:
                    await ff.autoWalk(221, 83, '医院');
                    break;
                case ff.ActGoSell:
                    await ff.autoWalk(233, 78);
                    await ff.turnDirWarpMap(6, '市场一楼 - 宠物交易区', 46, 16);
                    break;
                default:
                    break;
                }
            }
        }
        else if (ff.curMapIs('市场一楼 - 宠物交易区')) {
            switch (curAction) {
            default:
                await ff.walk(46, 16);
                await ff.turnDirWarpMap(6, '法兰城', 162, 130);
                break;
            }
        }
        else if (ff.curMapIs('医院')) {
            switch (curAction) {
            case ff.ActGoRecovery:
                await ff.autoWalk(6, 34);
                await ff.turnDirNpcDialog(6); // 打开补血对话框
                while ( cga.needSupplyInitial() ) {
                    console.log('请补血');
                    await ff.delay(1000);
                }
                curAction = ff.ActGoBattle;
                ff.actOutput(curAction);
                break;
            case ff.ActGoBattle:
                await ff.autoWalk(12, 42, '法兰城');
                break;
            }
        }
        else if (ff.curMapIs('艾尔莎岛')) {
            switch (curAction) {
            default:
                await ff.autoWalk(140, 105);
                await ff.turnDirNpcDialog(7);
                cga.ClickNPCDialog(4, 0); // 4=选是
                await ff.waitWarpMap('里谢里雅堡');
                break;
            }
        }
        else if (ff.curMapIs('里谢里雅堡')) {
            switch (curAction) {
            default:
                await ff.autoWalk(41, 98, '法兰城');
                break;
            }
        }
        else {
            console.log('处在未知地图');
            await ff.delay(1000);
        }
    }

} );
