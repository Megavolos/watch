#ifndef EEPROM_H
#define EEPROM_H
//***************************************************************************//**
//EEPROM LIB v 1.0
//******************************************************************************/
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "usart.h"
#include "delay.h"
//#define I2C_FLASH_PAGESIZE     32 //not used yet
#define EEPROM_HW_ADDRESS      0xA0   /* E0 = E1 = E2 = 0 */
#define I2C_EE             I2C1//interface number

void I2C_init(void);
void I2C_single_write(uint8_t HW_address, uint8_t addr, uint8_t data);
uint8_t I2C_single_read(uint8_t HW_address, uint8_t addr);
void I2C_write(uint8_t HW_address, uint8_t end_addr);
void I2C_send_to_usart(uint8_t HW_address, uint16_t end_addr);
#endif
