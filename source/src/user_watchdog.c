/*
 * user_watchdog.c
 *
 *  Created on: 2019年10月30日
 *      Author: Bough_004
 */
#include "user_watchdog.h"
#include "wdt.h"
#include "lpm.h"

uint8_t u32CountWdt;
uint8_t Data=0xff;
uint8_t cnt=0;
void watchDogConfig(void){
	    stc_wdt_config_t  stcWdt_Config;
	    DDL_ZERO_STRUCT(stcWdt_Config);

//	    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
	    Sysctrl_SetPeripheralGate(SysctrlPeripheralWdt,TRUE);//


	    stcWdt_Config.u8LoadValue = 0x0c;//6.5s
	    stcWdt_Config.enResetEnable = WRESET_EN;//WINT_EN;//WRESET_EN;////

	    Wdt_Init(&stcWdt_Config);
	    Wdt_Start();

}


