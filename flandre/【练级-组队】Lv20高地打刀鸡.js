require('./flandre').then( async () => {

    // 动作：练级、回补、卖石
    let curAction = ff.ActGoBattle;

    // 练级地：布拉基姆高地（刀鸡）
    let placeCoord = [38, 186];

    // 本人结束遇敌条件
    let cond = {
        playerMinHp: 200,           // 人物最小生命值
        playerMinHpPercent: 0.3,    // 人物最低生命值比率
        playerMinMp: 30,           // 人物最小魔力值
        playerMinMpPercent: 0.3,    // 人物最低魔力值比率
        petMinHp: 200,              // 宠物最小生命值
        petMinHpPercent: 0.3,       // 宠物最低生命值比率
        petMinMp: 20,              // 宠物最小魔力值
        petMinMpPercent: 0.3,       // 宠物最低魔力值比率
    };

    /*
        设置文件命名规则：scriptStamp + '_' + charaName + '.json'

        组队功能：
            如果已经处于组队的状态，如果有变化，把当前队伍信息写入设置文件：
                把当前队伍信息更新至设置文件。

            如果没有组队：
                如果有设置文件：
                    则从设置文件里读取队员信息尝试组队直至组满。
                如果没有设置文件：
                    则需队长输入信息(游戏内输入：<组5人>，说话的角色设成队长)，等待组满相应人数，把当前队伍信息写入设置文件。
    */

    // 取得本人的脚本设置文件名
    console.log('本人脚本设置文件:', ff.getMyselfScriptSettingsFileName(__filename) );
    // 读取设置
    let scriptSettings = ff.readMyselfScriptSettings(__filename);
    if ( scriptSettings.cond ) cond = scriptSettings.cond;

    if (ff.isInTeam()) { // 已经组队
        let players = [];
        cga.getTeamPlayers().forEach( (t) => {
            players.push(t.name);
        } );

        if ( JSON.stringify(players) != JSON.stringify(scriptSettings.players) ) { // 有变化
            scriptSettings.players = players;
            scriptSettings.cond = cond; // 本人的退出条件
            ff.writeMyselfScriptSettings(__filename, scriptSettings);
        }
    }
    else { // 尚未组队
        if (ff.isEmptyObject(scriptSettings)) { // 没有设置文件
            let amsg = await ff.waitChat(/组(\d+)人/);
            let playersCount = parseInt(amsg.res[1]) + 0;
            console.log(playersCount);
            if (amsg.name == cga.GetPlayerInfo().name) { // 是自己说的
                // 等待别人组我
                console.log('我是队长');
                while (cga.getTeamPlayers().length < playersCount) {
                    await ff.delay(1000);
                    console.log('组队中...', cga.getTeamPlayers().length);
                }
            }
            else {
                // 组别人
                console.log('我是队员');
                let leader;
                while (!(leader = cga.findPlayerUnit(amsg.name)))
                    await ff.delay(1000);
                let coord = cga.getRandomSpace(leader.xpos, leader.ypos)
                await ff.walk(coord[0], coord[1]);
                await ff.joinTeam(amsg.name);
            }

            // 写入配置
            scriptSettings.players = [];
            cga.getTeamPlayers().forEach( (t) => {
                scriptSettings.players.push(t.name);
            } );
            scriptSettings.cond = cond; // 本人的退出条件
            ff.writeMyselfScriptSettings(__filename, scriptSettings);
        }
        else { // 有设置文件，读取到设置

            if (scriptSettings.players[0] == cga.GetPlayerInfo().name) { // 我是队长
                console.log('我是队长');
                while (await ff.waitTeam(scriptSettings.players)) {
                    await ff.delay(1000);
                    console.log('组队中...', cga.getTeamPlayers().length);
                }
            }
            else { // 我是队员
                console.log('我是队员');
                let leader;
                while (!(leader = cga.findPlayerUnit(scriptSettings.players[0])))
                    await ff.delay(1000);
                let coord = cga.getRandomSpace(leader.xpos, leader.ypos)
                await ff.autoWalk(coord[0], coord[1]);
                await ff.joinTeam(scriptSettings.players[0]);
            }
        }
    }

    // 是否为队长
    let isTeamLeader = ff.isTeamLeader();
    console.log('是否为队长', isTeamLeader);

    // 创建结束条件函数
    let endcond = ff.createEndCondFunc(cond);
    console.log('回补条件', cond);

    // 队员回补标记
    let teamPlayerBack = false;
    // 检查条件并取得下一步执行的动作
    let CheckCondGetAction = (endcond, sellStone = false) => {
        let act = curAction;
        if ( endcond() || teamPlayerBack ) { // 退出条件达成，回补
            act = ff.ActGoRecovery;
            if (sellStone && cga.getSellStoneItem().length > 0) { // 背包里有可卖的东西
                act = ff.ActGoSell;
            }
            teamPlayerBack = false;
        }
        else if ( sellStone && ff.getInventoryEmptySlotCount() < 1 ) { // 背包满了，卖东西
            act = ff.ActGoSell;
            teamPlayerBack = false;
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
            if (ff.curInPtRange( placeCoord, 2 )) { // 练级地附近
                switch (curAction) {
                case ff.ActGoBattle:
                    if (isTeamLeader) {
                        // 队长监听
                        cga.waitTeammateSay((fromTeammate, msg) => {
                            console.log(msg);
                            if (msg.indexOf('<回补>') >= 0) {
                                teamPlayerBack = true;
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
                            while (await ff.encounterEnemy(coord, dir, 300, endcond, 59503)) {
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
                        if (endcond()) {
                            cga.EnableFlags(cga.ENABLE_FLAG_TEAMCHAT, true);
                            cga.SayWords('<回补>', 0, 3, 1);
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
                teamPlayerBack = false;
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
