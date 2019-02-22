/*************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 ********************
 * 文件名  ：main.c
 * 描述    ：uCOS-V290 多任务测试程序   
 * 实验平台：风驰 iCreate STM8开发板 V2
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 修改时间 ：2012-12-28
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "includes.h"


/* Private defines -----------------------------------------------------------*/
/* 定义任务堆栈大小 */
#define  OS_TASK_0_STK_SIZE                128
#define  OS_TASK_1_STK_SIZE                128
#define  OS_TASK_2_STK_SIZE                128
#define  OS_TASK_3_STK_SIZE                128

/* 定义任务优先级 */
#define  OS_TASK_0_PRIO                    0
#define  OS_TASK_1_PRIO                    1
#define  OS_TASK_2_PRIO                    2
#define  OS_TASK_3_PRIO                    3

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* 定义任务栈空间 */
OS_STK	Task0Stack[OS_TASK_0_STK_SIZE];	
OS_STK	Task1Stack[OS_TASK_1_STK_SIZE];
OS_STK	Task2Stack[OS_TASK_2_STK_SIZE];
OS_STK	Task3Stack[OS_TASK_3_STK_SIZE];

/*	定义信号量	*/
OS_EVENT *sem_led1;
OS_EVENT *sem_led2;

void delay_xx(u16 j);
#if 0
/* LED灯闪烁任务1--LED1每隔1s闪烁 */
void LedTask1( void * pdata )
{
  	
  	Tim1_Init();	/* 初始化系统时钟节拍 */
	
	while(1)
	{
		LED_Operation(LED_1, ON);
		OSTimeDlyHMSM(0,0,1,0 );
		//delay_xx(100);
		LED_Operation(LED_1, OFF);
		OSTimeDlyHMSM(0,0,1,0 );
		//delay_xx(100);
	}
	
}
#else
void LedTask1( void * pdata )
{
  	
  	Tim1_Init();	/* 初始化系统时钟节拍 */
	
	while(1)
	{
		INT8U sem_err;
		OSSemPend(sem_led1,0,&sem_err);
		LED_Operation(LED_1, ON);
		OSTimeDlyHMSM(0,0,1,0 );
		LED_Operation(LED_1, OFF);
		//OSTimeDlyHMSM(0,0,1,0 );
		OSSemPost(sem_led1);
	}
	
}
#endif
#if 0
/* LED灯闪烁任务2--LED2每隔2s闪烁 */
void LedTask2(void * pdata )
{
  
	while(1)
	{
		LED_Operation(LED_2, ON);
		OSTimeDlyHMSM(0,0,1,0 );
		//delay_xx(100);
		LED_Operation(LED_2, OFF);
		OSTimeDlyHMSM(0,0,1,0 );
		//delay_xx(100);
	}
	
}
#else
/* LED灯闪烁任务2--LED2每隔2s闪烁 */
void LedTask2(void * pdata )
{
  
	while(1)
	{
		INT8U sem_err;
		OSSemPend(sem_led1,0,&sem_err);
		LED_Operation(LED_2, ON);
		OSTimeDlyHMSM(0,0,1,0 );
		LED_Operation(LED_2, OFF);
		//OSTimeDlyHMSM(0,0,1,0 );
		OSSemPost(sem_led1);
	}
	
}
#endif
/* LED灯闪烁任务3--LED3每隔3s闪烁 */
void LedTask3(void * pdata)
{
	while(1)
	{
		LED_Operation(LED_3, ON);
		OSTimeDlyHMSM(0,0,1,0 );
		//delay_xx(100);
		LED_Operation(LED_3, OFF);
		OSTimeDlyHMSM(0,0,1,0);
		//delay_xx(100);
	}	
}

/*	串口任务	*/
volatile u8 uart_rearptr;
volatile u8 uart_frontptr;
volatile u8 uart_rxbuf[32];
OS_EVENT *sem_uart;
void Uart_RxPrc(void)
{
	u8 ovr_sr = UART2->SR&UART2_FLAG_OR_LHE;
	u8 dat = UART2_ReceiveData8();
	
	//UART2->DR = dat;
	{
		#if 1
		if (OSRunning == OS_TRUE){
		u8 nxt_ptr = uart_frontptr+1;
		if(nxt_ptr>=sizeof(uart_rxbuf)){
			nxt_ptr = 0;
		}
		if(nxt_ptr!=uart_rearptr){
			//OSSemPost(sem_uart);
			uart_rxbuf[uart_frontptr] = dat;
			uart_frontptr = nxt_ptr;
		}
		}
		#endif
	}
}
void UartTask4(void * pdata)
{
	GPIO_Init(GPIOD,GPIO_PIN_6,GPIO_MODE_IN_PU_NO_IT);//RX
	GPIO_Init(GPIOD,GPIO_PIN_5,GPIO_MODE_OUT_PP_HIGH_FAST);//TX
	UART2_DeInit();
	UART2_Init(9600, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, UART2_PARITY_NO, UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TXRX_ENABLE);
	UART2_ITConfig(UART2_IT_RXNE_OR, ENABLE);
	UART2_Cmd(ENABLE);
	uart_rearptr = 0;
	uart_frontptr = 0;

	while(1)
	{
		#if 1
		INT8U sem_err;
		OSSemPend(sem_uart,0,&sem_err);
		UART2_SendData8(uart_rxbuf[uart_rearptr]);
		while(UART2_GetFlagStatus(UART2_FLAG_TXE )==RESET);
		if(uart_rearptr<sizeof(uart_rxbuf)-1)
			uart_rearptr++;
		else
			uart_rearptr = 0;
		#else
		UART2_SendData8(0x22);
		while(UART2_GetFlagStatus(UART2_FLAG_TXE )==RESET);
		OSTimeDlyHMSM(0,0,1,0);
		#endif
	}	
}
void SetLedON(void)
{
	GPIO_WriteLow(LED_PORTC, LED_3);
	GPIO_WriteLow(LED_PORTD, LED_1);
	GPIO_WriteLow(LED_PORTD, LED_2);
}

volatile u8 gbl8_tmp1;
volatile u8 gbl8_tmp2;

void delay_xx(u16 j)
{
	while(j>0){
		u16 i;
		for(i=1000;i>0;i--){
			;
		}
		j--;
	}

}
/*
inser main comment for test
test on branch experimental
*/

int main()
{
	//__disable_interrupt();
	_asm("sim");
	SystemClock_Init(HSI_Clock);
	LED_Init();
	SetLedOFF();
	//__enable_interrupt();
	#if 0
	//while(0)
	{
		#if 1
		SetLedON();
		delay_xx(100);
		SetLedOFF();
		delay_xx(100);
		#endif
	}
	#endif
	_asm("rim");
	
	OSInit();	/* 初始化uCOS系统 */
	sem_led2 = OSSemCreate(0);
	sem_led1 = OSSemCreate(1);
	sem_uart = OSSemCreate(0);
	/* 创建任务 */
	OSTaskCreate(LedTask1, (void *)OS_TASK_0_STK_SIZE, &Task0Stack[OS_TASK_0_STK_SIZE-1], OS_TASK_0_PRIO );
	OSTaskCreate(LedTask2, (void *)OS_TASK_1_STK_SIZE, &Task1Stack[OS_TASK_1_STK_SIZE-1], OS_TASK_1_PRIO );
	OSTaskCreate(LedTask3, (void *)OS_TASK_2_STK_SIZE, &Task2Stack[OS_TASK_2_STK_SIZE-1], OS_TASK_2_PRIO );
	OSTaskCreate(UartTask4, (void *)OS_TASK_3_STK_SIZE, &Task3Stack[OS_TASK_3_STK_SIZE-1], OS_TASK_3_PRIO );
	OSStart();	/* 启动uCOS系统 */
	
	return	0;
}





#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/
