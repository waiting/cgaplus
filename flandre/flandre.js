const request = require('request');
const fs = require('fs');

global.ff = {};

// 常量设置
// 脚本当前动作常量
/** 去战斗 */
ff.ActGoBattle = 1;
/** 去回复 */
ff.ActGoRecovery = 2;
/** 去卖东西 */
ff.ActGoSell = 3;
/** 是什么动作 */
ff.actString = (act) => {
    switch (act) {
    case ff.ActGoBattle:
        return '去战斗';
        break;
    case ff.ActGoRecovery:
        return '去恢复';
        break;
    case ff.ActGoSell:
        return '去卖店';
        break;
    }
    return '未知' + act;
}
/** 输出是什么动作 */
ff.actOutput = (act) => {
    console.log( '========', ff.actString(act), '========' );
}
/** 当前动作 */
ff.curAction = 0;
ff.setAction = (act = ff.ActGoBattle) => {
    ff.curAction = act;
}

/** 根据`cond`创建终止条件函数，`MaxHp*MinHpPercent`和`minHp`谁小就以谁为准，Mp一样。
 *  {
        playerMinHp: 100,           // 人物最小生命值
        playerMinHpPercent: 0.5,    // 人物最低生命值比率
        playerMinMp: 15,            // 人物最小魔力值
        playerMinMpPercent: 0.5,    // 人物最低魔力值比率
        petMinHp: 100,              // 宠物最小生命值
        petMinHpPercent: 0.5,       // 宠物最低生命值比率
        petMinMp: 20,               // 宠物最小魔力值
        petMinMpPercent: 0.5,       // 宠物最低魔力值比率
        teamMinPlayers?: 5,          // 最小队员数，小于这个数停止遇敌
        itemMinEmptySlot?: 1,        // 最小道具空位数，小于这个数停止遇敌
        //playerHp: 0,              // 用于比较的人物生命值，自动根据上面的数值计算
        //playerMp: 0,              // 用于比较的人物魔法值，自动根据上面的数值计算
        //petHp: 0,                 // 用于比较的宠物生命值，自动根据上面的数值计算
        //petMp: 0,                 // 用于比较的宠物魔法值，自动根据上面的数值计算
    }
 */
ff.createEndCondFunc = (cond) => {
    let player = cga.GetPlayerInfo();
    let pet = cga.GetPetInfo(player.petid);
    if ( player.maxhp * cond.playerMinHpPercent < cond.playerMinHp ) cond.playerHp = player.maxhp * cond.playerMinHpPercent;
    else cond.playerHp = cond.playerMinHp;
    if ( player.maxmp * cond.playerMinMpPercent < cond.playerMinMp ) cond.playerMp = player.maxmp * cond.playerMinMpPercent;
    else cond.playerMp = cond.playerMinMp;
    if ( pet.maxhp * cond.petMinHpPercent < cond.petMinHp ) cond.petHp = pet.maxhp * cond.petMinHpPercent;
    else cond.petHp = cond.petMinHp;
    if ( pet.maxmp * cond.petMinMpPercent < cond.petMinMp ) cond.petMp = pet.maxmp * cond.petMinMpPercent;
    else cond.petMp = cond.petMinMp;

    if (cond.itemMinEmptySlot !== undefined && cond.teamMinPlayers !== undefined) {
        return () => {
            let player = cga.GetPlayerInfo();
            let pet = cga.GetPetInfo(player.petid);
            return player.hp < cond.playerHp || player.mp < cond.playerMp || pet.hp < cond.petHp || pet.mp < cond.petMp || ff.getInventoryEmptySlotCount() < cond.itemMinEmptySlot || cga.GetTeamPlayerInfo().length < cond.teamMinPlayers;
        };
    }
    else if (cond.itemMinEmptySlot !== undefined) {
        return () => {
            let player = cga.GetPlayerInfo();
            let pet = cga.GetPetInfo(player.petid);
            return player.hp < cond.playerHp || player.mp < cond.playerMp || pet.hp < cond.petHp || pet.mp < cond.petMp || ff.getInventoryEmptySlotCount() < cond.itemMinEmptySlot;
        };
    }
    else if (cond.teamMinPlayers !== undefined) {
        return () => {
            let player = cga.GetPlayerInfo();
            let pet = cga.GetPetInfo(player.petid);
            return player.hp < cond.playerHp || player.mp < cond.playerMp || pet.hp < cond.petHp || pet.mp < cond.petMp || cga.GetTeamPlayerInfo().length < cond.teamMinPlayers;
        };
    }
    else {
        return () => {
            let player = cga.GetPlayerInfo();
            let pet = cga.GetPetInfo(player.petid);
            return player.hp < cond.playerHp || player.mp < cond.playerMp || pet.hp < cond.petHp || pet.mp < cond.petMp;
        };
    }
}

// 同步方法 ===========================================================================================

// 工具类函数 ------------------------------------------------------------------------------------------
let arrayContain = function(arr, e) {
    let found = false;
    arr.forEach( (v) => {
        if (v == e) {
            found = true;
            return;
        }
    } );
    return found;
}
/** 数组中是否包含指定元素或元素集 */
ff.isArrayContain = (arr, e) => {
    if (e instanceof Array) {
        let foundComplete = e.length > 0 ? true : false;
        e.forEach( (v) => {
            if (!arrayContain(arr, v)) {
                foundComplete = false;
                return;
            }
        } );
        return foundComplete;
    }
    return arrayContain(arr, e);
}

/** 坐标在一个点的半径范围内 */
ff.coordInPtRange = (coord, pt, r) => {
    if (pt instanceof Array) {
        pt = { x: pt[0], y: pt[1] };
    }
    return coord.x >= (pt.x - r) && coord.x <= (pt.x + r) && coord.y >= (pt.y - r) && coord.y <= (pt.y + r);
}
/** 当前坐标在一个点的半径范围内 */
ff.curInPtRange = (pt, r) => {
    return ff.coordInPtRange(cga.GetMapXY(), pt, r);
}
/** 当前地图是否是指定地图 */
ff.curMapIs = (map) => {
    if (typeof(map) == 'string') {
        return cga.GetMapName() == map;
    }
    else if (typeof(map) == 'number') {
        return cga.GetMapIndex().index3 == map;
    }
    else if (map instanceof RegExp) {
        return map.exec(cga.GetMapName());
    }
}
/** object to urlencoded string */
ff.uri = (obj) => {
    let flag = false;
    let str = '';
    if ( typeof obj == 'object' )
        for (let k in obj) {
            if (flag) str += '&';
            if (!(obj[k] instanceof String) && typeof obj[k] == 'object') {
                str += encodeURIComponent(k) + '=' + encodeURIComponent(JSON.stringify(obj[k]));
            }
            else {
                str += encodeURIComponent(k) + '=' + encodeURIComponent(obj[k]);
            }
            flag = true;
        }
    else
        str = obj;
    return str;
}
/** 判断是否为绝对路径 */
ff.isAbsPath = (path) => {
    return path.length == 0 ? false : ( ( path[0] == '/' || path[0] == '\\' ) || ( path.length > 1 && path[1] == ':' ) );
}

/** 是否为空对象{} */
ff.isEmptyObject = (obj) => {
    return Object.keys(obj).length == 0;
}

/** 获取cga挂在游戏时的端口 */
ff.getGamePort = () => {
    let port;
    if (process.argv.length >= 3 && parseInt(process.argv[2]) > 0)
        port = parseInt(process.argv[2]);
    else if (process.env.CGA_GAME_PORT && parseInt(process.env.CGA_GAME_PORT) > 0)
        port = parseInt(process.env.CGA_GAME_PORT);
    else
        port = 0;

    return port;
}

/** 获取背包剩余容量 */
ff.getInventoryEmptySlotCount = (capacity = 20) => {
    cga.GetItemsInfo().forEach(item => {
        if (item.pos > 7) {
            capacity--;
        }
    });
    return capacity;
}

/** 从文件读取JSON设置 */
ff.readSettingsFromFile = (settingsFile) => {
    try {
        let data = fs.readFileSync(settingsFile).toString();
        return JSON.parse(data);
    }
    catch (e) {
        return {};
    }
}

/** 写入JSON设置到文件 */
ff.writeSettingsToFile = (settingsFile, settings) => {
    try {
        fs.writeFileSync(settingsFile, JSON.stringify(settings, undefined, '    '));
    }
    catch (e) {
    }
}

/** 取得本人脚本设置文件名 */
ff.getMyselfScriptSettingsFileName = (scriptFile) => {
    let scriptStamp, res;
    let reScriptStamp = /_(.+)\.js$/;
    if ( res = reScriptStamp.exec(scriptFile) ) {
        scriptStamp = res[1]; // 取得脚本戳
    }
    else {
        let pos1 = scriptFile.lastIndexOf('\\') + 1;
        let pos2 = scriptFile.lastIndexOf('.');
        let name = scriptFile.substr(pos1, pos2==-1 ? undefined : pos2 - pos1);
        scriptStamp = name;
    }
    return scriptStamp + '_' + cga.GetPlayerInfo().name + '.json';
}
/** 读取本人的脚本设置 */
ff.readMyselfScriptSettings = (scriptFile) => {
    let scriptSettingsFile = ff.getMyselfScriptSettingsFileName(scriptFile);
    return ff.readSettingsFromFile('script-settings/' + scriptSettingsFile);
}

/** 写入本人的脚本设置 */
ff.writeMyselfScriptSettings = (scriptFile, scriptSettings) => {
    let settingsFile = ff.getMyselfScriptSettingsFileName(scriptFile);
    if (!fs.existsSync('script-settings')) fs.mkdirSync('script-settings');
    ff.writeSettingsToFile('script-settings/' + settingsFile, scriptSettings);
}

/** 判断组队状态下是否为队长或者通过配置文件判断是否为队长 */
ff.isTeamLeaderOrScriptSettings = (scriptSettings, teamplayers = null) => {
    return ff.isTeamLeader(teamplayers) || (scriptSettings.players instanceof Array) && (scriptSettings.players.length == 0 || scriptSettings.players[0] == cga.GetPlayerInfo().name);
}

/** 是否是队长 */
ff.isTeamLeader = (teamplayers = null) => {
    teamplayers = teamplayers || cga.getTeamPlayers();
    return (teamplayers.length > 0 && teamplayers[0].is_me) ? true : false;
}

/** 是否在队 */
ff.isInTeam = (teamplayers = null) => {
    teamplayers = teamplayers || cga.getTeamPlayers();
    return teamplayers.length > 0;
}

/** 获取队伍平均等级 */
ff.getTeamAvgLevel = () => {
    let level = 0;
    let arr = cga.getTeamPlayers();
    if (arr.length) {
        arr.forEach( (player) => {
            level += player.level;
        } );
        return level / arr.length;
    }
    else {
        return cga.GetPlayerInfo().level;
    }
}

/** 获取队伍各队员名称 */
ff.getTeamPlayerNames = () => {
    let players = [];
    cga.getTeamPlayers().forEach( (t) => {
        players.push(t.name);
    } );
    return players;
}

// 异步方法 ===========================================================================================

/** 延迟 */
ff.delay = (millis) => new Promise( resolve => setTimeout( () => { resolve(); }, millis ) );

/** Promise调用链模拟循环 */
ff.loop = (func = () => { return Promise.resolve(); }, timer = Date.now()) => {
    const result = func(timer);
    if (result instanceof Promise) {
        return result.then(
            () => ff.loop(func, timer),
            e => {
                if (e && typeof e != 'number') console.log('loop', e);
                return e;
            }
        );
    }
    else if (result === true) {
        return ff.loop(func, timer);
    }
    else
        return Promise.resolve(result);
};

/** 自动走到目的地 targetList: [ [x, y, map?, desiredX?, desiredY?], ... ] */
ff.autoWalkList = ( targetList ) => new Promise( (resolve, reject) => {
    cga.isMoveThinking = false;
    cga.walkList( targetList, (err, reason) => {
        if (err) {
            err.reason = reason;
            reject(err);
        }
        else {
            resolve();
        }
    } );
} );

/** 自动走到目的地 x, y, map, desiredX, desiredY */
ff.autoWalk = (x, y, map, desiredX, desiredY) => new Promise( (resolve, reject) => {
    cga.isMoveThinking = false;
    if (x instanceof Array) {
        cga.walkList( [x], (err, reason) => {
            if (err) {
                err.reason = reason;
                reject(err);
            }
            else {
                resolve();
            }
        } );
    }
    else {
        cga.walkList( [ [x, y, map, desiredX, desiredY] ], (err, reason) => {
            if (err) {
                err.reason = reason;
                reject(err);
            }
            else {
                resolve();
            }
        } );
    }
} );

/** 走到指定位置，相当于鼠标点击，不自动寻路 x: int, y: int, map: string|int|null, desiredX: int|null, desiredY: int|null */
ff.walk = (x, y, map, desiredX, desiredY) => new Promise( (resolve, reject) => {
    cga.AsyncWalkTo(x, y, map, desiredX, desiredY, (err, reason) => {
        if (err) {
            err.reason = reason;
            reject(err);
        }
        else {
            resolve();
        }
    } );
} );

/** 转向切图 dir: 0~7, arg: { 'map': 地图数组|地图名|地图索引, 'x': x坐标, 'y': y坐标, 'delay': 延迟=1000, 'timeout': 超时=5000 } */
ff.turnDirWarpMap = (dir, map, x, y, delay, timeout) => new Promise( (resolve, reject) => {
    cga.turnDir(dir);
    resolve( ff.waitWarpMap(map, x, y, delay, timeout) );
} );

/** 等待切换地图 arg: { 'map': 地图数组|地图名|地图索引, 'x': x坐标, 'y': y坐标, 'delay': 延迟=1000, 'timeout': 超时=5000 } */
ff.waitWarpMap = (map, x, y, delay, timeout) => new Promise( (resolve, reject) => {
    let arg;

    if (typeof map == 'object' && !(map instanceof Array) && map.map !== undefined ) {
        arg = map;
    }
    else {
        arg = {};
        if (map !== undefined ) arg.map = map;
        if (x !== undefined ) arg.x = x;
        if (y !== undefined ) arg.y = y;
        if (delay !== undefined ) arg.delay = delay;
        if (timeout !== undefined ) arg.timeout = timeout;
    }

    if (arg.delay === undefined) arg.delay = 1000;
    if (arg.timeout === undefined) arg.timeout = 5000;

    /** 如果因为触发战斗打断了等待过程，则回调
        cb(new Error('Battle status.'), 2)
        如果等待超过arg.timeout指定的时间还没有到达目的地，则回调
        cb(new Error('Async callback timeout.'), 3)
        如果因为发生了坐标回滚（如吃了深蓝之后遇敌），则回调
        cb(new Error('Position are syncronized by server.'), 5) */
    console.log('等待地图', arg);
    cga.AsyncWaitMovement( arg, (err, code) => {
        if (err) {
            err.errcode = code;
            reject(err);
        }
        else {
            resolve(code);
        }
    } );
} );

/** 登回城 */
ff.logBack = (times = 1) => new Promise( (resolve, reject) => {
    if (times > 9) {
        reject( new Error('登回城失败') );
    }
    else {
        cga.LogBack();
    }
    resolve( ff.delay(2000) );
} ).then( () => {
    return ff.waitWarpMap( ['法兰城', '艾尔莎岛', '阿凯鲁法村', '哥拉尔镇'] ).catch( (e) => {
        console.log('等待切图出错', e);
        if (e.errcode == 2) { // 战斗打断
            return ff.waitBattleEnd(0).then( () => ff.waitWarpMap( ['法兰城', '艾尔莎岛', '阿凯鲁法村', '哥拉尔镇'] ).catch( (e) => ff.logBack(times + 1) ) );
        }
        else {
            return ff.logBack(times + 1);
        }
    } );
} ).catch( e => console.log(e) );

/** 转向打开对话框，返回对话框数据，timeout默认3秒 */
ff.turnDirNpcDialog = (dir, timeout = 3000) => Promise.resolve().then( () => {
    cga.turnDir(dir);
    return ff.waitNpcDialog(timeout);
} );

/** 转向坐标（坐标必须离人物1格或相邻）打开对话框，返回对话框数据，timeout默认3秒 */
ff.turnToNpcDialog = (x, y, timeout = 3000) =>  Promise.resolve().then( () => {
    cga.turnTo(x, y);
    return ff.waitNpcDialog(timeout);
} );

/** 等待打开对话框，timeout默认3秒 */
ff.waitNpcDialog = (timeout = 3000) => new Promise( (resolve, reject) => {
    /* 等待对话框弹出成功则回调
    cb( null, {
        type : Integer 对话框类型
        options : Integer 对话框按钮可选项，按位参与与或计算，1=确定，2=取消，4=是，8=否，16=上一步，32=下一步，比如（4|8=12表示存在是和否两个按钮）
        dialog_id : Integer 对话框id
        npc_id : Integer NPC ID
        message : String 消息体
    } )
    失败则回调cb(new Error('Unknown exception.'))。
    特别地，等待超时则回调cb(new Error('Async callback timeout.'))。 */
    cga.AsyncWaitNPCDialog( (err, dlg) => {
        if (err) {
            reject(err);
        }
        else {
            resolve(dlg);
        }
    }, timeout);
} );

/** 等待战斗结束，timeout为0表示一直等待 */
ff.waitBattleEnd = (timeout = 0) => new Promise( (resolve, reject) => {
    if (!cga.isInBattle()) {
        resolve(false);
        return;
    }

    cga.waitForBattleEnd( (err, r) => {
        if (err) {
            reject(err);
        }
        else {
            let waitNormalState = (times = 0) => {
                if (cga.isInNormalState()) {
                    resolve(true);
                    return;
                }
                if (times < 10) {
                    setTimeout(waitNormalState, 500, times + 1);
                }
                else {
                    let e = new Error('等不到正常非战斗状态');
                    e.battleAction = r;
                    reject(e);
                }
            }
            setTimeout(waitNormalState, 500);
        }
    }, timeout);
} );

/** 等待战斗动作，timeout为0表示一直等待
 *  battleAction：FL_BATTLE_ACTION_BEGIN=16,
 *  FL_BATTLE_ACTION_END=8:战斗结束,
 *  FL_BATTLE_ACTION_ISDOUBLE,
 *  FL_BATTLE_ACTION_ISPLAYER=1,
 *  FL_BATTLE_ACTION_ISSKILLPERFORMED */
ff.waitBattleAction = (battleAction = cga.FL_BATTLE_ACTION_END, timeout = 30000) => new Promise( (resolve, reject) => {
    cga.AsyncWaitBattleAction( (err, result) => {
        if (err) {
            reject(err);
        }
        else {
            if (result == battleAction) {
                setTimeout(resolve, 1000, result);
            }
            else {
                resolve( ff.waitBattleAction(battleAction, timeout) );
            }
        }
    }, timeout);
} );

/** 等待战斗或者切图 */
ff.waitAfterBattle = (battled = false) => Promise.resolve().then( () => {
    const world = cga.GetWorldStatus();
    const game = cga.GetGameStatus();
    console.log('waitAfterBattle', 'world=' + world, 'game=' + game, 'battled=' + battled );
    if (world == 9 && game == 3) {
        return battled;
    }
    else {
        return ff.delay(1000).then( () => ff.waitAfterBattle(battled ? battled : world == 10) );
    }
} ).catch( r => {
    console.log('waitAfterBattle catch', r);
    return ff.waitAfterBattle(battled);
} );

/** 原地遇敌，pos为当前位置，dir方向，ms为时间间隔，endcond为结束条件，map为遇敌地图名或者遇敌地图索引 */
ff.encounterEnemyV0 = (pos, dir, ms = 300, endcond = () => false, map = null) => new Promise( (resolve, reject) => {
    let wantMoveDirTable = [ 4, 5, 6, 7, 0, 1, 2, 3 ];
    let counter = 0;
    map = map || cga.GetMapIndex().index3;

    let move = () => {
        if (cga.isInBattle()) { // 进入战斗
            resolve(true);
            return;
        }
        else if ( endcond() ) { // 根据条件函数判断是否结束遇敌
            resolve(false);
            return;
        }

        if ( ff.curMapIs(map) ) {
            let curpos = cga.GetMapXY();
            if (dir == 0) {
                if (pos.x == curpos.x)
                    cga.ForceMove(dir, false);
                else
                    cga.ForceMove(wantMoveDirTable[dir], false);
            }
            else if (dir == 4) {
                if (pos.x == curpos.x)
                    cga.ForceMove(dir, false);
                else
                    cga.ForceMove(wantMoveDirTable[dir], false);
            }
            else if (dir == 2) {
                if (pos.y == curpos.y)
                    cga.ForceMove(dir, false);
                else
                    cga.ForceMove(wantMoveDirTable[dir], false);
            }
            else if (dir == 6) {
                if (pos.y == curpos.y)
                    cga.ForceMove(dir, false);
                else
                    cga.ForceMove(wantMoveDirTable[dir], false);
            }
            else if (dir == 1) {
                if (pos.x == curpos.x)
                    cga.ForceMove(dir, false);
                else
                    cga.ForceMove(wantMoveDirTable[dir], false);
            }
            else if (dir == 5) {
                if (pos.x == curpos.x)
                    cga.ForceMove(dir, false);
                else
                    cga.ForceMove(wantMoveDirTable[dir], false);
            }
            else if (dir == 3) {
                if (pos.y == curpos.y)
                    cga.ForceMove(dir, false);
                else
                    cga.ForceMove(wantMoveDirTable[dir], false);
            }
            else if (dir == 7) {
                if (pos.y == curpos.y)
                    cga.ForceMove(dir, false);
                else
                    cga.ForceMove(wantMoveDirTable[dir], false);
            }
            //console.log('遇敌中', pos, curpos );
            // 继续移动
            setTimeout(move, ms);
            counter++;
            //if (counter % 4 == 0) {
            //}
        }
        else { // 地图改变
            reject( new Error('地图不同，encounterEnemy()抛出异常') );
            return;
        }
    }

    move();
} );

/** 原地遇敌，pos为当前位置，dir方向，ms为时间间隔，endcond为结束条件，map为遇敌地图名或者遇敌地图索引 */
ff.encounterEnemy = (pos, dir, ms = 300, endcond = () => false, map = null) => new Promise( (resolve, reject) => {
    let wantMoveDirTable = [4, 5, 6, 7, 0, 1, 2, 3];
    map = map || cga.GetMapIndex().index3;

    let move = () => {
        if (cga.isInBattle()) { // 进入战斗
            resolve(true);
            return;
        }
        else if ( endcond() ) { // 根据条件函数判断是否结束遇敌
            resolve(false);
            return;
        }
        else {
            if ( ff.curMapIs(map) ) { // 地图尚未改变才继续遇敌
                let curpos = cga.GetMapXY();
                if (dir == 0) {
                    if (pos.x == curpos.x)
                        cga.ForceMove(dir, false);
                    else
                        cga.ForceMove(wantMoveDirTable[dir], false);
                }
                else if (dir == 4) {
                    if (pos.x == curpos.x)
                        cga.ForceMove(dir, false);
                    else
                        cga.ForceMove(wantMoveDirTable[dir], false);
                }
                else if (dir == 2) {
                    if (pos.y == curpos.y)
                        cga.ForceMove(dir, false);
                    else
                        cga.ForceMove(wantMoveDirTable[dir], false);
                }
                else if (dir == 6) {
                    if (pos.y == curpos.y)
                        cga.ForceMove(dir, false);
                    else
                        cga.ForceMove(wantMoveDirTable[dir], false);
                }
                else if (dir == 1) {
                    if (pos.x == curpos.x)
                        cga.ForceMove(dir, false);
                    else
                        cga.ForceMove(wantMoveDirTable[dir], false);
                }
                else if (dir == 5) {
                    if (pos.x == curpos.x)
                        cga.ForceMove(dir, false);
                    else
                        cga.ForceMove(wantMoveDirTable[dir], false);
                }
                else if (dir == 3) {
                    if (pos.y == curpos.y)
                        cga.ForceMove(dir, false);
                    else
                        cga.ForceMove(wantMoveDirTable[dir], false);
                }
                else if (dir == 7) {
                    if (pos.y == curpos.y)
                        cga.ForceMove(dir, false);
                    else
                        cga.ForceMove(wantMoveDirTable[dir], false);
                }
            }
            else { // 地图改变
                reject( new Error('地图不同，encounterEnemy()抛出异常') );
                return;
            }

            // 继续尝试移动
            setTimeout(move, ms);
        }
    }

    // 尝试移动
    move();
} );

ff.encounter = (pos, dir, ms = 300, endcond = null, map = null) => ff.encounterEnemy(pos, dir, ms, endcond, map).then( r => {
    if (r) {
        return ff.delay(1000).then( () => ff.waitBattleEnd(0) ).then( () => ff.delay(1000) ).then( () => ff.encounter(pos, dir, ms, endcond, map) );
    }
    else {
        return Promise.resolve(false);
    }
} );

ff.encounter1 = (pos, dir, ms = 300, endcond = null, map = null) => ff.encounterEnemy(pos, dir, ms, endcond, map).then( r => {
    if (r) {
        return ff.delay(1000).then( () => ff.waitAfterBattle() ).then( () => ff.delay(1000) ).then( () => ff.encounter1(pos, dir, ms, endcond, map) );
    }
    else {
        return Promise.resolve(false);
    }
} );

/** 等待聊天信息，或者超时
 *
 *  cb({
 *      unitid: number,
 *      name: string, // 人名
 *      msg: string,
 *      group: boolean
 *  })
 *  cb()返回值决定是否继续下一次等待,
 *  timeout 超时
 */
ff.asyncWaitAnyChat = (cb, timeout = 3000) => {
    cga.AsyncWaitChatMsg( (err, r) => {
        let listen = false;
        if (err) {
            listen = cb(null);
            if (listen) ff.asyncWaitAnyChat(cb, timeout);
            return;
        }
        let aMsg = {};
        aMsg.unitid = r.unitid;
        if (r.unitid != -1) {
            let re = /(.+?)\: /;
            let res;
            if (res = re.exec(r.msg)) {
                let re1 = /\[GP\](.+)/;
                let res1;
                if (res1 = re1.exec(res[1])) {
                    aMsg.name = res1[1];
                    aMsg.group = true;
                }
                else {
                    aMsg.name = res[1];
                }
                aMsg.msg = r.msg.substr( res.index + res[0].length );
            }
            else {
                aMsg.name = '';
                aMsg.msg = r.msg;
            }
        }
        else {
            aMsg.name = '';
            aMsg.msg = r.msg;
        }
        listen = cb(aMsg);
        if (listen) ff.asyncWaitAnyChat(cb, timeout);
    }, timeout);
}

/** 异步等待系统信息，或者超时
 *
 *  cb(msg: string)
 *  cb()返回值决定是否继续下一次等待,
 *  timeout 超时
 */
ff.asyncWaitSysMsg = (cb, timeout = 3000) => {
    cga.AsyncWaitChatMsg( (err, r) => {
        let listen = false;
        if (!r || r.unitid != -1) {
            listen = cb(null);
            if (listen) ff.asyncWaitSysMsg(cb, timeout);
            return;
        }
        listen = cb(r.msg);	
        if (listen) ff.asyncWaitSysMsg(cb, timeout);
    }, timeout);
}

/** 等待任意聊天信息，或超时。当timeout<1时，会一直等待直至收到任意信息 */
ff.waitAnyChat = (timeout = 3000) => new Promise( (resolve, reject) => {
    ff.asyncWaitAnyChat((amsg) => {
        if (amsg) {
            resolve(amsg);
            return false;
        }
        else {
            if (timeout > 0) {
                resolve(null);
                return false;
            }
            else {
                return true;
            }
        }
    }, timeout);
} );

/** 等待指定聊天信息，或超时。当timeout<1时，会一直等待直至收到指定信息 */
ff.waitSpecifiedChat = (filter, name = null, timeout = 0) => ff.waitAnyChat(timeout).then( (amsg) => {
    if (amsg) {
        let result;
        if (
            (typeof(filter) == 'number' && filter == amsg.msg ||
            typeof(filter) == 'string' && filter == amsg.msg ||
            filter instanceof RegExp && (result = filter.exec(amsg.msg))) &&
            (!name || (
                typeof(name) == 'number' && name == amsg.name ||
                typeof(name) == 'string' && name == amsg.name ||
                name instanceof RegExp && name.exec(amsg.name)
            ))
        ) {
            if (result) amsg.result = result;
            return amsg;
        }
        else {
            return ff.waitSpecifiedChat(filter, name, timeout);
        }
    }
    else {
        if (timeout > 0) {
            return null;
        }
        else {
            return ff.waitSpecifiedChat(filter, name, timeout);
        }
    }
} );

/** 等待任意聊天一次，或超时。当timeout<1时，会一直等待直至收到指定信息 */
ff.waitAnyChatOnce = (timeout = 3000) => new Promise( (resolve, reject) => {
    ff.asyncWaitAnyChat((amsg) => {
        resolve(amsg);
        return false;
    }, timeout);
} );

/**
 * 发送聊天信息
 * @param {string} msg 表示喊话内容，建议不要超过127个中文字符或255个英文字符，否则有可能被强制截断。
 * @param {number} color 0=白色#FFFFFF，1=青色#00FFFF，2=紫色#FF00FF，3=深蓝#0000FF，4和5=亮绿#00FF00，6=红色#FF0000，7=灰色#A0A0A4，8=淡蓝#A6CAF0，9=深海洋绿#C0DCC0
 * @param {number} range 0=关，1-5=1-5格子，6=交易，7=全图
 * @param {number} font 0=中，1=小，2=大
 */
ff.chat = (msg = '', color = 0, range = 5, font = 0) => {
    cga.SayWords(msg, color, range, font);
    return Promise.resolve();
}

/** 加队 */
ff.addTeammate = (name) => new Promise( (resolve, reject) => {
    cga.addTeammate(name, (r) => {
        if (r) {
            resolve();
        }
        else {
            reject();
        }
    } );
} );

/** 等待组队 */
ff.waitTeammates = (players) => new Promise( (resolve, reject) => {
    cga.waitTeammates( players, r => resolve(!r) );
} );

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
/**
 * 等待加载设置并初始化团队，自动组队，自动加载存储组队条件信息
 * @param {string} scriptFile 脚本文件名
 * @param {object} cond 回补条件
 * @returns scriptSettings
 */
ff.waitLoadSettingsAndInitTeam = async (scriptFile, cond) => {

    // 等待战斗结束
    await ff.waitBattleEnd(0);

    // 读取设置
    let scriptSettings = ff.readMyselfScriptSettings(scriptFile);
    // 如果从设置文件读到回补条件，那就设为当前回补条件
    if ( scriptSettings.cond ) cond = scriptSettings.cond;

    if (ff.isInTeam()) { // 已经组队
        if (ff.isEmptyObject(scriptSettings)) { // 没有设置文件
            scriptSettings.players = ff.getTeamPlayerNames();
            scriptSettings.cond = cond; // 本人的退出条件
            ff.writeMyselfScriptSettings(scriptFile, scriptSettings);
            console.log('更新队伍信息', scriptSettings.players);
        }
        else { // 有设置文件
            let amsg;
            while (cga.getTeamPlayers().length < scriptSettings.players.length && (!(amsg = await ff.waitAnyChatOnce(2000)) || amsg.msg.indexOf('确认队伍') < 0) ) {
                let players = ff.getTeamPlayerNames();
                console.log('尝试组队中，说<确认队伍>则更新为当前队伍信息...,', '当前队伍:', players);
            }

            let players = ff.getTeamPlayerNames();
            if ( JSON.stringify(players) != JSON.stringify(scriptSettings.players) ) { // 组队有变化
                scriptSettings.players = players;
                scriptSettings.cond = cond; // 本人的退出条件
                ff.writeMyselfScriptSettings(scriptFile, scriptSettings);
                console.log('更新队伍信息', scriptSettings.players);
            }
            else {
                console.log('读取队伍信息', scriptSettings.players);
            }
        }
    }
    else { // 尚未组队
        if (ff.isEmptyObject(scriptSettings)) { // 没有设置文件
            console.log('请说“组X人”，X可以是0，2，3，4，5人，说的人被当成队长');
            let amsg = await ff.waitSpecifiedChat(/组(\d+)人/);
            let playersCount = parseInt(amsg.result[1]) + 0;
            console.log('组队人数:' + playersCount);
            if (amsg.name == cga.GetPlayerInfo().name) { // 是自己说的
                // 是队长
                await ff.chat(amsg.msg, 1, 5, 1);
                console.log('我是队长，脚本会重复刚才的话，避免由于范围过小导致其他队员听不到。');
                while (cga.getTeamPlayers().length < playersCount) {
                    let players = ff.getTeamPlayerNames();
                    await ff.delay(1000);
                    console.log('尝试组队中...,', '当前队伍:', players);
                }
            }
            else {
                // 是队员
                console.log('我是队员');
                let leader;
                while (!(leader = cga.findPlayerUnit(amsg.name)))
                    await ff.delay(1000);
                let coord = cga.getRandomSpace(leader.xpos, leader.ypos)
                await ff.autoWalk(coord[0], coord[1]);
                await ff.addTeammate(amsg.name);

                // 等待别人组满
                while (cga.getTeamPlayers().length < playersCount) {
                    let players = ff.getTeamPlayerNames();
                    await ff.delay(1000);
                    console.log('等待别人中...,', '当前队伍:', players);
                }
            }

            // 写入配置
            scriptSettings.players = ff.getTeamPlayerNames();
            scriptSettings.cond = cond; // 本人的退出条件
            ff.writeMyselfScriptSettings(scriptFile, scriptSettings);
            console.log('更新队伍信息', scriptSettings.players);
        }
        else { // 有设置文件，读取到设置
            if (scriptSettings.players.length == 0 || scriptSettings.players[0] == cga.GetPlayerInfo().name) { // 我是队长
                console.log('我是队长');
                while (await ff.waitTeammates(scriptSettings.players)) {
                    let players = ff.getTeamPlayerNames();
                    await ff.delay(1000);
                    console.log('尝试组队中...,', '当前队伍:', players);
                }
            }
            else { // 我是队员
                console.log('我是队员');
                let leader;
                while (!(leader = cga.findPlayerUnit(scriptSettings.players[0])))
                    await ff.delay(1000);
                let coord = cga.getRandomSpace(leader.xpos, leader.ypos)
                await ff.autoWalk(coord[0], coord[1]);
                await ff.addTeammate(scriptSettings.players[0]);
            }
            console.log('读取队伍信息', scriptSettings.players);
        }
    }
    return scriptSettings;
}

/** 出售魔石、卡片？、已鉴定卡片 */
ff.sellMisc = (dir) => new Promise( (resolve, reject) => {
    cga.turnDir(dir);
    cga.sellStone( (err, r) => {
        if (err) {
            reject(err);
        }
        else {
            resolve();
        }
    } );
} ).then( () => {
    return new Promise((resolve, reject) => {
        cga.ClickNPCDialog(-1, 0);
        cga.AsyncWaitNPCDialog( (err, dlg) => {
            if (err) {
                reject(err);
            }
            else {
                setTimeout(resolve, 500, dlg);
            }
        });
    });
} ).then( (dlg) => {
    cga.ClickNPCDialog(0, 2);
} );

/** 等待工作中的成果 */
ff.waitWorkingResult = (timeout = 3000) => new Promise( (resolve, reject) => {
    cga.AsyncWaitWorkingResult( (err, r) => {
        if (!err) {
            resolve(r);
        }
        else {
            reject(err);
        }
    }, timeout);
} );

/** 存入全部指定物品到银行，maxcount表示最大堆叠 */
ff.saveToBankAll = (filter, maxcount) => new Promise( (resolve, reject) => {
    cga.saveToBankAll(filter, maxcount, (err) => {
        if (!err) {
            resolve();
        }
        else {
            reject(err);
        }
    } );
} );

/** HTTP GET请求 */
ff.httpGet = (url, vars = {}, timeout = undefined) => new Promise( (resolve, reject) => {
    let varsStr = ff.uri(vars);
    url += url.indexOf('?') != -1 ? (varsStr.length > 0 ? '&' + varsStr : '') : (varsStr.length > 0 ? '?' + varsStr : '');
    let options = {
        'url': url,
        'json': true
    };
    if (timeout !== undefined) options.timeout = timeout;
    try {
        request.get( options, (error, response, body) => {
            if (error) {
                reject(error);
                return;
            }
            if ( response.statusCode && response.statusCode == 200 ) {
                resolve(body);
            }
            else {
                let e = new Error('HTTP GET请求失败');
                e.url = url;
                reject(e);
            }
        } );
    }
    catch (e) {
        reject(e);
    }
} );

/** HTTP POST请求 */
ff.httpPost = (url, vars = {}, dataBody = {}, timeout = undefined) => new Promise( (resolve, reject) => {
    let varsStr = ff.uri(vars);
    url += url.indexOf('?') != -1 ? (varsStr.length > 0 ? '&' + varsStr : '') : (varsStr.length > 0 ? '?' + varsStr : '');
    let options = {
        'url': url,
        'json': true,
        'body': dataBody
    };
    if (timeout !== undefined) options.timeout = timeout;
    try {
        request.post( options, (error, response, body) => {
            if (error) {
                reject(error);
                return;
            }
            if ( response.statusCode && response.statusCode == 200 ) {
                resolve(body);
            }
            else {
                let e = new Error('HTTP POST请求失败');
                e.url = url;
                reject(e);
            }
        } );
    }
    catch (e) {
        reject(e);
    }
} );

// GUI相关 ------------------------------------------------------------------------------------------
ff.gui = {
    /** 设置脚本
    arg = {
        path : "路径",
        autorestart : true, //自动重启脚本开启
        injuryprot : true, //受伤保护开启
        soulprot : true, //掉魂受伤保护开启
    } */
    setScript: function(arg, timeout) {
        cga.gui.init();
        return ff.httpPost('http://127.0.0.1:' + cga.gui.port + '/cga/LoadScript', {}, arg, timeout);
    },
    /** 玩家设置 arg结构参考保存的json设置文件 */
    setSettings: function(arg, timeout) {
        cga.gui.init();
        return ff.httpPost('http://127.0.0.1:' + cga.gui.port + '/cga/LoadSettings', {}, arg, timeout);
    },
    /** 取得玩家设置 */
    getSettings: function(timeout) {
        cga.gui.init();
        return ff.httpGet('http://127.0.0.1:' + cga.gui.port + '/cga/GetSettings', {}, timeout);
    },
};

// CGA方法包装 -----------------------------------------------------------------------------------------
ff.requestPk = () => {
    cga.DoRequest(cga.REQUEST_TYPE_PK);
}
/** 发出组队请求 */
ff.requestJoinTeam = () => {
    cga.DoRequest(cga.REQUEST_TYPE_JOINTEAM);
}
ff.requestExchangeCard = () => {
    cga.DoRequest(cga.REQUEST_TYPE_EXCHANGECARD);
}
ff.requestTrade = () => {
    cga.DoRequest(cga.REQUEST_TYPE_TRADE);
}
ff.requestKickTeam = () => {
    cga.DoRequest(cga.REQUEST_TYPE_KICKTEAM);
}
/** 发出离队请求 */
ff.requestLeaveTeam = () => {
    cga.DoRequest(cga.REQUEST_TYPE_LEAVETEAM);
}
ff.requestTradeConfirm = () => {
    cga.DoRequest(cga.REQUEST_TYPE_TRADE_CONFIRM);
}
ff.requestTradeRefuse = () => {
    cga.DoRequest(cga.REQUEST_TYPE_TRADE_REFUSE);
}
ff.requestRebirthOn = () => {
    cga.DoRequest(cga.REQUEST_TYPE_REBIRTH_ON);
}
ff.requestRebirthOff = () => {
    cga.DoRequest(cga.REQUEST_TYPE_REBIRTH_OFF);
}

// 回调类功能 ------------------------------------------------------------------------------------------
ff.isListenKeepAlive = false;
/** 防掉线 */
ff.keepAlive = (listen = true, timeout = 1000) => {
    ff.isListenKeepAlive = listen;
    if (!ff.isListenKeepAlive) return;
    let startTime = new Date().valueOf();
    ff.asyncWaitSysMsg((msg) => {
        if (msg && msg.indexOf('您处于＂离开＂状态') != -1) {
            console.log('间隔了', (new Date().valueOf() - startTime) / 1000, '秒');
            ff.chat('', 0, 1, 1);
            startTime = new Date().valueOf();
        }
        return ff.isListenKeepAlive; // 是否继续监听下一次消息
    }, timeout);
}
// 模块导出
module.exports = new Promise( resolve => {
    let cga = require('../cgaapi')( () => setTimeout( () => resolve(cga), 0 ) );
} ).then( cga => {
    global.cga = cga;
    return cga;
} );
