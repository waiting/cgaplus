var cga = require('../cgaapi')(function(){
    let loop = () => {
        if (cga.isInNormalState()) {
            var xy = cga.GetMapXY();
            var dir = cga.getRandomSpaceDir(xy.x, xy.y);

            cga.registerMoveThink((arg)=>{
                if (cga.isInNormalState()) {
                    return true;
                }
                setTimeout(loop, 100);
                return false;
            });

            cga.freqMove(dir,250);
        }
        else {
            setTimeout(loop, 100);
        }
    };
    loop();
});
