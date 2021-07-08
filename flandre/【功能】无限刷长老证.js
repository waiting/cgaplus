
require('./flandre').then( async () => {

    /*ff.asyncWaitAnySay((r)=>{
        console.log(r);
        return true;
    });*/
    //if(msg = await ff.waitChat(/组(\d+)人/))
    //    console.log(msg);

/*    console.log(ff.getTeamAvgLevel());

    let msg;
    while (!(msg = await ff.waitAnyChatOnce())) {
        console.log('超时');
    }
    console.log(msg);*/

    while ( cga.getItemCount('长老之证') < 7 ) {
        await ff.delay(1000);
        cga.ClickNPCDialog(1, 0);
        await ff.delay(1000);
        await ff.waitBattleEnd(0);
    }

} );
