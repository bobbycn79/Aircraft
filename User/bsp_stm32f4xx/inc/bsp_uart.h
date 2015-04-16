/*
*********************************************************************************************************
*	                                  
*	ģ������ : ��������ģ��    
*	�ļ����� : bsp_uart.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_UART_H
#define __BSP_UART_H

extern uint8_t U1TxBuffer[258];
extern uint8_t U1TxCounter;
extern uint8_t U1count; 



void UART1_Put_Char(unsigned char DataToSend);
void UART1_Put_String(unsigned char *Str);
void bsp_InitUart(void);
void UsartSendData(uint8_t mode);
#endif


