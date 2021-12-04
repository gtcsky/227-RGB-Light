/*
 * user_i2c.c
 *
 *  Created on: 2019年10月16日
 *      Author: Bough_004
 */
#include "io_define.h"
#include "user_i2c.h"
#include "user_menu.h"
#include "user_pca.h"
#include "user_lcd.h"
#include "user_adc.h"
#include "user_lptimer.h"
#include "sysctrl.h"

 //广播地址时，地址发送0x00，广播地址从机只接收，不回传
//uint8_t u8Senddata[10] = {0x12,0x34,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x99};
uint8_t u8Senddata[10] = {0xd1,0x11,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x99};
uint8_t u8Recdata[10]={0x00};
uint8_t u8SendLen=0;
uint8_t u8RecvLen=0;
uint8_t SendFlg = 0,Comm_flg = 0;
uint8_t u8recvflg=0;
uint8_t u8State = 0;
uint8_t u8Addr = 0x00;//地址字节

//uint8_t	vChargeIcSysStts=0;
//uint8_t	vChargeIcChgStts=0;
//主发送函数
en_result_t I2C_MasterWriteData(en_i2c_channel_t enCh,uint8_t u8Addr,uint8_t *pu8Data,uint32_t u32Len);
//主接收函数
en_result_t I2C_MasterReadData(en_i2c_channel_t enCh,uint8_t u8Addr,uint8_t *pu8Data,uint32_t u32Len);
/**
 ******************************************************************************
 ** \brief  主机接收函数
 **
 ** \param u8Addr从机内存地址，pu8Data读数据存放缓存，u32Len读数据长度
 **
 ** \retval 读数据是否成功
 **
 ******************************************************************************/
en_result_t I2C_MasterReadData(en_i2c_channel_t enCh,uint8_t u8Addr,uint8_t *pu8Data,uint32_t u32Len)
{
	    en_result_t enRet = Error;
	    uint8_t u8i=0,u8State;
	    uint16_t vtTimerOut=0x10000 - vSystemLIRC5ms;

	    I2C_SetFunc(enCh,I2cStart_En);

		while(((vtTimerOut+vSystemLIRC5ms)&0xffff)<400)
		{
			while(0 == I2C_GetIrq(enCh)&&(((vtTimerOut+vSystemLIRC5ms)&0xffff)<400))
				{}
			u8State = I2C_GetState(enCh);
			switch(u8State)
			{
				case 0x08:
					I2C_ClearFunc(enCh,I2cStart_En);
					I2C_WriteByte(enCh,I2C_SLAVEADDR);
					break;
				case 0x18:
					I2C_WriteByte(enCh,u8Addr);
					break;
				case 0x28:
					I2C_SetFunc(enCh,I2cStart_En);
					break;
				case 0x10:
					I2C_ClearFunc(enCh,I2cStart_En);
					I2C_WriteByte(enCh,I2C_SLAVEADDR|0x01);//从机地址发送OK
					break;
				case 0x40:
					if(u32Len>1)
					{
						I2C_SetFunc(enCh,I2cAck_En);
					}
					break;
				case 0x50:
					pu8Data[u8i++] = I2C_ReadByte(enCh);
					if(u8i==u32Len-1)
					{
						I2C_ClearFunc(enCh,I2cAck_En);
					}
					break;
				case 0x58:
					pu8Data[u8i++] = I2C_ReadByte(enCh);
					I2C_SetFunc(enCh,I2cStop_En);
					break;
				case 0x38:
					I2C_SetFunc(enCh,I2cStart_En);
					break;
				case 0x48:
					I2C_SetFunc(enCh,I2cStop_En);
					I2C_SetFunc(enCh,I2cStart_En);
					break;
				default:
					I2C_SetFunc(enCh,I2cStart_En);//其他错误状态，重新发送起始条件
					break;
			}
			I2C_ClearIrq(enCh);
			if(u8i==u32Len)
			{
				break;
			}
		}
		if(((vtTimerOut+vSystemLIRC5ms)&0xffff)>=100){
				I2C_ClearFunc(enCh,I2cAck_En);
				while(0 == I2C_GetIrq(enCh)&&(((vtTimerOut+vSystemLIRC5ms)&0xffff)<100))
								{}
				I2C_SetFunc(enCh,I2cStop_En);
				return	ErrorTimeout;
		}
		enRet = Ok;
		return enRet;
}
/**
 ******************************************************************************
 ** \brief  主机发送函数
 **
 ** \param u8Addr从机内存地址，pu8Data写数据，u32Len写数据长度
 **
 ** \retval 写数据是否成功
 **
 ******************************************************************************/
 en_result_t I2C_MasterWriteData(en_i2c_channel_t enCh,uint8_t u8Addr,uint8_t *pu8Data,uint32_t u32Len)
 {
     en_result_t enRet = Error;
     uint8_t u8i=0,u8State;
     I2C_SetFunc(enCh,I2cStart_En);
 	while(1)
 	{
 		while(0 == I2C_GetIrq(enCh))
 		{;}
 		u8State = I2C_GetState(enCh);
 		switch(u8State)
 		{
 			case 0x08:
 				I2C_ClearFunc(enCh,I2cStart_En);
 				I2C_WriteByte(enCh,I2C_SLAVEADDR);//从设备地址发送
 				break;
 			case 0x18:
 				I2C_WriteByte(enCh,u8Addr);//从设备内存地址发送
 				break;
 			case 0x28:
 				I2C_WriteByte(enCh,pu8Data[u8i++]);
 				break;
 			case 0x20:
 			case 0x38:
 				I2C_SetFunc(enCh,I2cStart_En);
 				break;
 			case 0x30:
 				I2C_SetFunc(enCh,I2cStop_En);
 				break;
 			default:
 				break;
 		}
 		if(u8i>u32Len)
 		{
 			I2C_SetFunc(enCh,I2cStop_En);//此顺序不能调换，出停止条件
 			I2C_ClearIrq(enCh);
 			break;
 		}
 		I2C_ClearIrq(enCh);
 	}
     enRet = Ok;
     return enRet;
 }
 en_result_t	 readVolt(void){
	u16 vtTemp;
	u8	vtIICReadTemp=0;
	en_result_t enRet = Error;
	float Vbat=0,iBat=0,vBattReal=0;
	uint8_t	vChargeIcSysStts=0;
	uint8_t	vChargeIcChgStts=0;
//	uint8_t	vtVbusStts=0;
//	uint8_t	vtBoostPowerLow=0;
//	uint8_t	vtSysPowerLow=0;
//	if(!Get_IIC_Int_Stts){				//充电IC休眠模式{
//		Sysctrl_SetRCHTrim(SysctrlRchFreq16MHz);
//		if(readLEDPowerStts){
//			vSet16MHzPCLK=1;
//			updateTo16MPCA();
//		}
//	}

    	enRet=I2C_MasterReadData(I2C0,0x0E,&vtIICReadTemp,1);
    	if(enRet){
    		if((vtIICReadTemp&0x04)==0)
    			fIsI2CInitOK=0;
    			return enRet;
    	}

    	enRet=I2C_MasterReadData(I2C0, SYSTEM_STATUS_ADDR, &vChargeIcSysStts, 1);
    	if(enRet)
    		return enRet;
    	enRet=I2C_MasterReadData(I2C0, CHARGE_STATUS_ADDR, &vChargeIcChgStts, 1);
    	if(enRet)
    		return enRet;
	fIsQuickCharging=FALSE;
	if((vChargeIcSysStts&0x07)==0x04)
		fIsQuickCharging=TRUE;

	enRet=I2C_MasterReadData(I2C0,0x64,u8Recdata,1);			//BATPIN 的两端电压 低8bit
    	if(enRet)
    		return enRet;
	vtTemp=0x0000;
	vtTemp|=u8Recdata[0];
	enRet=I2C_MasterReadData(I2C0,0x65,u8Recdata,1);			//BATPIN 的两端电压 高8bit		VBAT=BATVADC*0.26855mv+2.6V
    	if(enRet)
    		return enRet;
	vtTemp|=u8Recdata[0]<<8;
	Vbat=(0.26855*vtTemp)/1000+2.6;
	vAdVoltOfBatt=Vbat;
	//------------------------------------------------------------------
//	I2C_MasterReadData(I2C0,0x7A,u8Recdata,1);			//BATPIN 的真实电压 低8bit
//	vtTemp=0x0000;
//	vtTemp|=u8Recdata[0];
//	I2C_MasterReadData(I2C0,0x7B,u8Recdata,1);			//BATPIN 的真实电压 高8bit		VBAT=BATVADC*0.26855mv+2.6V
//	vtTemp|=u8Recdata[0]<<8;
//	//vBattReal=(0.26855*vtTemp)/1000+2.6;

	//------------------------------------------------------------------

	enRet=I2C_MasterReadData(I2C0,0x66,u8Recdata,1);			//BATIADC 数据的低 8bit
    	if(enRet)
    		return enRet;
	vtTemp=0x0000;
	vtTemp|=u8Recdata[0];
	enRet=I2C_MasterReadData(I2C0,0x67,u8Recdata,1);			//BATIADC 数据的高 8bit		IBAT=BATVADC*1.27883mA
    	if(enRet)
    		return enRet;
	vtTemp|=u8Recdata[0]<<8;
	if(vtTemp&0x8000){
		vtTemp=0x10000-vtTemp;
		iBat=vtTemp*1.27883/1000;
		if(iBat>DISCHARGING_THRESHOLD){
			fIsDischarging=1;
		}else{
			resetBrakeBattDet();
		}
		vChargeCurrent=-1*iBat;
	}else{
		iBat=vtTemp*1.27883/1000;
		vChargeCurrent=iBat;
	}
	if(((vChargeIcSysStts & 0x40)==0)&&(iBat<=0.10)){					//not charging and not discharging
		vBattReal=vBattVol;
	}else{
		vBattReal=Vbat-vChargeCurrent*BATTERY_RESISTANCE ;
		if(vChargeCurrent>0){
			vBattReal-=CHARGE_VOLT_DIVIATION;				//充电电压固定误差消除
		}
		else if(vChargeCurrent<DISCHARGING_THRESHOLD){
			vBattReal-=CHARGE_VOLT_DIVIATION+0.03;				//放电电压固定误差消除
		}
		if(vBattReal>5.0&&vBattVol!=0){
			vBattReal=vBattVol;
		}else if(vBattReal<=4.5){
			vBattVol=vBattReal;				//有效值
		}
	}
//	I2C_MasterReadData(I2C0,SYSTEM_STATUS_ADDR,u8Recdata,1);
//	vChargeIcSysStts=u8Recdata[0];
//	I2C_MasterReadData(I2C0,CHARGE_STATUS_ADDR,u8Recdata,1);
//	vChargeIcChgStts=u8Recdata[0];

//	I2C_MasterReadData(I2C0, 0x81, &vtBoostPowerLow, 1);
//	I2C_MasterReadData(I2C0, 0x84, &vtSysPowerLow, 1);

//	if(!Get_IIC_Int_Stts)	{							//充电IC休眠模式
//		Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);
//		if(vSet16MHzPCLK){
//			updateTo24MPCA();
//			vSet16MHzPCLK=0;
//		}
//	}
	if (vChargeIcChgStts & 0x40) {
		fIsCharging = 1;
		fIsBattFully = 1;
		fIsDischarging = 0;
		//chargeTestDisplay(vChargeIcSysStts,vChargeIcChgStts,iBat,vBattReal);
		chargeTestDisplay(vChargeIcSysStts,vChargeIcChgStts,iBat,vBattReal,Vbat);
		 if(!fIsHotNow)
			fIsNeedUpdateBattLv=TRUE;
	} else {
		if (vChargeIcSysStts & 0x10) {
//			vBusVolt=5;
//			enRet=I2C_MasterReadData(I2C0,VIN_VBUS_STATE,&vtVbusStts,1);			//BATIADC 数据的低 8bit
//			if(enRet)
//				return enRet;
//			switch((vtVbusStts>>3)&0x03){
//			case 1:
//				vBusVolt=7;
//				break;
//			case 3:
//				vBusVolt=9;
//				break;
//			case	7:
//				vBusVolt=12;
//				break;
//			}
			if (!fIsCharging) {
				fIsCharging = 1;
				fIsBattFully = 0;
				fIsDischarging = 0;
				if (!fIsSystemInitialing && !vDisableGetBattTimer) {
					vCurrentBattLv = 0;
				}
				clrPowerTimerOutStts();
				fIsNeedUpdateBattLv=TRUE;
				if(fIsSystemOff&&fIsWakeupByCharge==DoNothing){
					fIsWakeupByCharge=SetWakeupLCD;
				}
			}
			 setBattLevel(vBattReal);
			 if(!fIsHotNow)
				fIsNeedUpdateBattLv=TRUE;
			 //-------------------------------------
			 //chargeTestDisplay(vChargeIcSysStts,vChargeIcChgStts,iBat,vBattReal);
			chargeTestDisplay(vChargeIcSysStts,vChargeIcChgStts,iBat,vBattReal,Vbat);
			 //-------------------------------------

		} else if (fIsCharging) {
			if(!readLEDPowerStts)							//如果拔出充电头时,灯光处理打开状态,则不用充电IC提供的电量刷新显示
				setBattLevel(vBattReal);
			fIsNeedUpdateBattLv=TRUE;
			if((vChargeIcSysStts&0x7)!=0x01&&(vChargeIcSysStts&0x7)!=0x04){
				fIsCharging = 0;
				fIsBattFully = 0;
				fIsDischarging = 0;
				fIsNeedUpdateBattLv=TRUE;
				powerDownFromChageMode();
			}
			fIsQuickCharging=FALSE;
		}else if(fIsDischarging){
			if(!fIsSystemOff)
				 if(!fIsHotNow)
					fIsNeedUpdateBattLv=TRUE;
				setBattLevel(vBattReal);
		}
	}
	vtTemp = 0;

//	I2C_MasterReadData(I2C0,0x68,u8Recdata,1);			//SYSVADC 数据的低 8bit
//	vtTemp=0x0000;
//	vtTemp|=u8Recdata[0];
//	I2C_MasterReadData(I2C0,0x69,u8Recdata,1);			//SYSVADC 数据的高 8bit		SYSV=SYSVADC * 1.61133mV+15.6V  （补码格式，有效值通常为负值）
//	vtTemp|=u8Recdata[0]<<8;
//	if(vtTemp&0x8000){
//		vtTemp=0x10000-vtTemp;
//		vSys=15.6-vtTemp*1.61133/1000;
//	}else{
//		vSys=15.6+vtTemp*1.61133/1000;
//
//	}
//
//	I2C_MasterReadData(I2C0,0x6A,u8Recdata,1);			//SYSIADC 数据的低 8bit
//	vtTemp=0x0000;
//	vtTemp|=u8Recdata[0];
//	I2C_MasterReadData(I2C0,0x6B,u8Recdata,1);			//SYSIADC 数据的高 8bit		SYSV=SYSI=SYSIADC *=0.6394mA  （补码格式，有效值通常为负值）
//	vtTemp|=u8Recdata[0]<<8;
//	if(vtTemp&0x8000){
//		vtTemp=0x10000-vtTemp;
//		vSysi=vtTemp*0.6394/1000;
//	}else{
//		vSysi=vtTemp*0.6394/1000;
//	}
//
//	I2C_MasterReadData(I2C0,0x6c,u8Recdata,1);			//SYSIADC 数据的低 8bit
//	vtTemp=0x0000;
//	vtTemp|=u8Recdata[0];
//	I2C_MasterReadData(I2C0,0x6d,u8Recdata,1);			//SYSIADC 数据的高 8bit		SYSV=SYSI=SYSIADC *=0.6394mA  （补码格式，有效值通常为负值）
//	vtTemp|=u8Recdata[0]<<8;
//	if(vtTemp&0x8000){
//		vtTemp=0x10000-vtTemp;
//		vVini=vtTemp*0.6394/1000;
//	}else{
//		vVini=vtTemp*0.6394/1000;
//	}
//
//
//	I2C_MasterReadData(I2C0,0x6E,u8Recdata,1);			//VBUSIADC 数据的低 8bit
//	vtTemp=0x0000;
//	vtTemp|=u8Recdata[0];
//	I2C_MasterReadData(I2C0,0x6F,u8Recdata,1);			//VBUSIADC 数据的高 8bit		VBUSI=VBUSIADC*0.6394mA   （补码格式，放电为正，充电为负）
//	vtTemp|=u8Recdata[0]<<8;
//	if(vtTemp&0x8000)
//		vtTemp=0x10000-vtTemp;
//	vBusi=vtTemp*0.6394/1000;
//	vtTemp=0;
	return	Ok;
}

en_result_t userI2CInit(void) {
	en_result_t enRet = Error;
	stc_gpio_config_t stcGpioCfg;
	stc_i2c_config_t stcI2cCfg;
	stc_sysctrl_clk_config_t stcCfg;
	u8	vtIICReadTemp=0;
	DDL_ZERO_STRUCT(stcCfg);
	DDL_ZERO_STRUCT(stcI2cCfg);
	DDL_ZERO_STRUCT(stcGpioCfg);


//	Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
//	Sysctrl_SysClkSwitch(SysctrlClkRCL);
//	Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);
//	Sysctrl_SysClkSwitch(SysctrlClkRCH);
//	Sysctrl_ClkSourceEnable(SysctrlClkRCL, FALSE);
//	Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);

	Sysctrl_SetPeripheralGate(SysctrlPeripheralI2c0, TRUE);

	stcGpioCfg.enDir = GpioDirOut;
	stcGpioCfg.enOD = GpioOdEnable; //开漏输出
	stcGpioCfg.enPuPd = GpioPu;

	Gpio_Init(IIC_SCL_Port, IIC_SCL_Pin, &stcGpioCfg); //端口初始化
	Gpio_Init(IIC_SDA_Port, IIC_SDA_Pin, &stcGpioCfg);

	Gpio_SetAfMode(IIC_SCL_Port, IIC_SCL_Pin, IIC_SCL_FuncSel); //SCL
	Gpio_SetAfMode(IIC_SDA_Port, IIC_SDA_Pin, IIC_SDA_FuncSel); //SDA

//	stcI2cCfg.enFunc = I2cBaud_En;
//	stcI2cCfg.u8Tm = 0x02; //1M=(24000000/(8*(2+1))波特率配置
//	stcI2cCfg.pfnI2c0Cb = NULL;
//	stcI2cCfg.bTouchNvic = FALSE;
//	if (TRUE == stcI2cCfg.bTouchNvic) {
//		EnableNvic(I2C0_IRQn, IrqLevel3, TRUE);
//	}
//	I2C_DeInit(I2C0);
//	I2C_Init(I2C0, &stcI2cCfg); //模块初始化
//	I2C_SetFunc(I2C0, I2cMode_En); //模块使能
//	I2C_SetFunc(I2C0, I2cStart_En); //开始信号
//	I2C_MasterWriteData(I2C0, u8Senddata, 10); //主机发送10字节数据
//	delay1ms(10);
//	I2C_MasterReadData(I2C0, u8Recdata, 10); //主机读取10字节数据


	    stcI2cCfg.enFunc = I2cBaud_En;
	    //stcI2cCfg.u8Tm = 0x04;//100K=(4000000/(8*(4+1))
	   // stcI2cCfg.u8Tm = 0x08;//375K=(24000000/(8*(7+1))
//	    stcI2cCfg.u8Tm = 254;//375K=(24000000/(8*(15+1))
	    stcI2cCfg.u8Tm = 20;//375K=(24000000/(8*(15+1))
	    stcI2cCfg.pfnI2c1Cb = NULL;
	    stcI2cCfg.bTouchNvic = FALSE;
		if(TRUE == stcI2cCfg.bTouchNvic)
		{
			EnableNvic(I2C0_IRQn,IrqLevel3,TRUE);
		}
	    I2C_DeInit(I2C0);
	    I2C_Init(I2C0,&stcI2cCfg);//初始化模块
	    I2C_SetFunc(I2C0,I2cMode_En);//模块使能
//	    I2C_SetFunc(I2C0,I2cStart_En);//开始信号
//		I2C_MasterWriteData(I2C1,0x00,u8Senddata,10);
//		delay1ms(100);

	    	enRet=I2C_MasterReadData(I2C0,SYS_CTL14,&vtIICReadTemp,1);
	    	if(enRet)
	    		return enRet;
	    	vtIICReadTemp=vtIICReadTemp|0x04;
	    	//*u8Senddata=0x74;
	    	enRet=I2C_MasterWriteData(I2C0,SYS_CTL14,&vtIICReadTemp,1);		//设置低速IIC
	    	if(enRet)
	    		return enRet;
//		I2C_MasterReadData(I2C0,CHG_CTL2,&vtIICReadTemp,1);			//
//		I2C_MasterReadData(I2C0,IC_TEMP,u8Recdata,1);			//
//		I2C_MasterReadData(I2C0,SYS_CTL4,u8Recdata,1);			//
//		I2C_MasterReadData(I2C0,NTC_FLAG,u8Recdata,1);			//
//		I2C_MasterReadData(I2C0,FLAG0,u8Recdata,1);				//


	    	//-----------------------------------------------------------------------------------
	    	enRet=I2C_MasterReadData(I2C0,BOOST_LINC,&vtIICReadTemp,1);
	    	if(enRet)
	    		return enRet;
	    	vtIICReadTemp|=0x02;													//模拟线补使能, 150mV@2A
	    	enRet=I2C_MasterWriteData(I2C0,BOOST_LINC,&vtIICReadTemp,1);				//
	    	if(enRet)
	    		return enRet;
	    	//-----------------------------------------------------------------------------------
//	    	enRet=I2C_MasterReadData(I2C0,DCP_DIG_CTL2,&vtIICReadTemp,1);
//	    	if(enRet)
//	    		return enRet;
//	    	vtIICReadTemp&=0x00;													//关闭所有快充
//	    	enRet=I2C_MasterWriteData(I2C0,DCP_DIG_CTL2,&vtIICReadTemp,1);			//
//	    	if(enRet)
//	    		return enRet;
//	    	enRet=I2C_MasterReadData(I2C0,QC_VMAX,&vtIICReadTemp,1);
//	    	if(enRet)
//	    		return enRet;
//	    	vtIICReadTemp&=0xBF;													//QC最高12V
//	    	enRet=I2C_MasterWriteData(I2C0,DCP_DIG_CTL2,&vtIICReadTemp,1);				//
//	    	if(enRet)
//	    		return enRet;
	    	//-----------------------------------------------------------------------------------

	    	enRet=I2C_MasterReadData(I2C0,SYS_CTL4,&vtIICReadTemp,1);				//
	    	if(enRet)
	    		return enRet;
	    	vtIICReadTemp|=0x02;													//NTC 接地时(NTC<0.2V)，关闭 NTC 功能使能
	    	enRet=I2C_MasterWriteData(I2C0,SYS_CTL4,&vtIICReadTemp,1);				//
	    	if(enRet)
	    		return enRet;

	    	enRet=I2C_MasterReadData(I2C0,VBAT_LOW,&vtIICReadTemp,1);				//
	    	if(enRet)
	    		return enRet;
	    	vtIICReadTemp&=0xF3;
	    	vtIICReadTemp|=0x0C;													//低电保护3.0~3.1
	    	enRet=I2C_MasterWriteData(I2C0,VBAT_LOW,&vtIICReadTemp,1);
	    	if(enRet)
	    		return enRet;

//	    	enRet=I2C_MasterReadData(I2C0,VBUSOV,&vtIICReadTemp,1);				//
//	    	if(enRet)
//	    		return enRet;
//
//	    	enRet=I2C_MasterReadData(I2C0,DCP_DIG_CTL2,&vtIICReadTemp,1);		//
//	    	if(enRet)
//	    		return enRet;

//	    	vtIICReadTemp|=0x02;													//5V充电欠压环阈值
//	    	enRet=I2C_MasterWriteData(I2C0,SYS_CTL4,&vtIICReadTemp,1);				//
//	    	if(enRet)
//	    		return enRet;
	    	//-----------------------------------------------------------------------------------
	    	enRet=I2C_MasterReadData(I2C0,SYS_CTL3,&vtIICReadTemp,1);				//
	    	if(enRet)
	    		return enRet;
	    	vtIICReadTemp&=0x7F;
	    	enRet=I2C_MasterWriteData(I2C0,SYS_CTL3,&vtIICReadTemp,1);				//设置为充电IC关机不复位
	    	if(enRet)
	    		return enRet;


	    	//-------------------------------------------------------------------------------------------------------
	    	enRet=I2C_MasterReadData(I2C0,CHG_TIMER_EN,&vtIICReadTemp,1);				//
	    	if(enRet)
	    		return enRet;
	    	vtIICReadTemp&=0xEF;
	    	enRet=I2C_MasterWriteData(I2C0,CHG_TIMER_EN,&vtIICReadTemp,1);				//启用恒压设置
	    	if(enRet)
	    		return enRet;

	    	enRet=I2C_MasterReadData(I2C0,CHG_CTL2,&vtIICReadTemp,1);				//
	    	if(enRet)
	    		return enRet;
	    	vtIICReadTemp&=0xF0;
//	    	vtIICReadTemp|=0x06;													//设置恒压为4.35v+0.028v
	    	vtIICReadTemp|=0x04;													//设置恒压为4.35v+0.028v
//	    	vtIICReadTemp|=0x01;
	    	enRet=I2C_MasterWriteData(I2C0,CHG_CTL2,&vtIICReadTemp,1);
	    	if(enRet)
	    		return enRet;

	    	//-------------------------------------------------------------------------------------------------------
	    	enRet=I2C_MasterReadData(I2C0,SYS_CTL7,&vtIICReadTemp,1);				//轻负载设置
	    	if(enRet)
	    		return enRet;
	    	vtIICReadTemp=vtIICReadTemp&0x3F;
	    	enRet=I2C_MasterWriteData(I2C0,SYS_CTL7,&vtIICReadTemp,1);				//轻负载设置8秒休眠
	    	//-------------------------------------------------------------------------------------------------------
	    	enRet=I2C_MasterReadData(I2C0,RSET,&vtIICReadTemp,1);				//内阻补偿
	    	if(enRet)
	    		return enRet;
	    	vtIICReadTemp=vtIICReadTemp&0x0F;
//	    	vtIICReadTemp=vtIICReadTemp|0x80;									//设置内阻为50mohm
//	    	vtIICReadTemp=vtIICReadTemp|0xd0;									//设置内阻为80mohm
	    	vtIICReadTemp=vtIICReadTemp|0xF0;									//设置内阻为94mohm
	    	enRet=I2C_MasterWriteData(I2C0,RSET,&vtIICReadTemp,1);				//设置内阻补偿
	    	if(enRet)
	    		return enRet;
	    	//-------------------------------------------------------------------------------------------------------
	    	fIsI2CInitOK=1;

	    	readVolt();
	    	return	Ok;
}

