#include <includes.h>

/*********** PWMģ�����ò��� *******************/
uint32_t usPrescaler = 83;   /* PWMʱ��Ԥ��Ƶϵ�� */
uint32_t usPeriod = 999;     /* PWM��ʱ���� */
uint32_t usPWMOffset = 0;    /* PWMռ�ձȲ��� */
/***********************************************/


void bsp_PWMInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	
	/* ʹ��ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* ���ӵ�AF���� */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM5);

	/* ����GPIO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*-----------------------------------------------------------------------
		system_stm32f4xx.c �ļ��� void SetSysClock(void) ������ʱ�ӵ��������£�

		HCLK = SYSCLK / 1	  (AHB1Periph)	168MHz
		PCLK2 = HCLK / 2	  (APB2Periph)	84MHz
		PCLK1 = HCLK / 4	  (APB1Periph)	42MHz

		��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM6, TIM12, TIM13,TIM14
		APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11

	----------------------------------------------------------------------- */
	TIM_Cmd(TIM5, DISABLE); 
	
	/* ����TIM5 */
	TIM_TimeBaseStructure.TIM_Period = usPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	
	/* ����ʹ�ܳ�ʼ�� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}



void bsp_SetPWMDutyCycle(uint16_t PWMValue, unsigned char PWMChannel)
{	
	TIM_OCInitTypeDef  TIM_OCInitStructure;
  uint32_t PWM_CCR;
	PWM_CCR = (usPeriod + 1) * PWMValue / 100 - 1 - usPWMOffset;  /* ����PWM����Ĵ�����ֵ */
	
	TIM_CtrlPWMOutputs(TIM5, DISABLE);  /* �ر�PWM�ź���� */
	TIM_Cmd(TIM5, DISABLE);       

	TIM_ARRPreloadConfig(TIM5, DISABLE);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	switch(PWMChannel)
	{
		case 1:  /* PWMͨ��1���� */
		{
			TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Disable);
			TIM_OCInitStructure.TIM_Pulse = PWM_CCR;
			TIM_OC1Init(TIM5, &TIM_OCInitStructure);
			TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
		}break;
	
		case 2:  /* PWMͨ��2���� */
		{
			TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Disable);
			TIM_OCInitStructure.TIM_Pulse = PWM_CCR;
			TIM_OC2Init(TIM5, &TIM_OCInitStructure);
			TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
		}break;
		
		case 3:  /* PWMͨ��3���� */
		{
			TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Disable);
			TIM_OCInitStructure.TIM_Pulse = PWM_CCR;	
			TIM_OC3Init(TIM5, &TIM_OCInitStructure);
			TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);
		}break;

		case 4:  /* PWMͨ��3���� */
		{
			TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Disable);
			TIM_OCInitStructure.TIM_Pulse = PWM_CCR;	
			TIM_OC4Init(TIM5, &TIM_OCInitStructure);
			TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);
		}break;
		
		default: 
			break;
	}
	
	TIM_ARRPreloadConfig(TIM5, ENABLE);
	TIM_Cmd(TIM5, ENABLE);
	TIM_CtrlPWMOutputs(TIM5, ENABLE);
}
