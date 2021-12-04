/*
 * io_define.h
 *
 *  Created on: 2019年9月27日
 *      Author: Bough_004
 */

#ifndef SOURCE_INC_IO_DEFINE_H_
#define SOURCE_INC_IO_DEFINE_H_
#include "const.h"
#include "gpio.h"
#include "adc.h"
#include "user_key.h"
#include "user_menu.h"
#define   ModuleRed    		Module0        ///< 模块0
#define	LED_Red_Port		GpioPortA
#define	LED_Red_Pin			GpioPin6
#define	LED_Red_FuncSel		GpioAf2

#define   ModuleBlue    		Module1       ///< 模块1
#define	LED_Blue_Port		GpioPortA
#define	LED_Blue_Pin			GpioPin7
#define	LED_Blue_FuncSel		GpioAf2

//#define   ModuleGreen    		Module2        ///< 模块2
//#define	LED_Green_Port		GpioPortB
//#define	LED_Green_Pin		GpioPin0
//#define	LED_Green_FuncSel	GpioAf1

//#define   ModuleRed    		Module3       ///< 模块3
//#define	LED_Red_Port		GpioPortB
//#define	LED_Red_Pin			GpioPin1
//#define	LED_Red_FuncSel		GpioAf1

#define   ModuleMw    		Module2        ///< 模块4
#define	LED_MW_Port		GpioPortB
#define	LED_MW_Pin			GpioPin0
#define	LED_MW_FuncSel		GpioAf1

#define   ModuleCw    			Module3          ///< 模块3
#define	LED_CW_Port			GpioPortB
#define	LED_CW_Pin			GpioPin1
#define	LED_CW_FuncSel		GpioAf1

#define   ModuleGreen    		Module4        ///< 模块4
#define	LED_Green_Port		GpioPortA
#define	LED_Green_Pin		GpioPin4
#define	LED_Green_FuncSel	GpioAf3


#define	KeyFunc_Port			GpioPortD
#define	KeyFunc_Pin			GpioPin0
#define	KeyFunc_IRQn		PORTD_IRQn
#define 	FUNC_KEY_PRESSED   		0x01
#define	checkFuncKeyPressed		(vIsKeyPressed&FUNC_KEY_PRESSED)
#define	clrFuncKeyPressed		vIsKeyPressed&=~FUNC_KEY_PRESSED//0xFE
#define	setFuncKeyPressed		vIsKeyPressed|=FUNC_KEY_PRESSED
#define	GET_FUNC_KEY_PIN_STTS	Gpio_GetInputIO(KeyFunc_Port, KeyFunc_Pin)

#define	KeyInc_Port				GpioPortA
#define	KeyInc_Pin				GpioPin10
#define	KeyInc_IRQn				PORTA_IRQn
#define 	Inc_KEY_PRESSED   		0x04
#define	checkIncKeyPressed		(vIsKeyPressed&Inc_KEY_PRESSED)
#define	clrIncKeyPressed			vIsKeyPressed&=~Inc_KEY_PRESSED//0xFB
#define	setIncKeyPressed			vIsKeyPressed|=Inc_KEY_PRESSED
#define	GET_INC_KEY_PIN_STTS	Gpio_GetInputIO(KeyInc_Port, KeyInc_Pin)


#define	KeyDec_Port				GpioPortA
#define	KeyDec_Pin				GpioPin9
#define	KeyDec_IRQn				PORTA_IRQn
#define 	Dec_KEY_PRESSED   		0x08
#define	checkDecKeyPressed		(vIsKeyPressed&Dec_KEY_PRESSED)
#define	clrDecKeyPressed			vIsKeyPressed&=~Dec_KEY_PRESSED//0xF7
#define	setDecKeyPressed			vIsKeyPressed|=Dec_KEY_PRESSED
#define	GET_DEC_KEY_PIN_STTS	Gpio_GetInputIO(KeyDec_Port, KeyDec_Pin)

#define	KeyBrightnessInc_Port					GpioPortA
#define	KeyBrightnessInc_Pin					GpioPin12
#define	KeyBrightnessInc_IRQn				PORTA_IRQn
#define 	BrightnessUp_KEY_PRESSED   			0x10
#define	checkBrightnessUpKeyPressed			(vIsKeyPressed&BrightnessUp_KEY_PRESSED)
#define	clrBrightnessUpKeyPressed			vIsKeyPressed&=~BrightnessUp_KEY_PRESSED//0xEF
#define	setBrightnessUpKeyPressed			vIsKeyPressed|=BrightnessUp_KEY_PRESSED
#define	GET_BRIGHTNESS_UP_KEY_PIN_STTS	Gpio_GetInputIO(KeyBrightnessInc_Port, KeyBrightnessInc_Pin)

#define	KeyBrightnessDec_Port		GpioPortA
#define	KeyBrightnessDec_Pin			GpioPin11
#define	KeyBrightnessDec_IRQn		PORTA_IRQn
#define 	BrightnessDown_KEY_PRESSED   			0x20
#define	checkBrightnessDownKeyPressed			(vIsKeyPressed&BrightnessDown_KEY_PRESSED)
#define	clrBrightnessDownKeyPressed				vIsKeyPressed&=~BrightnessDown_KEY_PRESSED//0xDF
#define	setBrightnessDownKeyPressed				vIsKeyPressed|=BrightnessDown_KEY_PRESSED
#define	GET_BRIGHTNESS_DOWN_KEY_PIN_STTS		Gpio_GetInputIO(KeyBrightnessDec_Port, KeyBrightnessDec_Pin)

#define	KeyPower_Port				GpioPortB
#define	KeyPower_Pin				GpioPin11
#define	KeyPower_IRQn				PORTB_IRQn
#define 	Power_KEY_PRESSED   			0x02
#define	checkPowerKeyPressed		(vIsKeyPressed&Power_KEY_PRESSED)
#define	clrPowerKeyPressed			vIsKeyPressed&=~Power_KEY_PRESSED//0xFD
#define	setPowerKeyPressed			vIsKeyPressed|=Power_KEY_PRESSED
#define	GET_POWER_KEY_PIN_STTS	Gpio_GetInputIO(KeyPower_Port, KeyPower_Pin)

//#define 	LED_PORT     	GpioPortD
//#define 	LED_PIN      	GpioPin5
//#define	set_LED		Gpio_WriteOutputIO(LED_PORT, LED_PIN, TRUE)
//#define	clr_LED		Gpio_WriteOutputIO(LED_PORT, LED_PIN, FALSE)
//#define	get_LED_Stts 	Gpio_ReadOutputIO(LED_PORT, LED_PIN)


//#define	BATT_DET_PORT			GpioPortA
//#define	BATT_DET_PIN			GpioPin2
//#define	Batt_Det_Chl				AdcExInputCH2
#define	BATT_DET_PORT			GpioPortA
#define	BATT_DET_PIN			GpioPin2
#define	Batt_Det_Chl				AdcExInputCH5

#define	SPI0_SCK_PORT			GpioPortB
#define	SPI0_SCK_PIN			GpioPin3
#define	SPI0_SCK_FuncSel			GpioAf1

#define	SPI0_CS_PORT			GpioPortA
#define	SPI0_CS_PIN				GpioPin15
#define	SPI0_CS_FuncSel			GpioAf1

#define	SPI0_MISO_PORT			GpioPortB
#define	SPI0_MISO_PIN			GpioPin4
#define	SPI0_MISO_FuncSel		GpioAf1

#define	SPI0_MOSI_PORT			GpioPortB
#define	SPI0_MOSI_PIN			GpioPin5
#define	SPI0_MOSI_FuncSel		GpioAf1

#define	Lcd_Res_Port				GpioPortC
#define	Lcd_Res_Pin				GpioPin14										//LCD  reset pin
#define	Lcd_Res_FuncSel			GpioAf0
#define	set_LcdResPin			Gpio_WriteOutputIO(Lcd_Res_Port, Lcd_Res_Pin, 1)
#define	clr_LcdResPin				Gpio_WriteOutputIO(Lcd_Res_Port, Lcd_Res_Pin, 0)

#define	Lcd_Dc_Port				GpioPortB
#define	Lcd_Dc_Pin				GpioPin4											//LCD  DC  (data/command) pin
#define	Lcd_Dc_FuncSel			GpioAf0
#define	set_LcdDcPin				Gpio_WriteOutputIO(Lcd_Dc_Port, Lcd_Dc_Pin, 1)
#define	clr_LcdDcPin				Gpio_WriteOutputIO(Lcd_Dc_Port, Lcd_Dc_Pin, 0)

//#define	Lcd_Power_Port			GpioPortC
//#define	Lcd_Power_Pin			GpioPin15											//LCD  Power control pin
//#define	Lcd_Power_FuncSel		GpioAf0
//#define	LcdPowerOn				Gpio_WriteOutputIO(Lcd_Power_Port, Lcd_Power_Pin, 1)
//#define	LcdPowerOff				Gpio_WriteOutputIO(Lcd_Power_Port, Lcd_Power_Pin, 0)


#define	LED_Ctrl_Port				GpioPortD
#define	LED_Ctrl_Pin				GpioPin1
#define	LED_Ctrl_FuncSel			GpioAf0
#define	LED_Ctrl_On				Gpio_WriteOutputIO(LED_Ctrl_Port, LED_Ctrl_Pin, 1)
#define	LED_Ctrl_Off				Gpio_WriteOutputIO(LED_Ctrl_Port, LED_Ctrl_Pin, 0)
#define	CLR_POWER_ON_LED		LED_Ctrl_Off;fTimerToPowerOnLED=0;vPowerOnLEDCnt=0

#define	LED_Power_Port			GpioPortA
#define	LED_Power_Pin			GpioPin8
#define	LED_Power_FuncSel		GpioAf0
//#define	LEDPowerOn				Gpio_WriteOutputIO(LED_Power_Port, LED_Power_Pin, 1);LED_Ctrl_On
#define	LEDPowerOn				Gpio_WriteOutputIO(LED_Power_Port, LED_Power_Pin, 1);fTimerToPowerOnLED=1;vPowerOnLEDCnt=ON_LED_DELAY_CONST
#define	LEDPowerOff				LED_Ctrl_Off;fTimerToPowerOnLED=0;vPowerOnLEDCnt=0;Gpio_WriteOutputIO(LED_Power_Port, LED_Power_Pin, 0)
#define	readLEDPowerStts 		Gpio_ReadOutputIO(LED_Power_Port, LED_Power_Pin)
#define	LEDPowerTestOn			Gpio_WriteOutputIO(LED_Power_Port, LED_Power_Pin, 1)
#define	LEDPowerTestOff			Gpio_WriteOutputIO(LED_Power_Port, LED_Power_Pin, 0)

//#define	IIC_Int_Port				GpioPortD
//#define	IIC_Int_Pin				GpioPin0
//#define	IIC_Int_FuncSel			GpioAf0
//#define	Get_IIC_Int_Stts			Gpio_GetInputIO(IIC_Int_Port, IIC_Int_Pin)
#define	IIC_Int_Port				GpioPortC
#define	IIC_Int_Pin				GpioPin15
#define	IIC_Int_FuncSel			GpioAf0
#define	Get_IIC_Int_Stts			Gpio_GetInputIO(IIC_Int_Port, IIC_Int_Pin)

#define	IIC_SCL_Port				GpioPortB
#define	IIC_SCL_Pin				GpioPin6
#define	IIC_SCL_FuncSel			GpioAf1

#define	IIC_SDA_Port				GpioPortB
#define	IIC_SDA_Pin				GpioPin7
#define	IIC_SDA_FuncSel			GpioAf1

#endif /* SOURCE_INC_IO_DEFINE_H_ */
