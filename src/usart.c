#include "usart.h"

void USART1_Send(char chr) {
  while(!(USART1->SR & USART_SR_TC));
  USART1->DR = chr;
}

void USART1_Send_String(char* str) {
  int i=0;
  while(str[i])
    USART1_Send(str[i++]);
}
