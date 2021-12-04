/*
 * user_exti.c
 *
 *  Created on: 2019年9月28日
 *      Author: Bough_004
 */
/* Includes ------------------------*/
#include "user_exti.h"
#include "user_key.h"
#include "lpt.h"
#include "user_menu.h"
#include "io_define.h"
#include "user_key.h"
//#include "misc.h"

//extern	u8	fIsBattLow;
//extern	u16	vSystemLIRC5ms;
//extern	u8	fIsNeedExitBattLowLoop;



void enableDcDectISR(void){
}

void disableDcDectISR(void){
}

void Gpio_IRQHandler(uint8_t u8Param)
 {
	///< PORT D
	entryNormalModeChk();
	switch (u8Param) {
	case PORTA_IRQn:
		if (TRUE == Gpio_GetIrqStatus(KeyInc_Port, KeyInc_Pin)) {
			Gpio_ClearIrq(KeyInc_Port, KeyInc_Pin);
			setIncKeyPressedFlag();
		}
		if (TRUE == Gpio_GetIrqStatus(KeyDec_Port, KeyDec_Pin)) {
			Gpio_ClearIrq(KeyDec_Port, KeyDec_Pin);
			setDecKeyPressedFlag();
		}
		if (TRUE == Gpio_GetIrqStatus(KeyBrightnessInc_Port, KeyBrightnessInc_Pin)) {
			Gpio_ClearIrq(KeyBrightnessInc_Port, KeyBrightnessInc_Pin);
			setBrightnessUpKeyPressedFlag();
			//testLEDFunc();
		}
		if (TRUE == Gpio_GetIrqStatus(KeyBrightnessDec_Port, KeyBrightnessDec_Pin)) {
			Gpio_ClearIrq(KeyBrightnessDec_Port, KeyBrightnessDec_Pin);
			setBrightnessDownKeyPressedFlag();
			//testLEDFunc();
		}
		break;
	case PORTB_IRQn:
		if (TRUE == Gpio_GetIrqStatus(KeyPower_Port, KeyPower_Pin)) {
			Gpio_ClearIrq(KeyPower_Port, KeyPower_Pin);
			setPowerKeyPressedFlag();
			//testLEDFunc();
		}
		break;
	case PORTC_IRQn:
		if (TRUE == Gpio_GetIrqStatus(IIC_Int_Port, IIC_Int_Pin)) {
			Gpio_ClearIrq(IIC_Int_Port, IIC_Int_Pin);
//			if(fIsSystemOff&&fIsWakeupByCharge==DoNothing){
//				//fIsNeedWakeupLCD=TRUE;
//				fIsWakeupByCharge=SetWakeupLCD;
//			}
		}
		break;
	case PORTD_IRQn:
		if (TRUE == Gpio_GetIrqStatus(KeyFunc_Port, KeyFunc_Pin)) {
			Gpio_ClearIrq(KeyFunc_Port, KeyFunc_Pin);
			setFuncKeyPressedFlag();
		}
		if (TRUE == Gpio_GetIrqStatus(GpioPortD, GpioPin4)) {
			Gpio_ClearIrq(GpioPortD, GpioPin4);
		}
		break;
	}
}

void EXTI_Key_Config(void)
{
	    stc_gpio_config_t pstcGpioCfg;
	    ///< 打开GPIO外设时钟门控
//	    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);

	    ///< 端口方向配置->输入
	    pstcGpioCfg.enDir = GpioDirIn;
	    ///< 端口驱动能力配置->低驱动能力
	    pstcGpioCfg.enDrv = GpioDrvL;
	    ///< 端口上下拉配置->上拉
	    pstcGpioCfg.enPuPd = GpioPu;
	    ///< 端口开漏输出配置->开漏输出关闭
	    pstcGpioCfg.enOD = GpioOdDisable;
	    ///< 端口输入/输出值寄存器总线控制模式配置->AHB
	    pstcGpioCfg.enCtrlMode = GpioAHB;

	    ///< GPIO IO PD04初始化(PD04在STK上外接KEY(USER))
	    Gpio_Init(GpioPortD, GpioPin4, &pstcGpioCfg);
	    Gpio_Init(KeyFunc_Port, KeyFunc_Pin, &pstcGpioCfg);
	    Gpio_Init(KeyPower_Port, KeyPower_Pin, &pstcGpioCfg);
	    Gpio_Init(KeyInc_Port, KeyInc_Pin, &pstcGpioCfg);
	    Gpio_Init(KeyDec_Port, KeyDec_Pin, &pstcGpioCfg);
	    Gpio_Init(KeyBrightnessInc_Port, KeyBrightnessInc_Pin, &pstcGpioCfg);
	    Gpio_Init(KeyBrightnessDec_Port, KeyBrightnessDec_Pin, &pstcGpioCfg);

	    ///< 打开并配置PD04为下降沿中断
	    Gpio_EnableIrq(GpioPortD, GpioPin4, GpioIrqFalling);
	    Gpio_EnableIrq(KeyFunc_Port, KeyFunc_Pin, GpioIrqFalling);
	    Gpio_EnableIrq(KeyPower_Port, KeyPower_Pin, GpioIrqFalling);
	    Gpio_EnableIrq(KeyInc_Port, KeyInc_Pin, GpioIrqFalling);
	    Gpio_EnableIrq(KeyDec_Port, KeyDec_Pin, GpioIrqFalling);
	    Gpio_EnableIrq(KeyBrightnessInc_Port, KeyBrightnessInc_Pin, GpioIrqFalling);
	    Gpio_EnableIrq(KeyBrightnessDec_Port, KeyBrightnessDec_Pin, GpioIrqFalling);

	    //***********************************************************
	    ///< 端口方向配置->输入
	    pstcGpioCfg.enDir = GpioDirIn;
	    ///< 端口驱动能力配置->低驱动能力
	    pstcGpioCfg.enDrv = GpioDrvL;
	    ///< 端口上下拉配置->下拉
	    pstcGpioCfg.enPuPd = GpioPd;
	    ///< 端口开漏输出配置->开漏输出关闭
	    pstcGpioCfg.enOD = GpioOdDisable;
	    ///< 端口输入/输出值寄存器总线控制模式配置->AHB
	    pstcGpioCfg.enCtrlMode = GpioAHB;
	    Gpio_Init(IIC_Int_Port, IIC_Int_Pin, &pstcGpioCfg);
	    Gpio_EnableIrq(IIC_Int_Port, IIC_Int_Pin, GpioIrqRising);				//IIC int 上升沿触发
	    //***********************************************************
	    ///< 使能端口PORTD系统中断
	    EnableNvic(PORTD_IRQn, IrqLevel3, TRUE);
	    EnableNvic(PORTC_IRQn, IrqLevel3, TRUE);
	    EnableNvic(PORTA_IRQn, IrqLevel3, TRUE);
	    EnableNvic(KeyPower_IRQn, IrqLevel3, TRUE);

//	    ///< 端口方向配置->输出(其它参数与以上（输入）配置参数一致)
//	    pstcGpioCfg.enDir = GpioDirOut;
//	    ///< 端口上下拉配置->下拉
//	    pstcGpioCfg.enPuPd = GpioPd;
//	    ///< GPIO IO PD05初始化(PD05在STK上外接LED(LED3))
//	    Gpio_Init(GpioPortD, GpioPin5, &pstcGpioCfg);
//	    ///< 端口PD05设置为低电平（LED关闭）
//	    Gpio_ClrIO(GpioPortD, GpioPin5);
}

