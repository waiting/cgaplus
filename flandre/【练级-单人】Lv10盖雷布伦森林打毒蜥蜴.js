require('./flandre').then( async () => {
    // 动作：练级、回补、卖石
    let curAction = ff.ActGoBattle;

    // 练级地：盖雷布伦森林
    let placeCoord = [181, 159];

    // 结束遇敌条件
    let cond = {
        playerMinHp: 100,           // 人物最小生命值
        playerMinHpPercent: 0.3,    // 人物最低生命值比率
        playerMinMp: 15,           // 人物最小魔力值
        playerMinMpPercent: 0.3,    // 人物最低魔力值比率
        petMinHp: 100,              // 宠物最小生命值
        petMinHpPercent: 0.3,       // 宠物最低生命值比率
        petMinMp: 20,              // 宠物最小魔力值
        petMinMpPercent: 0.3,       // 宠物最低魔力值比率
    };

    // 创建结束条件函数
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

    let curXp = cga.GetPlayerInfo().xp;
    // 脚本循环
    while (true) try {
        console.log( 'Map:', cga.GetMapName() + ',', 'Action:', ff.actString(curAction) );

        if ( ff.curMapIs('盖雷布伦森林') ) {
            if (ff.curInPtRange( placeCoord, 10 )) { // 练级地附近
                switch (curAction) {
                case ff.ActGoBattle:
                    try {
                        let coord = cga.GetMapXY();
                        let dir = cga.getRandomSpaceDir(coord.x, coord.y);
                        console.log('开始遇敌');
                        let cnt = 0;
                        console.log('遇敌...');
                        while (await ff.encounterEnemy(coord, dir, 300, endcond, 59500)) {
                            cnt++;
                            console.log('进入第', cnt, '次战斗');
                            await ff.delay(1000).then( () => ff.waitBattleEnd(0) ); // 等待战斗结束
                            console.log('结束第', cnt, '次战斗');
                            // 当动作不是‘去战斗’，退出循环
                            if ( (curAction = CheckCondGetAction(endcond)) != ff.ActGoBattle ) break;
                            console.log('遇敌...');
                        }
                        console.log('结束遇敌');
                        ff.actOutput(curAction);
                    }
                    catch (e) {
                        console.log(e);
                    }
                    break;
                case ff.ActGoRecovery:
                case ff.ActGoSell:
                    // 登回城
                    await ff.delay(1000).then( () => ff.logBack() );
                    break;
                }
            }
            else { // 其他位置
                switch (curAction) {
                case ff.ActGoBattle:
                    await ff.autoWalk(placeCoord);
                    break;
                default:
                    await ff.autoWalk(185, 186, '艾尔莎岛');
                    break;
                }
            }
        }
        else if ( ff.curMapIs('艾尔莎岛') ) {
            switch (curAction) {
            case ff.ActGoBattle:
                await ff.autoWalk(130, 50, '盖雷布伦森林');
                break;
            case ff.ActGoRecovery:
            case ff.ActGoSell:
            default:
                await ff.autoWalk(140, 105).then( () => {
                    return ff.turnDirOpenDialog(7);
                } ).then( () => {
                    cga.ClickNPCDialog(4, 0); // 4=选是
                } ).then( () => {
                    return ff.waitWarpMap('里谢里雅堡');
                } ).catch( e => {
                    console.log(e);
                } );
                break;
            }
        }
        else if ( ff.curMapIs('里谢里雅堡') ) {
            switch (curAction) {
            case ff.ActGoBattle:
                curXp = cga.GetPlayerInfo().xp;
                //console.log('当前经验', curXp);
                // 登回城
                await ff.logBack();
                break;
            case ff.ActGoRecovery:
                await ff.autoWalk(34, 88);
                await ff.turnDirOpenDialog(0); // 打开补血对话框
                while ( cga.needSupplyInitial() ) {
                    console.log('请补血...');
                    await ff.delay(1000);
                }
                //console.log('当前经验', curXp, cga.GetPlayerInfo().xp);
                console.log('本轮取得经验:', cga.GetPlayerInfo().xp - curXp);

                curAction = ff.ActGoBattle;
                ff.actOutput(curAction);

                break;
            case ff.ActGoSell:
                await ff.walk(30, 79);
                await ff.sellMisc(6); // 卖石头
                if ( cga.getSellStoneItem().length == 0 ) {
                    curAction = ff.ActGoRecovery;
                    ff.actOutput(curAction);
                }
                break;
            default:
                console.log('里谢里雅堡不支持这个动作:', curAction);
                //await ff.autoWalk(41, 98, '法兰城');
                break;
            }
        }
        else {
            console.log('处在未知地图');
            await ff.delay(1000);
        }
    }
    catch (e) {
        console.log(e);
    }

} );
