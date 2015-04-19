#include <includes.h>

float angleAx_temp, angleAy_temp, angleAz_temp;
float GyroAx_temp, GyroAy_temp;
float Angle_X_Final; //X������б�Ƕ�
float Angle_Y_Final; //Y������б�Ƕ�

float Gyro_x;		 //X�������������ݴ�
float Gyro_y;        //Y�������������ݴ�
float Gyro_z;		 //Z�������������ݴ�

void Kalman_Filter_X(float Accel,float Gyro);
void Kalman_Filter_Y(float Accel,float Gyro);

void Angle_Calcu(void)
{
	angleAx_temp = atan2(MPU6050_H.Accel_X, MPU6050_H.Accel_Z)*180/PI;  //���ٶȼ���Ƕ�
	angleAy_temp = atan2(MPU6050_H.Accel_Y, MPU6050_H.Accel_Z)*180/PI;
	angleAz_temp = atan2(HMC5883L_H.X, HMC5883L_H.Y);
	
  if(MPU6050_H.Accel_X<32764) angleAy_temp = +angleAy_temp;
	if(MPU6050_H.Accel_X>32764) angleAy_temp = -angleAy_temp;
	if(MPU6050_H.Accel_Y<32764) angleAx_temp = +angleAx_temp;
	if(MPU6050_H.Accel_Y>32764) angleAx_temp = -angleAx_temp;

	
	GyroAx_temp = MPU6050_H.GYRO_X * MPU6050_DEG_PER_LSB_2000;
	GyroAy_temp = MPU6050_H.GYRO_Y * MPU6050_DEG_PER_LSB_2000;
	
	Kalman_Filter_X(angleAx_temp, GyroAx_temp);
	Kalman_Filter_Y(angleAy_temp, GyroAy_temp);
}

float Q_angle = 0.001;  
float Q_gyro  = 0.003;
float R_angle = 0.5;
float dt      = 0.002;//dtΪkalman�˲�������ʱ��;
char  C_0     = 1;
float Q_bias, Angle_err;
float PCt_0, PCt_1, E;
float K_0, K_1, t_0, t_1;
float Pdot[4] ={0,0,0,0};
float PP[2][2] = { { 1, 0 },{ 0, 1 } };

void Kalman_Filter_X(float Accel,float Gyro)
{
	Angle_X_Final += (Gyro - Q_bias) * dt; //�������
	
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-����������Э�����΢��

	Pdot[1]= -PP[1][1];
	Pdot[2]= -PP[1][1];
	Pdot[3]= Q_gyro;
	
	PP[0][0] += Pdot[0] * dt;   // Pk-����������Э����΢�ֵĻ���
	PP[0][1] += Pdot[1] * dt;   // =����������Э����
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
		
	Angle_err = Accel - Angle_X_Final;	//zk-�������
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //����������Э����
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	Angle_X_Final += K_0 * Angle_err;	 //�������
	Q_bias        += K_1 * Angle_err;	 //�������
	Gyro_x         = Gyro - Q_bias;	 //���ֵ(�������)��΢��=���ٶ�
}


void Kalman_Filter_Y(float Accel,float Gyro) //����������		
{
	Angle_Y_Final += (Gyro - Q_bias) * dt; //�������
	
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-����������Э�����΢��

	Pdot[1]=- PP[1][1];
	Pdot[2]=- PP[1][1];
	Pdot[3]=Q_gyro;
	
	PP[0][0] += Pdot[0] * dt;   // Pk-����������Э����΢�ֵĻ���
	PP[0][1] += Pdot[1] * dt;   // =����������Э����
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
		
	Angle_err = Accel - Angle_Y_Final;	//zk-�������
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //����������Э����
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	Angle_Y_Final	+= K_0 * Angle_err;	 //�������
	Q_bias	+= K_1 * Angle_err;	 //�������
	Gyro_y   = Gyro - Q_bias;	 //���ֵ(�������)��΢��=���ٶ�
}
