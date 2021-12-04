/******************************************************************************
* Copyright (C) 2016, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd ("HDSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with HDSC
* components. This software is licensed by HDSC to be adapted only
* for use in systems utilizing HDSC components. HDSC shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. HDSC is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* Disclaimer:
* HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/******************************************************************************/
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2016-02-16  1.0  XYZ First version for Device Driver Library of Module.
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "lpm.h"
#include "gpio.h"
#include "user_pca.h"
#include "user_sysctrl.h"
#include "user_color.h"
#include "user_lptimer.h"
#include "user_menu.h"
#include "user_key.h"
#include "user_exti.h"
#include "user_adc.h"
#include "user_spi.h"
#include "user_lcd.h"
#include "user_i2c.h"
#include "user_lvr.h"
#include "user_watchdog.h"


/******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/



/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/

int32_t main(void)
 {
	stc_gpio_config_t stcGpioCfg;
	DDL_ZERO_STRUCT(stcGpioCfg);
	//Lvd_Disable();
//	delay1ms(1000);
	userSysctrl();
	fIsSystemInitialing=1;
	powerOnIoInit();
	EXTI_Key_Config();
//	stcGpioCfg.enDrv = GpioDrvH;
//	stcGpioCfg.enDir = GpioDirOut;
//
//	Gpio_Init(IIC_SCL_Port, IIC_SCL_Pin, &stcGpioCfg); //SCL
//	Gpio_Init(IIC_SDA_Port, IIC_SDA_Pin, &stcGpioCfg); //SDA
//
//	Gpio_WriteOutputIO(IIC_SCL_Port, IIC_SCL_Pin, 1);
//	Gpio_WriteOutputIO(IIC_SDA_Port, IIC_SDA_Pin, 1);
//	Gpio_WriteOutputIO(IIC_SCL_Port, IIC_SCL_Pin, 0);
//	Gpio_WriteOutputIO(IIC_SDA_Port, IIC_SDA_Pin, 0);
//	M0P_RESET->RESET_FLAG&=0xFFFFFFF0;


	powerOnUserDataInit();
	userPcaConfig();
	LPTIMER_Config();
	vSystem5ms=0;
	while(vSystem5ms<50);
	vSystem5ms=0;
	if(Get_IIC_Int_Stts){
		while(vSystem5ms<800);		//等待LDO稳定
//		while(vSystem5ms<400);		//等待LDO稳定
	}
	vSystem5ms=0;
									//如果上电时按键被按下,则开机
	getSingleChannelADC(AdcVref1P2Input);
	while(!fIsSystemAvddGot);
	getSingleChannelADC(Batt_Det_Chl);
	while(!fIsBattAdcValueGot);
	checkBattFunc();
	vSystem5ms=0;
//	userLvrFuncInit();
//	while(1);
//	vSystem5ms=0;
//	while(vSystem5ms<200);
//	vSystem5ms=0;
	fIsI2CInitOK=0;
	fIsCharging=0;

	if(Get_IIC_Int_Stts){
		userI2CInit();
		if(fIsI2CInitOK){
			if(!fIsCharging&&!fIsDischarging){
				fIsI2CInitOK=0;
			}
		}
	}
	vSystem5ms=0;
	powerOnLcdInit();
	fIsSystemInitialing=0;
	resetEntrySleepTime();
	fIsSystemOff=1;
	if(!Gpio_GetInputIO(KeyPower_Port, KeyPower_Pin))
		fIsSystemOff=0;
	if(fIsSystemOff)
		checkRealEntryPowerDownMode();
	while (1) {
		funcKeyTask();
		if(vKeyValue){
			if(FactoryMode !=displayParams.DisplayModeIndex ){
				MenuFunction();
			}
		}
//		if(fIsWakeUpByWKT){														//wake up from sleep mode per 52s. for check batt low stts
//				fIsWakeUpByWKT=0;
//				battLowCheck();
//				//vTimeToSleep=1;													//3*50ms=150ms
//				Enter_DeepSleep(AWK_COUNTERCLKDIV,AWK_VALUE);
//		}
//		motorStartupCheck();
		if (vSystem5ms != 0) {
			vSystem5ms--;
			fucnPer5ms();
		}
	}
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


