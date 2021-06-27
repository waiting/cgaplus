require('./flandre').then( async () => {

    // 动作：练级、回补、卖石
    let curAction = ff.ActGoBattle;

    // 练级地：布拉基姆高地（黄金龙骨）
    let placeCoord = [135, 175];

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


    // 取得本人的脚本设置文件名
    console.log('本人脚本设置文件:', ff.getMyselfScriptSettingsFileName(__filename) );

    // 等待初始化团队
    let scriptSettings = await ff.waitInitTeam(__filename, cond);
    cond = scriptSettings.cond;

    // 是否为队长
    let isTeamLeader = ff.isTeamLeader();
    isTeamLeader ? console.log('我是队长') : console.log('我是队员');

    // 创建结束条件函数
    let endcond = ff.createEndCondFunc(cond);
    console.log('回补条件', cond);

    // 检查条件并取得下一步执行的动作
    let CheckCondGetAction = (endcond, sellStone = false) => {
        let act = curAction;
        if (endcond()) { // 退出条件达成，回补
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
    while (1) try {
        if (isTeamLeader) {
            console.log( 'Map:', cga.GetMapName() + ',', 'Action:', ff.actString(curAction) );
        }

        if ( ff.curMapIs('布拉基姆高地') ) {
            if (ff.curInPtRange( placeCoord, 1 )) { // 练级地附近
                switch (curAction) {
                case ff.ActGoBattle:
                    if (isTeamLeader) {
                        // 队长监听
                        cga.waitTeammateSay((fromTeammate, msg) => {
                            console.log(fromTeammate.name+':', msg);
                            if (msg.indexOf('<回补>') >= 0) {
                                curAction = ff.ActGoRecovery;
                                return false;
                            }
                            return true;
                        });
                        try {
                            let coord = cga.GetMapXY();
                            let dir = cga.getRandomSpaceDir(coord.x, coord.y);
                            console.log('开始遇敌');
                            let cnt = 0;
                            console.log('遇敌...');
                            while (await ff.encounterEnemy(coord, dir, 230, endcond, 59503)) {
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
                    }
                    else {
                        await ff.delay(2000);
                        if (await ff.waitBattleEnd(0)) {
                            if (endcond()) {
                                console.log('<回补>', new Date);
                                cga.EnableFlags(cga.ENABLE_FLAG_TEAMCHAT, true);
                                await ff.chat('<回补>', 1, 5, 1);
                            }
                            console.log('一次战斗结束');
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
                switch (curAction) {
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
            switch (curAction) {
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
            switch (curAction) {
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
            switch (curAction) {
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
            switch (curAction) {
            case ff.ActGoBattle:
                if (isTeamLeader) {
                    await ff.autoWalk(28, 51);
                    await ff.autoWalk(28, 52, '艾夏岛');
                }
                else {
                    await ff.delay(2000);
                }
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
                curAction = ff.ActGoBattle;
                ff.actOutput(curAction);
                break;
            default:
                console.log('[' + cga.GetMapName() + ']不支持这个动作:', ff.actString(curAction));
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
