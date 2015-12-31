#include "init.h"
extern Bus_Type BUS1;

void adc_init (Bus_Type* BUSx)
{
	
		RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV2;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
 
    ADC1->SQR3 = ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_3;
    ADC1->CR2 = ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2
            | ADC_CR2_EXTTRIG;
    ADC1->CR2 |= ADC_CR2_ADON;
 
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while ((ADC1->CR2 & ADC_CR2_RSTCAL) == ADC_CR2_RSTCAL)
    {
    }
 
    ADC1->CR2 |= ADC_CR2_CAL;
    while ((ADC1->CR2 & ADC_CR2_RSTCAL) == ADC_CR2_CAL)
    {
    }
	 ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_SR_EOC) != ADC_SR_EOC)
    {
 
    }
		
}

void gpio_init (void)
{
	GPIO_InitTypeDef gpio_cfg;
  GPIO_StructInit(&gpio_cfg);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
  gpio_cfg.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  gpio_cfg.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOA, &gpio_cfg);
	
	gpio_cfg.GPIO_Mode  = GPIO_Mode_Out_PP;
	gpio_cfg.GPIO_Pin = GPIO_Pin_0;
	gpio_cfg.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOA, &gpio_cfg);
	
	gpio_cfg.GPIO_Mode  = GPIO_Mode_Out_PP;
	gpio_cfg.GPIO_Pin = GPIO_Pin_1;
	gpio_cfg.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOA, &gpio_cfg);
	
	gpio_cfg.GPIO_Mode  = GPIO_Mode_Out_PP;
	gpio_cfg.GPIO_Pin = GPIO_Pin_2;
	gpio_cfg.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOA, &gpio_cfg);
	
	gpio_cfg.GPIO_Mode  = GPIO_Mode_Out_PP;
	gpio_cfg.GPIO_Pin = GPIO_Pin_3;
	gpio_cfg.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOA, &gpio_cfg);
	
	gpio_cfg.GPIO_Mode  = GPIO_Mode_Out_PP;
	gpio_cfg.GPIO_Pin = GPIO_Pin_4;
	gpio_cfg.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOA, &gpio_cfg);
	
	gpio_cfg.GPIO_Mode  = GPIO_Mode_Out_PP;
	gpio_cfg.GPIO_Pin = GPIO_Pin_5;
	gpio_cfg.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOA, &gpio_cfg);
	
	gpio_cfg.GPIO_Mode  = GPIO_Mode_Out_PP;
	gpio_cfg.GPIO_Pin = GPIO_Pin_11;
	gpio_cfg.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOA, &gpio_cfg);
	
	gpio_cfg.GPIO_Mode  = GPIO_Mode_Out_PP;
	gpio_cfg.GPIO_Pin = GPIO_Pin_7;
	gpio_cfg.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOA, &gpio_cfg);
	
	gpio_cfg.GPIO_Mode  = GPIO_Mode_IPD;
	gpio_cfg.GPIO_Pin = GPIO_Pin_2;
	gpio_cfg.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOE, &gpio_cfg);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);		//CLK
	GPIO_ResetBits(GPIOA,GPIO_Pin_5); 	//DATA (табло)
	GPIO_ResetBits(GPIOA,GPIO_Pin_7); 	//STROB (время)
	GPIO_ResetBits(GPIOA,GPIO_Pin_11); 	//STROB (дата)
	//GPIO_SetBits(GPIOA,GPIO_Pin_12); 	//OE (табло)
	//GPIO_ResetBits(GPIOA,GPIO_Pin_13);	//P/S	(для кнопок)
	//GPIO_ResetBits(GPIOA,GPIO_Pin_14);	//Q8 (выход кнопок)

}

void tim_init(void)
{
	TIM_TimeBaseInitTypeDef timer_base;
	TIM_ICInitTypeDef TIM_ICStructure;
	TIM_OCInitTypeDef TIM_OCStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM7, ENABLE);
  TIM_TimeBaseStructInit(&timer_base);
  timer_base.TIM_Prescaler = 72 - 1;
  TIM_TimeBaseInit(TIM3, &timer_base);
	
	TIM_PWMIConfig(TIM3, &TIM_ICStructure);
	TIM_ICStructure.TIM_Channel = TIM_Channel_1; 
	TIM_ICStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; 
	TIM_ICStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; 
	TIM_ICStructure.TIM_ICFilter = 0; 
	TIM_PWMIConfig(TIM3, &TIM_ICStructure);
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
  TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);
	
	
	TIM_OCStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCStructure.TIM_Pulse = 5000;
	TIM_OC3Init(TIM3, &TIM_OCStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

	
	TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
	TIM_ClearFlag(TIM3, TIM_FLAG_CC3);
	TIM_Cmd(TIM3, ENABLE);
//TIM3 init
	NVIC_SetPriority(TIM3_IRQn,1);
	NVIC_EnableIRQ(TIM3_IRQn);
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	TIM7->PSC=500;
	TIM7->ARR=1;
	TIM7->DIER |= TIM_DIER_UIE;
//	TIM7->CR1 |= TIM_CR1_CEN;
	NVIC_SetPriority(TIM7_IRQn,2);
	NVIC_EnableIRQ(TIM7_IRQn);
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->PSC=60000;
	TIM6->ARR=599;
	TIM6->DIER |= TIM_DIER_UIE;	
	TIM6->CR1 |= TIM_CR1_CEN;
	NVIC_SetPriority(TIM6_IRQn,3);
	NVIC_EnableIRQ(TIM6_IRQn);
	
}



void line_init ()
{
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);
	AFIO->EXTICR[0]|=AFIO_EXTICR1_EXTI2_PE;   //Выбираем альтернативную ф-ю
	EXTI->IMR|=EXTI_IMR_MR2;								//Разрешаем прерывание
	EXTI->FTSR|=EXTI_FTSR_TR2;							//По срезу
	NVIC_EnableIRQ (EXTI2_IRQn);
}


