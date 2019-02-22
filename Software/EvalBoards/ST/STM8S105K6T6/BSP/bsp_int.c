/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2012; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM32F0518-EVAL
*                                          Evaluation Board
*
* Filename      : bsp_int.c
* Version       : V1.00
* Programmer(s) : FF
*                 MD
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_INT_MODULE
#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_INT_SRC_NBR                                 29     /* Available Interrupts in Design                       */


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

static  CPU_FNCT_VOID  BSP_IntVectTbl[BSP_INT_SRC_NBR];


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_IntHandler     (CPU_DATA  int_id);
static  void  BSP_IntHandlerDummy(void);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              BSP_IntClr()
*
* Description : Clear interrupt.
*
* Argument(s) : int_id      Interrupt to clear.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) An interrupt does not need to be cleared within the interrupt controller.
*********************************************************************************************************
*/

void  BSP_IntClr (CPU_DATA  int_id)
{

}


/*
*********************************************************************************************************
*                                              BSP_IntDis()
*
* Description : Disable interrupt.
*
* Argument(s) : int_id      Interrupt to disable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDis (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcDis(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                           BSP_IntDisAll()
*
* Description : Disable ALL interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDisAll (void)
{
    CPU_IntDis();
}


/*
*********************************************************************************************************
*                                               BSP_IntEn()
*
* Description : Enable interrupt.
*
* Argument(s) : int_id      Interrupt to enable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntEn (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcEn(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               isr         Handler to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntVectSet (CPU_DATA       int_id,
                      CPU_FNCT_VOID  isr)
{
    CPU_SR_ALLOC();


    if (int_id < BSP_INT_SRC_NBR) {
        CPU_CRITICAL_ENTER();
        BSP_IntVectTbl[int_id] = isr;
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntPrioSet()
*
* Description : Assign ISR priority.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               prio        Priority to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntPrioSet (CPU_DATA    int_id,
                      CPU_INT08U  prio)
{
    CPU_SR_ALLOC();


    if (int_id < BSP_INT_SRC_NBR) {
        CPU_CRITICAL_ENTER();
        CPU_IntSrcPrioSet(int_id + 16, prio);
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           INTERNAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              BSP_IntInit()
*
* Description : Initialize interrupts:
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntInit (void)
{
    CPU_DATA  int_id;


    for (int_id = 0; int_id < BSP_INT_SRC_NBR; int_id++) {
        BSP_IntVectSet(int_id, BSP_IntHandlerDummy);
    }
}


/*
*********************************************************************************************************
*                                        BSP_IntHandler####()
*
* Description : Handle an interrupt.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void    BSP_IntHandlerWWDG                          (void)    { BSP_IntHandler(BSP_INT_ID_WWDG);                            }
void    BSP_IntHandlerPVD                           (void)    { BSP_IntHandler(BSP_INT_ID_PVD);                             }
void    BSP_IntHandlerRTC                           (void)    { BSP_IntHandler(BSP_INT_ID_RTC);                             }
void    BSP_IntHandlerFLASH                         (void)    { BSP_IntHandler(BSP_INT_ID_FLASH);                           }
void    BSP_IntHandlerRCC                           (void)    { BSP_IntHandler(BSP_INT_ID_RCC);                             }
void    BSP_IntHandlerEXTI0_1                       (void)    { BSP_IntHandler(BSP_INT_ID_EXTI0_1);                         }
void    BSP_IntHandlerEXTI2_3                       (void)    { BSP_IntHandler(BSP_INT_ID_EXTI2_3);                         }
void    BSP_IntHandlerEXTI4_15                      (void)    { BSP_IntHandler(BSP_INT_ID_EXTI4_15);                        }
void    BSP_IntHandlerTSC                           (void)    { BSP_IntHandler(BSP_INT_ID_TSC);                             }
void    BSP_IntHandlerDMA_CH1                       (void)    { BSP_IntHandler(BSP_INT_ID_DMA_CH1);                         }
void    BSP_IntHandlerDMA_CH2_3                     (void)    { BSP_IntHandler(BSP_INT_ID_DMA_CH2_3);                       }
void    BSP_IntHandlerDMA_CH4_5                     (void)    { BSP_IntHandler(BSP_INT_ID_DMA_CH4_5);                       }
void    BSP_IntHandlerADC_COMP                      (void)    { BSP_IntHandler(BSP_INT_ID_ADC_COMP);                        }
void    BSP_IntHandlerTIM1_BRK_UP_TRG_COM           (void)    { BSP_IntHandler(BSP_INT_ID_TIM1_BRK_UP_TRG_COM);             }
void    BSP_IntHandlerTIM1_CC                       (void)    { BSP_IntHandler(BSP_INT_ID_TIM1_CC);                         }
void    BSP_IntHandlerTIM2                          (void)    { BSP_IntHandler(BSP_INT_ID_TIM2);                            }
void    BSP_IntHandlerTIM3                          (void)    { BSP_IntHandler(BSP_INT_ID_TIM3);                            }
void    BSP_IntHandlerTIM6_DAC                      (void)    { BSP_IntHandler(BSP_INT_ID_TIM6_DAC);                        }
void    BSP_IntHandlerTIM14                         (void)    { BSP_IntHandler(BSP_INT_ID_TIM14);                           }
void    BSP_IntHandlerTIM15                         (void)    { BSP_IntHandler(BSP_INT_ID_TIM15);                           }
void    BSP_IntHandlerTIM16                         (void)    { BSP_IntHandler(BSP_INT_ID_TIM16);                           }
void    BSP_IntHandlerTIM17                         (void)    { BSP_IntHandler(BSP_INT_ID_TIM17);                           }
void    BSP_IntHandlerI2C1                          (void)    { BSP_IntHandler(BSP_INT_ID_I2C1);                            }
void    BSP_IntHandlerI2C2                          (void)    { BSP_IntHandler(BSP_INT_ID_I2C2);                            }
void    BSP_IntHandlerSPI1                          (void)    { BSP_IntHandler(BSP_INT_ID_SPI1);                            }
void    BSP_IntHandlerSPI2                          (void)    { BSP_IntHandler(BSP_INT_ID_SPI2);                            }
void    BSP_IntHandlerUSART1                        (void)    { BSP_IntHandler(BSP_INT_ID_USART1);                          }
void    BSP_IntHandlerUSART2                        (void)    { BSP_IntHandler(BSP_INT_ID_USART2);                          }
void    BSP_IntHandlerCEC                           (void)    { BSP_IntHandler(BSP_INT_ID_CEC);                             }


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          BSP_IntHandler()
*
* Description : Central interrupt handler.
*
* Argument(s) : int_id          Interrupt that will be handled.
*
* Return(s)   : none.
*
* Caller(s)   : ISR handlers.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandler (CPU_DATA  int_id)
{
    CPU_FNCT_VOID  isr;
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();                                       /* Tell the OS that we are starting an ISR            */

    OSIntEnter();

    CPU_CRITICAL_EXIT();

    if (int_id < BSP_INT_SRC_NBR) {
        isr = BSP_IntVectTbl[int_id];

        if (isr != (CPU_FNCT_VOID)0) {
            isr();
        }

    }

    OSIntExit();                                                /* Tell the OS that we are leaving the ISR            */
}


/*
*********************************************************************************************************
*                                        BSP_IntHandlerDummy()
*
* Description : Dummy interrupt handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_IntHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandlerDummy (void)
{

}
