require('./flandre').then( async () => {
    console.log( cga.ForceMove( cga.GetPlayerInfo().direction, true) );
} );