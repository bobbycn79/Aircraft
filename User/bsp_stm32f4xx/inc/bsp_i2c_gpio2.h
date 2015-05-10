/*
*********************************************************************************************************
*
*	ģ������ : I2C��������ģ��
*	�ļ����� : bsp_i2c_gpio.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ���
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_I2C_CCC_H
#define _BSP_I2C_CCC_H

#define I2C_WR	0		/* д����bit */
#define I2C_RD	1		/* ������bit */

void bsp_InitI2C2(void);
void i2c_Start2(void);
void i2c_Stop2(void);
void i2c_SendByte2(uint8_t _ucByte);
uint8_t i2c_ReadByte2(void);
uint8_t i2c_WaitAck2(void);
void i2c_Ack2(void);
void i2c_NAck2(void);
uint8_t i2c_CheckDevice2(uint8_t _Address);

#endif
