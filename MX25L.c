/*
* MX25L.c
*/

#include "MX25L.h"



//#define SLAVESELECT (PORTB &= ~(1<<PORTB2))
//#define SLAVEDESELECT (PORTB |= (1<<PORTB2))





HAL_StatusTypeDef SPI_Transfer(uint8_t data)
{
	uint8_t temp_data = data ; 
	return HAL_SPI_Transmit(&hspi2 , &temp_data , 1  , SPI_FLASH_DELAY);
}

void Get_Identification(uint8_t data[3])
{
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_SELECT_STATE);
	SPI_Transfer(COMMAND_RDID);
	HAL_SPI_Receive(&hspi2 , data , 3  , SPI_FLASH_DELAY);
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_DESELECT_STATE);
}

uint8_t GetStatus()
{
	uint8_t status;
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_SELECT_STATE);
	SPI_Transfer(COMMAND_RDSR);
	HAL_SPI_Receive(&hspi2 , &status , 1  , SPI_FLASH_DELAY);
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_DESELECT_STATE);
	return status;
}

void SetWriteEnable(bool enable)
{
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_SELECT_STATE);
	SPI_Transfer(enable ? COMMAND_WREN : COMMAND_WRDI);
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_DESELECT_STATE);
}

void SetStatus(uint8_t status)
{
	SetWriteEnable(true);
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_SELECT_STATE);
	SPI_Transfer(COMMAND_WRSR);
	SPI_Transfer(status);
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_DESELECT_STATE);
}

void Erase(uint8_t command, uint32_t address)
{
}

void SectorErase(uint32_t address)
{
	SetWriteEnable(true);
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_SELECT_STATE);
	SPI_Transfer(COMMAND_SE);
	SetAddress(address);
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_DESELECT_STATE);
	while(GetStatus() & STATUS_WIP);
}

void BlockErase(uint32_t address)
{
	SetWriteEnable(true);
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_SELECT_STATE);
	SPI_Transfer(COMMAND_BE);
	SetAddress(address);
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_DESELECT_STATE);
	while(GetStatus() & STATUS_WIP);
}

void ChipErase()
{
	SetWriteEnable(true);
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_SELECT_STATE);
	SPI_Transfer(COMMAND_CE);
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_DESELECT_STATE);
	while(GetStatus() & STATUS_WIP);
}

void SetAddress(uint32_t address)
{
	SPI_Transfer( (uint8_t) (address >> 16) );
	SPI_Transfer( (uint8_t) (address >> 8) );
	SPI_Transfer( (uint8_t) address );
}


/*
	@info : Page program command
*/
void Write(uint32_t address, const uint8_t * data, uint32_t length)
{
	SetWriteEnable(true);
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_SELECT_STATE);
	SPI_Transfer(COMMAND_WRITE);
	SetAddress(address);
	for(uint32_t i = 0;i < length;i ++)
	{
		SPI_Transfer(*(data + i));
	}
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_DESELECT_STATE);
	while(GetStatus() & STATUS_WIP);
}

void ext_flash_write_multiple_pages(uint32_t address, const uint8_t * data, uint32_t length){
	int temp_length = length ;
	int newAddress = address ;
	int data_index = 0 ;
	while(temp_length > 0 ){
		if(temp_length > MX25_PAGE_SIZE){
			Write(newAddress, &data[data_index], MX25_PAGE_SIZE);
		}
		else{
			Write(newAddress, &data[data_index], temp_length);
		}
		temp_length -= MX25_PAGE_SIZE ;
		data_index +=  MX25_PAGE_SIZE ;
		newAddress +=  MX25_PAGE_SIZE;
	}
}


void Read(uint32_t address, uint8_t * data, uint32_t length)
{
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_SELECT_STATE);
	SPI_Transfer(COMMAND_READ);
	SetAddress(address);
	HAL_SPI_Receive(&hspi2 , data , length  , SPI_FLASH_DELAY);
//	for(uint32_t i = 0;i < length;i ++)
//	{
//		*(data + i) = SPI_Transfer(DUMMY);
//	}
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_DESELECT_STATE);
	while(GetStatus() & STATUS_WIP);
}

void FastRead(uint32_t address, uint8_t * data, uint32_t length)
{
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_SELECT_STATE);
	SPI_Transfer(COMMAND_READ);
	SetAddress(address);
	SPI_Transfer(DUMMY);
	HAL_SPI_Receive(&hspi2 , data , length  , SPI_FLASH_DELAY);
//	for(uint32_t i = 0;i < length;i ++)
//	{
//		*(data + i) = SPI_Transfer(DUMMY);
//	}
	HAL_GPIO_WritePin(SLAVE_GPIO_PORT , SLAVELINE , SLAVE_DESELECT_STATE);
	while(GetStatus() & STATUS_WIP);
}

void ProtectBlock(bool bp0, bool bp1, bool bp2, bool bp3)
{
	uint8_t status = GetStatus();
	if(bp0) status |= STATUS_BP0;
	if(bp1) status |= STATUS_BP1;
	if(bp2) status |= STATUS_BP2;
	if(bp3) status |= STATUS_BP3;
	SetStatus(status);

}