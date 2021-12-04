/*
 * user_lcd.h
 *
 *  Created on: 2019年10月11日
 *      Author: Bough_004
 */

#ifndef SOURCE_INC_USER_LCD_H_
#define SOURCE_INC_USER_LCD_H_
#include	"const.h"
#include	"io_define.h"

#define	LCD_POWER_ON_CONST	20		//20*5=100ms

#define 	SIZE 			16
#define 	XLevelL			0x02
#define 	XLevelH			0x10
#define 	Max_Column		128
#define 	Max_Row		64
#define	Brightness		0xFF
#define 	X_WIDTH 		128
#define 	Y_WIDTH 		64
#define 	OLED_CMD  		0			//写命令
#define 	OLED_DATA 		1			//写数据
#define 	OLED_MODE 		0

#define	ICON_Degree_ADDRESS		59+32
#define	ICON_Arrow_ADDRESS		60+32
#define	ICON_Colon_ADDRESS			61+32

#define	ICON_HUES_X				8
#define	ICON_HUES_Y				0
#define	ICON_HUES_ADDRESS			0
#define	Value_Hues_X				ICON_HUES_X+16


#define	ICON_Saturation_X				ICON_HUES_X
#define	ICON_Saturation_Y				2
#define	ICON_Saturation_ADDRESS		1
#define	Value_Saturation_X				ICON_HUES_X+16

#define	ICON_BRIGHTNESS_X				ICON_HUES_X
#define	ICON_BRIGHTNESS_Y				4
#define	ICON_BRIGHTNESS_ADDRESS		2
#define	Value_Brightness_X				ICON_HUES_X+16
#define	Value_Percent_Addr				25

#define	ICON_ColorTemp_X				ICON_HUES_X
#define	ICON_ColorTemp_Y				6
#define	ICON_ColorTemp_ADDRESS		3
#define	Value_ColorTemp_X				ICON_HUES_X+16+2

#define	ICON_batt_X0			94
#define	ICON_batt_X0_systemOff	50
#define	ICON_batt_X1			ICON_batt_X0+16
#define	ICON_batt_X1_systemOff	ICON_batt_X0_systemOff+16
#define	ICON_batt_Y				0
#define	ICON_batt_Y_systemOff	4
#define	ICON_batt_lv0_Addr		11
#define	ICON_batt_lv1_Addr		13
#define	ICON_batt_lv2_Addr		15
#define	ICON_batt_lv3_Addr		17
#define	ICON_batt_lv4_Addr		19
#define	ICON_batt_lv5_Addr		21
#define	ICON_batt_lv6_Addr		23

#define	ICON_Hot_X				ICON_batt_X0
#define	ICON_Hot_X1				ICON_batt_X1
#define	ICON_Hot_Y				ICON_batt_Y
#define	ICON_Hot_Addr			28

#define	ICON_Percent_X			ICON_HUES_X
#define	ICON_Percent_Y			0


#define	ICON_charge_X0			ICON_batt_X0-32
#define	ICON_charge_X1			ICON_charge_X0+16
#define	ICON_charge_Y			ICON_batt_Y
#define	ICON_charge_TableAddr	26

#define	ICON_Flash_X				ICON_batt_X0
#define	ICON_Flash_Y				2
#define	ICON_Flash_ADDRESS		4

#define	ICON_Loop_X				ICON_Flash_X
#define	ICON_Loop_Y				4
#define	ICON_Loop_ADDRESS		5

#define	ICON_Lock_X				ICON_Flash_X
#define	ICON_Lock_Y				6
#define	ICON_Lock_ADDRESS		6

#define	ICON_Style1_X			ICON_Flash_X+16
#define	ICON_Style1_Y			2
#define	ICON_Style2_X			ICON_Style1_X
#define	ICON_Style3_X			ICON_Style1_X
#define	ICON_Style2_Y			4
#define	ICON_Style3_Y				6
#define	ICON_StyleA_ADDRESS		7
#define	ICON_StyleB_ADDRESS		8
#define	ICON_StyleC_ADDRESS			9

#define	ICON_T_X				ICON_HUES_X+16*3
#define	ICON_T_Y				2
#define	ICON_T_Addr				10

#define	Value_CD_X				ICON_HUES_X+16*2+8
#define	Value_CD_Y				4

#define	Counting_X				ICON_HUES_X
#define	Counting_Y				4

#define	TimerBarX				Counting_X
#define	TimerBarY				6
#define	TimerBarStartAddr		62+32
#define	TimerBarEndX				Counting_X+4+100
#define	TimerBarEndAddr		63+32


extern	void 	powerOnLcdInit(void);
extern	void  	lcdExitSleep(void);
extern	void  	lcdEnterSleep(void);
extern	void 	batterDisplay(u8 level) ;
extern	void		updateHotDisplay(void);
extern	void	 	batteryPercentDisplay(u8 level);
extern	void 	updateCountDownDisplay(displayParamsStruct * disParams);
extern	void 	updateColorTempDisplay(displayParamsStruct * disParams) ;
extern	void 	updateBrightnessDisplay(displayParamsStruct * disParams);
extern	void 	updateSaturationDisplay(displayParamsStruct * disParams);
extern	void 	updateHuesDisplay(displayParamsStruct * disParams);
extern	void 	displaySystemMenu(displayParamsStruct * disParams);
extern	void 	updateArrowDisplay(displayParamsStruct * disParams);
extern	void 	updateLightEffectDisplay(displayParamsStruct * disParams);
//extern	void chargeTestDisplay(u8 vChargeIcSysStts,u8 vChargeIcChgStts, float  iBat,float vBattReal);
extern	void chargeTestDisplay(u8 vChargeIcSysStts,u8 vChargeIcChgStts, float  iBat,float vBattReal,float vbat);
void 	OLED_ShowCHinese(u8 x,u8 y,u8 no);
void 	OLED_ShowString(u8 x,u8 y,u8 *chr);
//void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2);
u8 		OLED_ShowNum(u8 x,u8 y,u32 num);
void 	OLED_ShowChar(u8 x,u8 y,u8 chr);
void 	OLED_ShowOneSegment(u8 x,u8 y,u8  data1,u8 data2);
void OLED_Set_Pos(unsigned char x, unsigned char y);
extern	void clear(void);
extern	void  versionDisplay(void);
extern	void  temperatureDisplay(void);
extern	void	 factoryModeTemperatureDisplay(void);
extern	void	factoryModeVoltDisplay(float volt);
//extern	void	oledTestModeChineseString(u8 x,u8 y,const u8 *str,u8 len);
void	displayFactoryMode(u8 Index);
#endif /* SOURCE_INC_USER_LCD_H_ */
