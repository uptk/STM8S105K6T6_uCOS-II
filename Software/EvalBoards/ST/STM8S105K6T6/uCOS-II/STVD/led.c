
/******************** (C) COPYRIGHT  ���iCreateǶ��ʽ���������� **************
 * �ļ���  ��led.c
 * ����    ��LED����⺯��   
 * ʵ��ƽ̨�����STM8������
 * ��汾  ��V2.1.0
 * ����    �����  QQ��779814207
 * ����    ��
 * �Ա�    ��http://shop71177993.taobao.com/
 * �޸�ʱ�� ��2012-10-20

  ���STM8������Ӳ������
    |--------------------|
    |  LED1-PD0          |
    |  LED2-PD1          |
    |  LED3-PD2          |
    |  LED4-PD3          |
    |--------------------|

*******************************************************************************/

/* ����ϵͳͷ�ļ� */

/* �����Զ���ͷ�ļ� */
#include "led.h"

/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */


/*******************************************************************************
 * ����: LED_Init
 * ����: LED����GPIO���ų�ʼ������
 * �β�: ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void LED_Init(void)
{
	GPIO_Init(LED_PORTC, LED_3, GPIO_MODE_OUT_PP_HIGH_FAST );	
	GPIO_Init(LED_PORTD,(LED_1|LED_2), GPIO_MODE_OUT_PP_HIGH_FAST );//����LED�Ĺܽ�Ϊ���ģʽ
}

/*******************************************************************************
 * ����: SetLedOFF
 * ����: �ر�����LED��
 * �β�: ��
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void SetLedOFF(void)
{
	GPIO_WriteHigh(LED_PORTC, LED_3);
	GPIO_WriteHigh(LED_PORTD, LED_1);
	GPIO_WriteHigh(LED_PORTD, LED_2);
}
/*******************************************************************************
 * ����: LED_Operation
 * ����: LED ���������ò���
 * �β�: ledx -> Ҫ������led��
 * 		 state -> ��������
 * ����: ��
 * ˵��: �� 
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
 * ����: LED_ShowOneToOne
 * ����: �궨��ķ�ʽʵ����ˮ��
 * �β�: ��
 * ����: ��
 * ˵��: �� 
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
 * ����: Delay
 * ����: �򵥵���ʱ����
 * �β�: nCount -> ��ʱʱ����
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
static void Delay(u32 nCount)
{
	/* Decrement nCount value */
	while (nCount != 0)
	{
		nCount--;
	}
}


/******************* (C) COPYRIGHT ���iCreateǶ��ʽ���������� *****END OF FILE****/