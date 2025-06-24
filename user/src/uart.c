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
//    LPUART_TransmitPoll(LPUART1, (uint8_t)(ch)); // 使用自定义串口发送函数
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

    /* 外设模块时钟使能 */
    SYSCTRL_PeriphClockEnable(PeriphClockLpuart1);

    /* LPUART 初始化 */
    LPUART_StcInit(&stcLpuartInit);                                    /* 结构体初始化         */
    stcLpuartInit.u32StopBits               = LPUART_STOPBITS_1;       /* 1停止位              */
    stcLpuartInit.u32FrameLength            = LPUART_FRAME_LEN_8B_PAR; /* 数据8位，奇偶校验1位 */
    stcLpuartInit.u32Parity                 = LPUART_B8_PARITY_EVEN;   /* 偶校验               */
    stcLpuartInit.u32TransMode              = LPUART_MODE_TX_RX;       /* 收发模式             */
    stcLpuartInit.stcBaudRate.u32SclkSelect = LPUART_SCLK_SEL_PCLK;    /* 传输时钟源           */
    stcLpuartInit.stcBaudRate.u32Sclk       = SYSCTRL_HclkFreqGet();   /* HCLK获取             */
    stcLpuartInit.stcBaudRate.u32Baud       = 9600;                    /* 波特率               */
    LPUART_Init(LPUART1, &stcLpuartInit);

    LPUART_IntFlagClearAll(LPUART1); /* 清除所有状态标志 */
}
static void uart_config()
{
		 stc_gpio_init_t stcGpioInit = {0};

    /* 外设模块时钟使能 */
    SYSCTRL_PeriphClockEnable(PeriphClockGpio);

    /* 配置PA08为LPUART1_TX */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Mode      = GPIO_MD_OUTPUT_PP;
    stcGpioInit.u32PullUp    = GPIO_PULL_UP;
    stcGpioInit.bOutputValue = TRUE;
    stcGpioInit.u32Pin       = GPIO_PIN_08;
    GPIO_Init(GPIOA, &stcGpioInit);
    GPIO_PA08_AF_LPUART1_TXD();

    /* 配置PA09为LPUART1_RX */
    GPIO_StcInit(&stcGpioInit);
    stcGpioInit.u32Mode = GPIO_MD_INPUT;
    stcGpioInit.u32Pin  = GPIO_PIN_09;
    GPIO_Init(GPIOA, &stcGpioInit);
    GPIO_PA09_AF_LPUART1_RXD();

}

void uart_init()
{
  /* 端口配置 */
    uart_config();

    /* LPUART配置 */
    LpUartConfig();

}
extern void log_printf(const char *fmt, ...);
#define log_putchar(c) do { \
    uint8_t ch = (uint8_t)(c); \
    LPUART_TransmitPoll(LPUART1, &ch, 1); \
} while(0)

#if 1

// 将整数转换为字符串并输出（支持负数）
static void log_num(int num, uint8_t dec)
{
     // 0的特殊处理
    if (num == 0) {
        log_putchar('0');
        return;
    }

    char buffer[33]; // 支持32位整数（含符号）
    uint8_t i = 0;
    uint8_t is_negative = 0;
    
    // 处理负数（仅10进制带符号）
    if (num < 0 && dec == 10) {
        is_negative = 1;
        // 避免最小负数溢出
        if (num == -2147483648) {
            memcpy(buffer, "-2147483648", 11);
            i = 11;
            goto OUTPUT; // 直接输出特殊值
        }
        num = -num;
    }
    
    // 处理十六进制负数（转为无符号）
    unsigned unum = (unsigned)num;
    if (dec == 16 && num < 0) {
        unum = (unsigned)num;
    }
    
    // 数字转换主循环
    while ((dec == 10 && num > 0) || (dec == 16 && unum > 0)) {
        uint8_t digit = (dec == 10) ? (num % dec) : (unum % dec);
        
        if (digit < 10) {
            buffer[i++] = digit + '0';
        } else {
            buffer[i++] = (digit - 10) + 'a'; // 十六进制小写字母
        }
        
        if (dec == 10) num /= dec;
        else unum /= dec;
    }
    
    // 添加负号
    if (is_negative) {
        buffer[i++] = '-';
    }
    
OUTPUT:
    // 反向输出
    while (i > 0) {
        log_putchar(buffer[--i]);
    }
}

// 简化版 printf
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
                char c = va_arg(args, int); // char 在可变参数中提升为 int
                log_putchar(c);
            }
            break;
            case '%':
            {
                log_putchar('%');
            }
            break;
            case 'x':   // 十六进制输出
            case 'X':   // 十六进制输出
            {
                unsigned int num = va_arg(args, unsigned int);
                log_num(num, 16);
            }
            break;
            default:
            {
                log_putchar('%'); // 输出未识别的格式符
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

