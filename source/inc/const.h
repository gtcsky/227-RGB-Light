/*
 * const.h
 *
 *  Created on: 2019��8��12��
 *      Author: Bough_004
 */

#ifndef USER_INC_CONST_H_
#define USER_INC_CONST_H_
#include "gpio.h"

typedef signed char 			i8;
typedef unsigned char 		u8;
typedef unsigned char 		U8;
typedef unsigned char 		UINT8;
typedef short	 				i16;
typedef unsigned short 		u16;
typedef unsigned short 		U16;
typedef unsigned short 		UINT16;
typedef int 					i32;
typedef unsigned 				u32;
typedef unsigned 				U32;
typedef unsigned 				UINT32;
typedef long long 			i64;
typedef unsigned long long 	u64;
typedef unsigned long long 	U64;
typedef unsigned long long 	UINT64;


#define	SOFTWARE_VERSION		"SW:2020-06-05"
#define	HARDWARE_VERSION		"HW:VER11"

#define	Max_Batt_level		6
#define	Max_Arrow_Index		6
#define	MAX_SLOW_ADJUST_STEPS	40

#define 	USER_DOG_CONST			1600				//1600*5ms=8s
#define	MAX_CD_TIMER		100

#define	MAX_ColorTemp		56
#define	MIN_ColorTemp		32

//#define	BATTERY_RESISTANCE		0.060
#define	BATTERY_RESISTANCE		0.080		//80mR	for charging
#define	CHARGE_VOLT_DIVIATION		0.06//		0.06V
#define	DISCHARGING_THRESHOLD	0.3			//0.3A
#define 	RED_POWER_RATING			1.06*4.07
#define 	GREEN_POWER_RATING		1.04*4.07
#define 	BLUE_POWER_RATING		1.67*3.97
#define	CW_POWER_RATING			9.72//(2.4*4.05)//2.42*3.85
#define	MW_POWER_RATING			9.234//(2.28*4.05)//2.42*3.85
//#define 	DISCHARGE_RESISTENCE		0.162
#define 	DISCHARGE_RESISTENCE		0.080		//80mR for discharging
//#define	MW_POWER_RATING			2.00*3.91*(1.0/0.9)


#define	MAX_LightEffect		2

#define	ON_LED_DELAY_CONST	20

//#define	PWM_FRQ_CONST	1020				//分成PWM 2560级,   即频率为24M/(2560*2)=4.68K  ,因为PCA时小分频设定为PCLK/2
#define	PWM_FRQ_CONST	1200				//分成PWM 2560级,   即频率为24M/(2560*2)=4.68K  ,因为PCA时小分频设定为PCLK/2
#define	PWM_MAX_CW		1080
#define	PWM_INIT_DATA		0x0000					//PCA PWM初始化占空比  0000/2560

#define 	DISABLE_BATT_ADC_TIMER_CONST	40;			//40*50ms=2s

typedef struct{

	UINT16	red;
	UINT16	green;
	UINT16	blue;
	UINT16	hues;
	float	saturation;
	float	brightness;
}colorStructType;


#define	DEFAULT_HUES				0//33
#define	DEFAULT_BRIGHTNESS		5//10
//#define	DEFAULT_BRIGHTNESS		100//10
#define	DEFAULT_SATURATION		100//100
#define	DEFAULT_ARROR_INDEX		0//3
//#define	DEFAULT_ARROR_INDEX		5//3
#define	DEFAULT_COLOR_TEMP		43
#define	DEFAULT_STYLE1_VALUE		0
#define	DEFAULT_STYLE2_VALUE		0
//#define	DEFAULT_STYLE2_VALUE		2
#define	DEFAULT_STYLE3_VALUE		0

#define	TEST_MODE_RED_DUTY		1140//(0.1*PWM_FRQ_CONST)
#define	TEST_MODE_GEEN_DUTY		1140//(0.1*PWM_FRQ_CONST)
#define	TEST_MODE_BLUE_DUTY		1140//(0.1*PWM_FRQ_CONST)
#define	TEST_MODE_CW_DUTY		1030//(0.1*PWM_FRQ_CONST)
#define	TEST_MODE_MW_DUTY		1080//(0.1*PWM_FRQ_CONST)

#define	NORMAL_MODE_TIMEOUT				60*20			//60s
#define	POWER_DOWN_MODE_TIMEOUT		10				//10*50ms=500ms
//#define	POWER_DONW_LCD_TIMEOUT			60				//3S
#define	POWER_DONW_LCD_TIMEOUT			10				//750ms

#define	LED_REAL_START_DUTY		4		// LED 实际亮度起步4%
#define	RGBLED_REAL_START_DUTY	9		// LED 实际亮度起步4%
#define	RGBLED_REAL_START_DUTY_PERCENT	(RGBLED_REAL_START_DUTY*0.01)		// LED 实际亮度起步4%

//#define	LED_REAL_START_DATA		0x0F		//冷光和暖光最低有效亮度
#define	LED_REAL_START_DATA		0x18		//冷光和暖光最低有效亮度
#define	RedGreenRate	1.3
#define	BlueGreenRate	1.6
#define	RedGreenRateStep		(1-(1/RedGreenRate))
#define	BlueGreenRateStep		(1-(1/BlueGreenRate))
#define	BrightnessThreshold	0.40
#define	BrightnessGap	(BrightnessThreshold-RGBLED_REAL_START_DUTY*1.0/100)
#define	BrightnessCompersationGap	(RGBLED_REAL_START_DUTY-LED_REAL_START_DUTY)
#define	BrightnessCompersationStep	((RGBLED_REAL_START_DUTY-LED_REAL_START_DUTY)/(3*BrightnessGap*100))

#define	TEMPERATURE_COE_NORMAL	1.0
#define	TEMPERATURE_COE_LOW		0.8
#define 	OVER_TEMPERATURE_CONST_Lv1	80
#define 	OVER_TEMPERATURE_CONST_Lv0	70
#define 	NORMAL_TEMPERATURE_CONST	60
//#define 	OVER_TEMPERATURE_CONST_Lv1	55
//#define 	OVER_TEMPERATURE_CONST_Lv0	44
//#define 	NORMAL_TEMPERATURE_CONST		38


#define	ADC_LEVEL_CONST	4096




#define	RGB_MAX_RATE			1.0
#define	RGB_MIN_RATE			0.90


#define	BATT_ARRAY_SIZE		10
#define	BATT_LV0_THESHOLD		3.3
//#define	BATT_LV0_THESHOLD		3.45
#define	BATT_LV1_THESHOLD		3.5
//#define	BATT_LV2_THESHOLD		3.6
//#define	BATT_LV3_THESHOLD		3.7
#define	BATT_LV2_THESHOLD		3.62
#define	BATT_LV3_THESHOLD		3.71
#define	BATT_LV4_THESHOLD		3.8
#define	BATT_LV5_THESHOLD		3.9
#define	BATT_LV6_THESHOLD		4.05

#define	ADC_PERIOD_MAX_INDEX		9		//
#define	ADC_FINAL_TEMP_INDEX			8
typedef enum{
	AdcTimeToAvdd=0,				//adc序号为0时LDO电压
	AdcTimeToBatt=1,					//adc序号为1时处理电池
	ADCTimeToTemp1=2,				//adc序号为2时第1次处理温度
	ADCTimeToTemp2=3,				//adc序号为3时第2次处理温度
	ADCTimeToTemp3=4,				//adc序号为4时第3次处理温度
	ADCTimeToTemp4=5,				//adc序号为5时第4次处理温度
	ADCTimeToTemp5=6,				//adc序号为6时第5次处理温度
	ADCTimeToTemp6=7,				//adc序号为7时第6次处理温度
	ADCTimeToTemp7=8				//adc序号为8时第7次处理温度
}AdcIndexEnum;

typedef struct{
	UINT8	DisplayModeIndex;		//	参考值: displayModeEnum
	UINT8	arrowIndex;				//	光标序号	(0~6)		参考值:settingIndexEnum
	UINT8	battIndex;				//	电量标志	(0~6)
	UINT16	hues;					//	色调值	(0~360)
	UINT8	saturation;				//	饱和值	(0~100)
	UINT8	brightness;				//	亮度值	(0~100)%
	UINT8	colorTemperature;			//	33~65(00K)
	UINT8	style1Value;				//	0~2对应闪光灯标志A,B,C 三种模式
	UINT8	style2Value;				//	0~2对应循环标志A,B,C 三种模式
	UINT8	style3Value;				//	0~2对应锁标志A,B,C 三种模式
	UINT8	countDownTimer;			//	倒数模式计时
}displayParamsStruct;


typedef struct{
	UINT16	indicatorMaxIndex;
	UINT16	currentIndex;
	UINT8	indicatorStyle;
	UINT16	customizeLoopTimes;				//自定义模式循环次数
	UINT8	customizeLoopIndex;				//自定义模式循环已执行次数
	const 	UINT8 *	timeArray;
	const	UINT8 *	sttsArray;
	const	UINT16 * HuesArray;
	func_ptr_t  pfnLedFuncCallBack;
	func_ptr_t  pfncustomizeEffectOverCallBack;	//算定循环结束时回调
	UINT8	fIsGradualMode;					//0/1 闪烁模式/渐变模式
	UINT8	IndexOfHues;
	UINT8	MaxHuesIndex;
}LEDParametersStruct;

typedef struct{
	UINT16	valueOfRed;
	UINT16	valueOfGreen;
	UINT16	valueOfBlue;
	UINT16	valueOfCw;
	UINT16	valueOfMw;
}PcaDataStruct;


typedef enum{
	OFF=0,
	ON=1
}lightSttsEnum;

typedef enum{
	IdleIamgeDisplay=0,
	ModeTDisplay=1,
	CountDownDisplay=2,
	ChargingAtPowerDown=3,
	FactoryMode=4
}displayModeEnum;

typedef enum{									//关机充电状态指示
	DoNothing=0,
	SetWakeupLCD=1,
	WakeupLCDOver=2,
	TimeToPowerOff=3
}chargeSttsAtPowerModeEnum;

typedef enum{
	HuesSetting=0,
	SaturationSetting=1,
	BrightnessSetting=2,
	ColorTempSetting=3,
	Style1Setting=4,
	Style2Setting=5,
	Style3Setting=6
}settingIndexEnum;


typedef enum{
	Red_Current=0,
	Green_Current=1,
	Blue_Current=2,
	Cw_Current=3,
	Mw_Current=4,
	Standby_Current=5,
	Sleep_Current=6,
	Press_Any_key1=7,
	Press_Func_key=7,
	Press_Any_key2=8,
	Press_Binc_key=8,
	Press_Bdec_key=8,
	Press_Any_key3=9,
	Press_Dec_key=9,
	Press_Inc_key=9,
	System_Volt_Display=10,
}TestModeStringEnum;

typedef enum{
	VersionDisplay=0,
	WaitFuncKey,
	RedLedTesting,
	GreenLedTesting,
	BlueLedTesting,
	CoolLedTesting,
	WarmLedTesting,
	WaitBrightnesssDecKey,
	IdleModeTesting,
	SystemVoltTesting,
	WaitIncKey,
	SleepModeTesting

}factoryModeItem;
#define	MAX_FACTORY_ITEM	SleepModeTesting
#define	FACTORY_TIME_1S	100
#define	FACTORY_TIME_2S	200
#define	FACTORY_TIME_2S9	290
#define	FACTORY_TIME_1S9	190
typedef struct{
	factoryModeItem	factoryIndex;
	u16	factoryTimerCnt;			//step=10ms
	u8	factoryBrightIndex;
	u8	factoryIncIndex;

}factoryModeTypeDef;

#endif /* USER_INC_CONST_H_ */
