#include "uart.h"

#include "sysctrl.h"
#include "lpm.h"
#include "lpuart.h"
#include "ddl.h"
#include "gpio.h"
#include "stdio.h"
#include <stdarg.h>
static void uart_config();
static void LpUartConfig(void);


//int fputc(int ch, FILE *f)
//{
// 
//    LPUART_TransmitPoll(LPUART1, (uint8_t)(ch)); // ʹ���Զ��崮�ڷ��ͺ���
// 
//  return ch;
// 
//}
uint8_t GPIO_Read(GPIO_TypeDef *GPIOx,uint16_t pin)
{
	
		return READ_REG_BIT(GPIOx->IN,pin);

}
	
void GPIO_ResetBits(GPIO_TypeDef *GPIOx,uint16_t pin)
{
			WRITE_REG(GPIOx->BCLR, pin);
}

void GPIO_SetBits(GPIO_TypeDef *GPIOx,uint16_t pin)
{
		WRITE_REG(GPIOx->BSET, pin);

}
static void LpUartConfig(void)
{
    stc_lpuart_init_t stcLpuartInit;

    /* ����ģ��ʱ��ʹ�� */
    SYSCTRL_PeriphClockEnable(PeriphClockLpuart1);

    /* LPUART ��ʼ�� */
    LPUART_StcInit(&stcLpuartInit);                                    /* �ṹ���ʼ��         */
    stcLpuartInit.u32StopBits               = LPUART_STOPBITS_1;       /* 1ֹͣλ              */
    stcLpuartInit.u32FrameLength            = LPUART_FRAME_LEN_8B_PAR; /* ����8λ����żУ��1λ */
    stcLpuartInit.u32Parity                 = LPUART_B8_PARITY_EVEN;   /* żУ��               */
    stcLpuartInit.u32TransMode              = LPUART_MODE_TX_RX;       /* �շ�ģʽ             */
    stcLpuartInit.stcBaudRate.u32SclkSelect = LPUART_SCLK_SEL_PCLK;    /* ����ʱ��Դ           */
    stcLpuartInit.stcBaudRate.u32Sclk       = SYSCTRL_HclkFreqGet();   /* HCLK��ȡ             */
    stcLpuartInit.stcBaudRate.u32Baud       = 9600;                    /* ������               */
    LPUART_Init(LPUART1, &stcLpuartInit);

    LPUART_IntFlagClearAll(LPUART1); /* �������״̬��־ */
}
static void uart_config()
{
		 stc_gpio_init_t stcGpioInit = {0};

    /* ����ģ��ʱ��ʹ�� */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* ����PA08ΪLPUART1_TX */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Mode      = GPIO_MD_OUTPUT_PP;
    stcGpioInit.u32PullUp    = GPIO_PULL_UP;
    stcGpioInit.bOutputValue = TRUE;
    stcGpioInit.u32Pin       = GPIO_PIN_08;
    GPIO_Init(GPIOA, &stcGpioInit);
    GPIO_PA08_AF_LPUART1_TXD();

    /* ����PA09ΪLPUART1_RX */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Mode = GPIO_MD_INPUT;
    stcGpioInit.u32Pin  = GPIO_PIN_09;
    GPIO_Init(GPIOA, &stcGpioInit);
    GPIO_PA09_AF_LPUART1_RXD();

}

void uart_init()
{
  /* �˿����� */
    uart_config();

    /* LPUART���� */
    LpUartConfig();

}
extern void log_printf(const char *fmt, ...);
#define log_putchar(c) do { \
    uint8_t ch = (uint8_t)(c); \
    LPUART_TransmitPoll(LPUART1, &ch, 1); \
} while(0)

#if 1

// ������ת��Ϊ�ַ����������֧�ָ�����
static void log_num(int num, uint8_t dec)
{
     // 0�����⴦��
    if (num == 0) {
        log_putchar('0');
        return;
    }

    char buffer[33]; // ֧��32λ�����������ţ�
    uint8_t i = 0;
    uint8_t is_negative = 0;
    
    // ����������10���ƴ����ţ�
    if (num < 0 && dec == 10) {
        is_negative = 1;
        // ������С�������
        if (num == -2147483648) {
            memcpy(buffer, "-2147483648", 11);
            i = 11;
            goto OUTPUT; // ֱ���������ֵ
        }
        num = -num;
    }
    
    // ����ʮ�����Ƹ�����תΪ�޷��ţ�
    unsigned unum = (unsigned)num;
    if (dec == 16 && num < 0) {
        unum = (unsigned)num;
    }
    
    // ����ת����ѭ��
    while ((dec == 10 && num > 0) || (dec == 16 && unum > 0)) {
        uint8_t digit = (dec == 10) ? (num % dec) : (unum % dec);
        
        if (digit < 10) {
            buffer[i++] = digit + '0';
        } else {
            buffer[i++] = (digit - 10) + 'a'; // ʮ������Сд��ĸ
        }
        
        if (dec == 10) num /= dec;
        else unum /= dec;
    }
    
    // ��Ӹ���
    if (is_negative) {
        buffer[i++] = '-';
    }
    
OUTPUT:
    // �������
    while (i > 0) {
        log_putchar(buffer[--i]);
    }
}

// �򻯰� printf
void log_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 'd':
            {
                int num = va_arg(args, int);
                log_num(num, 10);
            }
            break;
            case 's':
            {
                char *str = va_arg(args, char*);
                while (*str) log_putchar(*str++);
            }
            break;
            case 'c':
            {
                char c = va_arg(args, int); // char �ڿɱ����������Ϊ int
                log_putchar(c);
            }
            break;
            case '%':
            {
                log_putchar('%');
            }
            break;
            case 'x':   // ʮ���������
            case 'X':   // ʮ���������
            {
                unsigned int num = va_arg(args, unsigned int);
                log_num(num, 16);
            }
            break;
            default:
            {
                log_putchar('%'); // ���δʶ��ĸ�ʽ��
                log_putchar(*fmt);
            }
            break;
            }
        }
        else
        {
            log_putchar(*fmt);
        }
        fmt++;
    }
    va_end(args);
}
#endif

