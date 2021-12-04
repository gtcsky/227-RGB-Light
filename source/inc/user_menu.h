/*
 * user_menu.h
 *
 *  Created on: 2019��8��15��
 *      Author: Bough_004
 */

#ifndef USER_INC_USER_MENU_H_
#define USER_INC_USER_MENU_H_
#include "const.h"
extern	u8	fIsBattAdcValueGot;
extern	u8 	fIsSystemTempGot;
extern	u8	vSkipBattDectCnt;
extern	u8 	fIsSystemAvddGot;
extern	u8 	fIsSingleChlAdcConverting;
extern	u8 	fIsCharging;
extern	u8 	fIsDischarging;
extern	u8 	fIsBattFully;
extern	u8	vCurrentBattLv;
extern	u8 	fIsNeedUpdateBattLv;
extern	u8 	fIsWakeupByCharge;
extern	u8	fIsI2CInitOK;
extern	u8 	fIsSystemOff;				//	0/1  系统开启/关闭
extern	u8	fIsChargeDisplaying;		//
extern	u8 	fIsWakeupByCharge;
extern	u8	fIsNeedWakeupLCD;
extern	u8 	vSet16MHzPCLK;
extern	float vChargeCurrent;				//充电电流
extern	u8 fIsSystemInitialing;
extern	u8 vDisableGetBattTimer;
extern	float vTemperatureCoe;
extern	u8	vLightEfftectVoltDectCnt;
extern	u8	fIsHotNow;
extern	u8	fIsHotDisplayOn;			//hot 正在显示
extern	colorStructType  userColor;
extern	colorStructType  *pUserColor;
extern	float vSystemVdd;
extern	u8	fTimerToPowerOnLED;
extern	u8	vPowerOnLEDCnt;
extern	u8	fIsAdjustSlowMode;
extern	u8	fIsQuickCharging;
extern	u8	vBusVolt;
extern	displayParamsStruct 	displayParams;
extern	displayParamsStruct   *pDisplayParams;

extern	PcaDataStruct		pcaDataStruct;
//extern	PcaDataStruct		*pPcaDataStruct;
void factoryModeCoolDownFunc(void);
extern	void MenuFunction(void );
extern	void checkRealEntryPowerDownMode(void);
extern	void fucnPer5ms(void );
extern	void resetEntrySleepTime(void);
extern	void entryNormalModeChk(void);
extern	void powerOnIoInit(void);
extern	void	powerOnUserDataInit(void);\
extern	void setBattLevel(float vBatt);
extern	void adcVoltCompensation(float *volt);
extern	void	battVoltCompensation(float *volt);
extern	void chkLightEffectModeVoltCompensation(float *volt);
void 	softwareReset(void);
void 	testLEDFunc(void);
void 	updateLEDStts(displayParamsStruct * disParams);
extern	void checkBattFunc(void);
void 	systemPowerDown(void);
void 	systemOffByManual(void);
extern	void powerDownModeChargeDisplay(void);
extern	void powerDownFromChageMode(void);
extern	void powerDownModeChargePercent(void);
extern	void	clrPowerTimerOutStts(void);
extern	void hotFuncDeinit(void);
extern	void resetBrakeBattDet(void);
#endif /* USER_INC_USER_MENU_H_ */
