#include "databus.h"

//..........................0....1....2....3....4....5....6.....7....8...9....-...
//unsigned char Digit[11] = {0xFC,0x0C,0xDA,0x9E,0x2E,0xB6,0xF6,0x1C,0xFE,0xBE,0x00};
unsigned char Digit[11] = {0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6,0x00};
unsigned char Seg16[56] = {
	0xFF,0x00, 		//0 0,1
	48,0x00, 			//1	2,3
	238,17, 			//2	4,5
	252,17,				//3	6,7
	49,17, 				//4	8,9
	221,17, 			//5	10,11
	223,17, 			//6	12,13
	240,0x00, 		//7	14,15
	255,17, 			//8	16,17
	253,17, 			//9	18,19
	243,17, 			//А	20,21
	159,81, 			//В	22
	195,0x00, 		//Г	24
	254,17, 			//Д	26
	207,17, 			//E	28
	51,34, 				//И	30
	3,41, 				//K	32
	48,34, 				//Л	34
	51,160, 			//M	36
	51,17, 				//H	38
	243,0x00, 		//П	40
	227,17, 			//Р	42
	207,0x00, 		//C	44
	192,68, 			//T	46
	225,85, 			//Ф	48
	123,69, 			//Ю	50
	241,19, 			//Я	52
	0x00,0x00  		//Гашение	54
};
Clock_Type CLK1;
Bus_Type BUS1;
Time_Type TIME1;
void CLK_Tik (Clock_Type* CLKx)
{
	*(uint32_t*)0x42210190=(CLKx->Tim_Cnt&2)>>1;
	CLKx->Tim_Cnt++;
}

void Half_Second_Tik (Time_Type* TIMEx, Clock_Type* CLKx)
{
	CLKx->Half_Second_Cnt++;	
	TIMEx->Dot=CLKx->Half_Second_Cnt&1;
	TIMEx->Delay_Cnt++;
	
}
char checkVal(unsigned int var,unsigned int val,unsigned int tol) 
{
	unsigned int left,right;
	left = val*(100-tol)/100;
	right = val*(100+tol)/100;
	if ((var>left)&&(var<right))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void BUS_Init (Bus_Type* BUSx)
{
	BUSx->Bit_Num=0;
	BUSx->Byte_Num=0;
	BUSx->CE=1;
	BUSx->Latch=0;
	BUSx->State=STATE_NotWait_End;
	BUSx->Mode|=0x00FF;
}


char Display_Send_Byte (Bus_Type* BUSx, Clock_Type* CLKx, unsigned char Byte)
{
	
	if (CLKx->Tim_Cnt_Prev!=CLKx->Tim_Cnt)									//Если произошло изменение счетчика на нечетный
	{
		switch (BUSx->State)
		{
			case STATE_NotWait_End:
				if (CLKx->Tim_Cnt&1)
				{				
					*(uint32_t*)0x42210194=1&(Byte>>BUSx->Bit_Num);  //сдвигаем передаваемый байт,применяем маску, и шлем в GPIOA_PIN5 		
					BUSx->State=STATE_Wait_Begin;
				}
			break;
				
			case STATE_Wait_Begin:
				BUSx->State=STATE_NotWait_Begin;
			break;
			
			case STATE_NotWait_Begin:
				*(uint32_t*)0x42210194=0; 							//завершаем передачу бита (ставим в ноль)
				BUSx->State=STATE_Wait_End;
			break;
			
			case STATE_Wait_End:
				BUSx->State=STATE_NotWait_End;
				BUSx->Bit_Num++;
				if (BUSx->Bit_Num==8) 
				{
					BUSx->Bit_Num=0;
					BUSx->Byte_Num++;	
					CLKx->Tim_Cnt_Prev=CLKx->Tim_Cnt;	
					return 1;			
				}
			break;
		}
	}
	CLKx->Tim_Cnt_Prev=CLKx->Tim_Cnt;	
	
	return 0;
}



char Display_Send_Time (Bus_Type* BUSx, Time_Type* TIMEx)
{
	TIM7->CR1 &= ~TIM_CR1_CEN;

 switch (BUSx->Byte_Num)
 {
	case 10:
		Display_Send_Byte (&BUS1, &CLK1, Digit[TIMEx->Min_0]);
	break;
	case 11:
		Display_Send_Byte (&BUS1, &CLK1, Digit[TIMEx->Min_1]);
	break;
	case 12:
		Display_Send_Byte (&BUS1, &CLK1, Digit[TIMEx->Hour_0]);
	break;
	case 13:
		if (Display_Send_Byte (&BUS1, &CLK1, Digit[TIMEx->Hour_1]))
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_7);
			BUSx->Byte_Num=0;
			Delay_ms(2);
			GPIO_ResetBits(GPIOA,GPIO_Pin_7);
			return 1;
		}
	break;

 }
TIM7->CR1 |= TIM_CR1_CEN;
return 0; 
	
}

char Display_Send_DataTime (Bus_Type* BUSx, Time_Type* TIMEx)
{
	data_to_letter(TIMEx);
	TIM7->CR1 &= ~TIM_CR1_CEN;

	switch (BUSx->Byte_Num)
	{
		case 0:
			Display_Send_Byte (&BUS1, &CLK1,TIMEx->display2[0]);
		break;
		case 1:
			Display_Send_Byte (&BUS1, &CLK1, TIMEx->display2[1]);
		break;
		case 2:
			Display_Send_Byte (&BUS1, &CLK1, TIMEx->display2[2]);
		break;
		case 3:
			Display_Send_Byte (&BUS1, &CLK1, TIMEx->display2[3]);
		break;
		case 4:
			Display_Send_Byte (&BUS1, &CLK1, TIMEx->display2[4]);
		break;
		case 5:
			Display_Send_Byte (&BUS1, &CLK1, TIMEx->display2[5]);
		break;
		case 6:
			Display_Send_Byte (&BUS1, &CLK1, TIMEx->display2[6]);
		break;
		case 7:
			Display_Send_Byte (&BUS1, &CLK1, TIMEx->display2[7]);
		break;
		case 8:
			Display_Send_Byte (&BUS1, &CLK1, TIMEx->display2[8]);
		break;
		case 9:
			if (Display_Send_Byte (&BUS1, &CLK1, TIMEx->display2[9]))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_11);
				Delay_ms(2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_11);
			}
		break;
		case 10:
			Display_Send_Byte (&BUS1, &CLK1, Digit[TIMEx->Min_0]);
		break;
		case 11:
			Display_Send_Byte (&BUS1, &CLK1, Digit[TIMEx->Min_1]|TIMEx->Dot);
		break;
		case 12:
			Display_Send_Byte (&BUS1, &CLK1, Digit[TIMEx->Hour_0]);
		break;
		case 13:
			if (Display_Send_Byte (&BUS1, &CLK1, Digit[TIMEx->Hour_1]))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_7);
				BUSx->Byte_Num=0;
				Delay_ms(2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_7);
				return 1;
		}

		break;
	}
	TIM7->CR1 |= TIM_CR1_CEN;
	return 0; 
}

void data_to_letter (Time_Type* TIMEx)
{

	// 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50 52
	//  А  В  Г  Д  Е  И  К  Л  М  Н  П  Р  С  Т  Ф  Ю  Я
	switch (TIMEx->Month)
	{
		case 0: // ЯНВ
			TIMEx->display2[0]=Seg16[22];	//В
			TIMEx->display2[1]=Seg16[23];
			TIMEx->display2[2]=Seg16[38]; 	//Н
			TIMEx->display2[3]=Seg16[39];
			TIMEx->display2[4]=Seg16[52];	//Я
			TIMEx->display2[5]=Seg16[53];
		break;
		case 1: // ФЕВ
			TIMEx->display2[0]=Seg16[22];	//В
			TIMEx->display2[1]=Seg16[23];
			TIMEx->display2[2]=Seg16[28]; 	//Е
			TIMEx->display2[3]=Seg16[29];
			TIMEx->display2[4]=Seg16[48];	//Ф
			TIMEx->display2[5]=Seg16[49];
		break;
		case 2: // МАР
			TIMEx->display2[0]=Seg16[42];	//Р
			TIMEx->display2[1]=Seg16[43];
			TIMEx->display2[2]=Seg16[20]; 	//А
			TIMEx->display2[3]=Seg16[21];
			TIMEx->display2[4]=Seg16[36];	//М
			TIMEx->display2[5]=Seg16[37];
		break;
		case 3: // АПР
			TIMEx->display2[0]=Seg16[42];	//Р
			TIMEx->display2[1]=Seg16[43];
			TIMEx->display2[2]=Seg16[40]; 	//П
			TIMEx->display2[3]=Seg16[41];
			TIMEx->display2[4]=Seg16[20];	//А
			TIMEx->display2[5]=Seg16[21];
		break;
		case 4: // МАИ
			TIMEx->display2[0]=Seg16[30];	//И
			TIMEx->display2[1]=Seg16[31];
			TIMEx->display2[2]=Seg16[20]; 	//А
			TIMEx->display2[3]=Seg16[21];
			TIMEx->display2[4]=Seg16[36];	//М
			TIMEx->display2[5]=Seg16[37];
		break;
		case 5: // ИЮН
			TIMEx->display2[0]=Seg16[38];	//Н
			TIMEx->display2[1]=Seg16[39];
			TIMEx->display2[2]=Seg16[50]; //Ю
			TIMEx->display2[3]=Seg16[51];
			TIMEx->display2[4]=Seg16[30];	//И
			TIMEx->display2[5]=Seg16[31];
		break;
		case 6: // ИЮЛ
			TIMEx->display2[0]=Seg16[34];	//Л
			TIMEx->display2[1]=Seg16[35];
			TIMEx->display2[2]=Seg16[50]; 	//Ю
			TIMEx->display2[3]=Seg16[51];
			TIMEx->display2[4]=Seg16[30];	//И
			TIMEx->display2[5]=Seg16[31];
		break;
		case 7: // АВГ
			TIMEx->display2[0]=Seg16[24];	//Г
			TIMEx->display2[1]=Seg16[25];
			TIMEx->display2[2]=Seg16[22]; 	//В
			TIMEx->display2[3]=Seg16[23];
			TIMEx->display2[4]=Seg16[20];	//А
			TIMEx->display2[5]=Seg16[21];
		break;
		case 8: // СЕН
			TIMEx->display2[0]=Seg16[38];	//Н
			TIMEx->display2[1]=Seg16[39];
			TIMEx->display2[2]=Seg16[28]; 	//Е
			TIMEx->display2[3]=Seg16[29];
			TIMEx->display2[4]=Seg16[44];	//С
			TIMEx->display2[5]=Seg16[45];
		break;
		case 9: // ОКТ
			TIMEx->display2[0]=Seg16[46];	//Т
			TIMEx->display2[1]=Seg16[47];
			TIMEx->display2[2]=Seg16[32]; 	//К
			TIMEx->display2[3]=Seg16[33];
			TIMEx->display2[4]=Seg16[0];	//О
			TIMEx->display2[5]=Seg16[1];
		break;
		case 10: // НОЯ
			TIMEx->display2[0]=Seg16[52];	//Я
			TIMEx->display2[1]=Seg16[53];
			TIMEx->display2[2]=Seg16[0]; 	//О
			TIMEx->display2[3]=Seg16[1];
			TIMEx->display2[4]=Seg16[38];	//Н
			TIMEx->display2[5]=Seg16[39];
		break;
		case 11: // ДЕК
			TIMEx->display2[0]=Seg16[32];	//К
			TIMEx->display2[1]=Seg16[33];
			TIMEx->display2[2]=Seg16[28]; 	//Е
			TIMEx->display2[3]=Seg16[29];
			TIMEx->display2[4]=Seg16[26];	//Д
			TIMEx->display2[5]=Seg16[27];
		break;
	}
	TIMEx->display2[6]=Seg16[TIMEx->Day_0*2];
	TIMEx->display2[7]=Seg16[TIMEx->Day_0*2+1];
	TIMEx->display2[8]=Seg16[TIMEx->Day_1*2];
	TIMEx->display2[9]=Seg16[TIMEx->Day_1*2+1];
}
unsigned int IR_decodeNEC(Bus_Type* BUSx)
{
	u8 i;
	uint16_t addr=0,frame=0;
	//uint32_t frame=0;
	uint16_t decoded;
	if (BUSx->IR_captCount<38)
	{
		BUSx->IR_captCount = 0;
		return IR_NO_COMMAND;
	}
	if (!checkVal(BUSx->IR_captArray[0], IR_NEC_START_WIDTH, IR_NEC_TOL)) 
	{
		BUSx->IR_captCount = 0;
		return IR_NO_COMMAND;
	}
	if (!checkVal(BUSx->IR_captArray[1], IR_NEC_START_PULSE, IR_NEC_TOL)) 
	{
		BUSx->IR_captCount = 0;
		return IR_NO_COMMAND;
	}
	
	if (!checkVal(BUSx->IR_captArray[34], IR_NEC_SPLIT_PULSE, IR_NEC_TOL))
	{
		BUSx->IR_captCount = 0;
		return IR_NO_COMMAND;
	}
	for(i=0; i<16; i++)
	{
		if (checkVal(BUSx->IR_captArray[(i+1)*2], IR_NEC_0_WIDTH, IR_NEC_TOL)&&checkVal(BUSx->IR_captArray[(i+1)*2+1], IR_NEC_0_PULSE, IR_NEC_TOL)) 
		{
			addr = addr >> 1;
			continue;
		}
		if (checkVal(BUSx->IR_captArray[(i+1)*2], IR_NEC_1_WIDTH, IR_NEC_TOL)&&checkVal(BUSx->IR_captArray[(i+1)*2+1], IR_NEC_1_PULSE, IR_NEC_TOL)) 
		{
			addr = (addr >> 1) | 0x8000;
			continue;
		}
		
		//return IR_NO_COMMAND;
	}
	if (addr!=hw_addr) 
	{
		
		return IR_NO_COMMAND;
	}
	for(i=21; i<38; i++)
	{
		if (checkVal(BUSx->IR_captArray[(i+1)*2], IR_NEC_0_WIDTH, IR_NEC_TOL)&&checkVal(BUSx->IR_captArray[(i+1)*2+1], IR_NEC_0_PULSE, IR_NEC_TOL)) 
		{
			frame = frame >> 1;
			continue;
		}
		if (checkVal(BUSx->IR_captArray[(i+1)*2], IR_NEC_1_WIDTH, IR_NEC_TOL)&&checkVal(BUSx->IR_captArray[(i+1)*2+1], IR_NEC_1_PULSE, IR_NEC_TOL)) 
		{
			frame = (frame >> 1) | 0x8000;
			continue;
		}
		
		//return IR_NO_COMMAND;
	}
	if ( (frame & 0x00ff) != ( (~(frame) & 0xFF00) >>8 )  )
	{
		
		return IR_NO_COMMAND;
	}
	decoded = (frame&0x00FF) ;
	BUSx->IR_captCount = 0;
	return decoded;

}

void ir_cmd (Bus_Type* BUSx)
{
	if ((BUSx->IR_recState==REC_Iddle)&&(BUSx->IR_captCount==MAX_CAPT_COUNT))
	{
		NVIC_DisableIRQ(TIM3_IRQn);
		BUSx->IR_cmd = IR_decodeNEC(&BUS1);
		BUSx->IR_recState=REC_Captured;
		BUSx->IR_captCount = 0;
		switch (BUSx->IR_cmd)
		{
			case 0:
				BUSx->Mode&=0xFF00;
			break;
			case 1:
				BUSx->Mode&=0xFF00;
				BUSx->Mode|=0x0001;
			break;
			case 2:
				BUSx->Mode&=0xFF00;
				BUSx->Mode|=0x0002;
			break;
			case 3:
				BUSx->Mode&=0xFF00;
				BUSx->Mode|=0x0003;
			break;
			case 4:
				BUSx->Mode&=0xFF00;
				BUSx->Mode|=0x0004;
			break;
			case 5:
				BUSx->Mode&=0xFF00;
				BUSx->Mode|=0x0005;
			break;
			case 6:
				BUSx->Mode&=0xFF00;
				BUSx->Mode|=0x0006;
			break;
			case 7:
				BUSx->Mode&=0xFF00;
				BUSx->Mode|=0x0007;
			break;
			case 8:
				BUSx->Mode&=0xFF00;
				BUSx->Mode|=0x0008;
			break;
			case 9:
				BUSx->Mode&=0xFF00;
				BUSx->Mode|=0x0009;
			break;
			case 10:
				BUSx->Mode&=0x8FFF; 			//1000 1111 1111 1111
				BUSx->Mode|=0x4000;				//0100 0000 0000 0000
			break;
			case 11: 											//установка времени
				BUSx->Mode&=0xCFFF; 				//1100 1111 1111 1111
				BUSx->Mode|=0x1000;   			//0001 0000 0000 0000  
			break;
			case 12:											//установка даты
				BUSx->Mode&=0xCFFF;					//1000 0111 1111 1111
			break;
			case 21:											//Установка пауз и режима
				BUSx->Mode&=0xCFFF;					
				BUSx->Mode|=0x2000;					//0010 0000 0000 0000	
			break;
			case 24:											//+1
				BUSx->Mode&=0xFF00;		
				BUSx->Mode|=0x0010;
			break;
			case 25:											//-1
				BUSx->Mode&=0xFF00;					
				BUSx->Mode|=0x0011;
			break;
			case 26:											//Влево
				BUSx->Mode&=0xFF00;					
				BUSx->Mode|=0x0012;
			break;				
			case 27:											//Вправо
				BUSx->Mode&=0xFF00;					
				BUSx->Mode|=0x0013;
			break;
			
			case 28: 											//Выход из  настроек
				BUSx->Mode&=0x8FFF; 				//1000 1111 1111 1111
			break;
			case 34:											//Калибровка датчика
				BUSx->Mode&=0xCFFF;
				BUSx->Mode|=0x3000;					//0011 0000 0000 0000
			  BUSx->confirm=17;
			break;		
		}
		NVIC_EnableIRQ(TIM3_IRQn);
	}
}

char show (u8 seg, u8 mask)
{
	u8 result;
	if (seg==mask) 
	{
		result=1;
	}
	else 
	{
		result=0;
	}
	mask = result&TIME1.Dot;
	seg = (!result)|mask;
	return seg;
}	
char BlinkDDMMMYYYY (Bus_Type* BUSx, Time_Type* TIMEx)
{
	u8 mask; 
	data_to_letter(TIMEx);
	TIM7->CR1 &= ~TIM_CR1_CEN;
	if (BUSx->BlinkSeg>4) BUSx->BlinkSeg=0;
	switch (BUSx->BlinkSeg) 
	{

		case 4:
			BUSx->Mode&=0xF8FF;
			BUSx->Mode|=0x0400;
			mask=1;
		break;
		case 3:
			BUSx->Mode&=0xF8FF;
			BUSx->Mode|=0x0300;
			mask=2;
		break;
		case 2:
			BUSx->Mode&=0xF8FF;
			BUSx->Mode|=0x0200;
			mask=4;
		break;
		case 1:
			BUSx->Mode&=0xF8FF;
			BUSx->Mode|=0x0100;
			mask=8;
		break;
		case 0:
			BUSx->Mode&=0xF8FF;
			mask=16;
		break;
	}
	
	switch (BUSx->Byte_Num)
	{
		case 0:	
			Display_Send_Byte (&BUS1, &CLK1, show(4, mask)*TIMEx->display2[0]);
		break;
		case 1:
			Display_Send_Byte (&BUS1, &CLK1, show(4, mask)*TIMEx->display2[1]);
		break;
		case 2:
			Display_Send_Byte (&BUS1, &CLK1, show(4, mask)*TIMEx->display2[2]);
		break;
		case 3:
			Display_Send_Byte (&BUS1, &CLK1, show(4, mask)*TIMEx->display2[3]);
		break;
		case 4:
			Display_Send_Byte (&BUS1, &CLK1, show(4, mask)*TIMEx->display2[4]);
		break;
		case 5:
			Display_Send_Byte (&BUS1, &CLK1, show(4, mask)*TIMEx->display2[5]);
		break;
		case 6:
			Display_Send_Byte (&BUS1, &CLK1, show(8, mask)*TIMEx->display2[6]);
		break;
		case 7:
			Display_Send_Byte (&BUS1, &CLK1, show(8, mask)*TIMEx->display2[7]);
		break;
		case 8:
			Display_Send_Byte (&BUS1, &CLK1, show(16, mask)*TIMEx->display2[8]);
		break;
		case 9:
			if (Display_Send_Byte (&BUS1, &CLK1, show(16, mask)*TIMEx->display2[9]))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_11);
				Delay_ms(2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_11);
			}
		break;
		case 10:
			Display_Send_Byte (&BUS1, &CLK1, show(1, mask)*Digit[TIMEx->Year_0]);
		break;
		case 11:
			Display_Send_Byte (&BUS1, &CLK1, show(2, mask)*Digit[TIMEx->Year_1]);
		break;
		case 12:
			Display_Send_Byte (&BUS1, &CLK1, Digit[0]);
		break;
		case 13:
			if (Display_Send_Byte (&BUS1, &CLK1, Digit[2]))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_7);
				BUSx->Byte_Num=0;
				Delay_ms(2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_7);
				return 1;
		}

		break;
	}
	TIM7->CR1 |= TIM_CR1_CEN;
	return 0; 
}

char BlinkHHMM (Bus_Type* BUSx, Time_Type* TIMEx)
{ 
	data_to_letter(TIMEx);
	TIM7->CR1 &= ~TIM_CR1_CEN;
	if (BUSx->BlinkSeg>3) BUSx->BlinkSeg=0;
	switch (BUSx->BlinkSeg) 
	{
		case 3:
			BUSx->Mode&=0xF8FF;
			BUSx->Mode|=0x0300;//M0
		break;
		case 2:
			BUSx->Mode&=0xF8FF;
			BUSx->Mode|=0x0200;	//M1
		break;
		case 1:
			BUSx->Mode&=0xF8FF;	
			BUSx->Mode|=0x0100;	//H0
		break;
		case 0:
			BUSx->Mode&=0xF8FF; //H1
		break;
	}
	
	switch (BUSx->Byte_Num)
	{
		case 0:	
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 1:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 2:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 3:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 4:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 5:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 6:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 7:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 8:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 9:
			if (Display_Send_Byte (&BUS1, &CLK1, 0))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_11);
				Delay_ms(2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_11);
			}
		break;
		case 10:
			Display_Send_Byte (&BUS1, &CLK1, show(3, BUSx->BlinkSeg)*Digit[TIMEx->Min_0]);
		break;
		case 11:
			Display_Send_Byte (&BUS1, &CLK1, (show(2, BUSx->BlinkSeg)*Digit[TIMEx->Min_1])|1);
		break;
		case 12:
			Display_Send_Byte (&BUS1, &CLK1, show(1, BUSx->BlinkSeg)*Digit[TIMEx->Hour_0]);
		break;
		case 13:
			if (Display_Send_Byte (&BUS1, &CLK1, show(0, BUSx->BlinkSeg)*Digit[TIMEx->Hour_1]))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_7);
				BUSx->Byte_Num=0;
				Delay_ms(2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_7);
				return 1;
		}

		break;
	}
	TIM7->CR1 |= TIM_CR1_CEN;
	return 0; 
}

char BlinkADC (Bus_Type* BUSx, Time_Type* TIMEx)
{
	unsigned char s0,s1;
	TIM7->CR1 &= ~TIM_CR1_CEN;
	if (BUSx->BlinkSeg>2) BUSx->BlinkSeg=2;
	if (BUSx->confirm==17)
	{
		BUSx->BlinkSeg=0;
	}
	if (BUSx->BlinkSeg)
	{
		ADC1->CR2 |= ADC_CR2_SWSTART;
		while (!(ADC1->SR & ADC_SR_EOC));
		BUSx->ADC_Dev=BUSx->ADC_Dev+(ADC1->DR - BUSx->ADC_Dev)*0.001;
	}
	switch (SRAM_ADC&0x8080)
	{
		case 0x0000:
			s0=0;
			s1=0;
		break;
		case 0x0080:
			s0=2;
			s1=0;
		break;
		case 0x8000:
			s0=0;
			s1=17;
		break;
		case 0x8080:
			s0=2;
			s1=17;
		break;
	}
	switch (BUSx->BlinkSeg) 
	{
		case 0:
			BUSx->Mode&=0xF8FF;
			BUSx->Mode|=0x0000;
		break;
		case 1:
			BUSx->Mode&=0xF8FF;
			BUSx->Mode|=0x0100;
		break;
		case 2:
			BUSx->Mode&=0xF8FF;
			BUSx->Mode|=0x0200;
		break;
	}

	switch (BUSx->Byte_Num)
	{
		case 0:	
			Display_Send_Byte (&BUS1, &CLK1, show(1, BUSx->BlinkSeg)*Seg16[2*(T2%10)]);
		break;
		case 1:
			Display_Send_Byte (&BUS1, &CLK1, show(1, BUSx->BlinkSeg)*Seg16[2*(T2%10)+1]);
		break;
		case 2:
			Display_Send_Byte (&BUS1, &CLK1, show(1, BUSx->BlinkSeg)*Seg16[2*(T2/10)]);
		break;
		case 3:
			Display_Send_Byte (&BUS1, &CLK1, show(1, BUSx->BlinkSeg)*Seg16[2*(T2/10)+1]);
		break;
		case 4:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break; 
		case 5:
			Display_Send_Byte (&BUS1, &CLK1, show(1, BUSx->BlinkSeg)*s1);
		break;
		case 6:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 7:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 8:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 9:
			if (Display_Send_Byte (&BUS1, &CLK1, show(0, BUSx->BlinkSeg)*BUSx->confirm))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_11);
				Delay_ms(2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_11);
			}
		break;
		case 10:
			Display_Send_Byte (&BUS1, &CLK1, show(2, BUSx->BlinkSeg)*Digit[T1%10]);
		break;
		case 11:
			Display_Send_Byte (&BUS1, &CLK1, show(2, BUSx->BlinkSeg)*Digit[T1/10]);
		break;
		case 12:
			Display_Send_Byte (&BUS1, &CLK1, show(2, BUSx->BlinkSeg)*s0);
		break; 
		case 13:
			if (Display_Send_Byte (&BUS1, &CLK1, 0))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_7);
				BUSx->Byte_Num=0;
				Delay_ms(2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_7);
				return 1;
		}

		break;
	}
	TIM7->CR1 |= TIM_CR1_CEN;
	return 0; 
}
char BlinkDelay (Bus_Type* BUSx, Time_Type* TIMEx)
{
	TIM7->CR1 &= ~TIM_CR1_CEN;
	if (BUSx->BlinkSeg>1) BUSx->BlinkSeg=1;
	switch (BUSx->BlinkSeg) 
	{
		case 1:
			BUSx->Mode&=0xF8FF;
			BUSx->Mode|=0x0100;
		break;
		case 0:
			BUSx->Mode&=0xF8FF;
		break;
	}
	
	switch (BUSx->Byte_Num)
	{
		case 0:	
			Display_Send_Byte (&BUS1, &CLK1, Seg16[20]);  //A
		break;
		case 1:
			Display_Send_Byte (&BUS1, &CLK1, Seg16[21]);	
		break;
		case 2:
			Display_Send_Byte (&BUS1, &CLK1, 220);				//З
		break;
		case 3:
			Display_Send_Byte (&BUS1, &CLK1, 48);
		break;
		case 4:
			Display_Send_Byte (&BUS1, &CLK1, 61);					//У
		break; 
		case 5:
			Display_Send_Byte (&BUS1, &CLK1, 17);
		break;
		case 6:
			Display_Send_Byte (&BUS1, &CLK1, Seg16[20]);	//А
		break;
		case 7:
			Display_Send_Byte (&BUS1, &CLK1, Seg16[21]);
		break;
		case 8:
			Display_Send_Byte (&BUS1, &CLK1, Seg16[40]);	//П
		break;
		case 9:
			if (Display_Send_Byte (&BUS1, &CLK1, Seg16[41]))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_11);
				Delay_ms(2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_11);
			}
		break;
		case 10:
			Display_Send_Byte (&BUS1, &CLK1, show(1, BUSx->BlinkSeg)*Digit[SRAM_delay2]);
		break;
		case 11:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 12:
			Display_Send_Byte (&BUS1, &CLK1, show(0, BUSx->BlinkSeg)*Digit[SRAM_delay1]);
		break;
		case 13:
			if (Display_Send_Byte (&BUS1, &CLK1, 0))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_7);
				BUSx->Byte_Num=0;
				Delay_ms(2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_7);
				return 1;
		}

		break;
	}
	TIM7->CR1 |= TIM_CR1_CEN;
	return 0; 
}

void IRaction (Bus_Type* BUSx, Time_Type* TIMEx)
{
	ir_cmd(&BUS1); //переводим код кнопок в команды
	switch (BUSx->Mode&0x77FF)
	{
		case 0x4000:
			SRAM_day=SRAM_day%10;
			if (SRAM_day==0) SRAM_day=1;
			TIMEx->Day_0 = SRAM_day%10;
			TIMEx->Day_1 = SRAM_day/10;
		break;
		case 0x4001:
			SRAM_day=SRAM_day%10+10;
			TIMEx->Day_0 = SRAM_day%10;
			TIMEx->Day_1 = SRAM_day/10;
		break;
		case 0x4002:
			if ((SRAM_day%10+20)<(TIMEx->Days_in_month[SRAM_month]+1))
			{
				SRAM_day=SRAM_day%10+20;
				TIMEx->Day_0 = SRAM_day%10;
				TIMEx->Day_1 = SRAM_day/10;
			}
		break;
		case 0x4003:
		if ((SRAM_day%10+30)<(TIMEx->Days_in_month[SRAM_month]+1))
		{
			SRAM_day=SRAM_day%10+30;
			TIMEx->Day_0 = SRAM_day%10;
			TIMEx->Day_1 = SRAM_day/10;
		}
		break;
		case 0x4010:					//+ к десяткам числа месяца
			if ((SRAM_day+10)<TIMEx->Days_in_month[SRAM_month]+1)
			{
				SRAM_day+=10;
				TIMEx->Day_1 = SRAM_day/10;	
			
			}
		break;
		case 0x4011:					//- к десяткам числа месяца
			if	(SRAM_day/10>0)
			{
				SRAM_day-=10;
				if (SRAM_day==0) SRAM_day=1;
				TIMEx->Day_1 = SRAM_day/10;
				TIMEx->Day_0 = SRAM_day%10;
			}
		break;
		//единицы числа месяца
		case 0x4100:
			SRAM_day=10*(SRAM_day/10);
			if (SRAM_day==0) SRAM_day=1;
			TIMEx->Day_0 = SRAM_day%10;
			TIMEx->Day_1 = SRAM_day/10;
		break;
		case 0x4101:
			if (((10*(SRAM_day/10))+1)<TIMEx->Days_in_month[SRAM_month]+1)
			{
				SRAM_day=(10*(SRAM_day/10))+1;
				TIMEx->Day_0 = SRAM_day%10;
				TIMEx->Day_1 = SRAM_day/10;
			}
		break;
		case 0x4102:
			if (((10*(SRAM_day/10))+2)<TIMEx->Days_in_month[SRAM_month]+1)
			{
				SRAM_day=(10*(SRAM_day/10))+2;
				TIMEx->Day_0 = SRAM_day%10;
				TIMEx->Day_1 = SRAM_day/10;
			}
		break;
		case 0x4103:
			if (((10*(SRAM_day/10))+3)<TIMEx->Days_in_month[SRAM_month]+1)
			{
				SRAM_day=(10*(SRAM_day/10))+3;
				TIMEx->Day_0 = SRAM_day%10;
				TIMEx->Day_1 = SRAM_day/10;
			}
		break;
		case 0x4104:
			if (((10*(SRAM_day/10))+4)<TIMEx->Days_in_month[SRAM_month]+1)
			{
				SRAM_day=(10*(SRAM_day/10))+4;
				TIMEx->Day_0 = SRAM_day%10;
				TIMEx->Day_1 = SRAM_day/10;
			}
		break;
		case 0x4105:
			if (((10*(SRAM_day/10))+5)<TIMEx->Days_in_month[SRAM_month]+1)
			{
				SRAM_day=(10*(SRAM_day/10))+5;
				TIMEx->Day_0 = SRAM_day%10;
				TIMEx->Day_1 = SRAM_day/10;
			}
		break;
		case 0x4106:
			if (((10*(SRAM_day/10))+6)<TIMEx->Days_in_month[SRAM_month]+1)
			{
				SRAM_day=(10*(SRAM_day/10))+6;
				TIMEx->Day_0 = SRAM_day%10;
				TIMEx->Day_1 = SRAM_day/10;
			}
		break;
		case 0x4107:
			if (((10*(SRAM_day/10))+7)<TIMEx->Days_in_month[SRAM_month]+1)
			{
				SRAM_day=(10*(SRAM_day/10))+7;
				TIMEx->Day_0 = SRAM_day%10;
				TIMEx->Day_1 = SRAM_day/10;
			}
		break;
		case 0x4108:
			if (((10*(SRAM_day/10))+8)<TIMEx->Days_in_month[SRAM_month]+1)
			{
				SRAM_day=(10*(SRAM_day/10))+8;
				TIMEx->Day_0 = SRAM_day%10;
				TIMEx->Day_1 = SRAM_day/10;
			}
		break;
		case 0x4109:
			if (((10*(SRAM_day/10))+9)<TIMEx->Days_in_month[SRAM_month]+1)
			{
				SRAM_day=(10*(SRAM_day/10))+9;
				TIMEx->Day_0 = SRAM_day%10;
				TIMEx->Day_1 = SRAM_day/10;
			}
		break;
		case 0x4110:					//+ к единицам числа месяца			
			if ((SRAM_day)<TIMEx->Days_in_month[SRAM_month])
			{
				SRAM_day+=1;
				TIMEx->Day_0 = SRAM_day%10;
				TIMEx->Day_1 = SRAM_day/10;
			}
		break;
		case 0x4111:					//- к единицам числа месяца	
			if	(SRAM_day>1)
			{
				SRAM_day-=1;
				TIMEx->Day_0 = SRAM_day%10;
				TIMEx->Day_1 = SRAM_day/10;
			}
		break;
		case 0x4210:					//+ к месяцу	
			if ((SRAM_month)<11)
			{
				SRAM_month+=1;
				if ((SRAM_day)>TIMEx->Days_in_month[SRAM_month])
				{
					SRAM_day=TIMEx->Days_in_month[SRAM_month];
					TIMEx->Day_0 = SRAM_day%10;
					TIMEx->Day_1 = SRAM_day/10;
				}
				TIMEx->Month=SRAM_month;
				data_to_letter(TIMEx);
			}
		break;
		case 0x4211:					//- к месяцу	
			if	(SRAM_month>0)
			{
				SRAM_month-=1;
				if ((SRAM_day)>TIMEx->Days_in_month[SRAM_month])
				{
					SRAM_day=TIMEx->Days_in_month[SRAM_month];
					TIMEx->Day_0 = SRAM_day%10;
					TIMEx->Day_1 = SRAM_day/10;
				}
				TIMEx->Month=SRAM_month;
				data_to_letter(TIMEx);
			}
		break;
		case 0x4310:					//+ к десяткам года	
			if ((SRAM_year+10)<100)
			{
				SRAM_year+=10;
				TIMEx->Year_0 = SRAM_year%10;
				TIMEx->Year_1 = SRAM_year/10;
				if (!(SRAM_year%4)) 
				{
					TIMEx->Days_in_month[1]=29;
				}
				else
				{
					TIMEx->Days_in_month[1]=28;
					if ((SRAM_day==29)&(SRAM_month==1))
					{
						SRAM_day=28;
						TIMEx->Day_0 = SRAM_day%10;
						TIMEx->Day_1 = SRAM_day/10;
					}
				}
			}
		break;
		case 0x4311:					//- к десяткам года	
			if	(SRAM_year/10>0)
			{
				SRAM_year-=10;
				TIMEx->Year_0 = SRAM_year%10;
				TIMEx->Year_1 = SRAM_year/10;
				if (!(SRAM_year%4)) 
				{
					TIMEx->Days_in_month[1]=29;
				}
				else
				{
					TIMEx->Days_in_month[1]=28;
					if ((SRAM_day==29)&(SRAM_month==1))
					{
						SRAM_day=28;
						TIMEx->Day_0 = SRAM_day%10;
						TIMEx->Day_1 = SRAM_day/10;
					}
				}
				
			}
		break;
		case 0x4410:					//+ к единицам года		
			if ((SRAM_year+1)<100)
			{
				SRAM_year+=1;
				TIMEx->Year_0 = SRAM_year%10;
				TIMEx->Year_1 = SRAM_year/10;
				if (!(SRAM_year%4)) 
				{
					TIMEx->Days_in_month[1]=29;
				}
				else
				{
					TIMEx->Days_in_month[1]=28;
					if ((SRAM_day==29)&(SRAM_month==1))
					{
						SRAM_day=28;
						TIMEx->Day_0 = SRAM_day%10;
						TIMEx->Day_1 = SRAM_day/10;
					}
				}
			}
		break;
		case 0x4411:					//- к единицам года	
			if	(SRAM_year>0)
			{
				SRAM_year-=1;
				TIMEx->Year_0 = SRAM_year%10;
				TIMEx->Year_1 = SRAM_year/10;
				if (!(SRAM_year%4))
				{
					TIMEx->Days_in_month[1]=29;
				}
				else
				{
					TIMEx->Days_in_month[1]=28;
					if ((SRAM_day==29)&(SRAM_month==1))
					{
						SRAM_day=28;
						TIMEx->Day_0 = SRAM_day%10;
						TIMEx->Day_1 = SRAM_day/10;
					}
				}
			}
		break;
		
		case 0x5000:
			SRAM_hour=SRAM_hour%10;
			TIMEx->Hour_0 = SRAM_hour%10;
			TIMEx->Hour_1 = SRAM_hour/10;
		break;
		case 0x5001:
			SRAM_hour=SRAM_hour%10 + 10;
			TIMEx->Hour_0 = SRAM_hour%10;
			TIMEx->Hour_1 = SRAM_hour/10;
		break;
		case 0x5002:
			if (SRAM_hour<14)
			{
				SRAM_hour=SRAM_hour%10 + 20;
				TIMEx->Hour_0 = SRAM_hour%10;
				TIMEx->Hour_1 = SRAM_hour/10;
			}
		break;
		case 0x5100:
			SRAM_hour=10*(SRAM_hour/10);
			TIMEx->Hour_0 = SRAM_hour%10;
			TIMEx->Hour_1 = SRAM_hour/10;
		break;
		case 0x5101:
			SRAM_hour=(10*(SRAM_hour/10))+1;
			TIMEx->Hour_0 = SRAM_hour%10;
			TIMEx->Hour_1 = SRAM_hour/10;
		break;
		case 0x5102:
			SRAM_hour=(10*(SRAM_hour/10))+2;
			TIMEx->Hour_0 = SRAM_hour%10;
			TIMEx->Hour_1 = SRAM_hour/10;
		break;
		case 0x5103:
			SRAM_hour=(10*(SRAM_hour/10))+1;
			TIMEx->Hour_0 = SRAM_hour%10;
			TIMEx->Hour_1 = SRAM_hour/10;
		break;
		case 0x5104:
			if (SRAM_hour<20)
			{
				SRAM_hour=(10*(SRAM_hour/10))+1;
				TIMEx->Hour_0 = SRAM_hour%10;
				TIMEx->Hour_1 = SRAM_hour/10;
			}
		break;
		case 0x5105:
			if (SRAM_hour<20)
			{
				SRAM_hour=(10*(SRAM_hour/10))+5;
				TIMEx->Hour_0 = SRAM_hour%10;
				TIMEx->Hour_1 = SRAM_hour/10;
			}
		break;
		case 0x5106:
			if (SRAM_hour<20)
			{
				SRAM_hour=(10*(SRAM_hour/10))+6;
				TIMEx->Hour_0 = SRAM_hour%10;
				TIMEx->Hour_1 = SRAM_hour/10;
			}
		break;
			case 0x5107:
			if (SRAM_hour<20)
			{
				SRAM_hour=(10*(SRAM_hour/10))+7;
				TIMEx->Hour_0 = SRAM_hour%10;
				TIMEx->Hour_1 = SRAM_hour/10;
			}
		break;
			case 0x5108:
			if (SRAM_hour<20)
			{
				SRAM_hour=(10*(SRAM_hour/10))+8;
				TIMEx->Hour_0 = SRAM_hour%10;
				TIMEx->Hour_1 = SRAM_hour/10;
			}
		break;
			case 0x5109:
			if (SRAM_hour<20)
			{
				SRAM_hour=(10*(SRAM_hour/10))+9;
				TIMEx->Hour_0 = SRAM_hour%10;
				TIMEx->Hour_1 = SRAM_hour/10;
			}
		break;
		case 0x5010:					//+ к десяткам часов
			if (SRAM_hour<14)
			{
				SRAM_hour+=10;
				if(SRAM_hour>23) SRAM_hour=23;
				TIMEx->Hour_0 = SRAM_hour%10;
				TIMEx->Hour_1 = SRAM_hour/10;
			}
		break;
		case 0x5011:					//- к десяткам часов
			if	(SRAM_hour/10>0)
			{
				SRAM_hour-=10;
				TIMEx->Hour_0 = SRAM_hour%10;
				TIMEx->Hour_1 = SRAM_hour/10;
			}
		break;
		case 0x5110:					//+ к единицам часов
			if (SRAM_hour<23)
			{
				SRAM_hour+=1;
				if(SRAM_hour>23) SRAM_hour=0;
				TIMEx->Hour_0 = SRAM_hour%10;
				TIMEx->Hour_1 = SRAM_hour/10;
			}
		break;
		case 0x5111:					//- к единицам часов
			if	(SRAM_hour>0)
			{
				SRAM_hour-=1;
				TIMEx->Hour_0 = SRAM_hour%10;
				TIMEx->Hour_1 = SRAM_hour/10;
			}
		break;	
// Минуты
		case 0x5200:
			SRAM_min=SRAM_min%10;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5201:
			SRAM_min=SRAM_min%10 + 10;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5202:
			SRAM_min=SRAM_min%10 + 20;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5203:
			SRAM_min=SRAM_min%10 + 30;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5204:
			SRAM_min=SRAM_min%10 + 40;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5205:
			SRAM_min=SRAM_min%10 + 50;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		
		case 0x5300:
			SRAM_min=(10*(SRAM_min/10));
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5301:
			SRAM_min=(10*(SRAM_min/10))+1;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5302:
			SRAM_min=(10*(SRAM_min/10))+2;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5303:
			SRAM_min=(10*(SRAM_min/10))+3;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5304:
			SRAM_min=(10*(SRAM_min/10))+4;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5305:
			SRAM_min=(10*(SRAM_min/10))+5;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5306:
			SRAM_min=(10*(SRAM_min/10))+6;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5307:
			SRAM_min=(10*(SRAM_min/10))+7;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5308:
			SRAM_min=(10*(SRAM_min/10))+8;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5309:
			SRAM_min=(10*(SRAM_min/10))+9;
			TIMEx->Min_0 = SRAM_min%10;
			TIMEx->Min_1 = SRAM_min/10;
		break;
		case 0x5210:					//+ к десяткам минут
			if (SRAM_min<50)
			{
				SRAM_min+=10;
				TIMEx->Min_0 = SRAM_min%10;
				TIMEx->Min_1 = SRAM_min/10;
			}
		break;
		case 0x5211:					//- к десяткам минут
			if	(SRAM_min/10>0)
			{
				SRAM_min-=10;
				TIMEx->Min_0 = SRAM_min%10;
				TIMEx->Min_1 = SRAM_min/10;
			}
		break;
		case 0x5310:					//+ к единицам минут
			if ((SRAM_min%10)<9)
			{
				SRAM_min+=1;
				TIMEx->Min_0 = SRAM_min%10;
				TIMEx->Min_1 = SRAM_min/10;
			}
		break;
		case 0x5311:					//- к единицам минут
			if	(SRAM_min>0)
			{
				SRAM_min-=1;
				TIMEx->Min_0 = SRAM_min%10;
				TIMEx->Min_1 = SRAM_min/10;
			}
		break;					
		case 0x6010:					//+ к Delay1
			SRAM_delay1++;
			if (SRAM_delay1>9) SRAM_delay1=9;
		break;
		case 0x6011:					//- к Delay1
			if (SRAM_delay1>0) SRAM_delay1--;
		break;
		case 0x6110:					//+ к Delay2
			SRAM_delay2++;
			if (SRAM_delay2>9) SRAM_delay2=9;
		break;
		case 0x6111:					//- к Delay2
			if (SRAM_delay2>0) SRAM_delay2--;
		break;
		case 0x7210:
			switch (SRAM_ADC&0x0080)
			{
				case 0x0000:
					if (T1<99) SRAM_ADC++;
				break;
				case 0x0080:
					if (T1>0)  
					{
						SRAM_ADC--;
						if (T1==0) SRAM_ADC&=~0x80;
					}
					else
					{
						SRAM_ADC&=~0x80;
						SRAM_ADC++;
					}
				break;
			}
			A1=(u16)BUSx->ADC_Dev;
		break;
		case 0x7211:
			switch (SRAM_ADC&0x0080)
			{
				case 0x0000:
					if (T1>0)  
					{
						if ((SRAM_ADC&0x00FF)!= (SRAM_ADC&0xFF00)>>8) SRAM_ADC--;
					}
					else
					{
						SRAM_ADC|=0x81;
					}
				break;
				case 0x0080:
					if ((T1<99) && ((SRAM_ADC&0x00FF)!= ((SRAM_ADC&0xFF00)>>8))) SRAM_ADC+=1;
				break;
			}
			A1=(u16)BUSx->ADC_Dev;
		break;
		case 0x7110:
		switch (SRAM_ADC&0x8000)
			{
				case 0x0000:
					if ((T2<99) && ((SRAM_ADC&0x00FF)!= ((SRAM_ADC&0xFF00)>>8))) SRAM_ADC+=256;
				break;
				case 0x8000:
					if (T2) 
					{
						if ((SRAM_ADC&0x00FF)!= (SRAM_ADC&0xFF00)>>8) SRAM_ADC-=256;
						if (T2==0) SRAM_ADC&=~0x8000;
					}
					else
					{
						if ((SRAM_ADC&0x00FF)!= (SRAM_ADC&0xFF00)>>8)
						{
							SRAM_ADC&=~0x8000;
							SRAM_ADC+=256;
						}
					}
				break;
			}
			A2=(u16)BUSx->ADC_Dev;
		break;
		case 0x7111:
			switch (SRAM_ADC&0x8000)
				{
					case 0x0000:
						if (T2)  
						{
						  SRAM_ADC-=256;
						}
						else
						{
							SRAM_ADC|=0x8100;
						}
					break;
					case 0x8000:
						if (T2<99) SRAM_ADC+=256;
					break;
				}
			A2=(u16)BUSx->ADC_Dev;
		break;	
		case 0x7010:
			BUSx->confirm=255;
		break;		
		case 0x7011:
			BUSx->confirm=17;
		break;	

	}
	
		
	
	if (((BUSx->Mode&0x40FF)==0x4013) && (BUSx->BlinkSeg<8))
	{
		BUSx->BlinkSeg+=1;
	}		
	if ((BUSx->Mode&0x40FF)==0x4012) 
	{
		BUSx->BlinkSeg-=1;
	}
	if (((BUSx->Mode&0x40FF)==0x4012) && (BUSx->BlinkSeg>8))
	{
		BUSx->BlinkSeg=8;
	}
	BUSx->Mode|=0x00FF;
}
char Display_Send_TempTime (Bus_Type* BUSx, Time_Type* TIMEx)
{
	u8 s0,s1;
	data_to_letter(TIMEx);
	TIM7->CR1 &= ~TIM_CR1_CEN;
	if (BUSx->Temp>0)
	{
		s0=2*(((u16)BUSx->Temp)%10);
		s1=2*((u16)BUSx->Temp/10);
		BUSx->sign=85;
	}
	if (BUSx->Temp<0)
	{
		s0=2*(((u16)-BUSx->Temp)%10);
		s1=2*((u16)-BUSx->Temp/10);
		BUSx->sign=17;
	}
	if (BUSx->Temp==0)
	{
		s0=0;
		s1=0;
		BUSx->sign=0;
	}
	switch (BUSx->Byte_Num)
	{
		case 0:
			Display_Send_Byte (&BUS1, &CLK1,Seg16[s0]);
		break;
		case 1:
			Display_Send_Byte (&BUS1, &CLK1, Seg16[s0+1]);
		break;
		case 2:
			Display_Send_Byte (&BUS1, &CLK1, Seg16[s1]);
		break;
		case 3:
			Display_Send_Byte (&BUS1, &CLK1, Seg16[s1+1]);
		break;
		case 4:
			Display_Send_Byte (&BUS1, &CLK1, 0);
		break;
		case 5:
			Display_Send_Byte (&BUS1, &CLK1, BUSx->sign);
		break;
		case 6:
			Display_Send_Byte (&BUS1, &CLK1, Seg16[44]);
		break;
		case 7:
			Display_Send_Byte (&BUS1, &CLK1, Seg16[45]);
		break;
		case 8:
			Display_Send_Byte (&BUS1, &CLK1, 96);
		break;
		case 9:
			if (Display_Send_Byte (&BUS1, &CLK1, 80))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_11);
				Delay_ms(2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_11);
			}
		break;
		case 10:
			Display_Send_Byte (&BUS1, &CLK1, Digit[TIMEx->Min_0]);
		break;
		case 11:
			Display_Send_Byte (&BUS1, &CLK1, Digit[TIMEx->Min_1]|TIMEx->Dot);
		break;
		case 12:
			Display_Send_Byte (&BUS1, &CLK1, Digit[TIMEx->Hour_0]);
		break;
		case 13:
			if (Display_Send_Byte (&BUS1, &CLK1, Digit[TIMEx->Hour_1]))
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_7);
				BUSx->Byte_Num=0;
				Delay_ms(2);
				GPIO_ResetBits(GPIOA,GPIO_Pin_7);
				return 1;
		}

		break;
	}
	TIM7->CR1 |= TIM_CR1_CEN;
	return 0; 
}

void get_temp (Bus_Type* BUSx)
{ 
	
	if (BUSx->ADC_count<16)
	{
		ADC1->CR2 |= ADC_CR2_SWSTART;
		while (!(ADC1->SR & ADC_SR_EOC));
		BUSx->ADC_count++;
		BUSx->ADC_Dev=BUSx->ADC_Dev+(ADC1->DR - BUSx->ADC_Dev)*0.5;
		return;
	}
	ADC1->CR2 |= ADC_CR2_SWSTART;
	while (!(ADC1->SR & ADC_SR_EOC));
	BUSx->ADC_Dev=BUSx->ADC_Dev+(ADC1->DR - BUSx->ADC_Dev)*0.001;
	BUSx->Temp=(BUSx->ADC_Dev-A2);
	BUSx->Temp/=(A1-A2);
	switch (SRAM_ADC&0x8080)
	{
		case 0x0000:	
			BUSx->Temp*=(T1-T2);
			BUSx->Temp+=T2;
		break;
		case 0x0080:
			BUSx->Temp*=(-T1-T2);
			BUSx->Temp+=T2;
		break;
		case 0x8000:
			BUSx->Temp*=T1+T2;
			BUSx->Temp-=T2;
		break;
		case 0x8080:
			BUSx->Temp*=(T2-T1);
			BUSx->Temp-=T2;		
		break;
	}
}

void TIME_Init (Time_Type* TIMEx)
{
	TIMEx->Days_in_month[0]=31;
	if (!SRAM_year%4) 
	{
		TIMEx->Days_in_month[1]=29;
	}
	else
	{
		TIMEx->Days_in_month[1]=28;
	}
	TIMEx->Days_in_month[2]=31;
	TIMEx->Days_in_month[3]=30;
	TIMEx->Days_in_month[4]=31;
	TIMEx->Days_in_month[5]=30;
	TIMEx->Days_in_month[6]=31;
	TIMEx->Days_in_month[7]=31;
	TIMEx->Days_in_month[8]=30;
	TIMEx->Days_in_month[9]=31;
	TIMEx->Days_in_month[10]=30;
	TIMEx->Days_in_month[11]=31;
	TIMEx->Min_0 = SRAM_min%10;
	TIMEx->Min_1 = SRAM_min/10;
	TIMEx->Hour_0 = SRAM_hour%10;
	TIMEx->Hour_1 = SRAM_hour/10;
	TIMEx->Day_0 = SRAM_day%10;
	TIMEx->Day_1 = SRAM_day/10;
	TIMEx->Month=SRAM_month;
	TIMEx->Year_0 = SRAM_year%10;
	TIMEx->Year_1 = SRAM_year/10;
	
}
void time_line_tick (Time_Type* TIMEx)
{
	SRAM_min++;
	TIMEx->Min_0 = SRAM_min%10;
	TIMEx->Min_1 = SRAM_min/10;
	
	if (SRAM_min==60)																		
	{
		SRAM_hour++; 
		TIMEx->Hour=SRAM_hour;
		TIMEx->Hour_0 = SRAM_hour%10;
		TIMEx->Hour_1 = SRAM_hour/10;
		TIMEx->Min_0 = 0;
		TIMEx->Min_1 = 0;
		SRAM_min=0;
	}	else return;
	
	if (SRAM_hour==24)																	
	{
		SRAM_day++; 
		TIMEx->Day=SRAM_day;
		TIMEx->Day_0 = SRAM_day%10;
		TIMEx->Day_1 = SRAM_day/10;
		TIMEx->Hour_0 = 0;
		TIMEx->Hour_1 = 0;
		SRAM_hour=0;
		TIMEx->Hour=0;
		TIMEx->Data_UPD=1;
	}	else return;
	
	if (SRAM_day==TIMEx->Days_in_month[SRAM_month]+1)  	
	{
		SRAM_month++;
		TIMEx->Month=SRAM_month;
		SRAM_day=1;
		TIMEx->Day=1;
		TIMEx->Day_0 = 1;
		TIMEx->Day_1 = 0;
	}	else return;
	
	if (SRAM_month==12)
	{
		SRAM_year++;
		TIMEx->Year=SRAM_year;
		TIMEx->Year_0 = SRAM_year%10;
		TIMEx->Year_1 = SRAM_year/10;
		SRAM_month=0;
		TIMEx->Month=0;
		if (!(SRAM_year%4)) 
		{
			TIMEx->Days_in_month[1]=29;
		}
		else
		{
			TIMEx->Days_in_month[1]=28;
		}
	} 
			
		
}

void time_line_action (Time_Type* TIMEx, Bus_Type* BUSx)
{
	
		if ((!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))&&!BUSx->Key_Plus_Pressed) 
		{
			Delay_ms(5);
			if (!GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
			{
				TIM6->CR1 &= ~TIM_CR1_CEN;
				time_line_tick(TIMEx);
				BUSx->Key_Plus_Pressed=1;
				TIM6->CR1 |= TIM_CR1_CEN;
			}
		}
		if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
		{
			if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2))
			{
				BUSx->Key_Plus_Pressed=0;
			}
		}

}


	



void capt_IRQ (Bus_Type* BUSx)
{
	
if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) 
	{
		TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		
		BUSx->IR_cnt1 = TIM_GetCapture1(TIM3);
		BUSx->IR_cnt2 = TIM_GetCapture2(TIM3);

		
		if (BUSx->IR_captCount < MAX_CAPT_COUNT) 
		{
			BUSx->IR_captArray[BUSx->IR_captCount*2] = BUSx->IR_cnt1; //width
			BUSx->IR_captArray[BUSx->IR_captCount*2+1] = BUSx->IR_cnt2; //pulse
			BUSx->IR_captCount++;
			BUSx->IR_recState = REC_Recording;
		}	
		else
		{
			BUSx->IR_recState = REC_Iddle;
		}
	}	

	if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET) 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
		if (BUSx->IR_recState == REC_Recording)
		{
			BUSx->IR_recState = REC_Iddle;	
			if (BUSx->IR_captCount< MAX_CAPT_COUNT) 
			{
				BUSx->IR_captCount = 0;
			}
		}
		TIM_ITConfig(TIM3, TIM_IT_CC3, DISABLE);
	}
}



