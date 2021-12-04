/*
 * user_pca.c
 *
 *  Created on: 2019年9月27日
 *      Author: Bough_004
 */

#include "user_pca.h"
#include "user_menu.h"
#include "pca.h"
#include "gpio.h"
#include "io_define.h"
#include "lpm.h"
#include "bt.h"
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
static volatile uint32_t u32PcaTestFlag = 0;
//static uint8_t  u8CmpHData;

/**********************************************************************************
 *
 * 8 bit 模式PWM
 *
 ***********************************************************************************/
/*int32_t userPcaConfig(void) {

	stc_pca_config_t stcConfig;
	stc_pca_capmodconfig_t stcModConfig;
	stc_gpio_config_t stcPCA0Port;

	DDL_ZERO_STRUCT(stcConfig);
	DDL_ZERO_STRUCT(stcModConfig);
	DDL_ZERO_STRUCT(stcPCA0Port);

	//PCA、GPIO外设时钟开启
	Sysctrl_SetPeripheralGate(SysctrlPeripheralPca, TRUE);
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);

	//INT ENABLE
	//EnableNvic(PCA_IRQn, 3, TRUE);
	//Pca_EnableIrq(Module0);

	//GPIO CFG
	//PA06设置为PCA_CH0
	//Gpio_ClrAnalogMode(GpioPortA, GpioPin6);
	stcPCA0Port.enDrv = GpioDrvH;
	stcPCA0Port.enDir = GpioDirOut;

	Gpio_Init(LED_CW_Port, LED_CW_Pin, &stcPCA0Port);
	Gpio_SetAfMode(LED_CW_Port, LED_CW_Pin, LED_CW_FuncSel);				//PWM_CW	CH0

	Gpio_Init(LED_Blue_Port, LED_Blue_Pin, &stcPCA0Port);
	Gpio_SetAfMode(LED_Blue_Port, LED_Blue_Pin, LED_Blue_FuncSel);			//PWM_Blue	CH1

	Gpio_Init(LED_Green_Port, LED_Green_Pin, &stcPCA0Port);
	Gpio_SetAfMode(LED_Green_Port, LED_Green_Pin, LED_Green_FuncSel);		//PWM Green	  CH2

	Gpio_Init(LED_Red_Port, LED_Red_Pin, &stcPCA0Port);
	Gpio_SetAfMode(LED_Red_Port, LED_Red_Pin, LED_Red_FuncSel);				//PWM Red    CH3

	Gpio_Init(LED_MW_Port, LED_MW_Pin, &stcPCA0Port);
	Gpio_SetAfMode(LED_MW_Port, LED_MW_Pin, LED_MW_FuncSel);				//PWM_MW	CH4

	stcConfig.enCIDL = IdleGoOn;                   			//休眠模式PCA工作
//	stcConfig.enCIDL =IdleStop;
	stcConfig.enWDTE = PCAWDTDisable;              		//wdt功能关闭
	stcConfig.enCPS = PCAPCLKDiv2;               			//PCLK/2

	stcConfig.pfnPcaCb = PcaInt;                   			//中断函数入口

	stcModConfig.enECOM = ECOMEnable;          		//比较器功能允许
	stcModConfig.enCAPP = CAPPDisable;         		//上升沿捕获禁止
	stcModConfig.enCAPN = CAPNDisable;         		//下降沿捕获禁止
	stcModConfig.enMAT = MATDisable;          		//禁止匹配
	stcModConfig.enTOG = TOGDisable;          			//禁止翻转
	stcModConfig.en8bitPWM = PCA8bitPWMEnable;    //允许8位PWM功能

	if (Ok != Pca_Init(&stcConfig)) {
		return Error;
	}
	if (Ok != Pca_CapModConfig(ModuleCw, &stcModConfig)) {
		return Error;
	}
	if (Ok != Pca_CapModConfig(ModuleBlue, &stcModConfig)) {
		return Error;
	}
	if (Ok != Pca_CapModConfig(ModuleGreen, &stcModConfig)) {
		return Error;
	}
	if (Ok != Pca_CapModConfig(ModuleRed, &stcModConfig)) {
		return Error;
	}
	if (Ok != Pca_CapModConfig(ModuleMw, &stcModConfig)) {
		return Error;
	}

	Pca_CmpDataSet(ModuleCw,PWM_INIT_DATA,PWM_INIT_DATA);
	Pca_CmpDataSet(ModuleBlue,PWM_INIT_DATA,PWM_INIT_DATA);
	Pca_CmpDataSet(ModuleGreen,PWM_INIT_DATA,PWM_INIT_DATA);
	Pca_CmpDataSet(ModuleRed,PWM_INIT_DATA,PWM_INIT_DATA);
	Pca_CmpDataSet(ModuleMw,PWM_INIT_DATA,PWM_INIT_DATA);
	Pca_Run();
	return Ok;
}*/

void	user_Pca_CapData16SetAt16M(en_pca_module_t enModule, uint16_t u16Data){
	if(!u16Data)
		Pca_CmpData16Set(enModule, PWM_FRQ_CONST*2/3-1);

	else
		Pca_CmpData16Set(enModule, PWM_FRQ_CONST*2/3-u16Data);
//		Pca_CmpDataSet(enModule, (PWM_FRQ_CONST*2/3-u16Data>>8)&0xFF,(PWM_FRQ_CONST*2/3-u16Data)&0xFF);
}

void updateTo16MPCA(void){
	Bt_M0_Stop(TIM1);                                       	//TIM1 运行。
	Pca_CARRSet(PWM_FRQ_CONST*2/3);                      				//周期重载值设置
	if(pcaDataStruct.valueOfCw>1)
		user_Pca_CapData16SetAt16M(ModuleCw,pcaDataStruct.valueOfCw*2/3);
	else
		user_Pca_CapData16SetAt16M(ModuleCw,pcaDataStruct.valueOfCw);
	if(pcaDataStruct.valueOfMw>1)
		user_Pca_CapData16SetAt16M(ModuleMw,pcaDataStruct.valueOfMw*2/3);
	else
		user_Pca_CapData16SetAt16M(ModuleMw,pcaDataStruct.valueOfMw);
	if(pcaDataStruct.valueOfRed>1)
		user_Pca_CapData16SetAt16M(ModuleRed,pcaDataStruct.valueOfRed*2/3);
	else
		user_Pca_CapData16SetAt16M(ModuleRed,pcaDataStruct.valueOfRed);
	if(pcaDataStruct.valueOfGreen>1)
		user_Pca_CapData16SetAt16M(ModuleGreen,pcaDataStruct.valueOfGreen*2/3);
	else
		user_Pca_CapData16SetAt16M(ModuleGreen,pcaDataStruct.valueOfGreen);
	if(pcaDataStruct.valueOfBlue>1)
		user_Pca_CapData16SetAt16M(ModuleBlue,pcaDataStruct.valueOfBlue*2/3);
	else
		user_Pca_CapData16SetAt16M(ModuleBlue,pcaDataStruct.valueOfBlue);
	Bt_M0_Run(TIM1);                                       	//TIM1 运行。

}
void updateTo24MPCA(void){
		Pca_CARRSet(PWM_FRQ_CONST);                      					//周期重载值设置
		user_Pca_CapData16Set(ModuleCw,pcaDataStruct.valueOfCw);
		user_Pca_CapData16Set(ModuleMw,pcaDataStruct.valueOfMw);
		user_Pca_CapData16Set(ModuleRed,pcaDataStruct.valueOfRed);
		user_Pca_CapData16Set(ModuleGreen,pcaDataStruct.valueOfGreen);
		user_Pca_CapData16Set(ModuleBlue,pcaDataStruct.valueOfBlue);
}

void user_Pca_CapData16Set(en_pca_module_t enModule, uint16_t u16Data){
	switch(enModule){
		case ModuleCw:
			pcaDataStruct.valueOfCw=u16Data;
			break;
		case ModuleMw:
			pcaDataStruct.valueOfMw=u16Data;
			break;
		case ModuleRed:
			pcaDataStruct.valueOfRed=u16Data;
			break;
		case ModuleGreen:
			pcaDataStruct.valueOfGreen=u16Data;
			break;
		case ModuleBlue:
			pcaDataStruct.valueOfBlue=u16Data;
			break;
	}

	//pcaDataStruct

	if(!u16Data)
		Pca_CmpData16Set(enModule, PWM_FRQ_CONST-1);
	else
		Pca_CmpData16Set(enModule, PWM_FRQ_CONST-u16Data);
	 //Pca_CmpData16Set(enModule, u16Data);
}
void timer1Init(void){
	    stc_bt_mode0_config_t     stcBtBaseCfg;
	    DDL_ZERO_STRUCT(stcBtBaseCfg);
	    Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE); //Base Timer外设时钟使能
	    stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //定时器模式
	    stcBtBaseCfg.enCT       = BtTimer;                      		//定时器功能，计数时钟为内部PCLK
	    stcBtBaseCfg.enPRS      = BtPCLKDiv1;                 	 	//PCLK/64
	    stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               	//自动重载16位计数器/定时器
	    stcBtBaseCfg.bEnTog     = FALSE;
	    stcBtBaseCfg.bEnGate    = FALSE;
	    stcBtBaseCfg.enGateP    = BtGatePositive;

	    Bt_Mode0_Init(TIM1, &stcBtBaseCfg);                     //TIM1 的模式0功能初始化
	    Bt_M0_ARRSet(TIM1, 0xffff);                        //设置重载值(周期 = 0x10000 - ARR)
	    Bt_M0_Cnt16Set(TIM1, 0xffff);                      //设置计数初值
	    Bt_ClearIntFlag(TIM1,BtUevIrq);                         //清中断标志

}

/**********************************************************************************
 *
 * 16 bit 模式PWM
 *
 ***********************************************************************************/

int32_t userPcaConfig(void) {

	stc_pca_config_t stcConfig;
	stc_pca_capmodconfig_t stcModConfig;
	stc_gpio_config_t stcPCA0Port;

	DDL_ZERO_STRUCT(stcConfig);
	DDL_ZERO_STRUCT(stcModConfig);
	DDL_ZERO_STRUCT(stcPCA0Port);


	//PCA、GPIO外设时钟开启
	Sysctrl_SetPeripheralGate(SysctrlPeripheralPca, TRUE);
	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
	Sysctrl_SetPeripheralGate(SysctrlPeripheralBTim, TRUE);
	 timer1Init();
	//INT ENABLE
	//EnableNvic(PCA_IRQn, 3, TRUE);
	//Pca_EnableIrq(Module0);

	//GPIO CFG
	//PA06设置为PCA_CH0
	//Gpio_ClrAnalogMode(GpioPortA, GpioPin6);
	stcPCA0Port.enDrv = GpioDrvH;
	stcPCA0Port.enDir = GpioDirOut;

	Gpio_Init(LED_CW_Port, LED_CW_Pin, &stcPCA0Port);
	Gpio_SetAfMode(LED_CW_Port, LED_CW_Pin, LED_CW_FuncSel);				//PWM_CW	CH0

	Gpio_Init(LED_Blue_Port, LED_Blue_Pin, &stcPCA0Port);
	Gpio_SetAfMode(LED_Blue_Port, LED_Blue_Pin, LED_Blue_FuncSel);			//PWM_Blue	CH1

	Gpio_Init(LED_Green_Port, LED_Green_Pin, &stcPCA0Port);
	Gpio_SetAfMode(LED_Green_Port, LED_Green_Pin, LED_Green_FuncSel);		//PWM Green	  CH2

	Gpio_Init(LED_Red_Port, LED_Red_Pin, &stcPCA0Port);
	Gpio_SetAfMode(LED_Red_Port, LED_Red_Pin, LED_Red_FuncSel);				//PWM Red    CH3

	Gpio_Init(LED_MW_Port, LED_MW_Pin, &stcPCA0Port);
	Gpio_SetAfMode(LED_MW_Port, LED_MW_Pin, LED_MW_FuncSel);				//PWM_MW	CH4

	stcConfig.enCIDL = IdleGoOn;                   			//休眠模式PCA工作
//	stcConfig.enCIDL =IdleStop;
	stcConfig.enWDTE = PCAWDTDisable;              		//wdt功能关闭
//	stcConfig.enCPS = PCAPCLKDiv2;               			//PCLK/2
	stcConfig.enCPS = PCATimer1Over;               			//

	stcConfig.pfnPcaCb = PcaInt;                   			//中断函数入口

	    stcModConfig.enECOM     = ECOMEnable;          //比较器功能允许
	    stcModConfig.enCAPP     = CAPPDisable;         //上升沿捕获禁止
	    stcModConfig.enCAPN     = CAPNDisable;         //下降沿捕获禁止
	    stcModConfig.enMAT      = MATEnable;           //允许匹配
	    stcModConfig.enTOG      = TOGEnable;           //允许翻转
	    stcModConfig.en8bitPWM  = PCA8bitPWMDisable;   //禁止8位PWM功能

	Pca_Enable16bitPWM();                          //增强PWM功能使能
	if (Ok != Pca_Init(&stcConfig)) {
		return Error;
	}
	if (Ok != Pca_CapModConfig(ModuleCw, &stcModConfig)) {
		return Error;
	}
	if (Ok != Pca_CapModConfig(ModuleBlue, &stcModConfig)) {
		return Error;
	}
	if (Ok != Pca_CapModConfig(ModuleGreen, &stcModConfig)) {
		return Error;
	}
	if (Ok != Pca_CapModConfig(ModuleRed, &stcModConfig)) {
		return Error;
	}
	if (Ok != Pca_CapModConfig(ModuleMw, &stcModConfig)) {
		return Error;
	}
	  Pca_CARRSet(PWM_FRQ_CONST);                      				//周期重载值设置
	  user_Pca_CapData16Set(ModuleCw, PWM_INIT_DATA);        	//(比较值)比较捕获寄存器设置
	  user_Pca_CapData16Set(ModuleBlue, PWM_INIT_DATA);        	//(比较值)比较捕获寄存器设置
	  user_Pca_CapData16Set(ModuleGreen, PWM_INIT_DATA);        	//(比较值)比较捕获寄存器设置
	  user_Pca_CapData16Set(ModuleRed, PWM_INIT_DATA);        	//(比较值)比较捕获寄存器设置
	  user_Pca_CapData16Set(ModuleMw, PWM_INIT_DATA);        	//(比较值)比较捕获寄存器设置
//	  user_Pca_CapData16Set(ModuleCw, 60);        	//(比较值)比较捕获寄存器设置
//	  user_Pca_CapData16Set(ModuleBlue, 0);        	//(比较值)比较捕获寄存器设置
//	  user_Pca_CapData16Set(ModuleGreen, 0);        	//(比较值)比较捕获寄存器设置
//	  user_Pca_CapData16Set(ModuleRed, 60);        	//(比较值)比较捕获寄存器设置
//	  user_Pca_CapData16Set(ModuleMw, 60);        	//(比较值)比较捕获寄存器设置
	Pca_Run();
	Bt_M0_Run(TIM1);                                       	//TIM1 运行。
//	while(1);
	return Ok;
}

/*******************************************************************************
 * Pca中断服务程序
 ******************************************************************************/
void PcaInt(void)
{
    if (TRUE == Pca_GetCntIntFlag())
    {
        Pca_ClearCntIntFlag();
        u32PcaTestFlag |= 0x20;
    }
    if (TRUE == Pca_GetIntFlag(Module0))
    {
        Pca_ClearIntFlag(Module0);
        u32PcaTestFlag |= 0x01;
    }
    if (TRUE == Pca_GetIntFlag(Module1))
    {
        Pca_ClearIntFlag(Module1);
        u32PcaTestFlag |= 0x02;
    }
    if (TRUE == Pca_GetIntFlag(Module2))
    {
        Pca_ClearIntFlag(Module2);
        u32PcaTestFlag |= 0x04;
    }
    if (TRUE == Pca_GetIntFlag(Module3))
    {
        Pca_ClearIntFlag(Module3);
        u32PcaTestFlag |= 0x08;
    }
    if (TRUE == Pca_GetIntFlag(Module4))
    {
        Pca_ClearIntFlag(Module4);
        u32PcaTestFlag |= 0x10;
    }

}
