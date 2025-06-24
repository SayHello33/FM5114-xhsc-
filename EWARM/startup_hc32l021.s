;*******************************************************************************
; Copyright (C) 2024, Xiaohua Semiconductor Co., Ltd. All rights reserved.
;
; This software component is licensed by XHSC under BSD 3-Clause license
; (the "License"); You may not use this file except in compliance with the
; License. You may obtain a copy of the License at:
;                    opensource.org/licenses/BSD-3-Clause
;
;/
;/*****************************************************************************/
;/*  Startup for IAR                                                          */
;/*  Version     V1.0                                                         */
;/*  Date        2024-12-01                                                   */
;/*  Target-mcu  M0+ Device                                                   */
;/*****************************************************************************/


                MODULE  ?cstartup

                ;; Forward declaration of sections.
                SECTION CSTACK:DATA:NOROOT(3)

                EXTERN  __iar_program_start
                EXTERN  SystemInit
                PUBLIC  __vector_table
                
                SECTION .intvec:CODE:ROOT(8)
                DATA
__vector_table  
                DCD     sfe(CSTACK)               ; Top of Stack
                DCD     Reset_Handler             ; Reset
                DCD     NMI_Handler               ; NMI
                DCD     HardFault_Handler         ; Hard Fault
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV
                DCD     SysTick_Handler           ; SysTick

; Numbered IRQ handler vectors

; Note: renaming to device dependent ISR function names are done in

                DCD     PORTA_IRQHandler          ;0
                DCD     PORTB_IRQHandler          ;1
                DCD     0                         
                DCD     0                         
                DCD     0                         
                DCD     ATIM3_IRQHandler          ;5
                DCD     0                         
                DCD     0                         
                DCD     LPUART0_IRQHandler        ;8
                DCD     LPUART1_IRQHandler        ;9
                DCD     SPI_IRQHandler            ;10
                DCD     0                         
                DCD     0                         
                DCD     0                         
                DCD     CTIM0_IRQHandler          ;14
                DCD     CTIM1_IRQHandler          ;15
                DCD     0                         
                DCD     0                         
                DCD     HSI2C_IRQHandler          ;18
                DCD     0                         
                DCD     0                         
                DCD     0                         
                DCD     IWDT_IRQHandler           ;22
                DCD     RTC_IRQHandler            ;23
                DCD     ADC_IRQHandler            ;24
                DCD     0                         
                DCD     VC0_IRQHandler            ;26
                DCD     VC1_IRQHandler            ;27
                DCD     LVD_IRQHandler            ;28
                DCD     0                         
                DCD     FLASH_IRQHandler          ;30
                DCD     CTRIM_CLKDET_IRQHandler   ;31


                THUMB

                PUBWEAK Reset_Handler
                SECTION .text:CODE:NOROOT:REORDER(2)
Reset_Handler
                ;reset NVIC if in rom debug
                LDR     R0, =0x20000000
                LDR     R2, =0x0              ; vector offset
                cmp     PC, R0
                bls     ROMCODE
              
              ; ram code base address. 
                ADD     R2, R0,R2
ROMCODE
              ; reset Vector table address.
                LDR     R0, =0xE000ED08
                STR     R2, [R0]
                
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__iar_program_start
                BX      R0

                PUBWEAK NMI_Handler
                SECTION .text:CODE:NOROOT:REORDER(1)
NMI_Handler
                B       NMI_Handler

                PUBWEAK HardFault_Handler
                SECTION .text:CODE:NOROOT:REORDER(1)
HardFault_Handler
                B       HardFault_Handler


                PUBWEAK SVC_Handler
                SECTION .text:CODE:NOROOT:REORDER(1)
SVC_Handler
                B       SVC_Handler

                PUBWEAK PendSV_Handler
                SECTION .text:CODE:NOROOT:REORDER(1)
PendSV_Handler
                B       PendSV_Handler

                PUBWEAK SysTick_Handler
                SECTION .text:CODE:NOROOT:REORDER(1)
SysTick_Handler
                B       SysTick_Handler

                PUBWEAK PORTA_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
PORTA_IRQHandler
                B       PORTA_IRQHandler

                PUBWEAK PORTB_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
PORTB_IRQHandler
                B       PORTB_IRQHandler

                PUBWEAK ATIM3_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
ATIM3_IRQHandler
                B       ATIM3_IRQHandler

                PUBWEAK LPUART0_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
LPUART0_IRQHandler
                B       LPUART0_IRQHandler


                PUBWEAK LPUART1_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
LPUART1_IRQHandler
                B       LPUART1_IRQHandler

                PUBWEAK SPI_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
SPI_IRQHandler
                B       SPI_IRQHandler

                PUBWEAK CTIM0_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
CTIM0_IRQHandler
                B       CTIM0_IRQHandler

                PUBWEAK CTIM1_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
CTIM1_IRQHandler
                B       CTIM1_IRQHandler

                PUBWEAK HSI2C_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
HSI2C_IRQHandler
                B       HSI2C_IRQHandler

                PUBWEAK IWDT_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
IWDT_IRQHandler
                B       IWDT_IRQHandler

                PUBWEAK RTC_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
RTC_IRQHandler
                B       RTC_IRQHandler

                PUBWEAK ADC_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
ADC_IRQHandler
                B       ADC_IRQHandler

                PUBWEAK VC0_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
VC0_IRQHandler
                B       VC0_IRQHandler

                PUBWEAK VC1_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
VC1_IRQHandler
                B       VC1_IRQHandler

                PUBWEAK LVD_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
LVD_IRQHandler
                B       LVD_IRQHandler

                PUBWEAK FLASH_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
FLASH_IRQHandler
                B       FLASH_IRQHandler

                PUBWEAK CTRIM_CLKDET_IRQHandler
                SECTION .text:CODE:NOROOT:REORDER(1)
CTRIM_CLKDET_IRQHandler
                B       CTRIM_CLKDET_IRQHandler                
                
                END
