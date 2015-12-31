#ifndef INIT_H
#define INIT_H


#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_adc.h"
#include "databus.h"
void tim_init(void);
void gpio_init (void);
void usart_init(void);
void line_init (void);
void adc_init (Bus_Type* BUSx);



#endif
