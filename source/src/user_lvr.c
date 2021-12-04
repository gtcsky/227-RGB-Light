/*
 * user_lvr.c
 *
 *  Created on: 2019年10月26日
 *      Author: Bough_004
 */
#include	"user_lvr.h"
#include	"user_lcd.h"
#include	"lvd.h"

boolean_t bPortVal = 0;
void LvdIrqCallback(void)
{
    Lvd_ClearIrq();
 //   clear();
 //   systemPowerDown();
}

void userLvrFuncInit(void){
	    stc_lvd_config_t stcLvdCfg;

	    DDL_ZERO_STRUCT(stcLvdCfg);//变量清0

//	    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE))//开GPIO时钟
//	    {
//	        return 1;
//	    }
	    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralVcLvd, TRUE))//开LVD时钟
	    {
	        return ;
	    }

	    //配置测试IO口
//	    Lvd_PortInit();

	    stcLvdCfg.bLvdReset = TRUE;//配置中断或者RESET功能
	    stcLvdCfg.enInput = LvdInputVCC;//配置LVD输入口			-
	    stcLvdCfg.enThreshold = LvdTH3p0V;//配置LVD基准电压		+
	    stcLvdCfg.bFilter = TRUE;
	    stcLvdCfg.enFilterTime = LvdFilter30us;
	    stcLvdCfg.enIrqType = LvdIrqRise;
	    stcLvdCfg.pfnIrqCbk = LvdIrqCallback;

	    Lvd_Init(&stcLvdCfg);
	    Lvd_EnableIrq(stcLvdCfg.enIrqType);
	    delay100us(1);
	    Lvd_Enable();//LVD使能

//	    while (1)
//	    {
//	        if (gu32Cnt > 0)
//	        {
//	            gu32Cnt = 0;
//	            Lvd_EnableIrq(stcLvdCfg.enIrqType);//LVD中断使能
//	        }
//	    }
}
