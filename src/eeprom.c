#include "eeprom.h"
unsigned char data[256];
unsigned char EDID[256]={
0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x34,0xA9,0x06,0xC1,0x44,0x00,0x00,0x00,
0x17,0x12,0x01,0x03,0x80,0x00,0x00,0x78,0x0A,0xDA,0xFF,0xA3,0x58,0x4A,0xA2,0x29,
0x17,0x49,0x4B,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x1D,0x00,0xBC,0x52,0xD0,0x1E,0x20,0xB8,0x28,
0x55,0x40,0xB9,0x88,0x21,0x00,0x00,0x1E,0x01,0x1D,0x00,0x72,0x51,0xD0,0x1E,0x20,
0x6E,0x28,0x55,0x00,0xB9,0x88,0x21,0x00,0x00,0x1E,0x00,0x00,0x00,0xFC,0x00,0x50,
0x41,0x4E,0x41,0x53,0x4F,0x4E,0x49,0x43,0x2D,0x54,0x56,0x0A,0x00,0x00,0x00,0xFD,
0x00,0x30,0x3D,0x0F,0x2D,0x08,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x01,0xD0,
0x02,0x03,0x1C,0x72,0x4C,0x93,0x84,0x14,0x12,0x05,0x03,0x11,0x07,0x16,0x06,0x15,
0x01,0x23,0x09,0x07,0x01,0x66,0x03,0x0C,0x00,0x20,0x00,0x80,0x01,0x1D,0x80,0xD0,
0x72,0x1C,0x16,0x20,0x10,0x2C,0x25,0x80,0xB9,0x88,0x21,0x00,0x00,0x9E,0x8C,0x0A,
0xD0,0x90,0x20,0x40,0x31,0x20,0x0C,0x40,0x55,0x00,0xB9,0x88,0x21,0x00,0x00,0x18,
0x01,0x1D,0x80,0x18,0x71,0x1C,0x16,0x20,0x58,0x2C,0x25,0x00,0xB9,0x88,0x21,0x00,
0x00,0x9E,0x8C,0x0A,0xD0,0x8A,0x20,0xE0,0x2D,0x10,0x10,0x3E,0x96,0x00,0xB9,0x88,
0x21,0x00,0x00,0x18,0x8C,0x0A,0xD0,0x90,0x20,0x40,0x31,0x20,0x0C,0x40,0x55,0x00,
0x0F,0x88,0x21,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xAF
};
void I2C_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	I2C_StructInit(&I2C_InitStructure);
	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_InitStructure.I2C_OwnAddress1 = 1;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

void I2C_single_write(uint8_t HW_address, uint8_t addr, uint8_t data)
{
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1, ENABLE);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Transmitter);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, addr);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C1, data);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2C1, ENABLE);
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
}

void I2C_write(uint8_t HW_address, uint8_t end_addr)
{
	uint16_t addr;
	
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));  														
	I2C_GenerateSTART(I2C1, ENABLE);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Transmitter);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, addr);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	for (addr=0;addr<=end_addr;addr++)																	//Проходим от 0 до конца
	{
		I2C_SendData(I2C1, EDID[addr]);																		//Шлем из массива data[]
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		if ((addr+1)%8) continue;					//Если остаток от деления адреса на 8 равен 1, запускаем цикл заново

		I2C_GenerateSTOP(I2C1, ENABLE);		//Если остаток равен нулю, то генерируем стоп
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); 
		if (addr==end_addr)	return;				//Если передан последний байт, то выходим из функции
		Delay_ms(5);
			while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));		
		I2C_GenerateSTART(I2C1, ENABLE);																		//Генерируем старт
			while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
		I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Transmitter);		//Шлем заголовок адреса
			while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
		I2C_SendData(I2C1, addr);																						//Шлем следующий адрес страницы
			while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));		//Ждем ответ
	}
}

uint8_t I2C_single_read(uint8_t HW_address, uint8_t addr)
{
	uint8_t data;
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1, ENABLE);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Transmitter);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, addr);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTART(I2C1, ENABLE);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Receiver);
		while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));
	
	data = I2C_ReceiveData(I2C1);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	I2C_GenerateSTOP(I2C1, ENABLE);
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	return data;
}

void I2C_send_to_usart(uint8_t HW_address, uint16_t end_addr)
{
	uint16_t addr;
		while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1, ENABLE);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Transmitter);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, 0);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTART(I2C1, ENABLE);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, HW_address, I2C_Direction_Receiver);
		while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	for (addr=0;addr<end_addr;addr++)
	{
		data[addr]=I2C_ReceiveData(I2C1);
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		I2C_AcknowledgeConfig(I2C1, ENABLE);
	}
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	data[end_addr]=I2C_ReceiveData(I2C1);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	for (addr=0;addr<=end_addr;addr++)
	{
		USART1_Send(data[addr]);
	}
}
