#include <includes.h>

uint8_t U1TxBuffer[258];
uint8_t U1TxCounter=0;
uint8_t U1count=0; 

void bsp_InitUart(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/* ����1 TX = PA9   RX = PA10 */

	/* ��1���� ����GPIO */

	/* �� GPIO ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* �� UART ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	/* �� PA9 ӳ��Ϊ USART1_TX */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	/* �� PA10 ӳ��Ϊ USART1_RX */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	/* ���� USART Tx Ϊ���ù��� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ���� USART Rx Ϊ���ù��� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��2���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = 115200;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);		/* ʹ�ܴ��� */
	
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	/* ʹ�ܽ����ж� */
  USART_ClearFlag(USART1,USART_FLAG_TC);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//ʹ�ܽ����ж�	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void UART1_Put_Char(unsigned char DataToSend)
{
	U1TxBuffer[U1count++] = DataToSend;  
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);  
}

void UART1_Put_String(unsigned char *Str)
{

	while(*Str)
	{
		if(*Str=='\r')UART1_Put_Char(0x0d);
			else if(*Str=='\n')UART1_Put_Char(0x0a);
				else UART1_Put_Char(*Str);
		
		Str++;
	}
}

void UsartSendData(uint8_t mode)
{
	switch (mode)
	{
		case 0x25:
			UART1_Put_Char(0xA5);
			UART1_Put_Char(0x25);
			UART1_Put_Char((uint8_t)g_tMPU6050.Accel_X);
			UART1_Put_Char((uint8_t)(g_tMPU6050.Accel_X >> 8));
			UART1_Put_Char((uint8_t)g_tMPU6050.Accel_Y);
			UART1_Put_Char((uint8_t)(g_tMPU6050.Accel_Y >> 8));
			UART1_Put_Char((uint8_t)g_tMPU6050.Accel_Z);
			UART1_Put_Char((uint8_t)(g_tMPU6050.Accel_Z >> 8));
			UART1_Put_Char((uint8_t)g_tMPU6050.GYRO_X);
			UART1_Put_Char((uint8_t)(g_tMPU6050.GYRO_X >> 8));
			UART1_Put_Char((uint8_t)g_tMPU6050.GYRO_Y);
			UART1_Put_Char((uint8_t)(g_tMPU6050.GYRO_Y >> 8));
			UART1_Put_Char((uint8_t)g_tMPU6050.GYRO_Z);
			UART1_Put_Char((uint8_t)(g_tMPU6050.GYRO_Z >> 8));
			UART1_Put_Char(0x5A);
		break;
		case 0x45:
			UART1_Put_Char(0xA5);
			UART1_Put_Char(0x45);
			UART1_Put_Char((uint8_t)g_tMag.X);
			UART1_Put_Char((uint8_t)(g_tMag.X >> 8));
			UART1_Put_Char((uint8_t)g_tMag.Y);
			UART1_Put_Char((uint8_t)(g_tMag.Y >> 8));
			UART1_Put_Char((uint8_t)g_tMag.Z);
			UART1_Put_Char((uint8_t)(g_tMag.Z >> 8));
			UART1_Put_Char(0x5A);
		break;
	}
	
}
