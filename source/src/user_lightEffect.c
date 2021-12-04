/*
 * user_lightEffect.c
 *
 *  Created on: 2019年10月23日
 *      Author: Bough_004
 */

#include "user_lightEffect.h"
#include "io_define.h"
#include "user_pca.h"
#include "user_color.h"
#include "user_lptimer.h"
#include "user_menu.h"
#include "user_key.h"
#include "user_exti.h"
#include "user_adc.h"


u8 	fIsLightEffectOn=0;		//是否开启光效模式
//u8  	ledStruct.fIsGradualMode=0;		// 是否渐变模式
u16	vLEDStepCtrlTimeCnt=0;
LEDParametersStruct	LedStruct;
LEDParametersStruct	*pLedStruct=&LedStruct;

u8  fIsRisingNow=0;
u8	vCandleIndex=0;


//SttsArray 参数
//0x00		所有灯灭
//0x01		R亮
//0x02		G亮
//0x04		B亮
//0x08		C亮
//0x10		W亮
#define	MAX_FLASH_MODE1_STYLE_A_INDEX	8
const u8 flashMode1StyleATimerArray[MAX_FLASH_MODE1_STYLE_A_INDEX+1]={15,20,15,120,60,20,14,200,200};
const u8 flashMode1StyleASttsArray[MAX_FLASH_MODE1_STYLE_A_INDEX+1]={0x08,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x00};

#define	MAX_FLASH_MODE1_STYLE_B_INDEX	14
const u8 flashMode1StyleBTimerArray[MAX_FLASH_MODE1_STYLE_B_INDEX+1]={20,20,15,10,   10,6,10,20,   40,20,15,20,    15,200,200};
const u8 flashMode1StyleBSttsArray[MAX_FLASH_MODE1_STYLE_B_INDEX+1]={0x08,0x00,0x08,0x00, 0x08,0x00,0x08,0x00,  0x08,0x00,0x08,0x00,  0x08,0x00,0x00};

#define	MAX_GRADUAL_EFFECT2_STYLE_B_INDEX  10
const u16 GradualEffectHuesArray[MAX_GRADUAL_EFFECT2_STYLE_B_INDEX+1]={0,240,30,270,60,300,90,345,120,330,180};
const u8	GradualEffect1StyleCTimeArray[1]={5};
const u8	GradualEffect2StyleATimeArray[1]={10};
const u8	GradualEffect2StyleBTimeArray[1]={2};
const u8	GradualEffect2StyleCTimeArray[1]={5};

//#define	GRADIENT_EFFECT1_STYLEC_STEP	10
//#define	GRADIENT_EFFECT2_STYLEA_STEP	10
//#define	GRADIENT_EFFECT2_STYLEB_STEP	2
//#define	GRADIENT_EFFECT2_STYLEC_STEP	10

#define	MAX_SIMULATION_MODE_STYLE_A_INDEX	19
const u8 simulationModeStyleATimerArray[MAX_SIMULATION_MODE_STYLE_A_INDEX+1]={20,20,20,20,   20,20,20,20,   20,20,20,20,    20,20,20,20,  20,20,20,20};
const u8 simulationModeStyleASttsArray[MAX_SIMULATION_MODE_STYLE_A_INDEX+1]={0x01,0x00,0x01,0x00,   0x01,0x00,0x01,0x00,   0x01,0x00,0x04,0x00,    0x04,0x00,0x04,0x00, 0x04,0x00,0x04,0x00};

#define	MAX_SIMULATION_MODE_STYLE_B_INDEX	11
const u8 simulationModeStyleBTimerArray[MAX_SIMULATION_MODE_STYLE_B_INDEX+1]={40,40,40,40,   40,40,40,40,   40,40,40,40};
//const u8 simulationModeStyleBSttsArray[MAX_SIMULATION_MODE_STYLE_B_INDEX+1]={0x04,0x00,0x04,0x00,   0x04,0x00,0x08,0x00,   0x08,0x00,0x08,0x00};
const u8 simulationModeStyleBSttsArray[MAX_SIMULATION_MODE_STYLE_B_INDEX+1]={0x04,0x00,0x04,0x00,   0x04,0x00,0x07,0x00,   0x07,0x00,0x07,0x00};

#define	MAX_SIMULATION_MODE_STYLE_C_INDEX	11
const u8 simulationModeStyleCTimerArray[MAX_SIMULATION_MODE_STYLE_C_INDEX+1]={40,50,40,50,   40,50,40,50,   40,50,40,50};
//const u8 simulationModeStyleCSttsArray[MAX_SIMULATION_MODE_STYLE_C_INDEX+1]={0x01,0x00,0x01,0x00,   0x01,0x00,0x08,0x00,   0x08,0x00,0x08,0x00};
const u8 simulationModeStyleCSttsArray[MAX_SIMULATION_MODE_STYLE_C_INDEX+1]={0x01,0x00,0x01,0x00,   0x01,0x00,0x07,0x00,   0x07,0x00,0x07,0x00};




//#define  GROSS_INDEX	8
//const	u8	lightEffectMaxIndexArray[GROSS_INDEX+1]={MAX_FLASH_MODE1_STYLE_A_INDEX,MAX_FLASH_MODE1_STYLE_B_INDEX,0X00,
//												0X00,0X00,0X00,
//												MAX_SIMULATION_MODE_STYLE_A_INDEX,MAX_SIMULATION_MODE_STYLE_B_INDEX,MAX_SIMULATION_MODE_STYLE_C_INDEX};

void  initLEDStyleParams(void){
	DDL_ZERO_STRUCT(LedStruct);
	DDL_ZERO_STRUCT(pcaDataStruct);
}


void setLEDFunc(LEDParametersStruct * ledStruct){
	UINT8 vtCurLED;
	vLEDStepCtrlTimeCnt=*(ledStruct->timeArray+ledStruct->currentIndex);
	vtCurLED=*(ledStruct->sttsArray+ledStruct->currentIndex);
	if (!vtCurLED){
		 turnOffColorTempLamp();
		 turnOffRGBLamp();
	}
	else{
		if(vtCurLED&0x01){
			user_Pca_CapData16Set(ModuleRed,PWM_FRQ_CONST*(((float)displayParams.brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100));
		}
		if(vtCurLED&0x02){
			user_Pca_CapData16Set(ModuleGreen,PWM_FRQ_CONST*(((float)displayParams.brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100));
		}
		if(vtCurLED&0x04){
			user_Pca_CapData16Set(ModuleBlue,PWM_FRQ_CONST*(((float)displayParams.brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100));
		}
		if(vtCurLED&0x08){
			user_Pca_CapData16Set(ModuleCw,PWM_FRQ_CONST*(((float)displayParams.brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100));
		}
		if(vtCurLED&0x10){
			user_Pca_CapData16Set(ModuleMw,PWM_FRQ_CONST*(((float)displayParams.brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100));
		}
	}
}


void lightEffectFunc(void) {
	if (fIsLightEffectOn) {
		if (!LedStruct.fIsGradualMode) {
			//resetEntrySleepTime();
			if (vLEDStepCtrlTimeCnt == 0) {	//Flash Lamp function executing
				if (LedStruct.currentIndex >= LedStruct.indicatorMaxIndex) {
					LedStruct.currentIndex = 0;
				} else {
					LedStruct.currentIndex++;
				}
				if (!LedStruct.fIsGradualMode)
					setLEDFunc(&LedStruct);
			} else {
				if (!LedStruct.fIsGradualMode)
					vLEDStepCtrlTimeCnt--;
			}
		} else {			//渐变模式
			if (vLEDStepCtrlTimeCnt == 0){
		        	LedStruct.pfnLedFuncCallBack();
			}else{
				vLEDStepCtrlTimeCnt--;
			}
		}
	}
}

void turnOffAllLightEffect(void) {
	fIsLightEffectOn = 0;
	vLightEfftectVoltDectCnt=0;
	vCandleIndex=0;
	fIsAdjustSlowMode=FALSE;
	turnOffColorTempLamp();
	turnOffRGBLamp();
}

void setFlashEffectPub(LEDParametersStruct * ledStruct){
	fIsLightEffectOn = 1;
	ledStruct->currentIndex=0;
	ledStruct->fIsGradualMode=0;
	setLEDFunc(ledStruct);
}

void setGradualEffectPub(LEDParametersStruct * ledStruct){
	fIsLightEffectOn =1;
	ledStruct->fIsGradualMode=1;
	ledStruct->sttsArray=NULL;		//不使用
	ledStruct->pfnLedFuncCallBack();
}

void	 set2FlashMode1StyleA(void){
	LedStruct.indicatorMaxIndex=MAX_FLASH_MODE1_STYLE_A_INDEX;
	LedStruct.indicatorStyle=0;					//flashMode1  styleA
	LedStruct.sttsArray=flashMode1StyleASttsArray;
	LedStruct.timeArray=flashMode1StyleATimerArray;
	setFlashEffectPub(&LedStruct);
}
void	 set2FlashMode1StyleB(void){
	LedStruct.indicatorMaxIndex=MAX_FLASH_MODE1_STYLE_B_INDEX;
	LedStruct.indicatorStyle=1;					//flashMode1  styleB
	LedStruct.sttsArray=flashMode1StyleBSttsArray;
	LedStruct.timeArray=flashMode1StyleBTimerArray;
	setFlashEffectPub(&LedStruct);
}

///******************************************************************************
// *
// * 	RGB色环轮转
// *
// *******************************************************************************/
//void funcGradientRGB(void){
//	if (++pUserColor->hues >= 360){
//		pUserColor->hues = 0;
//	}
//	pUserColor->saturation=(float)displayParams.saturation/100;
//	pUserColor->brightness=(float)displayParams.brightness/100;
//	hsb2Rgb(pUserColor);
//	updateColor(pUserColor);
//}


void	 flashMode1StyleCProess(void){
	vLEDStepCtrlTimeCnt = *(LedStruct.timeArray);
	if (fIsRisingNow) {
		if (++LedStruct.currentIndex >=LedStruct.indicatorMaxIndex)
			fIsRisingNow=0;
	}else{
		if(LedStruct.currentIndex ==0)
			fIsRisingNow=1;
		else
			LedStruct.currentIndex--;
	}
}
///****************************************************************************
// *
// * 灯光先降后升效果
// *
// *****************************************************************************/
//void FallingAndRising(u8* vtIndex, u8 lowThreshold, u8 hiThreshold) {
//	if (!fIsRisingNow) {
//		if (--LedStruct.currentIndex <= lowThreshold) {
//			fIsRisingNow = 1;
//		}
//	} else {
//		if (++LedStruct.currentIndex >= hiThreshold) {
//			fIsRisingNow = 0;
//			*vtIndex += 1;
//		}
//	}
//}
/****************************************************************************
 *
 * 灯光先降后升效果
 *
 *****************************************************************************/
void FallingAndRising(u8* vtIndex, u8 lowThreshold, u8 hiThreshold, u16 fallingStep5ms, u16 risingStep5ms) {
	if (!fIsRisingNow) {
		if (NULL != fallingStep5ms) {
			vLEDStepCtrlTimeCnt = fallingStep5ms;
		}
		if ((!LedStruct.currentIndex)||(--LedStruct.currentIndex <= lowThreshold)) {
			fIsRisingNow = TRUE;
		}
	} else {
		if (NULL != risingStep5ms) {
			vLEDStepCtrlTimeCnt = risingStep5ms;
		}
		if (++LedStruct.currentIndex >= hiThreshold) {
			fIsRisingNow = FALSE;
			*vtIndex += 1;
		}
	}
}
/****************************************************************************
 *
 * 灯光闪烁效果
 *
 *****************************************************************************/

void stepByStep(u8* vtIndex, u8 lowThreshold, u8 hiThreshold, u16 lowTime5ms, u16 hiTimes5ms) {
	if (!fIsRisingNow) {
		LedStruct.currentIndex = lowThreshold;
		vLEDStepCtrlTimeCnt = lowTime5ms;
		fIsRisingNow = TRUE;
	} else {
		LedStruct.currentIndex = hiThreshold;
		vLEDStepCtrlTimeCnt = hiTimes5ms;
		fIsRisingNow = FALSE;
		*vtIndex+=1;
	}
}

void keepStep(u8* vtIndex, u8 vtStep,u16 stepTime5ms){
	LedStruct.currentIndex = vtStep;
	vLEDStepCtrlTimeCnt = stepTime5ms;
	fIsRisingNow = FALSE;
	*vtIndex+=1;
}

void	 flashMode2StyleCProess(void){
	vLEDStepCtrlTimeCnt = *(LedStruct.timeArray);
	if(!vCandleIndex){
		if(fIsRisingNow){
			if (++LedStruct.currentIndex >=(LedStruct.indicatorMaxIndex-10)){
				fIsRisingNow=0;
				vCandleIndex=1;
			}
		}else{
			if(LedStruct.currentIndex ==0)
				fIsRisingNow=1;
			else
				LedStruct.currentIndex--;
		}
	}else if(vCandleIndex==1){
		FallingAndRising(&vCandleIndex,5,70,10,10);
		//vCandleIndex++;
	}else if(vCandleIndex==2){
		FallingAndRising(&vCandleIndex,5,40,1,1);
		//vCandleIndex++;
	}else if(vCandleIndex==3){
		keepStep(&vCandleIndex,38,200);
		//vCandleIndex++;
	}else if(vCandleIndex==4){
		FallingAndRising(&vCandleIndex,40,70,5,5);
		//vCandleIndex++;
	}else if(vCandleIndex==5){
		stepByStep(&vCandleIndex, 60,45, 100, 80);
		//vCandleIndex++;
	}else if(vCandleIndex==6){
		FallingAndRising(&vCandleIndex,2,20,5,10);
		//vCandleIndex++;
	}else if(vCandleIndex==7){
		keepStep(&vCandleIndex,0,2);
		//vCandleIndex++;
	}else if(vCandleIndex==8){
		FallingAndRising(&vCandleIndex,2,15,1,9);
		//vCandleIndex++;
	}else if(vCandleIndex==9){
		keepStep(&vCandleIndex,0,2);
		//vCandleIndex++;
	}else if(vCandleIndex==10){
		FallingAndRising(&vCandleIndex,2,20,1,10);
		//vCandleIndex++;
	}else if(vCandleIndex==11){
		FallingAndRising(&vCandleIndex,20,40,1,10);
		//vCandleIndex++;
	}else if(vCandleIndex==12){
		FallingAndRising(&vCandleIndex,40,60,1,5);
		//vCandleIndex++;
	}else if(vCandleIndex==13){
		keepStep(&vCandleIndex,50,600);
		//vCandleIndex++;
	}else if(vCandleIndex==14){
		FallingAndRising(&vCandleIndex,20,21,10,1);
		//vCandleIndex++;
	}else if(vCandleIndex==15){
		keepStep(&vCandleIndex,0,1);
		//vCandleIndex++;
	}else if(vCandleIndex==16){
		FallingAndRising(&vCandleIndex,20,50,1,10);
		//vCandleIndex++;
	}else if(vCandleIndex==17){
		FallingAndRising(&vCandleIndex,35,55,2,10);
		//vCandleIndex++;
	}else if(vCandleIndex==18){
		FallingAndRising(&vCandleIndex,2,85,5,5);
		//vCandleIndex++;
	}else if(vCandleIndex==19){
		FallingAndRising(&vCandleIndex,10,80,2,NULL);
		//vCandleIndex++;
	}else if(vCandleIndex==20){
		stepByStep(&vCandleIndex, 60, 45, 5, 100);
		//vCandleIndex++;
	}else if(vCandleIndex==21){
		FallingAndRising(&vCandleIndex, (LedStruct.indicatorMaxIndex/3), (LedStruct.indicatorMaxIndex-10),10,NULL);
		//vCandleIndex++;
	}else if(vCandleIndex==22){
		stepByStep(&vCandleIndex, 75, 45, 5, 100);
		//vCandleIndex++;
	}else if(vCandleIndex==23){
		FallingAndRising(&vCandleIndex,2,20,5,10);
		//vCandleIndex++;
	}else if(vCandleIndex==24){
		keepStep(&vCandleIndex,0,1);
		//vCandleIndex++;
	}else if(vCandleIndex==25){
		FallingAndRising(&vCandleIndex,2,25,1,10);
		//vCandleIndex++;
	}else if(vCandleIndex==26){
		FallingAndRising(&vCandleIndex,10,20,10,15);
	}else if(vCandleIndex==27){
		keepStep(&vCandleIndex,0,0);
	}else if(vCandleIndex==28){
		keepStep(&vCandleIndex,20,40);
	}else if(vCandleIndex==28){
		keepStep(&vCandleIndex,0,0);
	}else if(vCandleIndex==29){
		keepStep(&vCandleIndex,20,40);
	}else if(vCandleIndex==30){
		vCandleIndex++;
		//keepStep(&vCandleIndex,0,0);
	}else if(vCandleIndex==31){
		vCandleIndex++;
		//keepStep(&vCandleIndex,20,40);
	}else if(vCandleIndex==32){
		FallingAndRising(&vCandleIndex,10,60,1,10);
	}else if(vCandleIndex==33){
		if(!fIsRisingNow){
			if (--LedStruct.currentIndex <=1){
					fIsRisingNow=1;
					vCandleIndex=0;
			}
		}
	}

//	if (fIsRisingNow) {
//		if (++LedStruct.currentIndex >=LedStruct.indicatorMaxIndex)
//			fIsRisingNow=0;
//	}else{
//		if(LedStruct.currentIndex ==0)
//			fIsRisingNow=1;
//		else
//			LedStruct.currentIndex--;
//	}
}
/******************************************************************************
 *
 * 	光效模式1-C  渐变效果实现
 *
 *******************************************************************************/
void flashMode1StyleCfunc(void) {
	user_Pca_CapData16Set(ModuleCw, PWM_FRQ_CONST*(((float)displayParams.brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100) * ((float) (LedStruct.currentIndex + 10) / 100));
	flashMode1StyleCProess();
//	vLEDStepCtrlTimeCnt = *(LedStruct.timeArray);
//	if (fIsRisingNow) {
//		if (++LedStruct.currentIndex >=LedStruct.indicatorMaxIndex)
//			fIsRisingNow=0;
//	}else{
//		if(LedStruct.currentIndex ==0)
//			fIsRisingNow=1;
//		else
//			LedStruct.currentIndex--;
//	}
}
/******************************************************************************
 *
 * 	光效模式2-A  渐变效果实现
 *
 *******************************************************************************/
void	flashMode2StyleAfunc(void){
	if (++pUserColor->hues >= 360){
		pUserColor->hues = 0;
	}
	pUserColor->saturation=1.0;
//	pUserColor->brightness=(float)displayParams.brightness/100;
	pUserColor->brightness=((float)displayParams.brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100;
	hsb2Rgb(pUserColor);
	updateColor(pUserColor);
	vLEDStepCtrlTimeCnt = *(LedStruct.timeArray);
}
/******************************************************************************
 *
 * 	光效模式2-B  渐变效果实现
 *
 *******************************************************************************/
void	flashMode2StyleBfunc(void){
	u8 vtTemp1=0;
	float vtBrightnessCoe=0;
	if (++LedStruct.currentIndex > LedStruct.indicatorMaxIndex){
		LedStruct.currentIndex=0;
	}
	if(LedStruct.currentIndex==LedStruct.indicatorMaxIndex/2){
		if(++LedStruct.IndexOfHues>LedStruct.MaxHuesIndex)
			LedStruct.IndexOfHues=0;
	}
	pUserColor->hues=GradualEffectHuesArray[LedStruct.IndexOfHues];
	pUserColor->saturation=1.0;
	pUserColor->brightness=((float)displayParams.brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100;
	vtTemp1=LedStruct.currentIndex;
	if(LedStruct.currentIndex<=(LedStruct.indicatorMaxIndex/2)){
		vtBrightnessCoe=(float)(100-vtTemp1*3)/100;
	}else{
		vtBrightnessCoe=(float)(10+(vtTemp1-30)*3)/100;
	}
	pUserColor->brightness*=vtBrightnessCoe;
	hsb2Rgb(pUserColor);
	updateColor(pUserColor);
	vLEDStepCtrlTimeCnt = *(LedStruct.timeArray);
}
/******************************************************************************
 *
 * 	光效模式2-C  渐变效果实现
 *
 *******************************************************************************/
void	flashMode2StyleCfunc(void){
	flashMode2StyleCProess();
	user_Pca_CapData16Set(ModuleMw, PWM_FRQ_CONST*(((float)displayParams.brightness)*(100-LED_REAL_START_DUTY)/(100*100)+(float)LED_REAL_START_DUTY/100)  * ((float) (LedStruct.currentIndex + 10) / 100));
}

void	set2FlashMode1StyleC(void){
	LedStruct.indicatorStyle=2;					//flashMode1  styleC
	LedStruct.pfnLedFuncCallBack=flashMode1StyleCfunc;
	LedStruct.currentIndex=0;
	LedStruct.indicatorMaxIndex=90;
	LedStruct.timeArray=GradualEffect1StyleCTimeArray;
	fIsRisingNow=1;
	setGradualEffectPub(&LedStruct);
}
void	set2FlashMode2StyleA(void){
	LedStruct.indicatorStyle=4;					//flashMode2  styleA
	LedStruct.pfnLedFuncCallBack=flashMode2StyleAfunc;
	LedStruct.currentIndex=0;
	LedStruct.timeArray=GradualEffect2StyleATimeArray;
	LedStruct.HuesArray=GradualEffectHuesArray;
	fIsRisingNow=TRUE;
	setGradualEffectPub(&LedStruct);
}
void	set2FlashMode2StyleB(void){
	LedStruct.indicatorStyle=5;					//flashMode2  styleA
	LedStruct.pfnLedFuncCallBack=flashMode2StyleBfunc;
	LedStruct.currentIndex=0;
	LedStruct.indicatorMaxIndex=60;
	LedStruct.MaxHuesIndex=MAX_GRADUAL_EFFECT2_STYLE_B_INDEX;
	LedStruct.IndexOfHues=0;
	LedStruct.timeArray=GradualEffect2StyleBTimeArray;
	LedStruct.HuesArray=GradualEffectHuesArray;
	fIsRisingNow=TRUE;
	setGradualEffectPub(&LedStruct);
}
void	set2FlashMode2StyleC(void){
	LedStruct.indicatorStyle=5;					//flashMode2  styleC
	LedStruct.pfnLedFuncCallBack=flashMode2StyleCfunc;
	LedStruct.currentIndex=0;
	LedStruct.indicatorMaxIndex=90;
	LedStruct.timeArray=GradualEffect2StyleCTimeArray;
	setGradualEffectPub(&LedStruct);
}

void	 set2SimulatonModeStyleA(void){
	LedStruct.indicatorMaxIndex=MAX_SIMULATION_MODE_STYLE_A_INDEX;
	LedStruct.indicatorStyle=6;					//SimulatonMode  styleA
	LedStruct.sttsArray=simulationModeStyleASttsArray;
	LedStruct.timeArray=simulationModeStyleATimerArray;
	setFlashEffectPub(&LedStruct);
}
void	 set2SimulatonModeStyleB(void){
	LedStruct.indicatorMaxIndex=MAX_SIMULATION_MODE_STYLE_B_INDEX;
	LedStruct.indicatorStyle=7;					//SimulatonMode  styleC
	LedStruct.sttsArray=simulationModeStyleBSttsArray;
	LedStruct.timeArray=simulationModeStyleBTimerArray;
	setFlashEffectPub(&LedStruct);
}
void	 set2SimulatonModeStyleC(void){
	LedStruct.indicatorMaxIndex=MAX_SIMULATION_MODE_STYLE_C_INDEX;
	LedStruct.indicatorStyle=8;					//SimulatonMode  styleC
	LedStruct.sttsArray=simulationModeStyleCSttsArray;
	LedStruct.timeArray=simulationModeStyleCTimerArray;
	setFlashEffectPub(&LedStruct);
}

void startLightEffect(displayParamsStruct * disParams){
	turnOffAllLightEffect();
	if(Style1Setting==disParams->arrowIndex){
		if(disParams->style1Value==0)
			set2FlashMode1StyleA();
		else if(disParams->style1Value==1)
			set2FlashMode1StyleB();
		else if(disParams->style1Value==2)
			set2FlashMode1StyleC();
	}
	if(Style2Setting==disParams->arrowIndex){
		if(disParams->style2Value==0)
			set2FlashMode2StyleA();
		else if(disParams->style2Value==1)
			set2FlashMode2StyleB();
		else if(disParams->style2Value==2)
			set2FlashMode2StyleC();
	}
	if(Style3Setting==disParams->arrowIndex){
		if(disParams->style3Value==0)
			set2SimulatonModeStyleA();
		else if(disParams->style3Value==1)
			set2SimulatonModeStyleB();
		else if(disParams->style3Value==2)
			set2SimulatonModeStyleC();
	}
}
