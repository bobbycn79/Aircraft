#include <includes.h>

#define WIFI_TIMEOUT 20
#define COM_ESP8266	COM1		/* ѡ�񴮿� */
#define ESP8266_TMR_ID 3
#define AT_CR		'\r'
#define AT_LF		'\n'

enum WIFI_STATUS WifiStatus;  //WIFI����״̬

char ESP8266_rx_data[256] = {0};
char ESP8266_tx_data[256] = {0};
char ESP8266_current_mode = 0;
char *ptr_temp;

uint8_t ESP8266_connect_flag = 0;
struct ip_infomation ESP8266_connect[4];
extern BSP_OS_SEM wifi_send_sem;

OS_ERR err;
extern uint16_t LedDalay;
uint8_t ESP8266_WaitResponse(char *_pAckStr, uint16_t _usTimeOut)
{
	uint8_t ucData;
	uint8_t ucRxBuf[256];
	uint16_t pos = 0;
	uint32_t len;
	uint8_t ret;

	len = strlen(_pAckStr);
	if (len > 255)
	{
		return 0;
	}

	/* _usTimeOut == 0 ��ʾ���޵ȴ� */
	if (_usTimeOut > 0)
	{
		bsp_StartTimer(ESP8266_TMR_ID, _usTimeOut);		/* ʹ�������ʱ��3����Ϊ��ʱ���� */
	}
	while (1)
	{
		bsp_Idle();				/* CPU����ִ�еĲ����� �� bsp.c �� bsp.h �ļ� */

		if (_usTimeOut > 0)
		{
			if (bsp_CheckTimer(ESP8266_TMR_ID))
			{
				ret = 0;	/* ��ʱ */
				break;
			}
		}

		if (comGetChar(COM_ESP8266, &ucData))
		{
		//	ESP8266_PrintRxData(ucData);		/* �����յ����ݴ�ӡ�����Դ���1 */

			if (ucData == '\n')
			{
				if (pos > 0)	/* ��2���յ��س����� */
				{
					if (memcmp(ucRxBuf, _pAckStr,  len) == 0)
					{
						ret = 1;	/* �յ�ָ����Ӧ�����ݣ����سɹ� */
						break;
					}
					else
					{
						pos = 0;
					}
				}
				else
				{
					pos = 0;
				}
			}
			else
			{
				if (pos < sizeof(ucRxBuf))
				{
					/* ֻ����ɼ��ַ� */
					if (ucData >= ' ')
					{
						ucRxBuf[pos++] = ucData;
					}
				}
			}
		}
	}
	return ret;
}



uint16_t ESP8266_ReadResponse(char *_pBuf, uint16_t _usBufSize, uint16_t _usTimeOut)
{
	uint8_t ucData;
	uint16_t pos = 0;
	uint8_t ret;
	uint8_t status = 0;		/* ����״̬ */

	/* _usTimeOut == 0 ��ʾ���޵ȴ� */
	if (_usTimeOut > 0)
	{
		bsp_StartTimer(ESP8266_TMR_ID, _usTimeOut);		/* ʹ�������ʱ����Ϊ��ʱ���� */
	}
	while (1)
	{
		bsp_Idle();				/* CPU����ִ�еĲ����� �� bsp.c �� bsp.h �ļ� */

		if (status == 2)		/* ���ڽ�����ЧӦ��׶Σ�ͨ���ַ��䳬ʱ�ж����ݽ������ */
		{
			if (bsp_CheckTimer(ESP8266_TMR_ID))
			{
				_pBuf[pos]	 = 0;	/* ��β��0�� ���ں���������ʶ���ַ������� */
				ret = pos;		/* �ɹ��� �������ݳ��� */
				break;
			}
		}
		else
		{
			if (_usTimeOut > 0)
			{
				if (bsp_CheckTimer(ESP8266_TMR_ID))
				{
					ret = 0;	/* ��ʱ */
					break;
				}
			}
		}

		if (comGetChar(COM_ESP8266, &ucData))
		{
		//	ESP8266_PrintRxData(ucData);		/* �����յ����ݴ�ӡ�����Դ���1 */

			switch (status)
			{
				case 0:			/* ���ַ� */
					if (ucData == AT_CR)		/* ������ַ��ǻس�����ʾ AT������� */
					{
						_pBuf[pos++] = ucData;		/* ������յ������� */
						status = 2;	 /* ��Ϊ�յ�ģ��Ӧ���� */
					}
					else	/* ���ַ��� A ��ʾ AT������� */
					{
						status = 1;	 /* �����������͵�AT�����ַ�����������Ӧ�����ݣ�ֱ������ CR�ַ� */
					}
					break;

				case 1:			/* AT������Խ׶�, ����������. �����ȴ� */
					if (ucData == AT_CR)
					{
						status = 2;
					}
					break;

				case 2:			/* ��ʼ����ģ��Ӧ���� */
					/* ֻҪ�յ�ģ���Ӧ���ַ���������ַ��䳬ʱ�жϽ�������ʱ�����ܳ�ʱ�������� */
					bsp_StartTimer(ESP8266_TMR_ID, 5);
					if (pos < _usBufSize - 1)
					{
						_pBuf[pos++] = ucData;		/* ������յ������� */
					}
					break;
			}
		}
	}
	return ret;
}



void ESP8266_SendAT(char *_Cmd)
{
	comSendBuf(COM1, (uint8_t *)_Cmd, strlen(_Cmd));
	comSendBuf(COM1, "\r\n", 2);
}

void ESP8266_Reset(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	Mem_Set(&GPIO_InitStructure, 0x00, sizeof(GPIO_InitStructure));
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* ����GPIO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
	BSP_OS_TimeDlyMs(100);
	GPIO_SetBits(GPIOA, GPIO_Pin_11);
	BSP_OS_TimeDlyMs(100);

	
	
	
	comClearRxFifo(COM_ESP8266);
}




char data_temp[10];
extern uint16_t SetPwmValue[4];
extern uint16_t SetGeneralReinforce;
extern uint8_t  SetPwmDirection[4];
void AppCommTask(void *p_arg)
{
	uint8_t i;
	WifiStatus = IDLE;

	(void)p_arg;
	
	while(1)
	{
		switch (WifiStatus)
		{
			case IDLE:
				ESP8266_SendAT("AT+RST"); //���͡�AT+RST��
				if(ESP8266_WaitResponse("OK", 2000))
					WifiStatus = AP_BEGIN;
					
			break;
			case AP_BEGIN:
				if(ESP8266_connect_flag == 0)
				{
					while(1)  //����WIFI��APģʽ
					{
						Mem_Clr(ESP8266_rx_data, sizeof(ESP8266_rx_data));
						ESP8266_SendAT("AT+CWMODE=2"); //���͡�AT+RST?
						ESP8266_ReadResponse(ESP8266_rx_data, sizeof(ESP8266_rx_data), 2000);
						if(strstr(ESP8266_rx_data, "no change") != NULL) //��ѯ��ǰWIFIģʽ�Ƿ�ΪAPģʽ
							break; 
						else if(strstr(ESP8266_rx_data, "OK") != NULL)  //ת�����
							break;
					}
					while(1)  //���³�ʼ��WIFIģ��
					{
						ESP8266_SendAT("AT+RST"); //���͡�AT+RST��
						if(ESP8266_WaitResponse("OK", 2000))
						{
							ESP8266_current_mode = 2;
							break;
						}
					}
					while(1)  //����AP�ڵ�
					{
						ESP8266_SendAT("AT+CWSAP=\"XiaoSiZhou\",\"123456789\",1,3"); //���͡�AT+RST��
						if(ESP8266_WaitResponse("OK", 2000))
						{
							break;
						}
					}					
					while(1)  //��ѯ��ڵ�����
					{
						Mem_Clr(ESP8266_rx_data, sizeof(ESP8266_rx_data));
						ESP8266_SendAT("AT+CIPMUX=1"); //���͡�AT+CIPMUX=1��
						if(ESP8266_WaitResponse("OK", 2000))
						{
							ESP8266_SendAT("AT+CIPMUX?");
							if(ESP8266_WaitResponse("+CIPMUX:1", 2000))
							{
								break;
							}
						}
					}	
					while(1) //����������
					{
						ESP8266_SendAT("AT+CIPSERVER=1,8080"); //
						if(ESP8266_WaitResponse("OK", 2000))
						{
							break;
						}
					}
					while(1) //���÷�������ʱʱ��
					{
						ESP8266_SendAT("AT+CIPSTO=2880"); //���͡�AT+CIPSTO=2880��
						if(ESP8266_WaitResponse("OK", 2000))
						{
							ESP8266_connect_flag = 1;
							break;
						}		
					}
				}
				else if(ESP8266_connect_flag == 1)
				{
					
					Mem_Clr(ESP8266_rx_data, sizeof(ESP8266_rx_data));
					ESP8266_SendAT("AT+CIPSTATUS"); //ESP8266��ѯ��ǰ����״̬
					ESP8266_ReadResponse(ESP8266_rx_data, sizeof(ESP8266_rx_data), 2000);
					Mem_Clr(&ESP8266_connect[0], sizeof(struct ip_infomation) * 4);
					
					if(strstr(ESP8266_rx_data, "+CIPSTATUS:") != NULL)
					{
						WifiStatus = CONNECTING;
					}
				
				}
			break;
			case CONNECTING:
				BSP_OS_SemWait(&wifi_send_sem, 20000);
			
				ESP8266_ReadResponse(ESP8266_rx_data, sizeof(ESP8266_rx_data), 1000);
				ptr_temp = strstr(ESP8266_rx_data, "sync"); //���յ�ͬ����
				if(ptr_temp != NULL)
				{
					switch(*(ptr_temp+4))
					{
						case 'c':
							MPU6050Flag |= CALI_MODE;  //MPU6050У׼ģʽ
						break;
						
						case 'm':
							for(i=0; i<4; i++)
							{
								strncpy(data_temp, (ptr_temp+5+ 3*i), 3);
								data_temp[3] = '\0';
								SetPwmValue[i] = (uint16_t)atoi(data_temp);
							}
							
								strncpy(data_temp, (ptr_temp+5+ 3*i), 3);
								data_temp[3] = '\0';
								SetGeneralReinforce = (uint16_t)atoi(data_temp);
						break;
					}
					comClearRxFifo(COM_ESP8266);
				}
			
				BSP_OS_SemPost(&wifi_send_sem);
			break;			
			
		}
		
		BSP_OS_TimeDlyMs(500);
	}
}

void ESP8266_send_data(char* str)
{
	if(WifiStatus == CONNECTING)
	{
		BSP_OS_SemWait(&wifi_send_sem, 20000);
		
		Mem_Clr(ESP8266_tx_data, sizeof(ESP8266_tx_data));
		sprintf(ESP8266_tx_data, "AT+CIPSEND=0,%d", strlen(str));
		ESP8266_SendAT(ESP8266_tx_data);
		
		Mem_Clr(ESP8266_rx_data, sizeof(ESP8266_rx_data));
		ESP8266_ReadResponse(ESP8266_rx_data, sizeof(ESP8266_rx_data), 2000);
		
		if(strchr(ESP8266_rx_data, '>') != NULL)
		{
			ESP8266_SendAT(str);
		}
		
		Mem_Clr(ESP8266_rx_data, sizeof(ESP8266_rx_data));
		BSP_OS_SemPost(&wifi_send_sem);
		
	}
		
}

// uint8_t ESP8266_send_data(void)
// {
// 	
// 	if(WifiStatus == CONNECTING)
// 	{
// 		BSP_OS_SemWait(&wifi_send_sem, 20000);
// 		
// 		Mem_Clr(ESP8266_tx_data, sizeof(ESP8266_tx_data));
// 		sprintf(ESP8266_tx_data, "MPU6050----> Acc:%f,%f,%f;;Gyro:%f,%f,%f", \
// 							MPU6050_H.Accel_X, MPU6050_H.Accel_Y, MPU6050_H.Accel_Z, MPU6050_H.GYRO_X, MPU6050_H.GYRO_Y, MPU6050_H.GYRO_Z);
// 		
// 		
// 		Mem_Clr(ESP8266_rx_data, sizeof(ESP8266_rx_data));
// 		sprintf(ESP8266_rx_data, "AT+CIPSEND=0,%d", strlen(ESP8266_tx_data));
// 		ESP8266_SendAT(ESP8266_rx_data); //��ID �����ַ���
// 		
// 		Mem_Clr(ESP8266_rx_data, sizeof(ESP8266_rx_data));
// 		ESP8266_ReadResponse(ESP8266_rx_data, sizeof(ESP8266_rx_data), 2000);
// 		if(strchr(ESP8266_rx_data, '>') != NULL)
// 		{
// 			ESP8266_SendAT(ESP8266_tx_data);
// 			
// 		}
// 		Mem_Clr(ESP8266_rx_data, sizeof(ESP8266_rx_data));
// 		BSP_OS_SemPost(&wifi_send_sem);
// 	}
// 	
// 		return 0;
// }
