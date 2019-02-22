
/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 **************
 * 文件名  ：led.c
 * 描述    ：LED外设库函数   
 * 实验平台：风驰STM8开发板
 * 库版本  ：V2.1.0
 * 作者    ：风驰  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 * 修改时间 ：2012-10-20

  风驰STM8开发板硬件连接
    |--------------------|
    |  LED1-PD0          |
    |  LED2-PD1          |
    |  LED3-PD2          |
    |  LED4-PD3          |
    |--------------------|

*******************************************************************************/

/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "led.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */


/*******************************************************************************
 * 名称: LED_Init
 * 功能: LED外设GPIO引脚初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void LED_Init(void)
{
	GPIO_Init(LED_PORTC, LED_3, GPIO_MODE_OUT_PP_HIGH_FAST );	
	GPIO_Init(LED_PORTD,(LED_1|LED_2), GPIO_MODE_OUT_PP_HIGH_FAST );//定义LED的管脚为输出模式
}

/*******************************************************************************
 * 名称: SetLedOFF
 * 功能: 关闭所有LED灯
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void SetLedOFF(void)
{
	GPIO_WriteHigh(LED_PORTC, LED_3);
	GPIO_WriteHigh(LED_PORTD, LED_1);
	GPIO_WriteHigh(LED_PORTD, LED_2);
}
/*******************************************************************************
 * 名称: LED_Operation
 * 功能: LED 灯亮灭设置操作
 * 形参: ledx -> 要操作的led灯
 * 		 state -> 亮还是灭
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void LED_Operation(GPIO_Pin_TypeDef ledx, u8 state)
{
	if(ON == state) {
	  	switch (ledx) {
			case LED_1: 
			case LED_2:
		  		GPIO_WriteLow(LED_PORTD, ledx);
			break;
			
			case LED_3:
		  		GPIO_WriteLow(LED_PORTC, ledx);
			break;
	  	}
	} else {
	  	switch (ledx) {
			case LED_1: 
			case LED_2:
		  		GPIO_WriteHigh(LED_PORTD, ledx);
			break;
			
			case LED_3:
		  		GPIO_WriteHigh(LED_PORTC, ledx);
			break;
	  	}
	}
}

/*******************************************************************************
 * 名称: LED_ShowOneToOne
 * 功能: 宏定义的方式实现流水灯
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void LED_ShowOneToOne(void)
{
    LED_Operation(LED_1, ON);
    LED_Operation(LED_2, OFF);
    LED_Operation(LED_3, OFF);
    Delay(0x1ffff);
    LED_Operation(LED_1, OFF);
    LED_Operation(LED_2, ON);
    LED_Operation(LED_3, OFF);
    Delay(0x1ffff);
    LED_Operation(LED_1, OFF);
    LED_Operation(LED_2, OFF);
    LED_Operation(LED_3, ON);
    Delay(0x1ffff);

}

/*******************************************************************************
 * 名称: Delay
 * 功能: 简单的延时函数
 * 形参: nCount -> 延时时间数
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
static void Delay(u32 nCount)
{
	/* Decrement nCount value */
	while (nCount != 0)
	{
		nCount--;
	}
}


/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/