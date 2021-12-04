/*
 * user_adc.c
 *
 *  Created on: 2019年9月28日
 *      Author: Bough_004
 */
#include "user_adc.h"
#include "user_menu.h"
#include "io_define.h"
#include "gpio.h"
#include "bgr.h"

uint16_t u16AdcRestult0;
uint16_t u16AdcRestult2;
uint16_t u16AdcRestult5;
uint16_t u16AdcRestultTemp;
static stc_adc_irq_t stcAdcIrqFlag;
volatile uint32_t vSystemTemperature=0;
float vBattVol;
float battArray[BATT_ARRAY_SIZE]={0};
float vAdVoltOfBatt=0;							//仅用于开发时电压显示
float vTestCompBatt=0;						//仅用于开发时电压显示
u8	battIndex=0;
///*****************************************************************************
// * Function implementation - global ('extern') and local ('static')
// ******************************************************************************/
//
//void AdcContIrqCallback(void)
//{
//    Adc_GetSglResult(&u16AdcRestult0);
//
//    u32Temp = (uint32_t)(25 + 0.0803*2.5*(u16AdcRestult0 - (*((volatile uint16_t*)(0x00100C36)))));
//
//    stcAdcIrqFlag.bAdcIrq = TRUE;
//    Adc_Disable();
//}
/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

void AdcContIrqCallback(void)
{
	    Adc_GetSglResult(&u16AdcRestult0);
//	    vSystemTemperature = (uint32_t)(25 + 0.0803*2.5*(u16AdcRestult0 - (*((volatile uint16_t*)(0x00100C36)))));			//Vref=2.5
	    vSystemTemperature = (uint32_t)(25 + 0.0803*1.5*(u16AdcRestult0 - (*((volatile uint16_t*)(0x00100C34)))));			//Vref=1.5
	    stcAdcIrqFlag.bAdcIrq = TRUE;
	    fIsSingleChlAdcConverting&=~TEMP_CONVERTING;
	    fIsSystemTempGot=1;
	    Adc_Disable();
}

void AdcSingleContIrqCallback(void) {
	float vtBattVolTemp=0;
	Adc_GetSglResult(&u16AdcRestult0);
	if (fIsSingleChlAdcConverting & VOL1V2_CONVERTING) {
		fIsSystemAvddGot = 1;
		vSystemVdd=1.2*ADC_LEVEL_CONST/u16AdcRestult0;
		vSystemVdd-=0.05;							//实测误差
		fIsSingleChlAdcConverting &= ~VOL1V2_CONVERTING;
	} else if (fIsSingleChlAdcConverting & BATT_CONVERTING) {
		vtBattVolTemp = (vSystemVdd * u16AdcRestult0) / ADC_LEVEL_CONST;
//		vBattVol = (2.5 * u16AdcRestult0) / ADC_LEVEL_CONST;
		vtBattVolTemp = vtBattVolTemp * 3 / 2;
		vAdVoltOfBatt = vtBattVolTemp;
		fIsBattAdcValueGot = 1;
		chkLightEffectModeVoltCompensation(&vtBattVolTemp);
		vTestCompBatt = vtBattVolTemp;
		battArray[battIndex] = vtBattVolTemp;
		if(!fIsSystemOff)
			vBattVol=vtBattVolTemp;
		if (++battIndex >= BATT_ARRAY_SIZE)
			battIndex = 0;
		fIsSingleChlAdcConverting &= ~BATT_CONVERTING;
	}
	stcAdcIrqFlag.bAdcIrq = TRUE;
	Adc_Disable();
}

/******************************************************************************
 * 1) PERI_CLKEN.ADC 为 1 时，才可以操作此寄存器。
 * 2) BGR 使能 20us 后，才能输出稳定的高精度参考电压。BGR 稳定后才可以被其它
 * 		模块使用，故用户操作中应加入等待 BGR 稳定的步骤。
 * 3) 当使用 ADC,OPA 时，必须使能 BGR。
 * 4) 当使用 VC 时，需根据 VC 寄存器的配置决定是否使能 BGR。
 *******************************************************************************/
u8 adcInterVrefEnable(void){
	    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE))			//开启ADC模块时钟
	    {
	        return 1;
	    }
	    Adc_Enable();
	    M0P_BGR->CR_f.BGR_EN = 0x1u;                 		//BGR必须使能
	    M0P_BGR->CR_f.TS_EN = 0x1u;					//内置温度传感器使能控制
	    delay100us(1);
	    Adc_Disable();
	    return	0;
}

/****************************************************************************************************************
 *  转换指定通道的ADC结果并返回,
 *
 * 实参取值范围见  IS_ADC_CHANNEL_CMD
 *****************************************************************************************************************/
u16 getTemperatureADC(void){

	    //uint32_t                   u32AdcResultAcc;
	    stc_adc_cfg_t              stcAdcCfg;
	    stc_adc_irq_t              stcAdcIrq;
	    stc_adc_irq_calbakfn_pt_t  stcAdcIrqCalbaks;


	    DDL_ZERO_STRUCT(stcAdcCfg);
	    DDL_ZERO_STRUCT(stcAdcIrq);
	    DDL_ZERO_STRUCT(stcAdcIrqCalbaks);
	    DDL_ZERO_STRUCT(stcAdcIrqFlag);

	    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE))
	    {
	        return 1;
	    }

	    if(fIsSingleChlAdcConverting)
		    return 1;				//
	    else
		    fIsSingleChlAdcConverting|=TEMP_CONVERTING;

	    //ADC配置
	    Adc_Enable();
	    M0P_BGR->CR_f.BGR_EN = 0x1u;                 //BGR必须使能
	    M0P_BGR->CR_f.TS_EN = 0x1u;
	    delay100us(1);

	    stcAdcCfg.enAdcOpMode = AdcSglMode;         //单次转换模式
	    stcAdcCfg.enAdcClkDiv = AdcClkSysTDiv1;
	    stcAdcCfg.enAdcSampTimeSel = AdcSampTime8Clk;
	    //stcAdcCfg.enAdcRefVolSel   = RefVolSelInBgr2p5;
	    stcAdcCfg.enAdcRefVolSel   = RefVolSelInBgr1p5;
	    stcAdcCfg.bAdcInBufEn      = TRUE;

	    Adc_Init(&stcAdcCfg);
	    //Adc_SetVref(RefVolSelInBgr2p5);     //参考电压设置
	    Adc_SetVref(RefVolSelInBgr1p5);     //参考电压设置

	    Adc_ConfigSglMode(&stcAdcCfg);

	    Adc_ConfigSglChannel(AdcAiTsInput);

	    Adc_EnableIrq();
	    Adc_ClrSglIrqState();
	    EnableNvic(ADC_IRQn, IrqLevel3, TRUE);

	    stcAdcIrq.bAdcIrq = TRUE;
	    stcAdcIrqCalbaks.pfnAdcIrq = AdcContIrqCallback;

	    Adc_ConfigIrq(&stcAdcIrq, &stcAdcIrqCalbaks);

	    Adc_SGL_Start();
	    return	0;

}
/****************************************************************************************************************
 *  转换指定通道的ADC结果并返回,
 *
 * 实参取值范围见  IS_ADC_CHANNEL_CMD
 *****************************************************************************************************************/
u16 getMutilChlsADC(u32 channels) {

	    uint8_t                    u8AdcScanCnt;
	    stc_adc_cfg_t              stcAdcCfg;
	    stc_adc_irq_t              stcAdcIrq;
	    stc_adc_irq_calbakfn_pt_t  stcAdcIrqCalbaks;
	    stc_gpio_config_t          stcAdcAN0Port;
	    stc_gpio_config_t          stcAdcAN2Port;
	    stc_gpio_config_t          stcAdcAN5Port;


	    DDL_ZERO_STRUCT(stcAdcCfg);
	    DDL_ZERO_STRUCT(stcAdcIrq);
	    DDL_ZERO_STRUCT(stcAdcIrqCalbaks);
	    DDL_ZERO_STRUCT(stcAdcIrqFlag);
	    DDL_ZERO_STRUCT(stcAdcAN0Port);
	    DDL_ZERO_STRUCT(stcAdcAN2Port);
	    DDL_ZERO_STRUCT(stcAdcAN5Port);


	    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);

	    Gpio_SetAnalogMode(GpioPortA, GpioPin0);        //PA00 (AIN0)
	    stcAdcAN0Port.enDir = GpioDirIn;
	    Gpio_Init(GpioPortA, GpioPin0, &stcAdcAN0Port);


	    Gpio_SetAnalogMode(GpioPortA, GpioPin2);        //PA02 (AIN2)
	    stcAdcAN2Port.enDir = GpioDirIn;
	    Gpio_Init(GpioPortA, GpioPin2, &stcAdcAN2Port);



	    Gpio_SetAnalogMode(GpioPortA, GpioPin5);        //PA05 (AIN5)
	    stcAdcAN5Port.enDir = GpioDirIn;
	    Gpio_Init(GpioPortA, GpioPin5, &stcAdcAN5Port);

	    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE))
	    {
	        return 1;
	    }

	    //ADC配置
	    Adc_Enable();
	    M0P_BGR->CR_f.BGR_EN = 0x1u;                 //BGR必须使能
	    M0P_BGR->CR_f.TS_EN = 0x0u;
	    delay100us(1);

	    stcAdcCfg.enAdcOpMode = AdcSCanMode;         //扫描模式
	    stcAdcCfg.enAdcClkDiv = AdcClkSysTDiv8;
	    stcAdcCfg.enAdcSampTimeSel = AdcSampTime12Clk;
	    stcAdcCfg.enAdcRefVolSel   = RefVolSelInBgr1p5;
	    stcAdcCfg.bAdcInBufEn      =TRUE ;

	    Adc_Init(&stcAdcCfg);

	    Adc_ConfigSqrChannel(CH0MUX, AdcExInputCH5);
	    Adc_ConfigSqrChannel(CH1MUX, AdcAVccDiV3Input);
	    Adc_ConfigSqrChannel(CH2MUX, AdcVref1P2Input);
//	    Adc_ConfigSqrChannel(CH1MUX, AdcExInputCH2);
//	    Adc_ConfigSqrChannel(CH2MUX, AdcExInputCH5);

	    EnableNvic(ADC_IRQn, IrqLevel3, TRUE);

	    Adc_EnableIrq();

	    stcAdcIrq.bAdcSQRIrq = TRUE;
	    stcAdcIrqCalbaks.pfnAdcSQRIrq = AdcContIrqCallback;

	    Adc_ConfigIrq(&stcAdcIrq, &stcAdcIrqCalbaks);

	    u8AdcScanCnt = 3;

	    Adc_ConfigSqrMode(&stcAdcCfg, u8AdcScanCnt, FALSE);

	    Adc_SQR_Start();

//	    while(1)
//	    {
//	        while(FALSE == stcAdcIrqFlag.bAdcSQRIrq);
//
//	        stcAdcIrqFlag.bAdcSQRIrq = FALSE;
//
//	        Adc_SQR_Start();
//	    }
	    return 0;
}

void adcGPIOSet(en_adc_samp_ch_sel_t channel) {
	stc_gpio_config_t stcAdcAINPort;
	DDL_ZERO_STRUCT(stcAdcAINPort);
	switch (channel) {
	case AdcExInputCH2:
		Gpio_SetAnalogMode(GpioPortA, GpioPin2);        					//PA02 (AIN2)
		stcAdcAINPort.enDir = GpioDirIn;
		Gpio_Init(GpioPortA, GpioPin2, &stcAdcAINPort);
		break;
	case Batt_Det_Chl:
		Gpio_SetAnalogMode(BATT_DET_PORT, BATT_DET_PIN);        			//PA05 (AIN5)
		stcAdcAINPort.enDir = GpioDirIn;
		Gpio_Init(BATT_DET_PORT, BATT_DET_PIN, &stcAdcAINPort);
		break;
	}
}




/*************************************************************************
 *
 * 单通道ADC转换,请注意不要在前一次单次转换结果未出来之前调用
 *	即不能连续调用
 *
 *
*************************************************************************/

u8 getSingleChannelADC(en_adc_samp_ch_sel_t channel){

	    stc_adc_cfg_t              stcAdcCfg;
	    stc_adc_irq_t              stcAdcIrq;
	    stc_adc_irq_calbakfn_pt_t  stcAdcIrqCalbaks;
	    stc_gpio_config_t          stcAdcAN2Port;


	    DDL_ZERO_STRUCT(stcAdcCfg);
	    DDL_ZERO_STRUCT(stcAdcIrq);
	    DDL_ZERO_STRUCT(stcAdcIrqCalbaks);
	    DDL_ZERO_STRUCT(stcAdcIrqFlag);
	    DDL_ZERO_STRUCT(stcAdcAN2Port);

	    if(fIsSingleChlAdcConverting)
		    return 1;				//
	    else{
		    if(Batt_Det_Chl==channel)
			    fIsSingleChlAdcConverting|=BATT_CONVERTING;
		    else if(AdcVref1P2Input==channel){
			    fIsSystemAvddGot=0;
			    fIsSingleChlAdcConverting|=VOL1V2_CONVERTING;
		    }
	    }
	    if(channel>=30)
		    return	1;				//error

	    adcGPIOSet(channel);

	    //ADC配置
	    Adc_Enable();
//	    M0P_BGR->CR_f.BGR_EN = 0x1u;                 //BGR必须使能
//	    M0P_BGR->CR_f.TS_EN = 0x0u;
//	    delay100us(2);

	    stcAdcCfg.enAdcOpMode = AdcSglMode;         //单次转换模式
	    stcAdcCfg.enAdcClkDiv = AdcClkSysTDiv8;
	    stcAdcCfg.enAdcSampTimeSel = AdcSampTime12Clk;
	   stcAdcCfg.enAdcRefVolSel   = RefVolSelAVDD;
	   //stcAdcCfg.enAdcRefVolSel   = RefVolSelInBgr2p5;
	    stcAdcCfg.bAdcInBufEn      = TRUE;

	    Adc_Init(&stcAdcCfg);

	    Adc_ConfigSglMode(&stcAdcCfg);

	    Adc_ConfigSglChannel(channel);

	    EnableNvic(ADC_IRQn, IrqLevel3, TRUE);

	    Adc_EnableIrq();

	    stcAdcIrq.bAdcIrq = TRUE;
	    stcAdcIrqCalbaks.pfnAdcIrq = AdcSingleContIrqCallback;

	    Adc_ConfigIrq(&stcAdcIrq, &stcAdcIrqCalbaks);

	    Adc_SGL_Start();

	    return	0;
}
