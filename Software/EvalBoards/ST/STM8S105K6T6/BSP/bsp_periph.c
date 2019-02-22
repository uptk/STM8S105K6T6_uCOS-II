/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2007-2012; Micrium, Inc.; Weston, FL
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
* Filename      : bsp_periph.c
* Version       : V1.00
* Programmer(s) : MD
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_PERIPH_MODULE
#include <bsp.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_PERIPH_REG_RCC_BASE                  0x40021000
#define  BSP_PERIPH_REG_RCC_CFGR                  (*(CPU_REG32 *)(BSP_PERIPH_REG_RCC_BASE + 0x004))
#define  BSP_PERIPH_REG_RCC_AHBENR                (*(CPU_REG32 *)(BSP_PERIPH_REG_RCC_BASE + 0x014))
#define  BSP_PERIPH_REG_RCC_APB1ENR               (*(CPU_REG32 *)(BSP_PERIPH_REG_RCC_BASE + 0x018))
#define  BSP_PERIPH_REG_RCC_APB2ENR               (*(CPU_REG32 *)(BSP_PERIPH_REG_RCC_BASE + 0x01C))


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


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         BSP_PeriphClkFreqGet()
*
* Description : Get clock frequency of a peripheral.
*
* Argument(s) : pwr_clk_id      Power/clock ID.
*
* Return(s)   : clk_freq or 0.
*
* Caller(s)   : Application.
*
* Note(s)     : AHB Periphs divider chosen with HPRE[3:0] bits 7-4. APB Periphs
*               divider chosen with PPRE[2:0] bits 10-8.
*
*              ---PPRE PCLK Prescaler---        ---HPRE HCLK Prescaler---
*              0xx: HCLK not divided            0xxx: SYSCLK not divided
*              100: HCLK divided by 2           1000: SYSCLK divided by 2
*              101: HCLK divided by 4           1001: SYSCLK divided by 4
*              110: HCLK divided by 8           1010: SYSCLK divided by 8
*              111: HCLK divided by 16          1011: SYSCLK divided by 16
*                                               1100: SYSCLK divided by 64
*                                               1101: SYSCLK divided by 128
*                                               1110: SYSCLK divided by 256
*                                               1111: SYSCLK divided by 512
*
*********************************************************************************************************
*/

CPU_INT32U  BSP_PeriphClkFreqGet (CPU_DATA  pwr_clk_id)
{
    CPU_INT32U  clk_freq;
    CPU_INT32U  clk_div;
    CPU_INT16U  clk_shift;

    clk_freq  = BSP_CPU_ClkFreq();                              /* Set the clock frequency SYSCLK                       */
    clk_shift = 0x0001;                                         /* Used to shift into place for CLK divider             */

    clk_div = (BSP_PERIPH_REG_RCC_CFGR & 0x000000F0) >> 4;      /* Get bits 7-4 for HPRE prescaler value                */

    if ((clk_div < 12) & (clk_div > 7)){
        clk_div   = clk_shift << (clk_div - 7);                 /* Shifts the clock configure bits out from register    */
        clk_freq /= clk_div;

    } else if (clk_div > 11){                                   /* Accounts for the skip in dividers from 16 to 64      */
        clk_div   = clk_shift << (clk_div - 6);
        clk_freq /= clk_div;
    }                                                           /* If clk_div is 7 or less no clock division is done    */


    switch (pwr_clk_id) {
                                                                /*  AHB values for HCLK                                 */
         case BSP_PERIPH_ID_DMAEN:
             return (clk_freq);

                                                                /*  APB1 values for PCLK                                */
         case BSP_PERIPH_ID_SRAMEN:
         case BSP_PERIPH_ID_FLITFEN:
         case BSP_PERIPH_ID_CRCEN:
         case BSP_PERIPH_ID_IOPAEN:
         case BSP_PERIPH_ID_IOPBEN:
         case BSP_PERIPH_ID_IOPCEN:
         case BSP_PERIPH_ID_IOPDEN:
         case BSP_PERIPH_ID_IOPFEN:
         case BSP_PERIPH_ID_TSCEN:

             clk_div = (BSP_PERIPH_REG_RCC_CFGR &
                        0x00000700) >> 4;                       /* Get bits 10-8 for PPRE prescaler value               */

             if (clk_div < 4) {
                 return (clk_freq);
             } else {
                 clk_div   = clk_shift << (clk_div - 3);
                 clk_freq /= clk_div;
                 return (clk_freq);
                }
    }
    return ((CPU_INT32U)0);
}


/*
*********************************************************************************************************
*                                             BSP_PeriphEn()
*
* Description : Enable clock for peripheral.
*
* Argument(s) : pwr_clk_id      Power/clock ID.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_PeriphEn (CPU_DATA  pwr_clk_id)
{
    switch (pwr_clk_id) {
                                                                /* AHB Enable Peripherals                               */
         case BSP_PERIPH_ID_DMAEN:
         case BSP_PERIPH_ID_SRAMEN:
         case BSP_PERIPH_ID_FLITFEN:
         case BSP_PERIPH_ID_CRCEN:
         case BSP_PERIPH_ID_IOPAEN:
         case BSP_PERIPH_ID_IOPBEN:
         case BSP_PERIPH_ID_IOPCEN:
         case BSP_PERIPH_ID_IOPDEN:
         case BSP_PERIPH_ID_IOPFEN:
         case BSP_PERIPH_ID_TSCEN:
              BSP_PERIPH_REG_RCC_AHBENR |= DEF_BIT(pwr_clk_id);
              break;

                                                                /* APB2 Enable Peripherals                              */
         case BSP_PERIPH_ID_SYSCFGCOMPEN:
         case BSP_PERIPH_ID_ADCEN:
         case BSP_PERIPH_ID_TIM1EN:
         case BSP_PERIPH_ID_SPI1EN:
         case BSP_PERIPH_ID_USART1EN:
         case BSP_PERIPH_ID_TIM15EN:
         case BSP_PERIPH_ID_TIM16EN:
         case BSP_PERIPH_ID_TIM17EN:
         case BSP_PERIPH_ID_DBGMCUEN:
              BSP_PERIPH_REG_RCC_APB2ENR |= DEF_BIT(pwr_clk_id - 32);
              break;

                                                                /* APB1 Enable Peripherals                              */
         case BSP_PERIPH_ID_TIM2EN:
         case BSP_PERIPH_ID_TIM3EN:
         case BSP_PERIPH_ID_TIM6EN:
         case BSP_PERIPH_ID_TIM14EN:
         case BSP_PERIPH_ID_WWDGEN:
         case BSP_PERIPH_ID_SPI2EN:
         case BSP_PERIPH_ID_USART2EN:
         case BSP_PERIPH_ID_I2C1EN:
         case BSP_PERIPH_ID_I2C2EN:
         case BSP_PERIPH_ID_PWREN:
         case BSP_PERIPH_ID_DACEN:
         case BSP_PERIPH_ID_CECEN:
              BSP_PERIPH_REG_RCC_APB1ENR |= DEF_BIT(pwr_clk_id - 64);
              break;
    }
}


/*
*********************************************************************************************************
*                                             BSP_PeriphDis()
*
* Description : Disable clock for peripheral.
*
* Argument(s) : pwr_clk_id      Power/clock ID.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_PeriphDis (CPU_DATA  pwr_clk_id)
{
    switch (pwr_clk_id) {
                                                                /* AHB Enable Peripherals                               */
         case BSP_PERIPH_ID_DMAEN:
         case BSP_PERIPH_ID_SRAMEN:
         case BSP_PERIPH_ID_FLITFEN:
         case BSP_PERIPH_ID_CRCEN:
         case BSP_PERIPH_ID_IOPAEN:
         case BSP_PERIPH_ID_IOPBEN:
         case BSP_PERIPH_ID_IOPCEN:
         case BSP_PERIPH_ID_IOPDEN:
         case BSP_PERIPH_ID_IOPFEN:
         case BSP_PERIPH_ID_TSCEN:
              BSP_PERIPH_REG_RCC_AHBENR &= ~DEF_BIT(pwr_clk_id);
              break;

                                                                /* APB2 Enable Peripherals                              */
         case BSP_PERIPH_ID_SYSCFGCOMPEN:
         case BSP_PERIPH_ID_ADCEN:
         case BSP_PERIPH_ID_TIM1EN:
         case BSP_PERIPH_ID_SPI1EN:
         case BSP_PERIPH_ID_USART1EN:
         case BSP_PERIPH_ID_TIM15EN:
         case BSP_PERIPH_ID_TIM16EN:
         case BSP_PERIPH_ID_TIM17EN:
         case BSP_PERIPH_ID_DBGMCUEN:
              BSP_PERIPH_REG_RCC_APB2ENR &= ~DEF_BIT(pwr_clk_id - 32);
              break;

                                                                /* APB1 Enable Peripherals                              */
         case BSP_PERIPH_ID_TIM2EN:
         case BSP_PERIPH_ID_TIM3EN:
         case BSP_PERIPH_ID_TIM6EN:
         case BSP_PERIPH_ID_TIM14EN:
         case BSP_PERIPH_ID_WWDGEN:
         case BSP_PERIPH_ID_SPI2EN:
         case BSP_PERIPH_ID_USART2EN:
         case BSP_PERIPH_ID_I2C1EN:
         case BSP_PERIPH_ID_I2C2EN:
         case BSP_PERIPH_ID_PWREN:
         case BSP_PERIPH_ID_DACEN:
         case BSP_PERIPH_ID_CECEN:
              BSP_PERIPH_REG_RCC_APB1ENR &= ~DEF_BIT(pwr_clk_id - 64);
              break;
    }
}
