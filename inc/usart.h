#ifndef usart_h
#define usart_h

#include "stm32f10x_usart.h"
#include "eeprom.h"

void USART1_Send(char chr);
void USART1_Send_String(char* str);

#endif
