require('./flandre').then( async () => {

    // 动作：练级、回补、卖石
    ff.curAction = ff.ActGoBattle;

    // 遇敌速度
    let speed = 235;

    // 练级地：布拉基姆高地
    let placeCoord = [133, 118];

    // 根据队伍平均等级选择练级地点
    let ChoicePlace = () => {
        let lv = ff.getTeamAvgLevel();
        if (lv < 29) { // 刀鸡
            placeCoord = [38, 186];
            console.log('平均等级:' + lv, '打刀鸡');
        }
        else if (lv < 34) {
            placeCoord = [116, 204];
            console.log('平均等级:' + lv, '打龙骨');
        }
        else if (lv < 43) { // 黄金龙骨
            placeCoord = [135, 175];
            console.log('平均等级:' + lv, '打黄金龙骨');
        }
        else { // 银狮
            placeCoord = [133, 118];
            console.log('平均等级:' + lv, '打银狮');
        }
    }

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

    // 载入脚本设置
    let scriptSettings = await ff.readMyselfScriptSettings(__filename);
    if (scriptSettings.cond) cond = scriptSettings.cond;

    // 是否为队长
    let isTeamLeader = ff.isTeamLeaderOrScriptSettings(scriptSettings);

    // 创建结束条件函数
    let endcond = ff.createEndCondFunc(cond);
    console.log('回补条件', cond);

    // 检查条件并取得下一步执行的动作
    let CheckCondChangeAction = (cond) => {
        endcond = ff.createEndCondFunc(cond);
        if (endcond()) { // 退出条件达成，回补
            ff.curAction = ff.ActGoRecovery;
            if (cond.itemMinEmptySlot !== undefined && cga.getSellStoneItem().length > 0) { // 背包里有可卖的东西
                ff.curAction = ff.ActGoSell;
            }
        }
        return ff.curAction;
    }
    ff.actOutput(CheckCondChangeAction(cond));

    // 一轮统计
    let counter = 0;
    let curXp = cga.GetPlayerInfo().xp;

    // 监听回补
    ff.listenAction(true);

    // 脚本循环
    while (true) try {
        if (isTeamLeader || !ff.isInTeam()) {
            console.log( 'Map:', cga.GetMapName() + ',', 'Action:', ff.actString(ff.curAction) );
        }

        if ( ff.curMapIs('布拉基姆高地') ) {
            if (ff.curInPtRange(placeCoord, 3)) { // 练级地附近
                switch (ff.curAction) {
                case ff.ActGoBattle:
                    if (isTeamLeader) {
                        try {
                            let coord = cga.GetMapXY();
                            let dir = cga.getRandomSpaceDir(coord.x, coord.y);
                            console.log('开始遇敌');
                            console.log('遇敌...');
                            while (await ff.encounterEnemy(coord, dir, speed, endcond, 59503)) {
                                counter++;
                                console.log('进入第', counter, '次战斗');
                                await ff.delay(1000).then( () => ff.waitBattleEnd(0) ); // 等待战斗结束
                                console.log('结束第', counter, '次战斗');
                                await ff.delay(1000);
                                // 当动作不是‘去战斗’，退出循环
                                if ( CheckCondChangeAction(cond) != ff.ActGoBattle ) break;
                                console.log('遇敌...');
                            }
                            console.log('结束遇敌');
                            // 队长也要告诉队员要做什么
                            await ff.notifyAction();
                        }
                        catch (e) {
                            console.log(e);
                        }
                    }
                    else if (!ff.isInTeam()) { // 不在组队中，说明队长已经掉线，队员登回城
                        ff.setAction(ff.ActGoRecovery);
                        await ff.logBack();
                    }
                    else { // 队员在队中
                        await ff.delay(2000);
                        if (await ff.waitBattleEnd(0)) {
                            counter++;
                            console.log('第', counter, '次战斗结束');
                            CheckCondChangeAction(cond);
                            // 队员告诉队长需要做什么
                            await ff.notifyAction();
                        }
                    }
                    break;
                case ff.ActGoRecovery:
                case ff.ActGoSell:
                    if (isTeamLeader) {
                        await ff.autoWalk(30, 193, '盖雷布伦森林');
                    }
                    else if (!ff.isInTeam()) { // 不在组队中，说明队长已经掉线，队员登回城
                        await ff.logBack();
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
                    else if (!ff.isInTeam()) { // 不在组队中，说明队长已经掉线，队员登回城
                        ff.setAction(ff.ActGoRecovery);
                        await ff.logBack();
                    }
                    else {
                        await ff.delay(2000);
                    }
                    break;
                default:
                    if (isTeamLeader) {
                        await ff.autoWalk(30, 193, '盖雷布伦森林');
                    }
                    else if (!ff.isInTeam()) { // 不在组队中，说明队长已经掉线，队员登回城
                        await ff.logBack();
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
                else if (!ff.isInTeam()) { // 不在组队中，说明队长已经掉线，队员会艾夏岛
                    ff.setAction(ff.ActGoRecovery);
                    await ff.autoWalk(199, 211, '艾夏岛');
                }
                else {
                    await ff.delay(2000);
                }
                break;
            case ff.ActGoRecovery:
            case ff.ActGoSell:
            default:
                if (isTeamLeader || !ff.isInTeam()) {
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
                    if (isTeamLeader || !ff.isInTeam()) {
                        await ff.autoWalk(84, 112);
                        await ff.turnDirWarpMap(4, '艾夏岛', 164, 159);
                    }
                    else {
                        await ff.delay(2000);
                    }
                }
                else if (ff.curInPtRange([164, 159], 1)) {
                    if (isTeamLeader || !ff.isInTeam()) {
                        await ff.autoWalk(164, 159);
                        await ff.turnDirWarpMap(5, '艾夏岛', 151, 97);
                    }
                    else {
                        await ff.delay(2000);
                    }
                }
                else if (ff.curInPtRange([151, 97], 1)) {
                    if (isTeamLeader || !ff.isInTeam()) {
                        await ff.autoWalk(173, 114);
                    }
                    else {
                        await ff.delay(2000);
                    }
                }
                else if (ff.curInPtRange([173, 114], 1)) { // 组队点 173,114
                    if (isTeamLeader) {
                        scriptSettings = await ff.waitLoadSettingsAndInitTeam(__filename, cond);
                        if (scriptSettings.cond) cond = scriptSettings.cond;
                        // 根据队伍平均等级选择练级地点
                        ChoicePlace();

                        await ff.autoWalk(190, 116, '盖雷布伦森林');
                    }
                    else if (!ff.isInTeam()) {
                        scriptSettings = await ff.waitLoadSettingsAndInitTeam(__filename, cond);
                        if (scriptSettings.cond) cond = scriptSettings.cond;
                        // 重新判断队长
                        isTeamLeader = ff.isTeamLeaderOrScriptSettings(scriptSettings);

                        if (!isTeamLeader) {
                            // 根据队伍平均等级选择练级地点
                            ChoicePlace();
                        }
                    }
                    else {
                        await ff.delay(2000);
                    }
                }
                else {
                    if (isTeamLeader || !ff.isInTeam()) {
                        await ff.autoWalk(173, 114);
                    }
                    else {
                        await ff.delay(2000);
                    }
                }
                break;
            case ff.ActGoRecovery:
                if (isTeamLeader || !ff.isInTeam()) {
                    await ff.autoWalk(112, 81, '医院');
                }
                else {
                    await ff.delay(2000);
                }
                break;
            case ff.ActGoSell:
                if (isTeamLeader || !ff.isInTeam()) {
                    await ff.autoWalk(150, 125, '克罗利的店');
                }
                else {
                    await ff.delay(2000);
                }
                break;
            default:
                break;
            }
        }
        else if ( ff.curMapIs('克罗利的店') ) {
            switch (ff.curAction) {
            case ff.ActGoBattle:
            case ff.ActGoRecovery:
                if (isTeamLeader || !ff.isInTeam()) {
                    await ff.autoWalk(20, 23, '艾夏岛');
                }
                else {
                    await ff.delay(2000);
                }
                break;
            case ff.ActGoSell:
                if (isTeamLeader || !ff.isInTeam()) {
                    await ff.autoWalk(43, 23);
                    await ff.walk(43, 24);
                    await ff.walk(43, 23);
                    await ff.walk(43, 24);
                    await ff.walk(43, 23);
                    await ff.sellMisc([42, 23]);
                    ff.setAction(ff.ActGoRecovery);
                    ff.actOutput(ff.curAction);
                    if (isTeamLeader) {
                        await ff.delay(5000); // 等待队友卖石
                    }
                }
                else {
                    if (ff.curInPtRange([42, 23], 1)) {
                        await ff.sellMisc([42, 23]);
                        ff.setAction(ff.ActGoRecovery);
                        ff.actOutput(ff.curAction);
                    }
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
            case ff.ActGoRecovery:
            case ff.ActGoSell:
                if (isTeamLeader || !ff.isInTeam()) {
                    await ff.autoWalk(157, 93);
                    await ff.turnDirWarpMap(0, '艾夏岛');
                }
                else {
                    await ff.delay(2000);
                }
                break;
            default:
                break;
            }
        }
        else if ( ff.curMapIs('里谢里雅堡') ) {
            switch (ff.curAction) {
            case ff.ActGoBattle:
            case ff.ActGoRecovery:
                if (isTeamLeader || !ff.isInTeam()) {
                    await ff.logBack();
                }
                else {
                    await ff.delay(2000);
                }
                break;
            case ff.ActGoSell:
                if (isTeamLeader || !ff.isInTeam()) {
                    await ff.walk(30, 79);
                    await ff.sellMisc(6); // 卖石头
                    if ( cga.getSellStoneItem().length == 0 ) {
                        ff.curAction = ff.ActGoRecovery;
                        ff.actOutput(ff.curAction);
                    }
                }
                else {
                    await ff.delay(2000);
                }
                break;
            case ff.ActGoCure:
                let doctors = ff.getDoctorUnits();
                for (let i = 0; i < doctors.length; ++i) {
                    let doctor = doctors[i];
                    console.log(doctor.unit_name);
                }
                await ff.delay(2000);
                break;
            default:
                break;
            }
        }
        else if ( ff.curMapIs('医院') ) {
            switch (ff.curAction) {
            case ff.ActGoBattle:
                if (isTeamLeader || !ff.isInTeam()) {
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
                if (isTeamLeader || !ff.isInTeam()) {
                    await ff.autoWalk(35, 43);
                    await ff.turnDirNpcDialog(0); // 打开补血对话框
                    while ( cga.needSupplyInitial() ) {
                        console.log('补血中...');
                        await ff.delay(1000);
                    }
                }
                else {
                    await ff.delay(2000);
                }
                console.log( '本轮战斗场次:' + counter, '本轮获得经验:' + (cga.GetPlayerInfo().xp - curXp) );
                // 根据队伍平均等级选择练级地点
                ChoicePlace();
                ff.setAction(ff.ActGoBattle);
                ff.actOutput(ff.curAction);
                break;
            default:
                console.log('[' + cga.GetMapName() + ']不支持这个动作:', ff.actString(ff.curAction));
                break;
            }
        }
        else {
            console.log('处在未知地图[' + cga.GetMapName() + ']');
            await ff.delay(3000);
        }
    }
    catch (e) {
        console.log(e);
    }

} );
