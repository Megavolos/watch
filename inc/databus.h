#ifndef DATABUS_H
#define DATABUS_H
#define STATE_Byte_Sent 4
#define STATE_Wait_End 0
#define STATE_Wait_Begin 1
#define STATE_NotWait_End 2
#define STATE_NotWait_Begin 3
#define ON 1
#define OFF 0
//IR
#define REC_Recording 1
#define REC_Captured 2
#define REC_Iddle 0
#define IR_NO_COMMAND   0
#define IR_NEC_START_PULSE      4420
#define IR_NEC_START_WIDTH      8910
#define IR_NEC_0_WIDTH  950
#define IR_NEC_0_PULSE  550
#define IR_NEC_1_WIDTH 1950
#define IR_NEC_1_PULSE 550
#define IR_NEC_SPLIT_WIDTH 4900
#define IR_NEC_SPLIT_PULSE 4900
#define IR_NEC_TOL 50
#define MAX_CAPT_COUNT 38
#define hw_addr 2608
#define SRAM_min 		BKP->DR1
#define SRAM_hour		BKP->DR2
#define SRAM_day		BKP->DR3
#define SRAM_month	BKP->DR4
#define SRAM_year		BKP->DR5
#define SRAM_delay1 BKP->DR6
#define SRAM_delay2 BKP->DR7
#define SRAM_ADC		BKP->DR9
#define A1 BKP->DR11 
#define A2 BKP->DR10 
#define T1 (BKP->DR9&0x007F)
#define T2 ((BKP->DR9&0x7F00)>>8)
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "delay.h"
#include "stm32f10x_tim.h"

typedef struct
{
	unsigned char Bit_Num;
	unsigned char Byte_Num;
	unsigned char State;
	unsigned char Latch;	
	unsigned char CE;
	unsigned char REQ0;
	unsigned char REQ1;
	unsigned char Seg;	
	unsigned char Key_Plus_Pressed;
	unsigned int  IR_cmd;
	unsigned char IR_recState;
	unsigned char IR_captCount;
	unsigned int	IR_captArray[76];
	unsigned char IR_pwr_cnt;
	unsigned char BlinkSeg;
	uint16_t IR_cnt1;
	uint16_t IR_cnt2;
	uint16_t Mode;
	float ADC_Dev;
	u8 ADC_count;
	u8 sign;
	u8 confirm;
	float Temp;	
	
} Bus_Type;

typedef struct
{
	char Min_0;
	char Min_1;
	char Hour_0;
	char Hour_1;
	char Hour;
	char Day_0;
	char Day;
	char Day_1;
	char Month;
	char Year_0;
	char Year_1;
	char Year;
	char Dot;
	char Dot_Prev;
	char Data_UPD;
	unsigned char Days_in_month[12];
	unsigned char display2 [10];
	unsigned char Delay_Cnt;
} Time_Type;

typedef struct
{
	int Tim_Cnt;
	int Tim_Cnt_Prev;
	int Clk_Cnt;

	int Half_Second_Cnt;
} Clock_Type;



char Display_Send_Byte (Bus_Type* BUSx, Clock_Type* CLKx, unsigned char Byte);
void BUS_Init (Bus_Type* BUSx);
void Half_Second_Tik (Time_Type* TIMEx,Clock_Type* CLKx);
void Display_Send_4Byte (Bus_Type* BUSx, unsigned char* DIG3, unsigned char* DIG2, unsigned char* DIG1, unsigned char* DIG0);
char Display_Send_Time (Bus_Type* BUSx, Time_Type* TIMEx);
void CLK_Tik (Clock_Type* CLKx);
char Display_Send_DataTime (Bus_Type* BUSx, Time_Type* TIMEx);
void data_to_letter (Time_Type* TIMEx);
void IRaction (Bus_Type* BUSx, Time_Type* TIMEx);
char BlinkDDMMMYYYY (Bus_Type* BUSx, Time_Type* TIMEx);
void ir_cmd (Bus_Type* BUSx);
unsigned int IR_decodeNEC(Bus_Type* BUSx);
char BlinkHHMM (Bus_Type* BUSx, Time_Type* TIMEx);
char Display_Send_TempTime (Bus_Type* BUSx, Time_Type* TIMEx);
char BlinkADC (Bus_Type* BUSx, Time_Type* TIMEx);
char BlinkDelay (Bus_Type* BUSx, Time_Type* TIMEx);
void get_temp (Bus_Type* BUSx);
void TIME_Init (Time_Type* TIMEx);
void time_line_tick (Time_Type* TIMEx);
void time_line_action (Time_Type* TIMEx, Bus_Type* BUSx);
void capt_IRQ (Bus_Type* BUSx);
void ir_cmd (Bus_Type* BUSx);
#endif
