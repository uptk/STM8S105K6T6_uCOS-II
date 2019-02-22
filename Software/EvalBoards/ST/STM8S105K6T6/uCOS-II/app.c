/*
*********************************************************************************************************
*                                              EXAMPLE CODE
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
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                          STM32F0518-EVAL
*                                         Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : MD
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* --------------- APPLICATION GLOBALS ---------------- */
static  OS_STK          AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK          AppTaskLED2Stk[APP_CFG_TASK_LED2_STK_SIZE];
static  OS_STK          AppTaskLED3Stk[APP_CFG_TASK_LED3_STK_SIZE];
static  OS_STK          AppTaskLED4Stk[APP_CFG_TASK_LED4_STK_SIZE];

        OS_TMR         *AppLED3Tmr;
        OS_EVENT       *AppLED3Sem;

        OS_TMR         *AppLED4Tmr;
        OS_EVENT       *AppLED4Q;

        CPU_INT08U      AppLEDNbr;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart              (void        *p_arg);
static  void  AppTaskLED2               (void        *p_arg);   /* Toggle timer for LED 2                               */
static  void  AppTaskLED3               (void        *p_arg);   /* Toggle timer for LED 3                               */
static  void  AppTaskLED4               (void        *p_arg);   /* Toggle timer for LED 4                               */
static  void  AppEventCreate            (void);
static  void  AppTaskCreate             (void);
                                                                /* Timer callback functions for LED applications        */
        void  AppLED3TmrCallBack        (OS_TMR *p_tmr,
                                         void   *p_arg);

        void  AppLED4TmrCallBack        (OS_TMR *p_tmr,
                                         void   *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{
    CPU_INT08U  os_err;

    //CPU_Init();

    //BSP_IntDisAll();                                            /* Disable all interrupts.                              */

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"          */

    OSTaskCreateExt((void (*)(void *)) AppTaskStart,            /* Create the start task                                */
                    (void           *) 0,
                    (OS_STK         *)&AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&AppTaskStartStk[0],
                    (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));


    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

    return (1);
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;


    (void)p_arg;                                                /* Note #1                                             */


    OS_CPU_SysTickInit(OS_TICKS_PER_SEC);                       /* Initialize tick counter                              */


    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */


    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts         = cpu_clk_freq                                 /* Determine nbr SysTick increments                     */
                 / (CPU_INT32U)OS_TICKS_PER_SEC;

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */


#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                               */
#endif


    APP_TRACE_DBG(("\n\n\r"));
    APP_TRACE_INFO(("Creating Application Events...\n\r"));
    AppEventCreate();                                           /* Create Application Events                             */

    APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
    AppTaskCreate();                                            /* Create application tasks                              */


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */

        BSP_LED_Toggle(1);
        OSTimeDlyHMSM(0, 0, 0, 500);

        }
}


/*
*********************************************************************************************************
*                                      AppTaskEventCreate()
*
* Description : Create the application Events
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppEventCreate (void)
{
    CPU_INT08U  os_err;

    AppLED3Sem = OSSemCreate(0);                                /* Activate semaphore for LED3 task                     */

#if (OS_EVENT_NAME_EN > 0)
    OSEventNameSet((OS_EVENT    *)AppLED3Sem,
                   (INT8U       *)"LED3 Semaphore",
                   (INT8U       *)&os_err);
#endif

    AppLED4Q = OSQCreate((void  *)&AppLEDNbr,                   /* Create queue for LEd 4 toggle                        */
                         (INT8U  )4);

#if (OS_EVENT_NAME_EN > 0)
    OSEventNameSet((OS_EVENT    *)AppLED4Q,
                   (INT8U       *)"LED 4 Message",
                   (INT8U       *)&os_err);
#endif


    AppLED3Tmr = OSTmrCreate((INT32U          ) 4,
                             (INT32U          ) 400,
                             (INT8U           ) OS_TMR_OPT_PERIODIC,
                             (OS_TMR_CALLBACK ) AppLED3TmrCallBack,
                             (void           *) 0,
                             (INT8U          *) "LEDTimer3",
                             (INT8U          *) &os_err);

    AppLED4Tmr = OSTmrCreate((INT32U            )1,             /* Create timer event for utilizing message queue       */
                             (INT32U            )10,
                             (INT8U             )OS_TMR_OPT_PERIODIC,
                             (OS_TMR_CALLBACK   )AppLED4TmrCallBack,
                             (void             *)0,
                             (INT8U            *)"Blink LED 4 timer",
                             (INT8U            *)&os_err);

}


/*
*********************************************************************************************************
*                                      AppTaskCreate()
*
* Description : Create the application tasks.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
#if (OS_TASK_NAME_EN > 0)
    CPU_INT08U  os_err;
#endif

    OSTaskCreateExt((void (*)(void   *))AppTaskLED2,            /* Create the task for LED2 to blink                    */
                    (void            *) 0,
                    (OS_STK          *)&AppTaskLED2Stk[APP_CFG_TASK_LED2_STK_SIZE - 1],
                    (INT8U            ) APP_CFG_TASK_LED2_PRIO,
                    (INT16U           ) APP_CFG_TASK_LED2_PRIO,
                    (OS_STK          *)&AppTaskLED2Stk[0],
                    (INT32U           ) APP_CFG_TASK_LED2_STK_SIZE,
                    (void            *) 0,
                    (INT16U           ) OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);


#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet((INT8U  )APP_CFG_TASK_LED2_PRIO,
                  (INT8U *)"AppTaskLED2",
                  (INT8U *)&os_err);
#endif

    OSTaskCreateExt((void (*)(void   *))AppTaskLED3,            /* Create the task for LED3 semaphore blinking          */
                    (void            *) 0,
                    (OS_STK          *)&AppTaskLED3Stk[APP_CFG_TASK_LED3_STK_SIZE - 1],
                    (INT8U            ) APP_CFG_TASK_LED3_PRIO,
                    (INT16U           ) APP_CFG_TASK_LED3_PRIO,
                    (OS_STK          *)&AppTaskLED3Stk[0],
                    (INT32U           ) APP_CFG_TASK_LED3_STK_SIZE,
                    (void            *) 0,
                    (INT16U           )OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

#if  (OS_TASK_NAME_EN > 0)
     OSTaskNameSet((INT8U   )APP_CFG_TASK_LED3_PRIO,
                   (INT8U  *)"App Task LED3",
                   (INT8U  *)&os_err);
#endif

    OSTaskCreateExt((void (*)(void   *))AppTaskLED4,            /* Task creation for LED 4 queue control with LED 3 too */
                    (void            *) 0,
                    (OS_STK          *)&AppTaskLED4Stk[APP_CFG_TASK_LED4_STK_SIZE - 1],
                    (INT8U            ) APP_CFG_TASK_LED4_PRIO,
                    (INT16U           ) APP_CFG_TASK_LED4_PRIO,
                    (OS_STK          *)&AppTaskLED4Stk[0],
                    (INT32U           ) APP_CFG_TASK_LED4_STK_SIZE,
                    (void            *) 0,
                    (INT16U           ) OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

#if  (OS_TASK_NAME_EN > 0)
     OSTaskNameSet((INT8U   )APP_CFG_TASK_LED4_PRIO,
                   (INT8U  *)"App Task LED4",
                   (INT8U  *)&os_err);
#endif

}


/*
*********************************************************************************************************
*                                         AppTaskLED2()
*
* Description : Task to activate and toggle LED2 after timer tick system expires.
*
* Argument(s) : p_arg   is the argument passed to 'AppTaskLED2()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskLED2(void *p_arg)
{
    (void)p_arg;

    while(DEF_TRUE) {
        BSP_LED_Toggle(2);
        OSTimeDlyHMSM(0, 0, 0, 300);
    }
}


/*
*********************************************************************************************************
*                                         AppTaskLED3()
*
* Description : Toggle LED3 using semaphore services.
*
* Argument(s) : p_arg   is the argument passed to 'AppTaskLED3()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : Calls the LED function to activate LED 3.
*********************************************************************************************************
*/


static  void  AppTaskLED3(void *p_arg)
{
    CPU_BOOLEAN  status;
    INT8U        os_err;


    (void)p_arg;

    while (DEF_TRUE) {
        status = OSTmrStart(AppLED3Tmr, &os_err);
        if (status != DEF_OK) {
            while (DEF_TRUE) {
                ;
            }
        } else {
            OSSemPend((OS_EVENT *)AppLED3Sem,
                      (INT32U    )0,
                      (INT8U    *)&os_err);
        }

        BSP_LED_Toggle(3);
    }

}

void  AppLED3TmrCallBack (OS_TMR   *p_tmr,
                          void     *p_arg)
{
    OSSemPost((OS_EVENT *)AppLED3Sem);
}


/*
*********************************************************************************************************
*                                         AppTaskLED4()
*
* Description : Toggle LED3 and LED4 using a message queue service.
*
* Argument(s) : p_arg   is the argument passed to 'AppTaskLED4()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : Calls the LED function to activate LED 4 after timer expires.
*********************************************************************************************************
*/

static  void  AppTaskLED4 (void *p_arg)
{
    CPU_INT32U      *p_msg;
    CPU_INT08U       led_nbr;
    CPU_BOOLEAN      status;
    INT8U            os_err;


    (void)p_arg;

    AppLEDNbr = 2;
    status = OSTmrStart(AppLED4Tmr, &os_err);
    if (status != DEF_OK) {
        while (DEF_TRUE) {
                ;
        }
    }
    while (DEF_TRUE) {
        if (AppLEDNbr == 5) {
          AppLEDNbr = 3;
        }

        p_msg = (CPU_INT32U *)(OSQPend((OS_EVENT    *)AppLED4Q,
                                   (INT8U        )0,
                                   (INT8U       *)&os_err));

        if (os_err == OS_ERR_NONE) {
            led_nbr = (CPU_INT08U)*p_msg;
        }

    BSP_LED_Toggle(led_nbr);
    AppLEDNbr++;
    }
}

void  AppLED4TmrCallBack(OS_TMR *p_tmr,
                         void   *p_arg)
{
   OSQPost((OS_EVENT   *)AppLED4Q,
            (void       *)&AppLEDNbr);
}
