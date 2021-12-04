/*
 * user_lightEffect.h
 *
 *  Created on: 2019年10月23日
 *      Author: Bough_004
 */

#ifndef SOURCE_INC_USER_LIGHTEFFECT_H_
#define SOURCE_INC_USER_LIGHTEFFECT_H_
#include	"const.h"

extern	LEDParametersStruct	LedStruct;
extern	LEDParametersStruct	*pLedStruct;
extern	u8 	fIsLightEffectOn;		//是否开启光效模式
//extern	u8  	fIsGradientMode;		// 是否渐变模

extern	void  initLEDStyleParams(void);
extern	void lightEffectFunc(void);
extern	void turnOffAllLightEffect(void);
extern	void	 set2FlashMode1StyleA(void);
extern	void setLEDFunc(LEDParametersStruct * ledStruct);
extern	void startLightEffect(displayParamsStruct * disParams);
#endif /* SOURCE_INC_USER_LIGHTEFFECT_H_ */
