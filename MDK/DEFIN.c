#include "DEFINE.h"
#include "uart.h"

void GPIO_ExtiInit()
{
		 stc_gpio_init_t stcGpioInit1 = {0};

    GPIO_StcInit(&stcGpioInit1);                      /* �ṹ�������ʼֵ��ʼ�� */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);      /* ��GPIO����ʱ���ſ� */
		stcGpioInit1.u32Mode      = GPIO_MD_OUTPUT_PP;    /* �˿ڷ������� */
		stcGpioInit1.u32PullUp    = GPIO_PULL_NONE;       /* �˿��������� */
		stcGpioInit1.u32Pin       = PIN_NRST|PIN_XRST;         /* �˿��������� */
		GPIOA_Init(&stcGpioInit1);                        /* GPIO USER KEY��ʼ�� */
		GPIO_SetBits(PORT_NRST,PIN_XRST);
		 stc_gpio_init_t stcGpioInit2 = {0};
		GPIO_StcInit(&stcGpioInit2);
    stcGpioInit2.u32Mode      = GPIO_MD_INT_INPUT;    /* �˿ڷ������� */
    stcGpioInit2.u32PullUp    = GPIO_PULL_NONE;       /* �˿��������� */
    stcGpioInit2.u32ExternInt = GPIO_EXTI_FALLING;        /* �˿��ⲿ�жϴ�����ʽ���� */
		
    stcGpioInit2.u32Pin       = PIN_TOUCH_IRQ|PIN_READER_IRQ;         /* �˿��������� */
    GPIOA_Init(&stcGpioInit2);                        /* GPIO USER KEY��ʼ�� */
    EnableNvic(PORTA_IRQn, IrqPriorityLevel3, TRUE); /* ʹ�ܶ˿�PORTAϵͳ�ж� */
	
}
