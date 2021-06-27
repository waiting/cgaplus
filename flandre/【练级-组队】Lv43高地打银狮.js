require('./flandre').then( async () => {

    // 动作：练级、回补、卖石
    ff.curAction = ff.ActGoBattle;

    // 练级地：布拉基姆高地（银狮）
    let placeCoord = [133, 118];

    // 本人结束遇敌条件
    let cond = {
        playerMinHp: 200,           // 人物最小生命值
        playerMinHpPercent: 0.35,    // 人物最低生命值比率
        playerMinMp: 50,           // 人物最小魔力值
        playerMinMpPercent: 0.35,    // 人物最低魔力值比率
        petMinHp: 200,              // 宠物最小生命值
        petMinHpPercent: 0.35,       // 宠物最低生命值比率
        petMinMp: 40,              // 宠物最小魔力值
        petMinMpPercent: 0.35,       // 宠物最低魔力值比率
    };

    // 等待初始化团队
    let scriptSettings = await ff.waitLoadSettingsAndInitTeam(__filename, cond);
    cond = scriptSettings.cond;

    // 是否为队长
    let isTeamLeader = ff.isTeamLeader();
    isTeamLeader ? console.log('我是队长') : console.log('我是队员');

    // 创建结束条件函数
    let endcond = ff.createEndCondFunc(cond);
    console.log('回补条件', cond);

    // 检查条件并取得下一步执行的动作
    let CheckCondChangeAction = (endcond, sellStone = false) => {
        if (endcond()) { // 退出条件达成，回补
            ff.curAction = ff.ActGoRecovery;
            if (sellStone && cga.getSellStoneItem().length > 0) { // 背包里有可卖的东西
                ff.curAction = ff.ActGoSell;
            }
        }
        else if ( sellStone && ff.getInventoryEmptySlotCount() < 1 ) { // 背包满了，卖东西
            ff.curAction = ff.ActGoSell;
        }
        return ff.curAction;
    }
    ff.actOutput(CheckCondChangeAction(endcond, false));

    // 一轮统计
    let counter = 0;
    let curXp = cga.GetPlayerInfo().xp;

    // 监听回补
    cga.waitTeammateSay((fromTeammate, msg) => {
        if (msg.indexOf('<回补>') >= 0) {
            console.log(fromTeammate.name+':', msg);
            ff.curAction = ff.ActGoRecovery;
            ff.actOutput(ff.curAction);
        }
        return true;
    });
    
    // 脚本循环
    while (true) try {
        if (isTeamLeader) {
            console.log( 'Map:', cga.GetMapName() + ',', 'Action:', ff.actString(ff.curAction) );
        }

        if ( ff.curMapIs('布拉基姆高地') ) {
            if (ff.curInPtRange(placeCoord, 1)) { // 练级地附近
                switch (ff.curAction) {
                case ff.ActGoBattle:
                    if (isTeamLeader) {
                        try {
                            let coord = cga.GetMapXY();
                            let dir = cga.getRandomSpaceDir(coord.x, coord.y);
                            console.log('开始遇敌');
                            console.log('遇敌...');
                            while (await ff.encounterEnemy(coord, dir, 230, endcond, 59503)) {
                                counter++;
                                console.log('进入第', counter, '次战斗');
                                await ff.delay(1000).then( () => ff.waitBattleEnd(0) ); // 等待战斗结束
                                console.log('结束第', counter, '次战斗');
                                await ff.delay(1000);
                                // 当动作不是‘去战斗’，退出循环
                                if ( CheckCondChangeAction(endcond, cond.itemMinEmptySlot!==undefined) != ff.ActGoBattle ) break;
                                console.log('遇敌...');
                            }
                            console.log('结束遇敌');
                            //ff.actOutput(ff.curAction);
                        }
                        catch (e) {
                            console.log(e);
                        }
                    }
                    else {
                        await ff.delay(2000);
                        if (await ff.waitBattleEnd(0)) {
                            counter++;
                            console.log('第', counter, '次战斗结束');
                            if (endcond()) {
                                console.log('<回补>', new Date);
                                cga.EnableFlags(cga.ENABLE_FLAG_TEAMCHAT, true);
                                await ff.chat('<回补>', 1, 5, 1);
                            }
                        }
                    }
                    break;
                case ff.ActGoRecovery:
                case ff.ActGoSell:
                    if (isTeamLeader) {
                        await ff.autoWalk(30, 193, '盖雷布伦森林');
                    }
                    else {
                        await ff.delay(2000);
                    }
                    break;
                }
            }
            else { // 其他位置
                switch (ff.curAction) {
                case ff.ActGoBattle:
                    if (isTeamLeader) {
                        await ff.autoWalk(placeCoord);
                    }
                    else {
                        await ff.delay(2000);
                    }
                    break;
                default:
                    if (isTeamLeader) {
                        await ff.autoWalk(30, 193, '盖雷布伦森林');
                    }
                    else {
                        await ff.delay(2000);
                    }
                    break;
                }
            }
        }
        else if ( ff.curMapIs('盖雷布伦森林') ) {
            switch (ff.curAction) {
            case ff.ActGoBattle:
                if (isTeamLeader) {
                    await ff.autoWalk(231, 222, '布拉基姆高地');
                }
                else {
                    await ff.delay(2000);
                }
                break;
            case ff.ActGoRecovery:
            case ff.ActGoSell:
            default:
                if (isTeamLeader) {
                    await ff.autoWalk(199, 211, '艾夏岛');
                }
                else {
                    await ff.delay(2000);
                }
                break;
            }
        }
        else if ( ff.curMapIs('艾夏岛') ) {
            switch (ff.curAction) {
            case ff.ActGoBattle:
                if (ff.curInPtRange([84, 112], 1)) {
                    if (isTeamLeader) {
                        await ff.autoWalk(84, 112);
                        await ff.turnDirWarpMap(4, '艾夏岛', 164, 159);
                    }
                    else {
                        await ff.delay(2000);
                    }
                }
                else if (ff.curInPtRange([164, 159], 1)) {
                    if (isTeamLeader) {
                        await ff.autoWalk(164, 159);
                        await ff.turnDirWarpMap(5, '艾夏岛', 151, 97);
                    }
                    else {
                        await ff.delay(2000);
                    }
                }
                else if (ff.curInPtRange([151, 97], 1)) {
                    if (isTeamLeader) {
                        await ff.autoWalk(190, 116, '盖雷布伦森林');
                    }
                    else {
                        await ff.delay(2000);
                    }
                }
                else {
                    if (isTeamLeader) {
                        await ff.autoWalk(190, 116, '盖雷布伦森林');
                    }
                    else {
                        await ff.delay(2000);
                    }
                }
                break;
            case ff.ActGoRecovery:
            case ff.ActGoSell:
                if (isTeamLeader) {
                    await ff.autoWalk(112, 81, '医院');
                }
                else {
                    await ff.delay(2000);
                }
                break;
            default:
                break;
            }
        }
        else if ( ff.curMapIs('艾尔莎岛') ) {
            switch (ff.curAction) {
            case ff.ActGoBattle:
                if (isTeamLeader) {
                    await ff.autoWalk(157, 93);
                    await ff.turnDirWarpMap(0, '艾夏岛');
                }
                else {
                    await ff.delay(2000);
                }
                break;
            case ff.ActGoRecovery:
            case ff.ActGoSell:
            default:
                break;
            }
        }
        else if ( ff.curMapIs('医院') ) {
            switch (ff.curAction) {
            case ff.ActGoBattle:
                if (isTeamLeader) {
                    await ff.autoWalk(28, 51);
                    await ff.autoWalk(28, 52, '艾夏岛');
                }
                else {
                    await ff.delay(2000);
                }
                counter = 0;
                curXp = cga.GetPlayerInfo().xp;
                break;
            case ff.ActGoRecovery:
                if (isTeamLeader) {
                    await ff.autoWalk(35, 43);
                    await ff.turnDirOpenDialog(0); // 打开补血对话框
                    while ( cga.needSupplyInitial() ) {
                        console.log('补血中...');
                        await ff.delay(1000);
                    }
                }
                else {
                    await ff.delay(2000);
                }
                console.log( '本轮战斗场次:'+counter, '本轮获得经验:'+(cga.GetPlayerInfo().xp-curXp) );
                ff.curAction = ff.ActGoBattle;
                ff.actOutput(ff.curAction);
                break;
            default:
                console.log('[' + cga.GetMapName() + ']不支持这个动作:', ff.actString(ff.curAction));
                break;
            }
        }
        else {
            console.log('处在未知地图[' + cga.GetMapName() + ']');
            await ff.delay(1000);
        }
    }
    catch (e) {
        console.log(e);
    }

} );
