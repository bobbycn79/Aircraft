#include  <includes.h>
#include  "IMU.h"

float ypr[3]; // yaw pitch roll
uint16_t Math_hz = 0;




void FlyMain(void)
{
	MPU6050_DataDeal();  //����MPU6050��ȡ�����ݵõ���MPU6050_H�ṹ��
	HMC5883L_DataDeal();	//����HMC5883L��ȡ�����õ���HMC5883L_H�ṹ��
	
	IMU_getYawPitchRoll(ypr);
	Math_hz++;
	
	control(ypr[0], ypr[1], ypr[2]);
}
