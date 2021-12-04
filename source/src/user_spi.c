/*
 * user_spi.c
 *
 *  Created on: 2019年10月10日
 *      Author: Bough_004
 */

#include "user_spi.h"
#include "ddl.h"


uint8_t bIrq,bIrqData,Buff[20];

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/

boolean_t bSendIrq,bRecvIrq;
uint8_t u8IrqData,Buff[20];
uint8_t u8SendData;
//static void Spi0CallBack(void)
//{

//    if(Spi_GetState(Spi0)&0x38)
//    {
//        Spi_ClearStatus(Spi0);
//        return;
//    }
//    if(Spi_GetStatus(Spi0,SpiRxne))
//    {
//      bRecvIrq = 1;
//      Spi_FuncEn(Spi0,SpiRxNeIe,FALSE);
//      u8IrqData = M0P_SPI0->DATA;
//      Spi_ClearStatus(Spi0);
//    }
//    else if(Spi_GetStatus(Spi0,SpiTxe))
//    {
//      bSendIrq = 1;
//      Spi_FuncEn(Spi0,SpiTxEIe,FALSE);
//      M0P_SPI0->DATA = u8SendData;
//      Spi_ClearStatus(Spi0);
//    }
//}
/*******************************************************************************
 *
 * 	发送SPI DATA,
 * @param  enCh: 		SPI通道
 * @param sendstr:		指向待发送buff起始地址指针
 * @param sendlen:	发送长度
 *
 ********************************************************************************/
/*void WriteData(en_spi_channel_t enCh,uint8_t *sendstr,uint8_t sendlen)
{
    uint8_t i;
    Spi_SetCS(Spi0,FALSE);
    bSendIrq = 0;
	bRecvIrq = 0;
    for(i=0;i<sendlen;i++)
    {
        u8SendData = *(sendstr + i);
        Spi_FuncEn(Spi0,SpiRxNeIe,TRUE);
        Spi_FuncEn(Spi0,SpiTxEIe,TRUE);
		while(0 == bSendIrq);
		bSendIrq = 0;
		while(0 == bRecvIrq);
		bRecvIrq = 0;
    }
    Spi_SetCS(Spi0,TRUE);
}*/
void WriteData(en_spi_channel_t enCh,uint8_t *sendstr,uint8_t sendlen)
{
    uint8_t i;
    Spi_SetCS(Spi0,FALSE);
 //   Spi_SendData(Spi0,*sendstr);
    for(i=0;i<sendlen;i++)
    {
        Spi_SendData(Spi0,*(sendstr + i));
    }
    Spi_SetCS(Spi0,TRUE);
}
/*******************************************************************************
 *
 * 	接收SPI DATA,
 * @param  enCh: 		SPI通道
 * @param sendstr:		指向接收buff起始地址指针
 * @param sendlen:	接收长度
 *
 ********************************************************************************/
void ReadData(en_spi_channel_t enCh,uint8_t *recvstr,uint8_t recvlen)
{
    uint8_t i;
    uint16_t u32TimeOut;
    Spi_SetCS(Spi0,FALSE);
    bRecvIrq = 0;

    for(i=0;i<recvlen;i++)
    {
        Spi_FuncEn(Spi0,SpiRxNeIe,TRUE);
        u32TimeOut = 1000;
        M0P_SPI0->DATA = 0; //接收数据时，用于主机产生时钟
        while(u32TimeOut--)
        {
             if(1 == bRecvIrq)
            {
                *(recvstr+i) = u8IrqData;
                break;
            }
        }
        bRecvIrq = 0;
    }
    Spi_SetCS(Spi0,TRUE);
}
/*******************************************************************************
 *
 * 	SPI 接口初始化,
 *
 ********************************************************************************/
void Spi_PortInit(void)
{
    stc_gpio_config_t stcGpioCfg;
    DDL_ZERO_STRUCT(stcGpioCfg);

    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(SPI0_MOSI_PORT, SPI0_MOSI_PIN,&stcGpioCfg);					//MOSI
    Gpio_SetAfMode(SPI0_MOSI_PORT, SPI0_MOSI_PIN,SPI0_MOSI_FuncSel);
    Gpio_Init(SPI0_CS_PORT, SPI0_CS_PIN,&stcGpioCfg);						//CS
    Gpio_SetAfMode(SPI0_CS_PORT, SPI0_CS_PIN,SPI0_CS_FuncSel);
    Gpio_Init(SPI0_SCK_PORT, SPI0_SCK_PIN,&stcGpioCfg);						//SCK
    Gpio_SetAfMode(SPI0_SCK_PORT, SPI0_SCK_PIN,SPI0_SCK_FuncSel);

//    stcGpioCfg.enDir = GpioDirIn;
//    Gpio_Init(SPI0_MISO_PORT, SPI0_MISO_PIN,&stcGpioCfg);					//MISO
//    Gpio_SetAfMode(SPI0_MISO_PORT, SPI0_MISO_PIN,SPI0_MISO_FuncSel);
}

/*******************************************************************************
 *
 * 	SPI0 模块初始化,
 *
 ********************************************************************************/
void userSpiInit(void){
//	    stc_spi_config_t  SPIConfig;
//
//
//	    DDL_ZERO_STRUCT(SPIConfig);
//
//	    Sysctrl_SetPeripheralGate(SysctrlPeripheralSpi0,TRUE); 				//SPI0
//	    Spi_PortInit();
//
//
//	    Spi_SetCS(Spi0,TRUE);
//	    //SPI模块配置
//	    SPIConfig.bCPHA = Spicphafirst;									//模式0
//	    SPIConfig.bCPOL = Spicpolhigh;									//时钟极性选择
//	    SPIConfig.bIrqEn = TRUE;
//	    SPIConfig.bMasterMode = SpiMaster;//主机
//	    SPIConfig.u8BaudRate = SpiClkDiv8;								//波特率设置 12Mhz
//	    SPIConfig.pfnSpi0IrqCb = Spi0CallBack;
//
//	    Spi_Init(Spi0,&SPIConfig);

	    stc_spi_config_t  SPIConfig;
	    DDL_ZERO_STRUCT(SPIConfig);

	    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
	    Sysctrl_SetPeripheralGate(SysctrlPeripheralSpi0,TRUE); //SPI0

	    Spi_PortInit();//模块初始化

	    Spi_SetCS(Spi0,TRUE);
	    //SPI模块配置
	    SPIConfig.bCPHA = Spicphafirst;	//<第一边沿采样
	    SPIConfig.bCPOL = Spicpollow;	//<极性为低
	    SPIConfig.bIrqEn = FALSE;
	    //SPIConfig.bIrqEn = TRUE;
	    SPIConfig.bMasterMode = SpiMaster;
	    SPIConfig.u8BaudRate = SpiClkDiv2;
	    //SPIConfig.pfnSpi0IrqCb = Spi0CallBack;
	    SPIConfig.pfnSpi0IrqCb = NULL;
		if(SPIConfig.bIrqEn)
		{
			EnableNvic(SPI0_IRQn,IrqLevel3,TRUE);
		}
	    Spi_Init(Spi0,&SPIConfig);//模块初始化

//	    Spi_SetCS(Spi0,FALSE);//使能片选信号
//	    for(i=0;i<10;i++)
//	    {
//	     // Spi_SendData(Spi0,u8Senddata[i]);//发送数据
//	      Spi_SendData(Spi0,0x15);//发送数据
//	    }
//	    i = Spi_ReceiveData(Spi0,1);// 接收推回数据AA
//	    for(i=0;i<10;i++)
//	    {
//	      u8Recvdata[i]=Spi_ReceiveData(Spi0,1);//主机接收数据
//	    }
//	    Spi_SetCS(Spi0,TRUE);
//	    while(1);

}
