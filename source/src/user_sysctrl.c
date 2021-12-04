/*
 * user_sysctrl.c
 *
 *  Created on: 2019年9月27日
 *      Author: Bough_004
 */
#include "sysctrl.h"
#include "gpio.h"
#include "flash.h"
#include "user_sysctrl.h"

void userSysctrl(void){
	//Gpio_SfPClkOutputConfig(GpioSfPclkOutEnable, GpioSfHclkOutDiv1);
	    stc_sysctrl_clk_config_t stcCfg;
	    stc_gpio_config_t stcGpioCfg;
//	    stc_sysctrl_pll_config_t stcPLLCfg;
	    ///< 开启GPIO外设时钟
	    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
	    Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash, TRUE);
	    Sysctrl_SetPeripheralGate(SysctrlPeripheralTrim, TRUE);
	    Sysctrl_SetFunc(SysctrlEXTHEn,FALSE);
	    Sysctrl_SetFunc(SysctrlXTLAlwaysOnEn,FALSE);
	    Sysctrl_ClkSourceEnable(SysctrlClkXTH , FALSE );
	    ///<========================== 时钟输出端口模式配置 ===========================
	    ///< 端口方向配置->输出
	    stcGpioCfg.enDir = GpioDirOut;
	    ///< 端口驱动能力配置->高驱动能力
	    stcGpioCfg.enDrv = GpioDrvH;
	    ///< 端口上下拉配置->无上下拉
	    stcGpioCfg.enPuPd = GpioNoPuPd;
	    ///< 端口开漏输出配置->开漏输出关闭
	    stcGpioCfg.enOD = GpioOdDisable;
	    ///< GPIO IO PA01初始化
	    Gpio_Init(GpioPortA, GpioPin10, &stcGpioCfg);
	    ///< 配置PA01复用功能为HCLK输出
	    Gpio_SetAfMode(GpioPortA, GpioPin10, GpioAf6);

	    Gpio_SfPClkOutputConfig(GpioSfPclkOutEnable, GpioSfPclkOutDiv1);

	    ///<========================== 时钟初始化配置 ===================================
	    ///< 因要使用的时钟源HCLK小于24M：此处设置FLASH 读等待周期为0 cycle(默认值也为0 cycle)
	    Flash_WaitCycle(FlashWaitCycle2);

	    ///< 时钟初始化前，优先设置要使用的时钟源：此处设置RCH为4MHz
	    Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz);

	    ///< 选择内部RCH作为HCLK时钟源;
	    stcCfg.enClkSrc    = SysctrlClkRCH;
	    ///< HCLK SYSCLK/1
	    stcCfg.enHClkDiv   = SysctrlHclkDiv1;
	    ///< PCLK 为HCLK/1
	    stcCfg.enPClkDiv   = SysctrlPclkDiv1;
	    ///< 系统时钟初始化
	    Sysctrl_ClkInit(&stcCfg);

	///<============== 将时钟从RCH4MHz切换至RCH24MHz ==============================
	    ///< RCH内部时钟的切换需要从低到高依次切换到目标时钟（默认4MHz -> 24MHz）
	    ///< RCH时钟的频率切换，需要先将时钟切换到RCL
	    Sysctrl_SetRCLTrim(SysctrlRclFreq38400);
	    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
	    Sysctrl_SysClkSwitch(SysctrlClkRCL);
	    ///< 加载目标频率的RCH的TRIM值
	    delay1ms(2);
	    Sysctrl_SetRCHTrim(SysctrlRchFreq8MHz);
	    delay1ms(5);
	    Sysctrl_SetRCHTrim(SysctrlRchFreq16MHz);
	    delay1ms(5);
	    Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);
//	    Sysctrl_SetRCHTrim(SysctrlRchFreq16MHz);
	    ///< 使能RCH（默认打开，此处可不需要再次打开）
	    Sysctrl_ClkSourceEnable(SysctrlClkRCH, TRUE);
	    ///< 时钟切换到RCH
	    Sysctrl_SysClkSwitch(SysctrlClkRCH);
	    ///< 关闭RCL时钟
//	    Sysctrl_ClkSourceEnable(SysctrlClkRCL, FALSE);
	    delay1ms(10);

}


