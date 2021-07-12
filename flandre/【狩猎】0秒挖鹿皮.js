require('./flandre').then( async () => {
    let startTime = new Date().valueOf();
    let startItemCount = cga.getItemCount('鹿皮');
    await ff.autoWalk(130, 50, '盖雷布伦森林');
    await ff.autoWalk(175, 182);
    let counterTotal = 0; // 狩猎总次数
    let counter = 0; // 狩猎次数
    let counterTimeout = 0; // 超时次数，超时10次即判断技能工作状态可能失效，需要重新开启工作
    let workdelay = 0;
    let workdelay0Times = 30;
    let workdelay5500Times = 10;
    await ff.gui.setSettings({ player: { 'workdelay': workdelay } });
    let skillIndex = cga.findPlayerSkill('狩猎').index;
    cga.StartWork(skillIndex, 0);
    while ( cga.GetPlayerInfo().mp > 0 && ff.getInventoryEmptySlotCount() > 0 ) {
        try {
            let r = await ff.waitWorkingResult(1000);
            counter++;
            counterTotal++;
            counterTimeout = 0; // 超时次数置零
            if (workdelay == 0) {
                if (counter > workdelay0Times) {
                    counter = 0;
                    //workdelay = 5500;
                    //await ff.gui.setSettings({ player: { 'workdelay': workdelay } });
                    cga.LogOut();
                    console.log( '一轮耗时：', new Date().valueOf() - startTime, ' 鹿皮：', cga.getItemCount('鹿皮') - startItemCount );
                }
            }
            else if (workdelay == 5500) {
                if (counter > workdelay5500Times) {
                    counter = 0;
                    //workdelay = 0;
                    //await ff.gui.setSettings({ player: { 'workdelay': workdelay } });
                }
            }
            console.log(counterTotal, counter);
        } catch (e) {
            counterTimeout++; // 超时次数置零
            if ( counterTimeout > 10 ) {
                counterTimeout = 0;
                cga.StartWork(skillIndex, 0);
            }
        }
    }
} );