#include <includes.h>
#include "flymain.h"



MPU6050_T g_tMPU6050;		/* ����һ��ȫ�ֱ���������ʵʱ���� */
MPU6050_T g_tMPU6050_Stk[5];
struct mpu6050_info MPU6050_H;
uint8_t MPU6050Flag = 0;  //MPU6050У׼��־λ
uint8_t MPU6050FlagOld = 0;  //MPU6050У׼��־λ
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitMPU6050
*	����˵��: ��ʼ��MPU-6050
*	��    ��:  ��
*	�� �� ֵ: 1 ��ʾ������ 0 ��ʾ������
*********************************************************************************************************
*/
void bsp_InitMPU6050(void)
{
	MPU6050_WriteByte(PWR_MGMT_1, 0x00);	//�������״̬
	MPU6050_WriteByte(SMPLRT_DIV, 0x07);  //���÷�Ƶ
	MPU6050_WriteByte(CONFIG, 0x04);      //���õ�ͨ�˲���
	MPU6050_WriteByte(GYRO_CONFIG, 0x18); //��������������
	MPU6050_WriteByte(ACCEL_CONFIG, 0x01);//��������Ϊ+-2g
	
	Mem_Set(&g_tMPU6050_Stk[0], 0x00, sizeof(g_tMPU6050_Stk));
	
	MPU6050_H.GYRO_X_Offset = -59.0f;
	MPU6050_H.GYRO_Y_Offset = 4.0f;
	MPU6050_H.GYRO_Z_Offset = 4.0f;
	MPU6050_H.Accel_X_Offset = 0.0f;
	MPU6050_H.Accel_Y_Offset = 0.0f;
	//MPU6050_H.Accel_Z_Offset = 17204.0f;
}

/*
*********************************************************************************************************
*	�� �� ��: MPU6050_WriteByte
*	����˵��: �� MPU-6050 �Ĵ���д��һ������
*	��    ��: _ucRegAddr : �Ĵ�����ַ
*			  _ucRegData : �Ĵ�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MPU6050_WriteByte(uint8_t _ucRegAddr, uint8_t _ucRegData)
{
    i2c_Start();							/* ���߿�ʼ�ź� */

    i2c_SendByte(MPU6050_SLAVE_ADDRESS);	/* �����豸��ַ+д�ź� */
	i2c_WaitAck();

    i2c_SendByte(_ucRegAddr);				/* �ڲ��Ĵ�����ַ */
	i2c_WaitAck();

    i2c_SendByte(_ucRegData);				/* �ڲ��Ĵ������� */
	i2c_WaitAck();

    i2c_Stop();                   			/* ����ֹͣ�ź� */
}

/*
*********************************************************************************************************
*	�� �� ��: MPU6050_ReadByte
*	����˵��: ��ȡ MPU-6050 �Ĵ���������
*	��    ��: _ucRegAddr : �Ĵ�����ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t MPU6050_ReadByte(uint8_t _ucRegAddr)
{
	uint8_t ucData;

	i2c_Start();                  			/* ���߿�ʼ�ź� */
	i2c_SendByte(MPU6050_SLAVE_ADDRESS);	/* �����豸��ַ+д�ź� */
	i2c_WaitAck();
	i2c_SendByte(_ucRegAddr);     			/* ���ʹ洢��Ԫ��ַ */
	i2c_WaitAck();

	i2c_Start();                  			/* ���߿�ʼ�ź� */

	i2c_SendByte(MPU6050_SLAVE_ADDRESS+1); 	/* �����豸��ַ+���ź� */
	i2c_WaitAck();

	ucData = i2c_ReadByte();       			/* �����Ĵ������� */
	i2c_NAck();
	i2c_Stop();                  			/* ����ֹͣ�ź� */
	return ucData;
}


/*
*********************************************************************************************************
*	�� �� ��: MPU6050_ReadData
*	����˵��: ��ȡ MPU-6050 ���ݼĴ����� ���������ȫ�ֱ��� g_tMPU6050.  ��������Զ�ʱ���øó���ˢ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MPU6050_ReadData(void)
{
	uint8_t ucReadBuf[14];
	uint8_t i;

#if 0 /* ������ */
	i2c_Start();                  			/* ���߿�ʼ�ź� */
	i2c_SendByte(MPU6050_SLAVE_ADDRESS);	/* �����豸��ַ+д�ź� */
	i2c_WaitAck();
	i2c_SendByte(ACCEL_XOUT_H);     		/* ���ʹ洢��Ԫ��ַ  */
	i2c_WaitAck();

	i2c_Start();                  			/* ���߿�ʼ�ź� */

	i2c_SendByte(MPU6050_SLAVE_ADDRESS + 1); /* �����豸��ַ+���ź� */
	i2c_WaitAck();

	for (i = 0; i < 13; i++)
	{
		ucReadBuf[i] = i2c_ReadByte();       			/* �����Ĵ������� */
		i2c_Ack();
	}

	/* �����һ���ֽڣ�ʱ�� NAck */
	ucReadBuf[13] = i2c_ReadByte();
	i2c_NAck();

	i2c_Stop();                  			/* ����ֹͣ�ź� */

#else	/* ���ֽڶ� */
	for (i = 0 ; i < 14; i++)
	{
		ucReadBuf[i] = MPU6050_ReadByte(ACCEL_XOUT_H + i);
	}
#endif

	/* �����������ݱ��浽ȫ�ֽṹ����� */
	g_tMPU6050.Accel_X = (ucReadBuf[0] << 8) + ucReadBuf[1];
	g_tMPU6050.Accel_Y = (ucReadBuf[2] << 8) + ucReadBuf[3];
	g_tMPU6050.Accel_Z = (ucReadBuf[4] << 8) + ucReadBuf[5];

	g_tMPU6050.Temp = (int16_t)((ucReadBuf[6] << 8) + ucReadBuf[7]);

	g_tMPU6050.GYRO_X = (ucReadBuf[8] << 8) + ucReadBuf[9];
	g_tMPU6050.GYRO_Y = (ucReadBuf[10] << 8) + ucReadBuf[11];
	g_tMPU6050.GYRO_Z = (ucReadBuf[12] << 8) + ucReadBuf[13];
}


void MPU6050_DataDeal(void)
{
	int32_t Accel_X_Temp,Accel_Y_Temp,Accel_Z_Temp,GYRO_X_Temp,GYRO_Y_Temp,GYRO_Z_Temp;
	
	g_tMPU6050_Stk[0] = g_tMPU6050_Stk[1];
	g_tMPU6050_Stk[1] = g_tMPU6050_Stk[2];	
	g_tMPU6050_Stk[2] = g_tMPU6050_Stk[3];
	g_tMPU6050_Stk[3] = g_tMPU6050_Stk[4];
	
	g_tMPU6050_Stk[4].Accel_X = (g_tMPU6050.Accel_X + g_tMPU6050_Stk[4].Accel_X) / 2;
	g_tMPU6050_Stk[4].Accel_Y = (g_tMPU6050.Accel_Y + g_tMPU6050_Stk[4].Accel_Y) / 2;
	g_tMPU6050_Stk[4].Accel_Z = (g_tMPU6050.Accel_Z + g_tMPU6050_Stk[4].Accel_Z) / 2;
	g_tMPU6050_Stk[4].GYRO_X = (g_tMPU6050.GYRO_X + g_tMPU6050_Stk[4].GYRO_X) / 2;
	g_tMPU6050_Stk[4].GYRO_Y = (g_tMPU6050.GYRO_Y + g_tMPU6050_Stk[4].GYRO_Y) / 2;
	g_tMPU6050_Stk[4].GYRO_Z = (g_tMPU6050.GYRO_Z + g_tMPU6050_Stk[4].GYRO_Z) / 2;
	
	//g_tMPU6050_Stk[4] = g_tMPU6050;

	Accel_X_Temp = g_tMPU6050_Stk[0].Accel_X - MPU6050_H.Accel_X_Offset + 
														g_tMPU6050_Stk[1].Accel_X - MPU6050_H.Accel_X_Offset + 
														g_tMPU6050_Stk[2].Accel_X - MPU6050_H.Accel_X_Offset + 
														g_tMPU6050_Stk[3].Accel_X - MPU6050_H.Accel_X_Offset + 
														g_tMPU6050_Stk[4].Accel_X - MPU6050_H.Accel_X_Offset;
		
	Accel_Y_Temp = g_tMPU6050_Stk[0].Accel_Y - MPU6050_H.Accel_Y_Offset +		
														g_tMPU6050_Stk[1].Accel_Y - MPU6050_H.Accel_Y_Offset +	
														g_tMPU6050_Stk[2].Accel_Y - MPU6050_H.Accel_Y_Offset +	
														g_tMPU6050_Stk[3].Accel_Y - MPU6050_H.Accel_Y_Offset +	
														g_tMPU6050_Stk[4].Accel_Y - MPU6050_H.Accel_Y_Offset;
		
	Accel_Z_Temp = g_tMPU6050_Stk[0].Accel_Z - MPU6050_H.Accel_Z_Offset +		
														g_tMPU6050_Stk[1].Accel_Z - MPU6050_H.Accel_Z_Offset +	
														g_tMPU6050_Stk[2].Accel_Z - MPU6050_H.Accel_Z_Offset +	
														g_tMPU6050_Stk[3].Accel_Z - MPU6050_H.Accel_Z_Offset +	
														g_tMPU6050_Stk[4].Accel_Z - MPU6050_H.Accel_Z_Offset;	
		
	GYRO_X_Temp = g_tMPU6050_Stk[0].GYRO_X - MPU6050_H.GYRO_X_Offset +
	                         g_tMPU6050_Stk[1].GYRO_X - MPU6050_H.GYRO_X_Offset +
	                         g_tMPU6050_Stk[2].GYRO_X - MPU6050_H.GYRO_X_Offset +
	                         g_tMPU6050_Stk[3].GYRO_X - MPU6050_H.GYRO_X_Offset +
	                         g_tMPU6050_Stk[4].GYRO_X - MPU6050_H.GYRO_X_Offset;
	
	GYRO_Y_Temp = g_tMPU6050_Stk[0].GYRO_Y - MPU6050_H.GYRO_Y_Offset +
	                         g_tMPU6050_Stk[1].GYRO_Y - MPU6050_H.GYRO_Y_Offset +
	                         g_tMPU6050_Stk[2].GYRO_Y - MPU6050_H.GYRO_Y_Offset +
	                         g_tMPU6050_Stk[3].GYRO_Y - MPU6050_H.GYRO_Y_Offset +
	                         g_tMPU6050_Stk[4].GYRO_Y - MPU6050_H.GYRO_Y_Offset;
													 
	GYRO_Z_Temp = g_tMPU6050_Stk[0].GYRO_Z - MPU6050_H.GYRO_Z_Offset +
	                         g_tMPU6050_Stk[1].GYRO_Z - MPU6050_H.GYRO_Z_Offset +
	                         g_tMPU6050_Stk[2].GYRO_Z - MPU6050_H.GYRO_Z_Offset +
	                         g_tMPU6050_Stk[3].GYRO_Z - MPU6050_H.GYRO_Z_Offset +
	                         g_tMPU6050_Stk[4].GYRO_Z - MPU6050_H.GYRO_Z_Offset;
	
		
	MPU6050_H.Accel_X = Accel_X_Temp / 5.0f;
	MPU6050_H.Accel_Y = Accel_Y_Temp / 5.0f;
	MPU6050_H.Accel_Z = Accel_Z_Temp / 5.0f;
	MPU6050_H.GYRO_X = GYRO_X_Temp / 5.0f;
	MPU6050_H.GYRO_Y = GYRO_Y_Temp / 5.0f;
	MPU6050_H.GYRO_Z = GYRO_Z_Temp / 5.0f;
	
}

