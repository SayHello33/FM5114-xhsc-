#include "DEFINE.h"
#include "uart.h"

void GPIO_ExtiInit()
{
		 stc_gpio_init_t stcGpioInit1 = {0};

    GPIO_StcInit(&stcGpioInit1);                      /* 结构体变量初始值初始化 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);      /* 打开GPIO外设时钟门控 */
		stcGpioInit1.u32Mode      = GPIO_MD_OUTPUT_PP;    /* 端口方向配置 */
		stcGpioInit1.u32PullUp    = GPIO_PULL_NONE;       /* 端口上拉配置 */
		stcGpioInit1.u32Pin       = PIN_NRST|PIN_XRST;         /* 端口引脚配置 */
		GPIOA_Init(&stcGpioInit1);                        /* GPIO USER KEY初始化 */
		GPIO_SetBits(PORT_NRST,PIN_XRST);
		 stc_gpio_init_t stcGpioInit2 = {0};
		GPIO_StcInit(&stcGpioInit2);
    stcGpioInit2.u32Mode      = GPIO_MD_INT_INPUT;    /* 端口方向配置 */
    stcGpioInit2.u32PullUp    = GPIO_PULL_NONE;       /* 端口上拉配置 */
    stcGpioInit2.u32ExternInt = GPIO_EXTI_FALLING;        /* 端口外部中断触发方式配置 */
		
    stcGpioInit2.u32Pin       = PIN_TOUCH_IRQ|PIN_READER_IRQ;         /* 端口引脚配置 */
    GPIOA_Init(&stcGpioInit2);                        /* GPIO USER KEY初始化 */
    EnableNvic(PORTA_IRQn, IrqPriorityLevel3, TRUE); /* 使能端口PORTA系统中断 */
	
}
