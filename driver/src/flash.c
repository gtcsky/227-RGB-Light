/******************************************************************************
*Copyright(C)2018, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd("HDSC").
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

/** \file flash.c
 **
 ** Common API of flash.
 ** @link flashGroup Some description @endlink
 **
 **   - 2018-05-08
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "flash.h"
/**
 *******************************************************************************
 ** \addtogroup FlashGroup
 ******************************************************************************/
//@{

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
#define FLASH_END_ADDR              (0x0000FFFFu)
#define FLASH_BYPASS()              M0P_FLASH->BYPASS = 0x5A5A;\
                                    M0P_FLASH->BYPASS = 0xA5A5; 
#define FLASH_IE_TRUE               (0x03)
#define FLASH_IE_FALSE              (0x00)

#define FLASH_TIMEOUT_INIT          (0xFFu)
#define FLASH_TIMEOUT_PGM           (0xFFu)
#define FLASH_TIMEOUT_ERASE         (0xFFu)

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
/**
 ******************************************************************************
 ** \brief FLASH OP
 ** 
 ** Flash ????????????????????????????????? 
 ******************************************************************************/
typedef enum en_flash_op
{
    Read        = 0u,           ///<????????????
    Program     = 1u,           ///<???????????????
    SectorErase = 2u,           ///<?????????????????????
    ChipErase   = 3u,           ///<?????????????????????
} en_flash_op_t;

/**
 ******************************************************************************
 ** \brief FLASH LOCK
 ** 
 ** Flash ?????????????????????????????? 
 ******************************************************************************/
typedef enum en_flash_lock
{
    LockAll   = 0x00000000u,            ///<????????????  
    UnlockAll = (int)0xFFFFFFFFu,      ///<????????????
} en_flash_lock_t;

/**
 ******************************************************************************
 ** \brief FLASH ????????????????????????
 ** 
 ** FLASH???????????????????????????????????? (4MHz)
 ******************************************************************************/
const uint32_t pu32PcgTimer4M[] = { 
                                    0x20u,          //Tnvs   
                                    0x17u,          //Tpgs   
                                    0x1Bu,          //Tprog  
                                    0x4650u,        //Tserase
                                    0x222E0u,       //Tmerase
                                    0x18u,          //Tprcv  
                                    0xF0u,          //Tsrcv  
                                    0x3E8u          //Tmrcv  
                                  };
/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/
static func_ptr_t pfnFlashCallback = NULL;

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 *****************************************************************************
 ** \brief Flash??????????????????
 **
 **
 ** \param [in] u8Param        == 0
 **                                      
 *****************************************************************************/
void EfRam_IRQHandler(uint8_t u8Param)
{   
	if(NULL != pfnFlashCallback)
	{
		pfnFlashCallback();
	}
}

/**
 *****************************************************************************
 ** \brief Flash??????????????????
 **
 **
 ** \param [in]  enFlashIntType          Flash????????????
 ** 
 ** \retval TRUE or FALSE                                      
 *****************************************************************************/
boolean_t Flash_GetIntFlag(en_flash_int_type_t enFlashIntType)
{
    boolean_t bRetVal = FALSE;
    
    switch (enFlashIntType)
    {
        case FlashPCInt:
            bRetVal = M0P_FLASH->IFR_f.IF0 ? TRUE : FALSE;
            break;
        case FlashSlockInt:
            bRetVal = M0P_FLASH->IFR_f.IF1 ? TRUE : FALSE;
            break;
        default:
            bRetVal = FALSE;
            break;
    }
    
    return bRetVal;
}

/**
 *****************************************************************************
 ** \brief Flash??????????????????
 **
 **
 ** \param [in]  enFlashIntType          Flash????????????
 ** 
 ** \retval Ok or Error                                      
 *****************************************************************************/
en_result_t Flash_ClearIntFlag(en_flash_int_type_t enFlashIntType)
{
    en_result_t enResult = Error;

    switch (enFlashIntType)
    {
        case FlashPCInt:
            FLASH_BYPASS();
            M0P_FLASH->ICLR_f.ICLR0 = FALSE;
            enResult = Ok;
            break;
        case FlashSlockInt:
            FLASH_BYPASS();
            M0P_FLASH->ICLR_f.ICLR1 = FALSE;
            enResult = Ok;
            break;
        default:
            enResult = Error;
            break;
    }

    return enResult;
}

/**
 *****************************************************************************
 ** \brief Flash????????????
 **
 **
 ** \param [in]  enFlashIntType          Flash????????????
 ** 
 ** \retval Ok or Error                                      
 *****************************************************************************/
en_result_t Flash_EnableIrq (en_flash_int_type_t enFlashIntType)
{
    en_result_t enResult = Error;

    switch (enFlashIntType)
    {
        case FlashPCInt:
            FLASH_BYPASS();
            M0P_FLASH->CR_f.IE |= 0x01;
            enResult = Ok;
            break;
        case FlashSlockInt:
            FLASH_BYPASS();
            M0P_FLASH->CR_f.IE |= 0x02;
            enResult = Ok;
            break;
        default:
            enResult = Error;
            break;
    }

    return enResult;
}

/**
 *****************************************************************************
 ** \brief Flash????????????
 **
 **
 ** \param [in]  enFlashIntType          Flash????????????
 ** 
 ** \retval Ok or Error                                      
 *****************************************************************************/
en_result_t Flash_DisableIrq(en_flash_int_type_t enFlashIntType)
{
    en_result_t enResult = Error;

    switch (enFlashIntType)
    {
        case FlashSlockInt:
            FLASH_BYPASS();
            M0P_FLASH->CR_f.IE &= ~0x02u;
            enResult = Ok;
            break;
        case FlashPCInt:
            FLASH_BYPASS();
            M0P_FLASH->CR_f.IE &= ~0x01u;
            enResult = Ok;
            break;
        default:
            enResult = Error;
            break;
    }

    return enResult;
}

/**
 *****************************************************************************
 ** \brief FLASH ??????????????????????????????????????????????????????????????????????????????
 **
 ** ????????????????????????????????????????????????????????????????????????????????????FLASH???????????????????????????.
 **
 ** \param [in]  pfnFlashCb       Flash????????????????????????[void function(void)]
 ** \param [in]  u8FreqCfg        FLASH????????????????????????(??????HCLK????????????????????????)???
 **                               1      - 4MHz;
 **                               2      - 8MHz;
 **                               4      - 16MHz;
 **                               6      - 24MHz;
 **                               8      - 32MHz???????????????????????????1???FLASH???????????????;
 **                               12     - 48MHz???????????????????????????1???FLASH???????????????;
 **                               other   -  ?????????
 ** \param [in] bDpstbEn          TRUE  - ???????????????DeepSleep?????????FLASH?????????????????????;
 **                               FALSE - ???????????????DeepSleep?????????FLASH????????????????????????;
 ** 
 ** \retval Ok                    ????????????.
 ** \retval ErrorInvalidParameter ????????????.
 ** \retval ErrorUninitialized    ??????????????????
 *****************************************************************************/
en_result_t Flash_Init(func_ptr_t pfnFlashCb, uint8_t u8FreqCfg, boolean_t bDpstbEn)
{
    uint32_t                u32Index  = 0;
    volatile uint32_t       u32TimeOut = FLASH_TIMEOUT_INIT;
    en_result_t             enResult  = Ok;
    uint32_t                u32PrgTimer[8] = {0}; 
    volatile uint32_t       *pu32PrgTimerReg = (volatile uint32_t*)M0P_FLASH;
    
    if ((1  != u8FreqCfg) && (2  != u8FreqCfg) &&
        (4  != u8FreqCfg) && (6  != u8FreqCfg) &&
        (8  != u8FreqCfg) && (12 != u8FreqCfg))
    {
        enResult = ErrorInvalidParameter;
        return (enResult);
    }

    pfnFlashCallback = pfnFlashCb;
    
	M0P_FLASH->CR_f.DPSTB_EN = bDpstbEn;
	
    //flash???????????????????????????
    for(u32Index=0; u32Index<8; u32Index++)
    {
        u32PrgTimer[u32Index] = u8FreqCfg * pu32PcgTimer4M[u32Index];
    } 
    
    //?????????FLASH????????????
    if (8 == u8FreqCfg)
    {
        FLASH_BYPASS();
        M0P_FLASH->CR_f.WAIT = 0x01;
    }
    else if(12 == u8FreqCfg)
    {
        u32PrgTimer[1]       = 0xFF;      
        FLASH_BYPASS();
        M0P_FLASH->CR_f.WAIT = 0x01;
    }
    else
    {
        FLASH_BYPASS();    
        M0P_FLASH->CR_f.WAIT = 0x00;
    }
    
    //flash???????????????????????????
    for(u32Index=0; u32Index<8; u32Index++)
    {
        u32TimeOut = FLASH_TIMEOUT_INIT;
        while(pu32PrgTimerReg[u32Index]  != u32PrgTimer[u32Index])
        {
            if(u32TimeOut--)
            {
                FLASH_BYPASS();
                pu32PrgTimerReg[u32Index] = u32PrgTimer[u32Index];
            }
            else
            {
                return ErrorUninitialized;
            }
        }
    }
    
    return (enResult);
}

/**
 *****************************************************************************
 ** \brief FLASH ?????????
 **
 ** ?????????FLASH??????1????????????.
 **
 ** \param [in]  u32Addr          Flash??????
 ** \param [in]  u8Data           1????????????
 ** 
 ** \retval Ok                    ????????????.
 ** \retval ErrorInvalidParameter FLASH????????????
 ** \retval ErrorTimeout          ????????????
 *****************************************************************************/
en_result_t Flash_WriteByte(uint32_t u32Addr, uint8_t u8Data)
{
    en_result_t             enResult = Ok;    
    volatile uint32_t       u32TimeOut = FLASH_TIMEOUT_PGM;
    
    if (FLASH_END_ADDR < u32Addr)
    {
        enResult = ErrorInvalidParameter;
        return (enResult);
    }
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //set OP
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while(Program != M0P_FLASH->CR_f.OP)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->CR_f.OP = Program;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    //unlock flash
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while((uint32_t)UnlockAll != M0P_FLASH->SLOCK)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->SLOCK = (uint32_t)UnlockAll;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    //write data
    *((volatile uint8_t*)u32Addr) = u8Data;
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //lock flash
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while((uint32_t)LockAll != M0P_FLASH->SLOCK)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->SLOCK = (uint32_t)LockAll;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    return (enResult);
}

/**
 *****************************************************************************
 ** \brief FLASH ?????????
 **
 ** ?????????FLASH???????????????2???????????????.
 **
 ** \param [in]  u32Addr         Flash??????
 ** \param [in]  u16Data        ?????????2???????????????
 ** 
 ** \retval Ok                    ????????????.
 ** \retval ErrorInvalidParameter FLASH????????????
 ** \retval ErrorTimeout          ????????????
 *****************************************************************************/
en_result_t Flash_WriteHalfWord(uint32_t u32Addr, uint16_t u16Data)
{
    en_result_t             enResult = Ok;    
    volatile uint32_t       u32TimeOut = FLASH_TIMEOUT_PGM;
    
    if (FLASH_END_ADDR < u32Addr)
    {
        enResult = ErrorInvalidParameter;
        return (enResult);
    }
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //set OP
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while(Program != M0P_FLASH->CR_f.OP)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->CR_f.OP = Program;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    //unlock flash
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while((uint32_t)UnlockAll != M0P_FLASH->SLOCK)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->SLOCK = (uint32_t)UnlockAll;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    //write data
    *((volatile uint16_t*)u32Addr) = u16Data;
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //lock flash
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while((uint32_t)LockAll != M0P_FLASH->SLOCK)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->SLOCK = (uint32_t)LockAll;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    return (enResult);
}

/**
 *****************************************************************************
 ** \brief FLASH ??????
 **
 ** ?????????FLASH??????1???????????????.
 **
 ** \param [in]  u32Addr         Flash??????
 ** \param [in]  u32Data         1????????????
 ** 
 ** \retval Ok                    ????????????.
 ** \retval ErrorInvalidParameter FLASH????????????
 ** \retval ErrorTimeout          ????????????
 *****************************************************************************/
en_result_t Flash_WriteWord(uint32_t u32Addr, uint32_t u32Data)
{
    en_result_t             enResult = Ok;    
    volatile uint32_t       u32TimeOut = FLASH_TIMEOUT_PGM;
    
    if (FLASH_END_ADDR < u32Addr)
    {
        enResult = ErrorInvalidParameter;
        return (enResult);
    }
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //set OP
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while(Program != M0P_FLASH->CR_f.OP)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->CR_f.OP = Program;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    //unlock flash
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while((uint32_t)UnlockAll != M0P_FLASH->SLOCK)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->SLOCK = (uint32_t)UnlockAll;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    //write data
    *((volatile uint32_t*)u32Addr) = u32Data;
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //lock flash
    u32TimeOut = FLASH_TIMEOUT_PGM;
    while((uint32_t)LockAll != M0P_FLASH->SLOCK)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->SLOCK = (uint32_t)LockAll;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    return (enResult);
}

/**
 *****************************************************************************
 ** \brief FLASH ????????????
 **
 ** FLASH ????????????.
 **
 ** \param [in]  u32SectorAddr    ???????????????????????????
 ** 
 ** \retval Ok                    ????????????.
 ** \retval ErrorInvalidParameter FLASH???????????? 
 ** \retval ErrorTimeout          ????????????
 *****************************************************************************/
en_result_t Flash_SectorErase(uint32_t u32SectorAddr)
{
    en_result_t             enResult = Ok;    
    volatile uint32_t       u32TimeOut = FLASH_TIMEOUT_ERASE;
    
    if (FLASH_END_ADDR < u32SectorAddr)
    {
        enResult = ErrorInvalidParameter;
        return (enResult);
    }
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_ERASE;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //set OP
    u32TimeOut = FLASH_TIMEOUT_ERASE;
    while(SectorErase != M0P_FLASH->CR_f.OP)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->CR_f.OP = SectorErase;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    //unlock flash
    u32TimeOut = FLASH_TIMEOUT_ERASE;
    while((uint32_t)UnlockAll != M0P_FLASH->SLOCK)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->SLOCK = (uint32_t)UnlockAll;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    //write data
    *((volatile uint8_t*)u32SectorAddr) = 0;
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_ERASE;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //lock flash
    u32TimeOut = FLASH_TIMEOUT_ERASE;
    while((uint32_t)LockAll != M0P_FLASH->SLOCK)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->SLOCK = (uint32_t)LockAll;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    return (enResult);
}

/**
 *****************************************************************************
 ** \brief FLASH ????????????(???????????????RAM??????????????????)
 **
 ** FLASH ????????????.
 **
 ** 
 ** \retval Ok              ????????????.
 ** \retval ErrorTimeout    ????????????
 **
 *****************************************************************************/
en_result_t Flash_ChipErase(void)
{
    en_result_t             enResult = Ok;    
    volatile uint32_t       u32TimeOut = FLASH_TIMEOUT_ERASE; 
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_ERASE;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //set OP
    u32TimeOut = FLASH_TIMEOUT_ERASE;
    while(ChipErase != M0P_FLASH->CR_f.OP)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->CR_f.OP = ChipErase;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    //unlock flash
    u32TimeOut = FLASH_TIMEOUT_ERASE;
    while((uint32_t)UnlockAll != M0P_FLASH->SLOCK)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->SLOCK = (uint32_t)UnlockAll;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    //write data
    *((volatile uint8_t*)0) = 0;
    
    //busy?
    u32TimeOut = FLASH_TIMEOUT_ERASE;
    while (TRUE == M0P_FLASH->CR_f.BUSY)
    {
        if(0 == u32TimeOut--)
        {
            return ErrorTimeout;
        }
    }
    
    //lock flash
    u32TimeOut = FLASH_TIMEOUT_ERASE;
    while((uint32_t)LockAll != M0P_FLASH->SLOCK)
    {
        if(u32TimeOut--)
        {
            FLASH_BYPASS();
            M0P_FLASH->SLOCK = (uint32_t)LockAll;
        }
        else
        {
            return ErrorTimeout;
        }
    }
    
    return (enResult);
}

/**
 *****************************************************************************
 ** \brief FLASH ??????????????????
 **
 ** \param [in]  enFlashSector    ????????????????????????
 ** 
 ** \retval Ok                    ????????????
 ** \retval ErrorInvalidParameter ????????????                                
 *****************************************************************************/
en_result_t Flash_Lock(en_flash_sector_lock_t enFlashSector)
{
    en_result_t             enResult = Ok;    
   
    FLASH_BYPASS();
    M0P_FLASH->SLOCK_f.SLOCK |= (uint32_t)enFlashSector;

    return enResult;
}

/**
 *****************************************************************************
 ** \brief FLASH ??????????????????
 **
 ** \param [in]  enFlashSector    ????????????????????????
 ** 
 ** \retval Ok                    ????????????
 ** \retval ErrorInvalidParameter ????????????                                
 *****************************************************************************/
en_result_t Flash_Unlock(en_flash_sector_lock_t enFlashSector)
{
    en_result_t             enResult = Ok;    
   
    FLASH_BYPASS();
    M0P_FLASH->SLOCK_f.SLOCK &= ~(uint32_t)enFlashSector;

    return enResult;
}

/**
 *****************************************************************************
 ** \brief FLASH ?????????????????????
 **
 ** \param [in]  enWaitCycle  ??????FLASH??????????????????????????????
 ** 
 ** \retval Ok                    ????????????
 ** \retval ErrorInvalidParameter ????????????                                
 *****************************************************************************/
en_result_t Flash_WaitCycle(en_flash_waitcycle_t enWaitCycle)
{
    en_result_t enResult = Ok;
    
    //??????FLASH???????????????
    M0P_SYSCTRL->PERI_CLKEN_f.FLASH = 1;
    
    FLASH_BYPASS();
    if (0 == enWaitCycle)
    {
        M0P_FLASH->CR_f.WAIT = 0;
    }
    else if(1 == enWaitCycle)
    {
        M0P_FLASH->CR_f.WAIT = 1;
    }
    else
    {
        M0P_FLASH->CR_f.WAIT = 2;
    }
    
    return enResult;
}


//@} // FlashGroup

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
