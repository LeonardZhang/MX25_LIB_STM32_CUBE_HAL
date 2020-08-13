/*
* MX25L.h
*/ 

#ifndef MX25L_H_
#define MX25L_H_

#ifndef STM32L4xx_HAL_H
#include "stm32l4xx_hal.h"
#endif

extern SPI_HandleTypeDef hspi2;

#ifndef bool
	#define bool unsigned short
#endif


#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif
	
	
#define SLAVE_GPIO_PORT GPIOB 
#define SLAVELINE GPIO_PIN_12
#define SLAVE_SELECT_STATE GPIO_PIN_RESET
#define SLAVE_DESELECT_STATE GPIO_PIN_SET
#define SPI_FLASH_DELAY 10
	
#define MX25_PAGE_SIZE    256
#define MX25_SECTOR_SIZE  4096	


#define COMMAND_WREN  0x06
#define COMMAND_WRDI  0x04
#define COMMAND_RDSR  0x05
#define COMMAND_WRSR  0x01
#define COMMAND_READ  0x03
#define COMMAND_FREAD 0x0B
#define COMMAND_WRITE 0x02		// page program command
#define COMMAND_RDID  0x9F
#define COMMAND_SE    0x20
#define COMMAND_BE    0x52
#define COMMAND_CE    0x60

#define STATUS_WIP    0x01
#define STATUS_WEL    0x02
#define STATUS_BP0    0x04
#define STATUS_BP1    0x08
#define STATUS_BP2    0x10
#define STATUS_BP3    0x20
#define STATUS_RES    0x40
#define STATUS_SWRD   0x80

#define DUMMY         0xFF

	

HAL_StatusTypeDef SPI_Flash_Transfer(uint8_t data);
	
void Get_Identification(uint8_t data[3]);
	
uint8_t GetStatus(void);
	
void SetWriteEnable(bool enable);
	
void SetStatus(uint8_t status);
	
void SectorErase(uint32_t address);
	
void BlockErase(uint32_t address);

void ChipErase(void);

void SetAddress(uint32_t address);

void Write(uint32_t address, const uint8_t * data, uint32_t length);

void ext_flash_write_multiple_pages(uint32_t address, const uint8_t * data, uint32_t length);

void Read(uint32_t address, uint8_t * data, uint32_t length);
	
void FastRead(uint32_t address, uint8_t * data, uint32_t length);
	
void ProtectBlock(bool bp0, bool bp1, bool bp2, bool bp3);

#endif /* MX25L_H_ */


	