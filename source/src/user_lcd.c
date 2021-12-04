/*
 * user_lcd.c
 *
 *  Created on: 2019年10月11日
 *      Author: Bough_004
 */

#include "user_lcd.h"
#include "ddl.h"
#include "user_spi.h"
#include "user_adc.h"
#include "user_lptimer.h"
#include "user_character.h"
#include "user_menu.h"
#include "user_i2c.h"


u8	arrowIndex;
extern	u8 fIsSystemInitialing;
void lcdGpioInit(void) {
	stc_gpio_config_t stcGpioCfg;
	DDL_ZERO_STRUCT(stcGpioCfg);
	stcGpioCfg.enDir = GpioDirOut;
	Gpio_Init(Lcd_Res_Port, Lcd_Res_Pin, &stcGpioCfg); //
	set_LcdResPin;
//	Gpio_Init(Lcd_Power_Port, Lcd_Power_Pin, &stcGpioCfg); //
//	LcdPowerOn;
	Gpio_Init(Lcd_Dc_Port, Lcd_Dc_Pin, &stcGpioCfg); //
	set_LcdDcPin;

}

void writeLcdCommand(u8 cmd){
	u8 vtTemp=cmd;
	clr_LcdDcPin;
	//return;
	WriteData(Spi0,&vtTemp,1);
	set_LcdDcPin;
}
void writeLcdData(u8 data){
	u8 vtTemp=data;
	set_LcdDcPin;
	//return;
	WriteData(Spi0,&vtTemp,1);
}
/*********************************************************************
 *
 * 清除所有LCD显示
 *
 **********************************************************************/
void clear(void)
 {
	unsigned char x, y;
	fIsChargeDisplaying=0;
	writeLcdCommand(0x00);
	writeLcdCommand(0x10); /*set higher column address*/

	for (y = 0; y < 8; y++) {
		writeLcdCommand(0xB0 + y); /*set page address*/
		writeLcdCommand(0x00);
		writeLcdCommand(0x10);
		for (x = 0; x < 128; x++)
			writeLcdData(0x00);
		writeLcdData(0x00);
	}
}



///*********************************************************************
// *
// * LCD进入关机状态
// *
// **********************************************************************/
//void lcdEnterPowerDownMode(void){
//
//}

/*********************************************************************
 *
 * LCD进入休眠状态
 *
 **********************************************************************/
void  lcdEnterSleep(void){
	writeLcdCommand(0xAE); /*display off*/
	writeLcdCommand(0x8d); /*set charge pump disable*/
	writeLcdCommand(0x10);		//使用外置DC
	delay100us(5);
//	LcdPowerOff;
}
/*********************************************************************
 *
 * LCD退出休眠状态
 *
 **********************************************************************/
void  lcdExitSleep(void){
//	LcdPowerOn;
	vSystem5ms = 0;
	while (vSystem5ms <= LCD_POWER_ON_CONST)		;
	vSystem5ms = 0;
	writeLcdCommand(0xAE); /*display off*/
	writeLcdCommand(0x8d); /*set charge pump disable*/
	writeLcdCommand(0x14);		//使用内置DC
	writeLcdCommand(0xAF); /*display ON*/
	while (vSystem5ms <= LCD_POWER_ON_CONST)		;
	vSystem5ms = 0;
}

void batteryPercentDisplay(u8 level) {
	if (!level) {
		OLED_ShowChar(ICON_Percent_X, ICON_Percent_Y, ' ');
		OLED_ShowChar(ICON_Percent_X + 8, ICON_Percent_Y, ' ');
		OLED_ShowChar(ICON_Percent_X + 16, ICON_Percent_Y, ' ');
		OLED_ShowChar(ICON_Percent_X + 24, ICON_Percent_Y, ' ');
		OLED_ShowChar(ICON_Percent_X + 32, ICON_Percent_Y, ' ');
	} else {

		if (level != 100) {
			OLED_ShowChar(ICON_Percent_X, ICON_Percent_Y, ' ');
			OLED_ShowChar(ICON_Percent_X + 8, ICON_Percent_Y, level/10 + '0');
		} else {
			OLED_ShowChar(ICON_Percent_X, ICON_Percent_Y, '1');
			OLED_ShowChar(ICON_Percent_X + 8, ICON_Percent_Y, '0');
		}
		OLED_ShowChar(ICON_Percent_X + 16, ICON_Percent_Y, level % 10 + '0');
		OLED_ShowCHinese(ICON_Percent_X + 24, ICON_Percent_Y, Value_Percent_Addr);
	}
}


/**********
 *
 */
void batterDisplay(u8 level) {
	if(FactoryMode ==displayParams.DisplayModeIndex )
		return;
	if (level > Max_Batt_level)
		level = Max_Batt_level;
	switch (level) {
	case 0:
//		if(fIsSystemOff){
//			OLED_ShowCHinese(ICON_batt_X0_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv0_Addr);
//			OLED_ShowCHinese(ICON_batt_X1_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv0_Addr + 1);
//		}
//		else{
			OLED_ShowCHinese(ICON_batt_X0, ICON_batt_Y, ICON_batt_lv0_Addr);
			OLED_ShowCHinese(ICON_batt_X1, ICON_batt_Y, ICON_batt_lv0_Addr + 1);
//		}
			if(fIsSystemOff&&fIsCharging){
				batteryPercentDisplay(0);
			}
		break;
	case 1:
//		if(fIsSystemOff){
//			OLED_ShowCHinese(ICON_batt_X0_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv1_Addr);
//			OLED_ShowCHinese(ICON_batt_X1_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv1_Addr + 1);
//		}else{
			OLED_ShowCHinese(ICON_batt_X0, ICON_batt_Y, ICON_batt_lv1_Addr);
			OLED_ShowCHinese(ICON_batt_X1, ICON_batt_Y, ICON_batt_lv1_Addr + 1);
//		}
			if(fIsSystemOff&&fIsCharging){
				batteryPercentDisplay(0);
			}
		break;
	case 2:
//		if(fIsSystemOff){
//			OLED_ShowCHinese(ICON_batt_X0_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv2_Addr);
//			OLED_ShowCHinese(ICON_batt_X1_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv2_Addr + 1);
//		}else{
			OLED_ShowCHinese(ICON_batt_X0, ICON_batt_Y, ICON_batt_lv2_Addr);
			OLED_ShowCHinese(ICON_batt_X1, ICON_batt_Y, ICON_batt_lv2_Addr + 1);
//		}
			if(fIsSystemOff&&fIsCharging){
				batteryPercentDisplay(0);
			}
		break;

	case 3:
//		if(fIsSystemOff){
//			OLED_ShowCHinese(ICON_batt_X0_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv3_Addr);
//			OLED_ShowCHinese(ICON_batt_X1_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv3_Addr + 1);
//		}else{
			OLED_ShowCHinese(ICON_batt_X0, ICON_batt_Y, ICON_batt_lv3_Addr);
			OLED_ShowCHinese(ICON_batt_X1, ICON_batt_Y, ICON_batt_lv3_Addr + 1);
//		}
			if(fIsSystemOff&&fIsCharging){
				batteryPercentDisplay(0);
			}
		break;
	case 4:
//		if(fIsSystemOff){
//			OLED_ShowCHinese(ICON_batt_X0_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv4_Addr);
//			OLED_ShowCHinese(ICON_batt_X1_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv4_Addr + 1);
//		}else{
			OLED_ShowCHinese(ICON_batt_X0, ICON_batt_Y, ICON_batt_lv4_Addr);
			OLED_ShowCHinese(ICON_batt_X1, ICON_batt_Y, ICON_batt_lv4_Addr + 1);
//		}
			if(fIsSystemOff&&fIsCharging){
				batteryPercentDisplay(0);
			}
		break;
	case 5:
//		if(fIsSystemOff){
//			OLED_ShowCHinese(ICON_batt_X0_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv5_Addr);
//			OLED_ShowCHinese(ICON_batt_X1_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv5_Addr + 1);
//		}else{
			OLED_ShowCHinese(ICON_batt_X0, ICON_batt_Y, ICON_batt_lv5_Addr);
			OLED_ShowCHinese(ICON_batt_X1, ICON_batt_Y, ICON_batt_lv5_Addr + 1);
//		}
			if(fIsSystemOff&&fIsCharging){
				batteryPercentDisplay(0);
			}
		break;
	case 6:
//		if(fIsSystemOff){
//			OLED_ShowCHinese(ICON_batt_X0_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv6_Addr);
//			OLED_ShowCHinese(ICON_batt_X1_systemOff, ICON_batt_Y_systemOff, ICON_batt_lv6_Addr + 1);
//		}else{
			OLED_ShowCHinese(ICON_batt_X0, ICON_batt_Y, ICON_batt_lv6_Addr);
			OLED_ShowCHinese(ICON_batt_X1, ICON_batt_Y, ICON_batt_lv6_Addr + 1);
//		}
			if(fIsSystemOff&&fIsCharging){
				powerDownModeChargePercent();
			}

		break;
	}
	if(fIsCharging&&!fIsChargeDisplaying){
		fIsChargeDisplaying=1;
		OLED_ShowCHinese(ICON_charge_X0, ICON_charge_Y, ICON_charge_TableAddr);
		OLED_ShowCHinese(ICON_charge_X1, ICON_charge_Y, ICON_charge_TableAddr+1);
	}
	else if(!fIsCharging&&fIsChargeDisplaying){
		if(!fIsSystemOff){						//关机充电指示由再次进入睡眠来完成,防止图标闪烁
			fIsChargeDisplaying=0;
			OLED_ShowString(ICON_charge_X0,ICON_charge_Y,"    ");
		}
	}
}

/*****************************************************************************************
 *
 * 光标显示
 *
 ******************************************************************************************/
void 	updateArrowDisplay(displayParamsStruct * disParams){
		u8 i;
		for(i=0;i<=Max_Arrow_Index;i++){
			if(i==disParams->arrowIndex){
				OLED_ShowChar(ArrowArray[disParams->arrowIndex][0], ArrowArray[disParams->arrowIndex][1], ICON_Arrow_ADDRESS);
			}
			else{
				OLED_ShowChar(ArrowArray[i][0], ArrowArray[i][1], ' ');

			}
		}
}
/*****************************************************************************************
 *
 * 色调值显示
 *
 ******************************************************************************************/
void 	updateHuesDisplay(displayParamsStruct * disParams){
		u8 len=0;
		//OLED_ShowString(Value_Hues_X,ICON_HUES_Y,"    ");
		len=OLED_ShowNum(Value_Hues_X,ICON_HUES_Y,disParams->hues);
		OLED_ShowChar(Value_Hues_X+8*len, ICON_HUES_Y, ICON_Degree_ADDRESS);
		if(len<3)
			OLED_ShowChar(Value_Hues_X+8*3, ICON_HUES_Y, ' ');
		if(len<2)
			OLED_ShowChar(Value_Hues_X+8*2, ICON_HUES_Y, ' ');
}
/*****************************************************************************************
 *
 * 饱和度值显示
 *
 ******************************************************************************************/
void 	updateSaturationDisplay(displayParamsStruct * disParams){
		OLED_ShowString(Value_Saturation_X,ICON_Saturation_Y,"   ");
		OLED_ShowNum(Value_Saturation_X,ICON_Saturation_Y,disParams->saturation);
}
/*****************************************************************************************
 *
 * 亮度值显示
 *
 ******************************************************************************************/
void updateBrightnessDisplay(displayParamsStruct * disParams) {
//	u8 len = 0;
//	OLED_ShowString(Value_Brightness_X,ICON_BRIGHTNESS_Y,"     ");
//	len = OLED_ShowNum(Value_Brightness_X, ICON_BRIGHTNESS_Y, disParams->brightness);
//	OLED_ShowCHinese(Value_Brightness_X+8*len, ICON_BRIGHTNESS_Y, Value_Percent_Addr);
	if(disParams->brightness!=100){
		OLED_ShowChar(Value_Brightness_X, ICON_BRIGHTNESS_Y, ' ');
	}else{
		OLED_ShowChar(Value_Brightness_X, ICON_BRIGHTNESS_Y, '1');
	}
	if(disParams->brightness<=9){
		OLED_ShowChar(Value_Brightness_X+8, ICON_BRIGHTNESS_Y, ' ');
	}else{
		OLED_ShowChar(Value_Brightness_X+8, ICON_BRIGHTNESS_Y, disParams->brightness/10%10+'0');
	}
	OLED_ShowChar(Value_Brightness_X+16, ICON_BRIGHTNESS_Y,disParams->brightness%10+'0');
	OLED_ShowCHinese(Value_Brightness_X+24, ICON_BRIGHTNESS_Y, Value_Percent_Addr);
}
/*****************************************************************************************
 *
 * 色温值显示
 *
 ******************************************************************************************/
void updateColorTempDisplay(displayParamsStruct * disParams) {
	u8 len = 0;
	len = OLED_ShowNum(Value_ColorTemp_X, ICON_ColorTemp_Y, disParams->colorTemperature*100);
	OLED_ShowChar(Value_ColorTemp_X+8*len, ICON_ColorTemp_Y, 'K');
}
/*****************************************************************************************
 *
 *倒数初始画面显示
 *
 ******************************************************************************************/
void staticCountDownDisplay(displayParamsStruct * disParams) {
	u8 len = 0;
	len = OLED_ShowNum(Value_CD_X, Value_CD_Y, disParams->countDownTimer);
	OLED_ShowChar(Value_CD_X+8*len, Value_CD_Y, 'S');
}

/*****************************************************************************************
 *
 *计时条更新
 *
 ******************************************************************************************/
void updateTimerBar(displayParamsStruct * disParams) {
	u8 i;
	OLED_ShowChar(TimerBarX, TimerBarY, TimerBarStartAddr);
	for(i=0;i<MAX_CD_TIMER;i++){
		if(i==0)
			OLED_Set_Pos(TimerBarX+4,TimerBarY);
		if(disParams->countDownTimer>i){
			writeLcdData(0xEC);
//			OLED_ShowOneSegment(TimerBarX+3+i,TimerBarY,0xEC,0x37);
//			OLED_ShowOneSegment(TimerBarX+3,TimerBarY,0xEC,0x37);
//			OLED_ShowOneSegment(20,TimerBarY+1,0xEC,0x37);
//			OLED_ShowOneSegment(22,TimerBarY,0xEC,0x37);
		}
		else
			//OLED_ShowOneSegment(TimerBarX+3+i,TimerBarY,0x0C,0x30);
			writeLcdData(0x0C);
	}
	for(i=0;i<MAX_CD_TIMER;i++){
		if(i==0)
			OLED_Set_Pos(TimerBarX+4,TimerBarY+1);
		if(disParams->countDownTimer>i){
			writeLcdData(0x37);
		}
		else
			writeLcdData(0x30);
	}
	OLED_ShowChar(TimerBarEndX, TimerBarY, TimerBarEndAddr);
}
/*****************************************************************************************
 *
 *倒数模式计时值显示
 *
 ******************************************************************************************/
void updateCountDownDisplay(displayParamsStruct * disParams) {
	u8 len = 0;
	OLED_ShowString(Counting_X,Value_CD_Y,"    ");
	len = OLED_ShowNum(Counting_X, Value_CD_Y, disParams->countDownTimer);
	OLED_ShowChar(Counting_X+8*len, Value_CD_Y, 'S');
	updateTimerBar(disParams);
}
/*****************************************************************************************
 *
 *灯效样式显示
 *
 ******************************************************************************************/
void updateLightEffectDisplay(displayParamsStruct * disParams){
	if(Style1Setting==disParams->arrowIndex)
		OLED_ShowCHinese(ICON_Style1_X, ICON_Style1_Y,ICON_StyleA_ADDRESS+disParams->style1Value);
	else if(Style2Setting==disParams->arrowIndex)
		OLED_ShowCHinese(ICON_Style2_X, ICON_Style2_Y,ICON_StyleA_ADDRESS+disParams->style2Value);
	else if(Style3Setting==disParams->arrowIndex)
		OLED_ShowCHinese(ICON_Style3_X, ICON_Style3_Y,ICON_StyleA_ADDRESS+disParams->style3Value);
}
/*****************************************************************************************
 *
 *过热显示
 *
 ******************************************************************************************/
void updateHotDisplay(void){
	if(fIsHotNow){
		if(!fIsHotDisplayOn){
			OLED_ShowCHinese(ICON_Hot_X, ICON_Hot_Y,ICON_Hot_Addr);
			OLED_ShowCHinese(ICON_Hot_X1, ICON_Hot_Y,ICON_Hot_Addr+1);
			fIsHotDisplayOn=TRUE;
		}else{
			OLED_ShowCHinese(ICON_Hot_X, ICON_Hot_Y,ICON_batt_lv0_Addr);
			OLED_ShowCHinese(ICON_Hot_X1, ICON_Hot_Y,ICON_batt_lv0_Addr+1);
			fIsHotDisplayOn=FALSE;
		}
	}
}

/*****************************************************************************************
 *
 * 待机画面显示
 *
 ******************************************************************************************/
void displaySystemMenu(displayParamsStruct * disParams) {
	clear(); 										/* Clear Screen */
	batterDisplay(disParams->battIndex);
	updateHotDisplay();
	if (IdleIamgeDisplay==disParams->DisplayModeIndex ) {
		OLED_ShowChar(ArrowArray[disParams->arrowIndex][0], ArrowArray[disParams->arrowIndex][1], ICON_Arrow_ADDRESS);
		OLED_ShowCHinese(ICON_HUES_X, ICON_HUES_Y, ICON_HUES_ADDRESS);//色调图标
		updateHuesDisplay(disParams);
		OLED_ShowCHinese(ICON_Saturation_X, ICON_Saturation_Y, ICON_Saturation_ADDRESS);				//饱和度图标
		updateSaturationDisplay(disParams);
		OLED_ShowCHinese(ICON_BRIGHTNESS_X, ICON_BRIGHTNESS_Y, ICON_BRIGHTNESS_ADDRESS);		//亮度图标
		updateBrightnessDisplay(disParams);
		OLED_ShowCHinese(ICON_BRIGHTNESS_X, ICON_ColorTemp_Y, ICON_ColorTemp_ADDRESS);			//色温图标
		updateColorTempDisplay(disParams);

		OLED_ShowCHinese(ICON_Flash_X, ICON_Flash_Y, ICON_Flash_ADDRESS);							//右侧闪光灯图标
		OLED_ShowCHinese(ICON_Style1_X, ICON_Style1_Y, ICON_StyleA_ADDRESS+disParams->style1Value);

		OLED_ShowCHinese(ICON_Loop_X, ICON_Loop_Y, ICON_Loop_ADDRESS);	//右侧循环图标
		OLED_ShowCHinese(ICON_Style2_X, ICON_Style2_Y, ICON_StyleA_ADDRESS+disParams->style2Value);

		OLED_ShowCHinese(ICON_Lock_X, ICON_Lock_Y, ICON_Lock_ADDRESS);	//右侧警报图标
		OLED_ShowCHinese(ICON_Style3_X, ICON_Style3_Y, ICON_StyleA_ADDRESS+disParams->style3Value);

	}else if(ModeTDisplay==disParams->DisplayModeIndex){
		disParams->countDownTimer=MAX_CD_TIMER;
		OLED_ShowCHinese(ICON_T_X, ICON_T_Y, ICON_T_Addr);					//反显T图标
		OLED_ShowString(Counting_X, Counting_Y, "     ");
		OLED_ShowString(TimerBarX, TimerBarY, "                ");
		staticCountDownDisplay(disParams);
	}else if(CountDownDisplay==disParams->DisplayModeIndex){
		OLED_ShowCHinese(ICON_T_X, ICON_T_Y, ICON_T_Addr);					//反显T图标
		OLED_ShowString(Value_CD_X, Value_CD_Y, "    ");
		updateCountDownDisplay(disParams);
	}else if(ChargingAtPowerDown==disParams->DisplayModeIndex){
		return;
	}
}

/*********************************************************************
 *
 * LCD上电初始化
 *
 **********************************************************************/

void powerOnLcdInit(void) {
	userSpiInit();
	lcdGpioInit();
	set_LcdResPin;
	delay100us(10);
	clr_LcdResPin;
	delay100us(10);
	set_LcdResPin;
	delay100us(10);
	writeLcdCommand(0xAE); /*display off*/

	writeLcdCommand(0x00); /*set lower column address*/
	writeLcdCommand(0x10); /*set higher column address*/
	writeLcdCommand(0x40); /*set display start line*/

	writeLcdCommand(0xB0); /*set page address*/
	writeLcdCommand(0x81); /*contract control*/
	writeLcdCommand(0xcf); /*128*/

	writeLcdCommand(0xA1); /*set segment remap*/

	writeLcdCommand(0xA6); /*normal / reverse*/

	writeLcdCommand(0xA8); /*multiplex ratio*/
	writeLcdCommand(0x3F); /*duty = 1/64*/

	writeLcdCommand(0xC8); /*Com scan direction*/

	writeLcdCommand(0xD3); /*set display offset*/
	writeLcdCommand(0x00);
	writeLcdCommand(0xD5); /*set osc division*/
	writeLcdCommand(0x80);
	writeLcdCommand(0xD9); /*set pre-charge period*/
	writeLcdCommand(0x22);

	writeLcdCommand(0xDA); /*set COM pins*/
	writeLcdCommand(0x12);

	writeLcdCommand(0xdb); /*set vcomh*/
	writeLcdCommand(0x30);
	writeLcdCommand(0x8d); /*set charge pump disable*/

	//writeLcdCommand(0x10);		//使用外置DC
	writeLcdCommand(0x14);		//使用内置DC

	clear(); /* Clear Screen */
	writeLcdCommand(0xAF); /*display ON*/
	vSystem5ms = 0;
	while (vSystem5ms <= LCD_POWER_ON_CONST);
	vSystem5ms = 0;
	if(!fIsSystemInitialing)
		displaySystemMenu(&displayParams);
}
/************************************************************
 *
 * 定位OLED 显示坐标
 *
 *************************************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	writeLcdCommand(0xb0 + y);
	writeLcdCommand(((x & 0xf0) >> 4) | 0x10);
	writeLcdCommand((x & 0x0f) | 0x01);
}


//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示
//size:选择字体 16/12
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{
	unsigned char c=0,i=0;
		c=chr-' ';//得到偏移后的值
		if(x>Max_Column-1){x=0;y=y+2;}
//		if(SIZE ==16)
//			{
			OLED_Set_Pos(x,y);
			for(i=0;i<8;i++)
			writeLcdData(F8X16[c*16+i]);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			writeLcdData(F8X16[c*16+i+8]);
//			}
//			else {
//				OLED_Set_Pos(x,y+1);
//				for(i=0;i<6;i++)
//				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
//
//			}
}
/**********************************************************************
 *
 * 写单条Segment 数据(16条com线)数据
 *
 ***********************************************************************/
void OLED_ShowOneSegment(u8 x,u8 y,u8  data1,u8 data2){
	OLED_Set_Pos(x,y);
	writeLcdData(data1);
	OLED_Set_Pos(x,y+1);
	writeLcdData(data2);
}


//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)result*=m;
	return result;
}

u8 getNumLen(u32 num){
	u8 len=1;
	while (1) {
		if (num < 10)
			return	len;
		else
			num /= 10;
		len++;
	}
}
////显示2个数字
////x,y :起点坐标
////len :数字的位数
////size:字体大小
////mode:模式	0,填充模式;1,叠加模式
////num:数值(0~4294967295);
//void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
//{
//	u8 t,temp;
//	u8 enshow=0;
//	for(t=0;t<len;t++)
//	{
//		temp=(num/oled_pow(10,len-t-1))%10;
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp==0)
//			{
//				OLED_ShowChar(x+(size2/2)*t,y,' ');
//				continue;
//			}else enshow=1;
//		}
//	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0');
//	}
//}
/***************************************************************
 *
 * 显示数字 并返回数字长度
 *
 ****************************************************************/
u8 OLED_ShowNum(u8 x, u8 y, u32 num) {
	u8 t, temp;
	u8 enshow = 0;
	u8 len = getNumLen(num);
	for (t = 0; t < len; t++) {
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1)) {
			if (temp == 0) {
				OLED_ShowChar(x + 8 * t, y, ' ');
				continue;
			} else
				enshow = 1;
		}
		OLED_ShowChar(x + 8 * t, y, temp + '0');
	}
	return len;
}




//显示一个字符号串
void OLED_ShowString(u8 x,u8 y,u8 *chr)
 {
	unsigned char j = 0;
	while (chr[j] != '\0') {
		OLED_ShowChar(x, y, chr[j]);
		x += 8;
		if (x > 120) {
			x = 0;
			y += 2;
		}
		j++;
	}
}


//显示汉字
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
 {
	u8 t, adder = 0;
	OLED_Set_Pos(x, y);
	for (t = 0; t < 16; t++) {
		writeLcdData(Hzk[2 * no][t]);
		adder += 1;
	}
	OLED_Set_Pos(x, y + 1);
	for (t = 0; t < 16; t++) {
		writeLcdData(Hzk[2 * no + 1][t]);
		adder += 1;
	}
}



/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
 {
	unsigned int j = 0;
	unsigned char x, y;

	if (y1 % 8 == 0)
		y = y1 / 8;
	else
		y = y1 / 8 + 1;
	for (y = y0; y < y1; y++) {
		OLED_Set_Pos(x0, y);
		for (x = x0; x < x1; x++) {
			writeLcdData(BMP[j++]);
		}
	}
}

void HexDigitDis(u8 x,u8 y,u8 value){
	u8 vtTemp;
	vtTemp=(value>>4)&0x0F;
	if(vtTemp<=9)
		OLED_ShowChar(x, y,vtTemp+'0');
	else
		OLED_ShowChar(x, y,vtTemp+'A'-10);

	vtTemp=value&0x0F;
	if(vtTemp<=9)
		OLED_ShowChar(x+8, y,vtTemp+'0');
	else
		OLED_ShowChar(x+8, y,vtTemp+'A'-10);
}

void chargeTestDisplay(u8 vChargeIcSysStts,u8 vChargeIcChgStts, float  iBat,float vBattReal,float vbat){
	return;
	if(fIsSystemOff&&!fIsSystemInitialing) {
		if(vBattReal>5.0||vbat>5.0){
			vBattVol=3.0;
			vBattReal=3.0;
			vbat=3.0;
		}

		OLED_ShowChar(20, 2,0+'0');
		OLED_ShowChar(20+8, 2,'X');
		HexDigitDis(20+16,2,vChargeIcSysStts);
		OLED_ShowChar(20+32, 2,' ');
		OLED_ShowChar(20+40, 2,' ');
		OLED_ShowChar(20+48, 2,0+'0');
		OLED_ShowChar(20+56, 2,'X');
//		OLED_ShowChar(20+64, 4,((vChargeIcChgStts>>4)&0x0F)+'0');
//		OLED_ShowChar(20+72, 4,(vChargeIcChgStts&0x0F)+'0');
		HexDigitDis(20+64,2,vChargeIcChgStts);

		OLED_ShowChar(12, 4,((u8)iBat)+'0');
		OLED_ShowChar(12+8, 4,'.');
		OLED_ShowChar(12+16, 4,((u8)(iBat*10))%10+'0');
		OLED_ShowChar(12+24, 4,((u16)(iBat*100))%10+'0');
		OLED_ShowChar(12+32, 4,((u16)(iBat*1000))%10+'0');
		OLED_ShowChar(12+40, 4,'A');
		OLED_ShowChar(12+48, 4,' ');
		OLED_ShowChar(12+56, 4,((u8)vBattReal)+'0');
		OLED_ShowChar(12+64, 4,'.');
		OLED_ShowChar(12+72, 4,((u8)(vBattReal*10))%10+'0');
		OLED_ShowChar(12+80, 4,((u16)(vBattReal*100))%10+'0');
		OLED_ShowChar(12+88, 4,((u16)(vBattReal*1000))%10+'0');
		OLED_ShowChar(12+96, 4,'V');

		OLED_ShowChar(12, 6,((u8)vbat)+'0');
		OLED_ShowChar(12+8, 6,'.');
		OLED_ShowChar(12+16, 6,((u8)(vbat*10))%10+'0');
		OLED_ShowChar(12+24, 6,((u16)(vbat*100))%10+'0');
		OLED_ShowChar(12+32, 6,((u16)(vbat*1000))%10+'0');
		OLED_ShowChar(12+40, 6,'V');
	 }
}


void	 factoryModeTemperatureDisplay(void){
	OLED_ShowChar(0, 0,vSystemTemperature/10+'0');
	OLED_ShowChar(8, 0,vSystemTemperature%10+'0');
	OLED_ShowChar(16, 0,ICON_Degree_ADDRESS);
}

void	factoryModeVoltDisplay(float volt){
	//------------------
		OLED_ShowChar(50, 0, ((u8) volt) + '0');
		OLED_ShowChar(50 + 8, 0, '.');
		OLED_ShowChar(50 + 13, 0, ((u8) (volt * 10)) % 10 + '0');
		OLED_ShowChar(50 + 21, 0, ((u16) (volt * 100)) % 10 + '0');
		OLED_ShowChar(50 + 29, 0,  'V');
	//------------------
}

void  versionDisplay(void){
	clear();
	OLED_ShowString(8,2,SOFTWARE_VERSION);
	OLED_ShowString(8,4,HARDWARE_VERSION);
}

void temperatureDisplay(void) {
	//return;
	u8 vtTemperature_X = 0;
	u8 vtTemperature_Y = 0;
	u8 vtVolt_X = 0;
	u8 vtVolt_Y = 0;
	u8 vtCVolt_X = 0;
	u8 vtCVolt_Y = 0;
	if (fIsSystemTempGot) {
		fIsSystemTempGot = 0;
		if (IdleIamgeDisplay == displayParams.DisplayModeIndex) {
			vtTemperature_X = 52;
			vtTemperature_Y = 0;
			vtVolt_X = 52;
			vtVolt_Y = 2;
			vtCVolt_X =vtVolt_X+8 ;
			vtCVolt_Y = 4;
		} else {
			vtTemperature_X = 8;
			vtTemperature_Y = 2;
			vtVolt_X = 80;
			vtVolt_Y = 2;
			vtCVolt_X =vtVolt_X ;
			vtCVolt_Y = 4;
		}
		OLED_ShowChar(vtTemperature_X, vtTemperature_Y, vSystemTemperature / 10 + '0');
		OLED_ShowChar(vtTemperature_X+8, vtTemperature_Y, vSystemTemperature % 10 + '0');
		OLED_ShowChar(vtTemperature_X + 16, vtTemperature_Y, ICON_Degree_ADDRESS);
		//---------------------------------------------------------------------
		OLED_ShowChar(vtVolt_X, vtVolt_Y, ((u8) vAdVoltOfBatt) + '0');
		OLED_ShowChar(vtVolt_X + 8, vtVolt_Y, '.');
		OLED_ShowChar(vtVolt_X + 13, vtVolt_Y, ((u8) (vAdVoltOfBatt * 10)) % 10 + '0');
		OLED_ShowChar(vtVolt_X + 21, vtVolt_Y, ((u16) (vAdVoltOfBatt * 100)) % 10 + '0');
		OLED_ShowChar(vtVolt_X + 29, vtVolt_Y, 'V');
		//---------------------------------------------------------------------
		OLED_ShowChar(vtCVolt_X, vtCVolt_Y, ((u8) vTestCompBatt) + '0');
		OLED_ShowChar(vtCVolt_X + 8, vtCVolt_Y, '.');
		OLED_ShowChar(vtCVolt_X + 13, vtCVolt_Y, ((u8) (vTestCompBatt * 10)) % 10 + '0');
		OLED_ShowChar(vtCVolt_X + 21, vtCVolt_Y, ((u16) (vTestCompBatt * 100)) % 10 + '0');
		//---------------------------------------------------------------------
		OLED_ShowChar(vtCVolt_X, vtCVolt_Y+2, ((u8) vSystemVdd) + '0');
		OLED_ShowChar(vtCVolt_X + 8, vtCVolt_Y+2, '.');
		OLED_ShowChar(vtCVolt_X + 13, vtCVolt_Y+2, ((u8) (vSystemVdd * 10)) % 10 + '0');
		OLED_ShowChar(vtCVolt_X + 21, vtCVolt_Y+2, ((u16) (vSystemVdd * 100)) % 10 + '0');
	}
}

void	oledTestModeChineseString(u8 x,u8 y,const u8 *str,u8 len){
	u8 i=0;
	for(;i<len;i++){
		OLED_ShowCHinese(x+16*i,y,*(str+i));
	}
}

void	displayFactoryMode(u8	item){
	switch(item){
	case Red_Current:
		clear();
		oledTestModeChineseString(16+16,4,RED_LED_CURRENT_STRING,4);
		break;
	case Green_Current:
		//clear();
		oledTestModeChineseString(16+16,4,GREEN_LED_CURRENT_STRING,4);
		break;
	case Blue_Current:
		//clear();
		oledTestModeChineseString(16+16,4,BLUE_LED_CURRENT_STRING,4);
		break;
	case Cw_Current:
		//clear();
		oledTestModeChineseString(16+16,4,CW_LED_CURRENT_STRING,4);
		break;
	case Mw_Current:
		//clear();
		oledTestModeChineseString(16+16,4,MW_LED_CURRENT_STRING,4);
		break;
	case Standby_Current:
		clear();
		oledTestModeChineseString(16+16,4,STANDBY_CURRENT_STRING,4);
		break;
	case Sleep_Current:
		clear();
		oledTestModeChineseString(16+16,4,SLEEP_CURRENT_STRING,4);
		break;
	case Press_Func_key:
		clear();
		oledTestModeChineseString(16,2,PRESS_FUNC_KEY,6);
		oledTestModeChineseString(16,4,TEST,2);
		break;
	case Press_Binc_key:
		clear();
		oledTestModeChineseString(16,2,PRESS_BINC_KEY,7);
		oledTestModeChineseString(16,4,TEST,2);
		oledTestModeChineseString(16+32,4,STANDBY_CURRENT_STRING,4);
		oledTestModeChineseString(16,6,LEVEL,5);
		OLED_ShowString(16+5*16,6,":mA");
		break;
	case Press_Dec_key:
		clear();
		oledTestModeChineseString(16,2,PRESS_DEC_KEY,7);
		oledTestModeChineseString(16,4,TEST,2);
		oledTestModeChineseString(16+32,4,SLEEP_CURRENT_STRING,4);
		oledTestModeChineseString(16,6,LEVEL,5);
		OLED_ShowString(16+5*16,6,":uA");
		break;
	case System_Volt_Display:
		clear();
		oledTestModeChineseString(16,2,PRESS_ANY_KEY,6);
		oledTestModeChineseString(16,4,TEST,2);
		oledTestModeChineseString(16+32,4,BATT_VOLT,4);
		break;
	}
	if (Get_IIC_Int_Stts) {
		if (fIsI2CInitOK) {
				if (fIsCharging) {
				if ((item == 0xff) || (item == Press_Func_key)) {
					if (fIsQuickCharging) {
						oledTestModeChineseString(16, 0, QUICK_CHARGING, 5);
					} else {
						oledTestModeChineseString(16, 0, NORMAL_CHARGING, 5);
					}
				}
				}else{
					fIsQuickCharging=FALSE;
				}
		}
	}
}
