var cga = require('../cgaapi')(function(){
	
    cga.assessAllItems(()=>{console.log('鉴定成功');});

});
