/*
 * user_menu.c
 *
 *  Created on: 2019年8月15日
 *      Author: Bough_004
 */
//#include "user_advtime.h"
//#include "user_basetime.h"
#include "io_define.h"
#include "user_menu.h"
#include "user_key.h"
#include "user_lptimer.h"
#include "user_adc.h"
//#include "user_led.h"
//#include "user_awk.h"
#include "user_color.h"
#include "user_lcd.h"
#include "user_pca.h"
#include "user_spi.h"
#include "user_i2c.h"
#include "user_lightEffect.h"
//extern	u8		vKeyValue;
//extern	u8		fTime50ms;
//extern	u8		fTime100ms;
//extern	u8		fTime500ms;
//extern	u8		fTime1s;
//extern	u16 		vDaValue;
//extern	float 	vDaVolt;
u16		vSystem1s=0;

u8		vSkipBattDectCnt=0;							//当关闭马达或停止反向充电后,5s内,不做普通俄模式下的电池检测,待电池恢复后再检测

u8 	vtIndex=0;
u16 	vTimeToSleep=0;								//vTimeToSleep*50ms  timeout , entry sleep mode
u8	fIsTurnOnByKnob=0;
u8 	fIsCharging=0;
u8 	fIsDischarging=0;
u8 	fIsBattFully=0;
u8	fIsBattLow=0;
u8	fPowerOffByBattLow=0;
u8	fIsBattAdcValueGot=0;							//已经成功获取到电池 的ADC值
u8 	fIsSystemTempGot=0;							//已经刷新系统温度
u8 	fIsSystemAvddGot=0;								//已经刷新系统AVDD
u8 	fIsSingleChlAdcConverting=0;
u8	vCurrentBattLv=0;
u8 	fIsNeedUpdateBattLv=FALSE;
u8 	fIsWakeupByCharge=0;
u8	vTimeToTurnOffLCD=0;					//关机状态,退出充电模式后,延时一段时间关闭LCD,防止初IIC_Int再次唤醒
//u8	fIsNeedWakeupLCD=FALSE;
u8	fIsI2CInitOK=0;
u8	fTimerToPowerOnLED=0;
u8	vPowerOnLEDCnt=0;
colorStructType  userColor;
colorStructType  *pUserColor=&userColor;
float vSystemVdd=0;
u8 	fIsSystemOff=0;				//	0/1  系统开启/关闭
u8 	fIsLightActive;				//	初始画面 灯光是否开启
u8	fIsChargeDisplaying=0;		//
displayParamsStruct 	displayParams;
displayParamsStruct  	 *pDisplayParams=&displayParams;

PcaDataStruct		pcaDataStruct;
PcaDataStruct		*pPcaDataStruct=&pcaDataStruct;
PcaDataStruct		targetPwmDataStruct;			//调节目标值
PcaDataStruct		oriPwmDataStruct;				//起调时,原始值

u8		fIsAdjustSlowMode=FALSE;		//调节色温时,采用缓慢变化模式,以降低调节时的闪烁现象
u8		vAdjustSlowIndex=0;
float	vAdjustSlowCwStep=0;
float 	vAdjustSlowMwStep=0;

displayParamsStruct 	vtDisplayParams;
u8 vDisplayingBattLv=0;
u8	vChargePercent=0;
float vChargeCurrent=0;

u8 vSet16MHzPCLK=0;
u8 fIsSystemInitialing;
u8 vDisableGetBattTimer=0;		//LED关闭一段时间内,不进行DA检测,防止电压还没有恢复


u8	vLightEfftectVoltDectCnt=0;

float vTemperatureCoe=1.0;				//由温度引起的亮度变化系数			>60时 vTemperatureCoe=0.8  <50度时恢复1.0
u8	fIsHotNow=0;
u8	fIsHotDisplayOn=FALSE;			//hot 正在显示
u8	vAdcProcessIndex=0;
u8  aTemperatureArray[ADC_FINAL_TEMP_INDEX-ADCTimeToTemp1+1]={0};
u8	fIsQuickCharging=FALSE;
u8	vBusVolt=5;
u8	vTestModeStts=FALSE;

factoryModeTypeDef	factoryParams={
		.factoryIndex=VersionDisplay,
		.factoryTimerCnt=0,
		.factoryBrightIndex=50,
		.factoryIncIndex=50};
void powerOnIoInit(void) {
	stc_gpio_config_t 	stcLEDPort;
	stc_gpio_config_t stcGpioCfg;
	DDL_ZERO_STRUCT(stcLEDPort);
	DDL_ZERO_STRUCT(stcGpioCfg);

	stcLEDPort.enDrv = GpioDrvH;
	stcLEDPort.enDir = GpioDirOut;
//	Gpio_Init(LED_PORT, LED_PIN, &stcLEDPort);

	stcGpioCfg.enDir = GpioDirOut;
	Gpio_Init(LED_Power_Port, LED_Power_Pin, &stcGpioCfg); 					//
	Gpio_Init(LED_Ctrl_Port, LED_Ctrl_Pin, &stcGpioCfg); 							//

	LEDPowerOff;															//关闭LED电源



//	stcGpioCfg.enDir = GpioDirOut;
//	stcGpioCfg.enDrv = GpioDrvH;
//	Gpio_Init(IIC_Int_Port, IIC_Int_Pin, &stcGpioCfg);								//
//	Gpio_WriteOutputIO(IIC_Int_Port, IIC_Int_Pin, 0);
//	Gpio_WriteOutputIO(IIC_Int_Port, IIC_Int_Pin, 1);
//
//	stcGpioCfg.enDir = GpioDirIn;
//	stcGpioCfg.enPuPd = GpioPd;
//	Gpio_Init(IIC_Int_Port, IIC_Int_Pin, &stcGpioCfg);								//IIC_INT 设置改由外部中断文件完成
//	stcGpioCfg.enDir = GpioDirIn;
//	stcGpioCfg.enPuPd = GpioPu;
//	Gpio_Init(IIC_SCL_Port, IIC_SCL_Pin, &stcGpioCfg);								//I
//	Gpio_Init(IIC_SCL_Port, IIC_SDA_Pin, &stcGpioCfg);								//I

	pUserColor->hues=0;
	pUserColor->saturation=1.0;
	pUserColor->brightness=1.0;

	adcInterVrefEnable();
}

void resetEntrySleepTime(void){
		if(fIsSystemOff)
			vTimeToSleep=POWER_DOWN_MODE_TIMEOUT;							//(5+1)*50ms=300ms
		else
			vTimeToSleep=NORMAL_MODE_TIMEOUT;								//60s
}
void entryNormalModeChk(void){
//		LPTIMER_Cmd(DEBUG_LPTIMERx, ENABLE);
	 	 Lpt_Run();    										//醒来后重启LPT定时器
		resetEntrySleepTime();								//重置进入睡眠时间
}
float userAbs(float f1,float f2){
	if(f2>=f1)
		return f2-f1;
	else
		return	f1-f2;
}

//void testLEDFunc(void){
//	if(get_LED_Stts==TRUE){
//		clr_LED;
//	}else{
//		set_LED;
//	}
//}

void checkBattFunc(void) {
	u8	temp;
	if(fIsBattAdcValueGot){
		fIsBattAdcValueGot=0;
		if(fIsDischarging&&!Get_IIC_Int_Stts){
			fIsDischarging=0;
		}
		if(!fIsCharging&&!fIsDischarging){
			if(vCurrentBattLv>Max_Batt_level)
				vCurrentBattLv=Max_Batt_level;
			temp=vCurrentBattLv;
			setBattLevel(vBattVol);
			if(vCurrentBattLv!=temp){
				fIsNeedUpdateBattLv=TRUE;
			}
		}
	}
}

void resetBrakeBattDet(void){
	vDisableGetBattTimer=DISABLE_BATT_ADC_TIMER_CONST;
}
void	brakeBattDetProcess(void){
	if(vDisableGetBattTimer)
		vDisableGetBattTimer--;
}
//void adjustColor(void) {
//	if (++vtIndex >= 12)
//		vtIndex = 0;
//	if (vtIndex % 6 == 0) {
//		if (++pUserColor->hues >= 360){
//		testLEDFunc();
//
//			pUserColor->hues = 0;
//		}
//		hsb2Rgb(pUserColor);
//		updateColor(pUserColor);
//	}
//}
void TestColorTemp(void){
	if(displayParams.colorTemperature++>=MAX_ColorTemp)
		displayParams.colorTemperature=MIN_ColorTemp;
	updateColorTemp(&displayParams);
}

/***************************************************************
 *
 * 清除关机充电状态,拔出从充电头后,延时熄灭防止闪烁的计时状态
 *
***************************************************************/
void clrPowerTimerOutStts(void){
	vTimeToTurnOffLCD=0;
	fIsWakeupByCharge=DoNothing;
}

void fucnPer45ms(void) {
	holdingKeyFunc();
}

void fucnPer50ms(void) {
	brakeBattDetProcess();
//	if(fIsSystemOff){
	if(!GET_POWER_KEY_PIN_STTS||Get_IIC_Int_Stts||fIsCharging){				//关机模式按键和充放电阻止进入睡眠
		resetEntrySleepTime();
	}
	if(!fIsSystemOff){
		if(readLEDPowerStts)									//灯光开启时,阻止休眠
			resetEntrySleepTime();
		if(displayParams.DisplayModeIndex>=ModeTDisplay)		//非初始画面都不休眠
			resetEntrySleepTime();
	}
	if (vTimeToSleep&&!--vTimeToSleep){
		if(fIsSystemOff){
			if(vTimeToTurnOffLCD){
//				vTimeToTurnOffLCD=0;
//				fIsWakeupByCharge=DoNothing;
				clrPowerTimerOutStts();
				lcdEnterSleep();
			}
			clrPowerKeyPressed;
			vPowerKeyTimer=0;
			vIsPowerKeyGod&=~(POWER_KEY_SHOT_PRRESS_GOD|POWER_KEY_LONG_PRRESS_GOD);
			Enter_DeepSleep();
		}
		else{
			systemPowerDown();
		}
	}
	if(fIsSystemOff){
		if(fIsWakeupByCharge==TimeToPowerOff){
			if(vTimeToTurnOffLCD&&!--vTimeToTurnOffLCD){
				fIsWakeupByCharge=DoNothing;
				lcdEnterSleep();
			}
		}
	}

}

void adcProcessFunc(void){
		if(AdcTimeToAvdd==vAdcProcessIndex){
				getSingleChannelADC(AdcVref1P2Input);
		}
		else if(vAdcProcessIndex==AdcTimeToBatt){
			if(!vDisableGetBattTimer){
				getSingleChannelADC(Batt_Det_Chl);
			}
		}else if(vAdcProcessIndex<=ADC_FINAL_TEMP_INDEX){
			getTemperatureADC();
		}
		if(vAdcProcessIndex>=ADC_PERIOD_MAX_INDEX)
			vAdcProcessIndex=0;
		else
			vAdcProcessIndex++;
}


void fucnPer100ms(void) {
	adcProcessFunc();
}
void fucnPer500ms(void) {
	updateHotDisplay();
//	if(!fIsTurnOffByException&&vExceptionLEDCnt&&--vExceptionLEDCnt==0){
//		turnOffRedLED();
//	}

//	if (fIsCharging) {
//		if (fIsBattFully == 1) {
//			batterDisplay(Max_Batt_level);
//		} else {
//			if (vtTestIndex > Max_Batt_level){
//				if(vCurrentBattLv>1)
//					vtTestIndex = vCurrentBattLv-1;
//				else
//					vtTestIndex = 0;
//			}
//			batterDisplay(vtTestIndex++);
//		}
//	}

	if(TRUE==fIsNeedUpdateBattLv){
		fIsNeedUpdateBattLv=FALSE;
		batterDisplay(vCurrentBattLv);
	}
	//TestColorTemp();
}
void fucnPer1s(void) {
//	userI2cInit();
	if(fIsI2CInitOK&&Get_IIC_Int_Stts){
//	if(fIsI2CInitOK&&(Get_IIC_Int_Stts||(!fIsSystemOff&&!readLEDPowerStts))){
			if(vSecondCnt%2==0)
			readVolt();
	}
	if (fIsCharging&&!fIsHotNow) {
		if (fIsBattFully == 1) {
			batterDisplay(Max_Batt_level);
		} else {
			batterDisplay(vCurrentBattLv);
		}
	}

	//getSingleChannelADC(AdcAVccDiV3Input);
	if (CountDownDisplay == displayParams.DisplayModeIndex) {

		if (displayParams.countDownTimer != 0) {
			displayParams.countDownTimer--;
			updateCountDownDisplay(&displayParams);


		}
		if (!displayParams.countDownTimer) {
			displayParams.DisplayModeIndex = ModeTDisplay;
			displaySystemMenu(&displayParams);
			LEDPowerOff;
			resetBrakeBattDet();
			turnOffColorTempLamp();
		}
	}

}

/**************************************************************
 *
 *从高温模式值 获取常温模式PWM的值
 *
 ***************************************************************/
u16 getOriginalPWMData(u16 curData){
	u16	vtTemp16=0;
	vtTemp16=curData*TEMPERATURE_COE_NORMAL/TEMPERATURE_COE_LOW;
	return	((vtTemp16>PWM_FRQ_CONST)?PWM_FRQ_CONST:vtTemp16);
//	if(vtTemp16>PWM_FRQ_CONST)
//		return	PWM_FRQ_CONST;
//	else
//		return	vtTemp16;
}


/**********************************************************************************
 *
 *  ADC结果处理
 *
 ***********************************************************************************/
void adcResultProcess(void) {
	u8 vtMin = 0xff;
	u8 vtMax = 0;
	u8 vtTemp;
	u16 vResultACC = 0;
	u8 i = 0;

	checkBattFunc();
	if ((vAdcProcessIndex > AdcTimeToBatt) && (vAdcProcessIndex <= (ADC_FINAL_TEMP_INDEX + 1))) {
		if (fIsSystemTempGot) {
			aTemperatureArray[vAdcProcessIndex - (ADCTimeToTemp1+1)] = (u8)vSystemTemperature;
			if (vAdcProcessIndex == ADC_FINAL_TEMP_INDEX + 1) {
				for (; i < ADC_FINAL_TEMP_INDEX ; i++) {
					vtTemp = aTemperatureArray[i];
					if (vtTemp <= vtMin) {
						vtMin = vtTemp;
					}
					if (vtTemp >= vtMax) {
						vtMax = vtTemp;
					}
					vResultACC += vtTemp;
				}
//				vResultACC -= vtMin;		//去除一次最小值
//				vResultACC -= vtMax;		//去除一次最大值
//				vSystemTemperature = vResultACC / (ADC_FINAL_TEMP_INDEX-2);
				vSystemTemperature = vResultACC / (ADC_FINAL_TEMP_INDEX-ADCTimeToTemp1+1);
				//temperatureDisplay();
				if ((fIsSystemOff == FALSE)||(fIsSystemOff&&fIsCharging)) {
					if (vSystemTemperature >= OVER_TEMPERATURE_CONST_Lv1) {//温度大于60度关机
						systemOffByManual();
					} else if (vSystemTemperature >= OVER_TEMPERATURE_CONST_Lv0 && !fIsHotNow) {
						vTemperatureCoe = TEMPERATURE_COE_LOW;
						fIsHotNow = TRUE;
						if (readLEDPowerStts) {
							//memcpy(&pcaDataBackup,&pcaDataStruct,sizeof(pcaDataStruct));
							updateBrightnessByTemperature();
						}
					} else if (vSystemTemperature <= NORMAL_TEMPERATURE_CONST && fIsHotNow) {
						hotFuncDeinit();
						if (readLEDPowerStts) {
							pcaDataStruct.valueOfRed = getOriginalPWMData( pcaDataStruct.valueOfRed);
							pcaDataStruct.valueOfGreen = getOriginalPWMData( pcaDataStruct.valueOfGreen);
							pcaDataStruct.valueOfBlue = getOriginalPWMData( pcaDataStruct.valueOfBlue);
							pcaDataStruct.valueOfCw = getOriginalPWMData( pcaDataStruct.valueOfCw);
							pcaDataStruct.valueOfMw = getOriginalPWMData( pcaDataStruct.valueOfMw);
							//memcpy(&pcaDataStruct,&pcaDataBackup,sizeof(pcaDataStruct));
							updateBrightnessByTemperature();
						}
					}
				}
			}
			fIsSystemTempGot=0;
		}
	}
}
/*********************************************************************
 *
 * 色温模式调节色温时,缓慢变化,防止过快产生的闪烁感
 *
 **********************************************************************/
void colorTemperatureAdjustSlowFunc(void) {
	if (fIsAdjustSlowMode) {
		if (ColorTempSetting == displayParams.arrowIndex) {
			if (++vAdjustSlowIndex < (MAX_SLOW_ADJUST_STEPS + 1)) {
				user_Pca_CapData16Set(ModuleMw, oriPwmDataStruct.valueOfMw + vAdjustSlowMwStep * vAdjustSlowIndex);
				user_Pca_CapData16Set(ModuleCw, oriPwmDataStruct.valueOfCw + vAdjustSlowCwStep * vAdjustSlowIndex);
			} else {
				fIsAdjustSlowMode = 0;
				vAdjustSlowIndex = 0;
				user_Pca_CapData16Set(ModuleMw, targetPwmDataStruct.valueOfMw);
				user_Pca_CapData16Set(ModuleCw, targetPwmDataStruct.valueOfCw);
			}
		} else {
			fIsAdjustSlowMode = FALSE;
		}
	}
}
void fucnPer5ms(void ){
		if(fPowerOffByBattLow){
			fPowerOffByBattLow=0;
			if(!fIsSystemOff){
				systemOffByManual();
			}
		}

		if(Get_IIC_Int_Stts){
			if(!fIsI2CInitOK)
				userI2CInit();
		}
		lightEffectFunc();
		colorTemperatureAdjustSlowFunc();
//		checkBattFunc();
//		temperatureDisplay();
		adcResultProcess();
		powerDownModeChargeDisplay();
		if(!fIsSystemOff&&fIsLightActive&&readLEDPowerStts){
			if(fTimerToPowerOnLED){
				if(vPowerOnLEDCnt){
					if(!--vPowerOnLEDCnt){
							fTimerToPowerOnLED=0;
						if(displayParams.DisplayModeIndex == CountDownDisplay){
//							DDL_ZERO_STRUCT(vtDisplayParams);
//							vtDisplayParams.arrowIndex = ColorTempSetting;
//							vtDisplayParams.colorTemperature = MAX_ColorTemp;
//							vtDisplayParams.brightness = 100;
//							updateLEDStts(&vtDisplayParams);
							user_Pca_CapData16Set(ModuleCw,PWM_FRQ_CONST);
						}
					}else{
						if(vPowerOnLEDCnt&&vPowerOnLEDCnt<=(ON_LED_DELAY_CONST-2)){
							LED_Ctrl_On;
						}
						if(displayParams.DisplayModeIndex == CountDownDisplay){					//爆亮模式开启时,使用渐亮模式
//							DDL_ZERO_STRUCT(vtDisplayParams);
//							vtDisplayParams.arrowIndex = ColorTempSetting;
//							//vtDisplayParams.colorTemperature = MAX_ColorTemp;
//							vtDisplayParams.colorTemperature = (MAX_ColorTemp-MIN_ColorTemp)-vPowerOnLEDCnt;
//							vtDisplayParams.brightness = 100;
//							updateLEDStts(&vtDisplayParams);
							superFlashMode((MAX_ColorTemp-MIN_ColorTemp)-vPowerOnLEDCnt);
						}
					}
				}
//				if(vPowerOnLEDCnt&&!--vPowerOnLEDCnt){
//					fTimerToPowerOnLED=0;
//					DDL_ZERO_STRUCT(vtDisplayParams);
//					vtDisplayParams.arrowIndex = ColorTempSetting;
//					//vtDisplayParams.colorTemperature = MAX_ColorTemp;
//					vtDisplayParams.colorTemperature = 2;
//					vtDisplayParams.brightness = 100;
//					updateLEDStts(&vtDisplayParams);
//				}
//				if(vPowerOnLEDCnt&&vPowerOnLEDCnt<=(ON_LED_DELAY_CONST-2)){
//					LED_Ctrl_On;
//				}
			}
		}
		if(vSystemLIRC5ms%2==0){
			factoryModeCoolDownFunc();
		}
		if(fTime45ms==1){
					fTime45ms=0;
					fucnPer45ms();
		}
		if(fTime50ms==1){
					fTime50ms=0;
					fucnPer50ms();
		}
		if(fTime100ms==1){
					fTime100ms=0;
					fucnPer100ms();
		}
		if(fTime500ms==1){

					fTime500ms=0;
					fucnPer500ms();
		}
		if(fTime1s==1){
					fTime1s=0;
					vSystem1s+=1;
					fucnPer1s();
//					if(vSystem1s>=60){
//							RCC->REGLOCK = 0x55AA6699;
//							RCC->IOMUX =   0x5A690001;			//set swdio
//							RCC->REGLOCK = 0x55AA6698;
//					}
		}
}
void updateLEDStts(displayParamsStruct * disParams){
	if((disParams->arrowIndex==HuesSetting)||(disParams->arrowIndex==SaturationSetting)){
		pUserColor->hues=disParams->hues;
		pUserColor->saturation=(float)disParams->saturation/100;
//		pUserColor->brightness=(float)disParams->brightness/100;
//		pUserColor->brightness=((float)disParams->brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100;			//从9%起步
		pUserColor->brightness=((float)disParams->brightness)*(100-RGBLED_REAL_START_DUTY)/(100*100)+(float)RGBLED_REAL_START_DUTY/100;			//从9%起步
		hsb2Rgb(pUserColor);
		updateColor(pUserColor);
	}
	else if(ColorTempSetting==disParams->arrowIndex){
		if(!fIsAdjustSlowMode){
			updateColorTemp(disParams);
		}else{
			setTargeColorTempData(disParams,&targetPwmDataStruct);
			vAdjustSlowMwStep=((float)(targetPwmDataStruct.valueOfMw-pcaDataStruct.valueOfMw)/MAX_SLOW_ADJUST_STEPS);
			vAdjustSlowCwStep=((float)(targetPwmDataStruct.valueOfCw-pcaDataStruct.valueOfCw)/MAX_SLOW_ADJUST_STEPS);
		}
	}else if(disParams->arrowIndex>=Style1Setting&&disParams->arrowIndex<=Style3Setting){
		startLightEffect(disParams);
	}
}
/**********************************************************************************
 *
 * 系统关机
 *
 ***********************************************************************************/
void systemPowerDown(void){
	Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);
	fIsSystemOff=1;
	LEDPowerOff ;
	fIsLightActive=OFF;
	lcdEnterSleep();
	turnOffAllLightEffect();
	clrPowerKeyPressed;
	vPowerKeyTimer=0;
	vIsPowerKeyGod&=~(POWER_KEY_SHOT_PRRESS_GOD|POWER_KEY_LONG_PRRESS_GOD);
	vTimeToTurnOffLCD=0;
	fIsCharging=0;
	fIsDischarging=0;
	fIsBattFully=0;
	fIsChargeDisplaying=0;
	fIsWakeupByCharge=DoNothing;
	Enter_DeepSleep();
}



void checkRealEntryPowerDownMode(void){
//	if(Get_IIC_Int_Stts&&fIsCharging){
	if(Get_IIC_Int_Stts&&fIsCharging){
		Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);
		fIsLightActive=OFF;
		fIsSystemOff=1;
		LEDPowerOff ;
		turnOffAllLightEffect();
		clrPowerKeyPressed;
		vPowerKeyTimer=0;
		vIsPowerKeyGod&=~(POWER_KEY_SHOT_PRRESS_GOD|POWER_KEY_LONG_PRRESS_GOD);
		vTimeToTurnOffLCD=0;
		fIsChargeDisplaying=0;
		fIsWakeupByCharge=WakeupLCDOver;
		displayParams.DisplayModeIndex = ChargingAtPowerDown;
		vCurrentBattLv=0;	//用于解决电池放完电后,充电启动,会一直显示6格的问题
		readVolt();
		if(!fIsCharging)
			vTimeToTurnOffLCD=TRUE;
		displaySystemMenu(&displayParams);
		vChargePercent=0;
		resetEntrySleepTime();
	}
	else
		systemPowerDown();
}


void systemOffByManual(void){
	fIsSystemOff=TRUE;
	if(fIsCharging){
		Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);
		fIsLightActive=OFF;
		LEDPowerOff ;
		turnOffAllLightEffect();
		clrPowerKeyPressed;
		vPowerKeyTimer=0;
		vIsPowerKeyGod&=~(POWER_KEY_SHOT_PRRESS_GOD|POWER_KEY_LONG_PRRESS_GOD);
		vTimeToTurnOffLCD=0;
		fIsChargeDisplaying=0;
		fIsWakeupByCharge=WakeupLCDOver;
		displayParams.DisplayModeIndex = ChargingAtPowerDown;
		displaySystemMenu(&displayParams);
		vChargePercent=0;
	}
	else
		systemPowerDown();
}
void factoryModeCoolDownFunc(void) {
	if (FactoryMode == displayParams.DisplayModeIndex) {
		if (factoryParams.factoryTimerCnt && --factoryParams.factoryTimerCnt == 0) {
			switch (factoryParams.factoryIndex) {
			case WaitFuncKey:
				break;
			case RedLedTesting:
//				displayParams.brightness=20;

				factoryParams.factoryIndex++;
				factoryParams.factoryTimerCnt=FACTORY_TIME_2S;
				displayParams.hues=120;
				updateLEDStts(&displayParams);
				displayFactoryMode(Green_Current);
				break;
			case GreenLedTesting:
				factoryParams.factoryIndex++;
				factoryParams.factoryTimerCnt=FACTORY_TIME_2S;
				displayParams.hues=240;
				updateLEDStts(&displayParams);
				displayFactoryMode(Blue_Current);
				break;
			case BlueLedTesting:
				factoryParams.factoryIndex++;
				factoryParams.factoryTimerCnt=FACTORY_TIME_2S;
				displayFactoryMode(Cw_Current);
				factoryModeTemperatureDisplay();
				displayParams.colorTemperature=MAX_ColorTemp;
				displayParams.arrowIndex=ColorTempSetting;
				updateLEDStts(&displayParams);
				break;
			case CoolLedTesting:
				factoryParams.factoryIndex++;
				factoryParams.factoryTimerCnt=FACTORY_TIME_2S;
				displayParams.colorTemperature=MIN_ColorTemp;
				updateLEDStts(&displayParams);
				displayFactoryMode(Mw_Current);
				factoryModeTemperatureDisplay();
				break;
			case WarmLedTesting:
				factoryParams.factoryIndex++;
				factoryParams.factoryTimerCnt=0;
//				setOnOffData(ON_OFF_DATA_LENGTH,&displayParams,OFF);
				LEDPowerOff;
				displayFactoryMode(Press_Bdec_key);
				break;
			case WaitBrightnesssDecKey:
				break;
			case IdleModeTesting:
				factoryParams.factoryIndex++;
				factoryParams.factoryTimerCnt=2;
//				displayFactoryMode(System_Volt_Display);
				break;
			case	SystemVoltTesting:
				factoryParams.factoryIndex++;
				factoryParams.factoryTimerCnt=2;
//				displayFactoryMode(Press_Inc_key);
				break;
			case WaitIncKey:
				factoryParams.factoryTimerCnt=30;
				break;
			case SleepModeTesting:
				break;
			case VersionDisplay:
			default:
				factoryParams.factoryIndex++;
				displayFactoryMode(Press_Func_key);
				factoryParams.factoryTimerCnt=0;
				fIsCharging = FALSE;
				break;
			}
		}
		else{
				switch (factoryParams.factoryIndex) {
				case VersionDisplay:
					break;
				case WaitFuncKey:
					if (!GET_FUNC_KEY_PIN_STTS&&\
							GET_POWER_KEY_PIN_STTS && \
							GET_DEC_KEY_PIN_STTS &&\
							GET_INC_KEY_PIN_STTS &&\
							GET_BRIGHTNESS_DOWN_KEY_PIN_STTS &&\
							GET_BRIGHTNESS_UP_KEY_PIN_STTS){
						factoryParams.factoryIndex++;
						factoryParams.factoryTimerCnt=FACTORY_TIME_2S;
						displayFactoryMode(Red_Current);
						//------------------
						factoryModeVoltDisplay(vBattVol);
						//------------------
						displayParams.saturation=100;
						displayParams.brightness=100;
						displayParams.hues=0;
						displayParams.arrowIndex=HuesSetting;
						updateLEDStts(&displayParams);
						vIsKeyPressed=0;
						vKeyValue=0;
					}

					break;
				case BlueLedTesting:
					if(FACTORY_TIME_1S9==factoryParams.factoryTimerCnt){
						fIsSystemTempGot=0;
						fIsBattAdcValueGot=0;
						vAdcProcessIndex=AdcTimeToAvdd;
					}
					if(FACTORY_TIME_1S==factoryParams.factoryTimerCnt){
						factoryModeTemperatureDisplay();
					}
					break;
				case WaitBrightnesssDecKey:
						factoryParams.factoryIndex++;
						factoryParams.factoryTimerCnt=FACTORY_TIME_2S;
						displayFactoryMode(Standby_Current);
						vIsKeyPressed=0;
						vKeyValue=0;
					break;
				case SystemVoltTesting:
					break;
				case WaitIncKey:
						vSystem5ms=0;
						displayParams.DisplayModeIndex=IdleIamgeDisplay;
						factoryParams.factoryTimerCnt=0;
						factoryParams.factoryBrightIndex=0;
						factoryParams.factoryIncIndex=0;
						factoryParams.factoryIndex=VersionDisplay;
//						vSystem5ms=1800;
//						while(1);
						vIsKeyPressed=0;
						vKeyValue=0;
						powerOnUserDataInit();
						systemOffByManual();
					break;
				}

			if(vKeyValue){
				if((factoryParams.factoryIndex>=RedLedTesting)&&(factoryParams.factoryIndex<=WarmLedTesting)){
						//OLED_ShowNum
					if(INC_KEY_SHORT_PRESSED==vKeyValue){
							if(++factoryParams.factoryIncIndex>99)
								factoryParams.factoryIncIndex=99;
							if(factoryParams.factoryIncIndex<10)
								factoryParams.factoryIncIndex=10;
							OLED_ShowNum(8,2,factoryParams.factoryIncIndex);
					}
					if(DEC_KEY_SHORT_PRESSED==vKeyValue){
							if(factoryParams.factoryIncIndex--<=10)
								factoryParams.factoryIncIndex=10;
							OLED_ShowNum(8,2,factoryParams.factoryIncIndex);
					}
					if(BRIGHTNESS_UP_KEY_SHORT_PRESSED==vKeyValue){
							if(++factoryParams.factoryBrightIndex>99)
								factoryParams.factoryBrightIndex=99;
							if(factoryParams.factoryBrightIndex<10)
								factoryParams.factoryBrightIndex=10;
							OLED_ShowNum(104,2,factoryParams.factoryBrightIndex);
					}
					if(BRIGHTNESS_DOWN_KEY_SHORT_PRESSED==vKeyValue){
							if(factoryParams.factoryBrightIndex--<=10)
								factoryParams.factoryBrightIndex=10;
							OLED_ShowNum(104,2,factoryParams.factoryBrightIndex);
					}
					vKeyValue=0;
				}
				}
			}
	}
}

void MenuFunction(void) {
	if (vKeyValue == POWER_KEY_LONG_PRESSED) {
		fIsSystemOff ^= 0x01;
		if (fIsSystemOff) {			//turn off system
			systemOffByManual();
		} else {					//turn on system
			//lcdExitSleep();
			powerOnLcdInit();
			hotFuncDeinit();
			if(!GET_BRIGHTNESS_DOWN_KEY_PIN_STTS&&!GET_INC_KEY_PIN_STTS&&GET_DEC_KEY_PIN_STTS&&GET_BRIGHTNESS_UP_KEY_PIN_STTS){
				displayParams.DisplayModeIndex = FactoryMode;
				fIsLightActive = ON;
				versionDisplay();
				displayFactoryMode(0xff);
				fIsSystemTempGot=0;
				fIsBattAdcValueGot=0;
				vAdcProcessIndex=AdcTimeToAvdd;
				fIsSystemOff=FALSE;
				LEDPowerOn;
				factoryParams.factoryIndex=VersionDisplay;
				factoryParams.factoryTimerCnt=160;
				factoryParams.factoryBrightIndex=50;
				factoryParams.factoryIncIndex=50;
				return;
			}
			fPowerOffByBattLow=0;
			fIsLightActive = OFF;
			CLR_POWER_ON_LED;
			userPcaConfig();
			displayParams.DisplayModeIndex = IdleIamgeDisplay;
			displaySystemMenu(&displayParams);
		}
	}
	if (!fIsSystemOff) {
		if (vKeyValue == FUNC_KEY_SHORT_PRESSED) {
			if (IdleIamgeDisplay == displayParams.DisplayModeIndex) {
				if (displayParams.arrowIndex++ >= Max_Arrow_Index) {
					displayParams.arrowIndex = 0;
					turnOffAllLightEffect();
				}
				if (BrightnessSetting == displayParams.arrowIndex) displayParams.arrowIndex = ColorTempSetting;
				updateArrowDisplay(&displayParams);
				if (ON == fIsLightActive) {
					LEDPowerOn ;
					updateLEDStts(&displayParams);
				}
			} else if (ModeTDisplay == displayParams.DisplayModeIndex) {
				//fIsLightActive=OFF;
				LEDPowerOn ;
				displayParams.DisplayModeIndex = CountDownDisplay;
				displaySystemMenu(&displayParams);

				DDL_ZERO_STRUCT(vtDisplayParams);
				vtDisplayParams.arrowIndex = ColorTempSetting;
				//vtDisplayParams.colorTemperature = MAX_ColorTemp;
				vtDisplayParams.colorTemperature =(MAX_ColorTemp-MIN_ColorTemp)-ON_LED_DELAY_CONST+1;
				vtDisplayParams.brightness = 100;
				updateLEDStts(&vtDisplayParams);
			} else if (CountDownDisplay == displayParams.DisplayModeIndex) {
				//fIsLightActive=ON;
				LEDPowerOff ;
				resetBrakeBattDet();
				displayParams.DisplayModeIndex = ModeTDisplay;
				displaySystemMenu(&displayParams);
			}
		} else if (vKeyValue == POWER_KEY_SHORT_PRESSED) {
			if (IdleIamgeDisplay != displayParams.DisplayModeIndex) {
				displayParams.DisplayModeIndex = IdleIamgeDisplay;
				if (ON == fIsLightActive) {
					LEDPowerOn ;
					updateLEDStts(&displayParams);
				}
			} else {
				if (OFF == fIsLightActive) {
					fIsLightActive = ON;//如果在初始画面,灯光尚未开启,则开启灯光
					LEDPowerOn ;
					updateLEDStts(&displayParams);
				} else {
					displayParams.DisplayModeIndex = ModeTDisplay;
					LEDPowerOff ;
					resetBrakeBattDet();
					CLR_POWER_ON_LED;
					turnOffAllLightEffect();
				}
			}
			displaySystemMenu(&displayParams);
		} else if (vKeyValue == INC_KEY_SHORT_PRESSED) {
			if (IdleIamgeDisplay == displayParams.DisplayModeIndex) {
				if (HuesSetting == displayParams.arrowIndex) {//色调调节
					if (displayParams.hues++ >= 360)
						displayParams.hues = 0;
					updateHuesDisplay(&displayParams);
				} else if (SaturationSetting == displayParams.arrowIndex) {//饱和度调节
					if (displayParams.saturation < 100) {
						displayParams.saturation++;
						updateSaturationDisplay( &displayParams);
					}
				} else if (ColorTempSetting == displayParams.arrowIndex) {//色温调节
					if (displayParams.colorTemperature < MAX_ColorTemp) {
						displayParams.colorTemperature++;
						updateColorTempDisplay( &displayParams);
						fIsAdjustSlowMode=TRUE;
						oriPwmDataStruct.valueOfMw=pcaDataStruct.valueOfMw;
						oriPwmDataStruct.valueOfCw=pcaDataStruct.valueOfCw;
					}
				} else if (Style1Setting == displayParams.arrowIndex) {//灯效1 调节
					if (displayParams.style1Value++ >= MAX_LightEffect)
						displayParams.style1Value = 0;
					updateLightEffectDisplay( &displayParams);
				} else if (Style2Setting == displayParams.arrowIndex) {//灯效2 调节
					if (displayParams.style2Value++ >= MAX_LightEffect)
						displayParams.style2Value = 0;
					updateLightEffectDisplay( &displayParams);
				} else if (Style3Setting == displayParams.arrowIndex) {//灯效3 调节
					if (displayParams.style3Value++ >= MAX_LightEffect)
						displayParams.style3Value = 0;
					updateLightEffectDisplay( &displayParams);
				}
				if (ON == fIsLightActive) {
					LEDPowerOn ;
					updateLEDStts(&displayParams);
				}
			}
		} else if (vKeyValue == DEC_KEY_SHORT_PRESSED) {
			if (IdleIamgeDisplay == displayParams.DisplayModeIndex) {
				if (HuesSetting == displayParams.arrowIndex) {//色调调节
					if (displayParams.hues-- <= 0)
						displayParams.hues = 360;
					updateHuesDisplay(&displayParams);
				} else if (SaturationSetting == displayParams.arrowIndex) {//饱和度调节
					if (displayParams.saturation > 1) {
						displayParams.saturation--;
						updateSaturationDisplay( &displayParams);
					}
				} else if (ColorTempSetting == displayParams.arrowIndex) {//色温调节
					if (displayParams.colorTemperature > MIN_ColorTemp) {
						displayParams.colorTemperature--;
						updateColorTempDisplay( &displayParams);
						fIsAdjustSlowMode=TRUE;
						oriPwmDataStruct.valueOfMw=pcaDataStruct.valueOfMw;
						oriPwmDataStruct.valueOfCw=pcaDataStruct.valueOfCw;
					}
				} else if (Style1Setting == displayParams.arrowIndex) {//灯效1 调节
					if (displayParams.style1Value-- <= 0)
						displayParams.style1Value = MAX_LightEffect;
					updateLightEffectDisplay( &displayParams);
				} else if (Style2Setting == displayParams.arrowIndex) {//灯效2 调节
					if (displayParams.style2Value-- <= 0)
						displayParams.style2Value = MAX_LightEffect;
					updateLightEffectDisplay( &displayParams);
				} else if (Style3Setting == displayParams.arrowIndex) {//灯效3 调节
					if (displayParams.style3Value-- <= 0)
						displayParams.style3Value = MAX_LightEffect;
					updateLightEffectDisplay( &displayParams);
				}
				if (ON == fIsLightActive) {
					updateLEDStts(&displayParams);
				}
			}

		} else if (vKeyValue == BRIGHTNESS_UP_KEY_SHORT_PRESSED) {
			if (IdleIamgeDisplay == displayParams.DisplayModeIndex) {
				if (displayParams.brightness < 100) {
					displayParams.brightness++;
					updateBrightnessDisplay(&displayParams);
				}
				if (ON == fIsLightActive) {
					if (Style1Setting > displayParams.arrowIndex) {//光效模式不即时更新亮度
						updateLEDStts(&displayParams);
					} else {
						//if (!fIsGradientMode)
						//setLEDFunc(&LedStruct);
					}
				}

			}
		} else if (vKeyValue == BRIGHTNESS_DOWN_KEY_SHORT_PRESSED) {
			if (IdleIamgeDisplay == displayParams.DisplayModeIndex) {
				if (displayParams.brightness > 1) {
					displayParams.brightness--;
					updateBrightnessDisplay(&displayParams);
				}
				if (ON == fIsLightActive) {
					if (Style1Setting > displayParams.arrowIndex) {//光效模式不即时更新亮度
						updateLEDStts(&displayParams);
					} else {
						//if (!fIsGradientMode)
						//setLEDFunc(&LedStruct);
					}
				}
			}
		}
		if (!fIsSystemOff) {
			resetEntrySleepTime();
		}
	}
	vKeyValue = 0;
}



void battLowCheck(void) {

}
void	powerOnUserDataInit(void){
	displayParams.hues=DEFAULT_HUES;
	displayParams.DisplayModeIndex=IdleIamgeDisplay;
	displayParams.brightness=DEFAULT_BRIGHTNESS;
	displayParams.saturation=DEFAULT_SATURATION;
	displayParams.arrowIndex=DEFAULT_ARROR_INDEX;
	displayParams.colorTemperature=DEFAULT_COLOR_TEMP;
	displayParams.style1Value=DEFAULT_STYLE1_VALUE;
	displayParams.style2Value=DEFAULT_STYLE2_VALUE;
	displayParams.style3Value=DEFAULT_STYLE3_VALUE;
	displayParams.battIndex=6;
	displayParams.countDownTimer=MAX_CD_TIMER;
	vCurrentBattLv=6;
	initLEDStyleParams();
	fIsCharging=0;
	fIsBattAdcValueGot=0;
	hotFuncDeinit();
}

void setBattLevelBrakeRising(float vBattReal){
				if (vBattReal >= BATT_LV6_THESHOLD) {
					if (vCurrentBattLv >= Max_Batt_level)
						vCurrentBattLv = Max_Batt_level;
				} else if (vBattReal >= BATT_LV5_THESHOLD) {
					if (vCurrentBattLv >= 5)//非充电模式, 电池电量不允许往上升
						vCurrentBattLv = 5;
				} else if (vBattReal >= BATT_LV4_THESHOLD) {
					if (vCurrentBattLv >= 4)
						vCurrentBattLv = 4;
				} else if (vBattReal >= BATT_LV3_THESHOLD) {
					if (vCurrentBattLv >= 3)
						vCurrentBattLv = 3;
				} else if (vBattReal >= BATT_LV2_THESHOLD) {
					if (vCurrentBattLv >= 2)
						vCurrentBattLv = 2;
				} else if (vBattReal >= BATT_LV1_THESHOLD) {
					if (vCurrentBattLv >= 1)
						vCurrentBattLv = 1;
				} else if (vBattReal < BATT_LV1_THESHOLD) {
					vCurrentBattLv = 0;
				}
				if(vBattReal<BATT_LV0_THESHOLD){
					fPowerOffByBattLow=1;
				}
}

void setBattLevel(float vBattReal) {
	if (!fIsSystemInitialing && !vDisableGetBattTimer) {

		if (Get_IIC_Int_Stts) {		//充电IC处于工作状态,读取充电IC中的电池数据
			battVoltCompensation(&vBattReal);
			vTestCompBatt = vBattReal;
			if (fIsCharging && OFF == fIsLightActive) {
				if (vBattReal >= BATT_LV6_THESHOLD) {
					vCurrentBattLv = Max_Batt_level;
				} else if (vBattReal >= BATT_LV5_THESHOLD&&(vCurrentBattLv<=5)) {//充电状态,非LED点亮模式,不允许测量误差引起的电量等级下降
					vCurrentBattLv = 5;
				} else if (vBattReal >= BATT_LV4_THESHOLD&&(vCurrentBattLv<=4)) {
					vCurrentBattLv = 4;
				} else if (vBattReal >= BATT_LV3_THESHOLD&&(vCurrentBattLv<=3)) {
					vCurrentBattLv = 3;
				} else if (vBattReal >= BATT_LV2_THESHOLD&&(vCurrentBattLv<=2)) {
					vCurrentBattLv = 2;
				} else if (vBattReal >= BATT_LV1_THESHOLD&&(vCurrentBattLv<=1)) {
					vCurrentBattLv = 1;
				} else if (vBattReal < BATT_LV1_THESHOLD&&(vCurrentBattLv<=0)) {
					vCurrentBattLv = 0;
				}
			} else {
				if(fIsDischarging){
					setBattLevelBrakeRising(vBattReal);
				}else{
				if (vBattReal >= BATT_LV6_THESHOLD) {
					vCurrentBattLv = Max_Batt_level;
				} else if (vBattReal >= BATT_LV5_THESHOLD) {
					vCurrentBattLv = 5;
				} else if (vBattReal >= BATT_LV4_THESHOLD) {
					vCurrentBattLv = 4;
				} else if (vBattReal >= BATT_LV3_THESHOLD) {
					vCurrentBattLv = 3;
				} else if (vBattReal >= BATT_LV2_THESHOLD) {
					vCurrentBattLv = 2;
				} else if (vBattReal >= BATT_LV1_THESHOLD) {
					vCurrentBattLv = 1;
				} else if (vBattReal < BATT_LV1_THESHOLD) {
					vCurrentBattLv = 0;
				}
				if(vBattReal<BATT_LV0_THESHOLD){
					fPowerOffByBattLow=1;
				}
				}
			}
		} else {
			setBattLevelBrakeRising(vBattReal);
//			if (vBattReal >= BATT_LV6_THESHOLD) {
//				if (vCurrentBattLv >= Max_Batt_level)
//					vCurrentBattLv = Max_Batt_level;
//			} else if (vBattReal >= BATT_LV5_THESHOLD) {
//				if (vCurrentBattLv >= 5)//非充电模式, 电池电量不允许往上升
//					vCurrentBattLv = 5;
//			} else if (vBattReal >= BATT_LV4_THESHOLD) {
//				if (vCurrentBattLv >= 4)
//					vCurrentBattLv = 4;
//			} else if (vBattReal >= BATT_LV3_THESHOLD) {
//				if (vCurrentBattLv >= 3)
//					vCurrentBattLv = 3;
//			} else if (vBattReal >= BATT_LV2_THESHOLD) {
//				if (vCurrentBattLv >= 2)
//					vCurrentBattLv = 2;
//			} else if (vBattReal >= BATT_LV1_THESHOLD) {
//				if (vCurrentBattLv >= 1)
//					vCurrentBattLv = 1;
//			} else if (vBattReal < BATT_LV1_THESHOLD) {
//				vCurrentBattLv = 0;
//			}
//			if(vBattReal<BATT_LV0_THESHOLD){
//				fPowerOffByBattLow=1;
//			}
		}
		displayParams.battIndex = vCurrentBattLv;
	}
}


/***************************************************************************
 *
 *  	电压补偿.灯开启时的电压补偿
 *
 ***************************************************************************/
void chkLightEffectModeVoltCompensation(float *volt){
	float vtMin=5.0;
	int i=0;
	if(fIsLightEffectOn){
		if(++vLightEfftectVoltDectCnt>BATT_ARRAY_SIZE+2){
			vLightEfftectVoltDectCnt=0;
			for(;i<BATT_ARRAY_SIZE;i++){
				if(vtMin>battArray[i]){
					vtMin=battArray[i];
				}
			}
			adcVoltCompensation(&vtMin);
			*volt=vtMin;
		}else{
			fIsBattAdcValueGot=0;
		}
	}else{
		adcVoltCompensation(volt);
	}
}
/***************************************************************************
 *
 *  	电压补偿.灯开启时的电压补偿
 *
 ***************************************************************************/
void	battVoltCompensation(float *volt){
	float vtTotalPowerRating=0;
	float vtCurrent=0;
	if (readLEDPowerStts) {				//在灯开启的情况下
		if(fIsLightEffectOn){
			if ((Style1Setting == displayParams.arrowIndex)
					|| ((Style2Setting == displayParams.arrowIndex) && (displayParams.style2Value != 2))
					|| ((Style3Setting == displayParams.arrowIndex) && (displayParams.style2Value == 2))) {			//有暖光或白光

				vtCurrent=CW_POWER_RATING*(0.9/1.0)/(*volt);
				//*volt += vtCurrent * DISCHARGE_RESISTENCE;
			}else{
				vtCurrent=BLUE_POWER_RATING*(0.9/1.0)/(*volt);
				//*volt += vtCurrent * DISCHARGE_RESISTENCE;
			}
		} else {
			if (pcaDataStruct.valueOfRed)
				vtTotalPowerRating += ((float) pcaDataStruct.valueOfRed / PWM_FRQ_CONST) * RED_POWER_RATING;
			if (pcaDataStruct.valueOfGreen)
				vtTotalPowerRating += ((float) pcaDataStruct.valueOfGreen / PWM_FRQ_CONST) * GREEN_POWER_RATING;
			if (pcaDataStruct.valueOfBlue)
				vtTotalPowerRating += ((float) pcaDataStruct.valueOfBlue / PWM_FRQ_CONST) * BLUE_POWER_RATING;
			if (pcaDataStruct.valueOfCw)
				vtTotalPowerRating += ((float) pcaDataStruct.valueOfCw / PWM_FRQ_CONST) * CW_POWER_RATING;
			if (pcaDataStruct.valueOfMw)
				vtTotalPowerRating += ((float) pcaDataStruct.valueOfMw / PWM_FRQ_CONST) * MW_POWER_RATING;
			vtCurrent = vtTotalPowerRating / (*volt);
			//*volt += vtCurrent * DISCHARGE_RESISTENCE;
		}
			*volt += vtCurrent * DISCHARGE_RESISTENCE;
//		if (IdleIamgeDisplay == displayParams.DisplayModeIndex) {
//			if (ColorTempSetting==displayParams.arrowIndex||Style1Setting==displayParams.arrowIndex
//					|| (displayParams.arrowIndex == Style2Setting && displayParams.style2Value == 2)
//					|| (displayParams.arrowIndex == Style3Setting && displayParams.style3Value != 0)) {//有暖光或白光情况
//				if (displayParams.brightness >= 95) {
//					*volt += 0.4;
//				} else if (displayParams.brightness >= 80) {
//					*volt += 0.3;
//				} else if (displayParams.brightness >= 54) {
//					*volt += 0.2;
//				} else if (displayParams.brightness >= 36) {
//						*volt += 0.1;
//				} else if (displayParams.brightness >= 18) {
//					*volt += 0.05;
//				}
//			} else {				//无暖光或白光模式
//				if (displayParams.brightness >= 98) {
//					*volt += 0.25;
//				} else if (displayParams.brightness >= 80) {
//					*volt += 0.20;
//				} else if (displayParams.brightness >= 53) {
//					*volt += 0.15;
//				} else if (displayParams.brightness >= 25) {
//					*volt += 0.10;
//				}
//			}
//		} else if (CountDownDisplay == displayParams.DisplayModeIndex) {
//			*volt += 0.3;
//		}
	}
}


/***************************************************************************
 *
 *  	ADC电压补偿.在灯开启时的电压补偿
 *
 ***************************************************************************/

void adcVoltCompensation(float *volt) {
	if (Get_IIC_Int_Stts)
		return;
	battVoltCompensation(volt);
//	if (ON == fIsLightActive) {				//在灯开启的情况下
//		if (IdleIamgeDisplay == displayParams.DisplayModeIndex) {
//			if (ColorTempSetting==displayParams.arrowIndex||Style1Setting==displayParams.arrowIndex
//					|| (displayParams.arrowIndex == Style2Setting && displayParams.style2Value == 2)
//					|| (displayParams.arrowIndex == Style3Setting && displayParams.style3Value != 0)) {//有暖光或白光情况
//				if (displayParams.brightness >= 95) {
//					*volt += 0.4;
//				} else if (displayParams.brightness >= 80) {
//					*volt += 0.3;
//				} else if (displayParams.brightness >= 54) {
//					*volt += 0.2;
//				} else if (displayParams.brightness >= 36) {
//					*volt += 0.1;
//				} else if (displayParams.brightness >= 18) {
//					*volt += 0.05;
//				}
//			} else {				//无暖光或白光模式
//				if (displayParams.brightness >= 98) {
//					*volt += 0.25;
//				} else if (displayParams.brightness >= 80) {
//					*volt += 0.20;
//				} else if (displayParams.brightness >= 53) {
//					*volt += 0.15;
//				} else if (displayParams.brightness >= 25) {
//					*volt += 0.10;
//				}
//			}
//		} else if (CountDownDisplay == displayParams.DisplayModeIndex) {
//			*volt += 0.3;
//		}
//	}
}


void powerDownModeChargeDisplay(void){
	if(fIsCharging&&fIsWakeupByCharge==SetWakeupLCD){
		//fIsNeedWakeupLCD=FALSE;
		fIsWakeupByCharge=WakeupLCDOver;
		lcdExitSleep();
		fIsLightActive = OFF;
		CLR_POWER_ON_LED;
		displayParams.DisplayModeIndex = ChargingAtPowerDown;
		displaySystemMenu(&displayParams);
		vChargePercent=0;
	}
}

void powerDownFromChageMode(void){
	if(fIsWakeupByCharge==WakeupLCDOver&&fIsSystemOff){
		fIsWakeupByCharge=TimeToPowerOff;
		vTimeToTurnOffLCD=POWER_DONW_LCD_TIMEOUT;
		//systemPowerDown();
	}
}

void powerDownModeChargePercent(void){
	if(fIsSystemOff&&fIsCharging){
//		if(fIsBattFully==1){
//			vChargePercent=100;
//		}else if(vBattVol>=(4.30-CHARGE_VOLT_DIVIATION)&&vChargePercent<=99&&vChargeCurrent>0.05&&vChargeCurrent<=0.50){
//			vChargePercent=99;
//		}else if(vBattVol>=(4.30-CHARGE_VOLT_DIVIATION)&&vChargePercent<=98&&vChargeCurrent>0.05&&vChargeCurrent<=0.65){
//			vChargePercent=98;
//		}else if(vBattVol>=(4.30-CHARGE_VOLT_DIVIATION)&&vChargePercent<=97&&vChargeCurrent>0.05&&vChargeCurrent<=0.80){
//			vChargePercent=97;
//		}else if(vBattVol>=(4.28-CHARGE_VOLT_DIVIATION)&&vChargePercent<=96&&vChargeCurrent>0.05&&vChargeCurrent<=0.95){
//			vChargePercent=96;
//		}else if(vBattVol>=(4.24-CHARGE_VOLT_DIVIATION)&&vChargePercent<=95&&vChargeCurrent>0.05&&vChargeCurrent<=1.05){
//			vChargePercent=95;
//		}else if(vBattVol>=(4.20-CHARGE_VOLT_DIVIATION)&&vChargePercent<=94&&vChargeCurrent>0.05&&vChargeCurrent<=1.20){
//			vChargePercent=94;
//		}else if(vBattVol>=(4.20-CHARGE_VOLT_DIVIATION)&&vChargePercent<=93&&vChargeCurrent>0.05&&vChargeCurrent<=1.30){
//			vChargePercent=93;
//		}else if(vBattVol>=(4.15-CHARGE_VOLT_DIVIATION)&&vChargePercent<=92&&vChargeCurrent>0.05&&vChargeCurrent<=1.38){
//			vChargePercent=92;
//		}else if(vBattVol>=(4.14-CHARGE_VOLT_DIVIATION)&&vChargePercent<=91&&vChargeCurrent>0.05&&vChargeCurrent<=1.45){
//			vChargePercent=91;
//		}else if(vBattVol>=(BATT_LV6_THESHOLD+0.07-CHARGE_VOLT_DIVIATION)&&vChargePercent<=90){
//			vChargePercent=90;
//		}else if(vBattVol>=(BATT_LV6_THESHOLD+0.05-CHARGE_VOLT_DIVIATION)&&vChargePercent<=89){
//			vChargePercent=89;
//		}else if(vBattVol>=(BATT_LV6_THESHOLD+0.03-CHARGE_VOLT_DIVIATION)&&vChargePercent<=88){
//			vChargePercent=88;
//		}else if(vBattVol>=(BATT_LV6_THESHOLD+0.01-CHARGE_VOLT_DIVIATION)&&vChargePercent<=87){
//			vChargePercent=87;
//		}else if(vBattVol>=(BATT_LV6_THESHOLD-CHARGE_VOLT_DIVIATION)&&vChargePercent<=86){
//			vChargePercent=86;
//		}
		if(fIsBattFully==1){
			vChargePercent=100;
		}else if(vBattVol>=(4.21)&&vChargePercent<=99&&vChargeCurrent>0.05&&vChargeCurrent<=0.50){
			vChargePercent=99;
		}else if(vBattVol>=(4.21)&&vChargePercent<=98&&vChargeCurrent>0.05&&vChargeCurrent<=0.65){
			vChargePercent=98;
		}else if(vBattVol>=(4.21)&&vChargePercent<=97&&vChargeCurrent>0.05&&vChargeCurrent<=0.80){
			vChargePercent=97;
		}else if(vBattVol>=(4.21)&&vChargePercent<=96&&vChargeCurrent>0.05&&vChargeCurrent<=0.95){
			vChargePercent=96;
		}else if(vBattVol>=(4.21)&&vChargePercent<=95&&vChargeCurrent>0.05&&vChargeCurrent<=1.05){
			vChargePercent=95;
		}else if(vBattVol>=(4.20)&&vChargePercent<=94&&vChargeCurrent>0.05&&vChargeCurrent<=1.20){
			vChargePercent=94;
		}else if(vBattVol>=(4.20)&&vChargePercent<=93&&vChargeCurrent>0.05&&vChargeCurrent<=1.30){
			vChargePercent=93;
		}else if(vBattVol>=(4.17)&&vChargePercent<=92&&vChargeCurrent>0.05&&vChargeCurrent<=1.38){
			vChargePercent=92;
		}else if(vBattVol>=(4.15)&&vChargePercent<=91&&vChargeCurrent>0.05&&vChargeCurrent<=1.45){
			vChargePercent=91;
		}else if(vBattVol>=4.15&&vChargePercent<=90){
			vChargePercent=90;
		}else if(vBattVol>=(BATT_LV6_THESHOLD+0.06)&&vChargePercent<=89){
			vChargePercent=89;
		}else if(vBattVol>=(BATT_LV6_THESHOLD+0.04)&&vChargePercent<=88){
			vChargePercent=88;
		}else if(vBattVol>=(BATT_LV6_THESHOLD+0.02)&&vChargePercent<=87){
			vChargePercent=87;
		}else if(vBattVol>=(BATT_LV6_THESHOLD)&&vChargePercent<=86){
			vChargePercent=86;
		}
	}
	batteryPercentDisplay(vChargePercent);
}

void hotFuncDeinit(void){
//	DDL_ZERO_STRUCT(pcaDataBackup);
	vTemperatureCoe=TEMPERATURE_COE_NORMAL;				//由温度引起的亮度变化系数			>60时 vTemperatureCoe=0.8  <50度时恢复1.0
	fIsHotNow=FALSE;
	fIsHotDisplayOn=FALSE;
	if(!fIsSystemInitialing){
		batterDisplay(vCurrentBattLv);
	}
}
