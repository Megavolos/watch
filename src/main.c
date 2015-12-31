#include "main.h"


//#define checkVal(var,val,tol) (var>(val*(100-tol)/100) && var<(val*(100+tol)/100))
#define CLK_stop TIM7->CR1 &= ~TIM_CR1_CEN
#define CLK_start TIM7->CR1 |= TIM_CR1_CEN
unsigned int	tim_cnt,tim_cnt_prev;
extern Clock_Type CLK1;
extern Bus_Type BUS1;
extern Time_Type TIME1;


void TIM7_IRQHandler()
{

	TIM7->SR &= ~TIM_SR_UIF;

	//if (!(CLK1->Tim_Cnt%2)) 
	//{
	//	*(uint32_t*)0x42210190^=1; //BitBanding: GPIOB_ODR addr = 0x4001080C, ODR4 = bit 4
		
	//}
	CLK_Tik (&CLK1);
	
}

void TIM6_IRQHandler()
{
	TIM6->SR &= ~TIM_SR_UIF;
	Half_Second_Tik (&TIME1, &CLK1);
	
}

void TIM3_IRQHandler() 
{
	capt_IRQ (&BUS1);
}

int main()
{
	
	gpio_init();
	BUS_Init (&BUS1);
	TIME_Init (&TIME1);
	tim_init();
	BKP_init();
	adc_init(&BUS1);
	TIME1.Dot=1;
	if (!(BKP->DR5%4)) 
	{
		TIME1.Days_in_month[1]=29;
	}
	else
	{
		TIME1.Days_in_month[1]=28;
	}
	while(!Display_Send_DataTime(&BUS1, &TIME1));	
	
	while (1)
	{ 
		if (BUS1.Mode&0x4000)
		{
			switch (BUS1.Mode&0x3000)
			{
				case 0x0000:
						while(!BlinkDDMMMYYYY(&BUS1, &TIME1));
				break;
				case 0x1000:
						while(!BlinkHHMM(&BUS1, &TIME1));
				break;
				case 0x2000:
					while(!BlinkDelay(&BUS1, &TIME1));
				break;
				case 0x3000:
					while(!BlinkADC(&BUS1, &TIME1));
				break;
			}
		}
		else
		{
			time_line_action (&TIME1, &BUS1);
			if (TIME1.Delay_Cnt<(SRAM_delay1*2))
			{
				while(!Display_Send_DataTime(&BUS1, &TIME1));
			}
			if ((SRAM_delay2)&&(TIME1.Delay_Cnt>=(SRAM_delay1*2))&&(TIME1.Delay_Cnt<((SRAM_delay1+SRAM_delay2)*2)))
			{	
				while(!Display_Send_TempTime(&BUS1, &TIME1));				
			}	
			if (TIME1.Delay_Cnt>=((SRAM_delay1+SRAM_delay2)*2)) TIME1.Delay_Cnt=0;
		}
	IRaction(&BUS1, &TIME1);	
	get_temp (&BUS1);
	}
}
