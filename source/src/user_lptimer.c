/*
 * user_lptimer.c
 *
 *  Created on: 2019年9月27日
 *      Author: Bough_004
 */
#include "ddl.h"
#include "lpm.h"
#include "gpio.h"
#include "io_define.h"
#include "user_lptimer.h"

u16 vSystemLIRC5ms=0;
u16 vSystem5ms=0;
u16 	vSecondCnt=0;
u8	vTimerCnt=0;
u8	fTime45ms=0;
u8	fTime50ms=0;
u8	fTime100ms=0;
u8	fTime500ms=0;
u8	fTime1s=0;



/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/* *********************************************************************************
 *		*进入深度睡眠函数*
 *
 *clkDIV : 唤醒时钟分频  取值:0x00~0x0f   分频比2^(clkDIV+1)
 *		clkDIV=0x0e时: 分频比=2^(14+1)=32768
 *
 *awakeValue: 唤醒计数器
 *		awakeValue=0x100-35时
 *		唤醒周期: 35*(32768/38400)=29.867s
 **********************************************************************************/
//void Enter_DeepSleep(u8 clkDIV,u8 awakeValue)
void Enter_DeepSleep(void)
 {
	/*
	 *配置AWK功能
	 */
	stc_lpm_config_t stcLpmCfg;
	DDL_ZERO_STRUCT(stcLpmCfg);
//	vSkipBattDectCnt = 0;
//	AWK_Config(clkDIV,awakeValue);
//	NVIC_EnableIRQ(AWK_IRQn); 							/*打开ARM中断使能*/
	Lpt_Stop();						//关闭LPT, 防止唤醒
	//Wdt_Start();
//	stcLpmCfg.enSEVONPEND = SevPndDisable;
//	stcLpmCfg.enSLEEPDEEP = SlpDpEnable;   //使能:执行WFI进入深度休眠;
//	stcLpmCfg.enSLEEPONEXIT = SlpExtDisable; //不使能使能:退出异常处理并返回时,处理器自动进入休眠模式的特性禁止.
	stcLpmCfg.enSEVONPEND = SevPndDisable;
	stcLpmCfg.enSLEEPDEEP = SlpDpEnable;   //使能:执行WFI进入深度休眠;
	stcLpmCfg.enSLEEPONEXIT = SlpExtDisable; //不使能使能:退出异常处理并返回时,处理器自动进入休眠模式的特性禁止.
	Lpm_Config(&stcLpmCfg);
	__WFI();
}

/** @defgroup Enter_Sleep
  * @{
  */
void Enter_Sleep(void)
{
//	Lpt_Stop();									//关闭LPT, 防止唤醒
//	SCB->SCR &= (~0x04);
	__WFI();
}




void LPTIMER_Config(void){
    stc_lpt_config_t          stcConfig;
    stc_sysctrl_clk_config_t  stcClkCfg;
    volatile uint8_t          u8TestFlag = 0;
//	stc_gpio_config_t 	stcLEDPort;
//	DDL_ZERO_STRUCT(stcLEDPort);

    DDL_ZERO_STRUCT(stcConfig);
    DDL_ZERO_STRUCT(stcClkCfg);

    //CLK INIT
    stcClkCfg.enClkSrc  = SysctrlClkRCH;
    stcClkCfg.enHClkDiv = SysctrlHclkDiv1;
    stcClkCfg.enPClkDiv = SysctrlPclkDiv1;
    Sysctrl_ClkInit(&stcClkCfg);

    //使能RCL
//    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);

    //使能Lpt,GPIO外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpTim, TRUE);
//    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);


    //PB03设置为LP Timer GATE 引脚
    //Gpio_ClrAnalogMode(LptETR_PORT, LptETR_PIN);
//    stcGatePort.enDrv  = GpioDrvH;
//    stcGatePort.enDir  = GpioDirIn;
//    Gpio_Init(LptGate_PORT, LptGate_PIN, &stcGatePort);
//    Gpio_SetAfMode(LptGate_PORT,LptGate_PIN,GpioAf5);

    //PD05设置为GPIO-->LED控制脚
    //Gpio_ClrAnalogMode(LED_PORT, LED_PIN);
//    stcLEDPort.enDrv  = GpioDrvH;
//    stcLEDPort.enDir  = GpioDirOut;
//    Gpio_Init(LED_PORT, LED_PIN, &stcLEDPort);

    //PD04设置为GPIO<--SW2控制脚
    //Gpio_ClrAnalogMode(Button_PORT, Button_PIN);
//    stcButtonPort.enDrv  = GpioDrvH;
//    stcButtonPort.enDir  = GpioDirIn;
//    Gpio_Init(Button_PORT, Button_PIN, &stcButtonPort);

 //   Gpio_WriteOutputIO(LED_PORT, LED_PIN, TRUE);     //输出高，点亮LED

    stcConfig.pfnLpTimCb = LptInt;           //中断函数入口

    stcConfig.enGate   = LptGateDisable;      //Gate Enable
    stcConfig.enGateP  = LptActLowLevel;     //Gate_P 低电平有效
    stcConfig.enTckSel = LptIRC32K;          	//LPT时钟选择RCL
    stcConfig.enTog    = LptTogDisable;      	//TOG功能Disable
    stcConfig.enCT     = LptTimer;           	//定时器功能
    stcConfig.enMD     = Lpt16bitArrMode;   //无重载16位计数器/定时器

    Lpt_Init(&stcConfig);                    //LPT初始化
    Lpt_ARRSet(0xffff-192);
    Lpt_EnableIrq();                         //中断使能
    Lpt_ClearIntFlag();
    EnableNvic(LPTIM_IRQn, IrqLevel3, TRUE); //开中断


//    //Lpm Cfg		深度睡眠功能
//    stcLpmCfg.enSEVONPEND   = SevPndDisable;
//    stcLpmCfg.enSLEEPDEEP   = SlpDpEnable;
//    stcLpmCfg.enSLEEPONEXIT = SlpExtEnable;
//    Lpm_Config(&stcLpmCfg);

    //按下user按键(PD04)，则进入低功耗模式，否则在等待
    //注：若芯片处于低功耗模式，则芯片无法使用SWD进行调式和下载功能。
    //如需要再次下载程序，需要将芯片复位，且不按user按键
//    while (1 == Gpio_GetInputIO(Button_PORT,Button_PIN));

    Lpt_Run();    //LPT 运行

//    Gpio_WriteOutputIO(LED_PORT, LED_PIN, FALSE);     //输出低，熄灭LED

//    Lpm_GotoLpmMode();

}


/*******************************************************************************
 * Lpt中断服务函数
 ******************************************************************************/
void LptInt(void)
{

    if (TRUE == Lpt_GetIntFlag())
    {
        Lpt_ClearIntFlag();								//设定为5ms中断一次

//        if(i%2 == 0)
//        {
//            Gpio_WriteOutputIO(LED_PORT, LED_PIN, TRUE);     //输出高，点亮LED
//        }
//        else
//        {
//            Gpio_WriteOutputIO(LED_PORT, LED_PIN, FALSE);     //输出低，熄灭LED
//        }
//        i++;
	vSystemLIRC5ms++;
	vSystem5ms++;
	vTimerCnt++;
	if(vTimerCnt%8==0){
		fTime45ms=1;
	}
	if(vTimerCnt%10==0){
		fTime50ms=1;
	}
	if(vTimerCnt%20==0){
		fTime100ms=1;
	}
	if(vTimerCnt%100==0){
		fTime500ms=1;
	}
	if(vTimerCnt%200==0){
		vSecondCnt++;
		fTime1s=1;
		vTimerCnt=0;
	}
	if(vSystem5ms>=USER_DOG_CONST){
		NVIC_SystemReset();
	}
    }


}
