#include "stm32f4xx.h"                  // Device header


void I2C_Config (void){
	
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	
	GPIOB->MODER |= 0xA0000;
	GPIOB->OTYPER |=0x300;
	GPIOB->OSPEEDR |=0xF0000;
	GPIOB->PUPDR |= 0x50000;
	GPIOB->AFR[1] |=0x44;
	
	// Reset the I2C 
	I2C1->CR1 |= 0x8000;
	I2C1->CR1 &= ~0x8000;
	
	I2C1->CR2 = 45;  //45MHz peripheral clock frequency
	
	I2C1->CCR = 225;
	
	I2C1->TRISE = 46;
	
	I2C1->CR1 |= 0x1;

}


void I2C_Start (void){
	
	I2C1->CR1 |= 0x400;  // Enable the ACK
	I2C1->CR1 |= 0x100;  // Generate START
	while (!(I2C1->SR1 & 0x1));  // Wait fror SB bit to set

}

void I2C_Write (uint8_t data)
{

	while (!(I2C1->SR1 & 0x80));  // wait for TXE bit to set
	I2C1->DR = data;
	while (!(I2C1->SR1 & 0x4));  // wait for BTF bit to set
}

void I2C_Address (uint8_t Address)
{

	I2C1->DR = Address;  //  send the address
	while (!(I2C1->SR1 & 0x2));  // wait for ADDR bit to set
	uint8_t temp = I2C1->SR1 | I2C1->SR2;  // read SR1 and SR2 to clear the ADDR bit
}


void I2C_Stop (void){
	I2C1->CR1 |= 0x200;  
	}



void I2C_WriteMulti (uint8_t *data, uint8_t size)
{

	while (!(I2C1->SR1 & 0x80));  // wait for TXE bit to set 
	while (size)
	{
		while (!(I2C1->SR1 & 0x80));  // wait for TXE bit to set 
		I2C1->DR = (uint32_t )*data++;  // send data
		size--;
	}
	
	while (!(I2C1->SR1 & 0x4));  // wait for BTF to set
}



void I2C_Read (uint8_t Address, uint8_t *buffer, uint8_t size)
{

	int remaining = size;
	
/**** STEP 1 ****/	
	if (size == 1)
	{
		/**** STEP 1-a ****/	
		I2C1->DR = Address;  //  send the address
		while (!(I2C1->SR1 & (1<<1)));  // wait for ADDR bit to set
		
		/**** STEP 1-b ****/	
		I2C1->CR1 &= ~(1<<10);  // clear the ACK bit 
		uint8_t temp = I2C1->SR1 | I2C1->SR2;  // read SR1 and SR2 to clear the ADDR bit.... EV6 condition
		I2C1->CR1 |= (1<<9);  // Stop I2C

		/**** STEP 1-c ****/	
		while (!(I2C1->SR1 & (1<<6)));  // wait for RxNE to set
		
		/**** STEP 1-d ****/	
		buffer[size-remaining] = I2C1->DR;  // Read the data from the DATA REGISTER
		
	}

/**** STEP 2 ****/		
	else 
	{
		/**** STEP 2-a ****/
		I2C1->DR = Address;  //  send the address
		while (!(I2C1->SR1 & (1<<1)));  // wait for ADDR bit to set
		
		/**** STEP 2-b ****/
		uint8_t temp = I2C1->SR1 | I2C1->SR2;  // read SR1 and SR2 to clear the ADDR bit
		
		while (remaining>2)
		{
			/**** STEP 2-c ****/
			while (!(I2C1->SR1 & (1<<6)));  // wait for RxNE to set
			
			/**** STEP 2-d ****/
			buffer[size-remaining] = I2C1->DR;  // copy the data into the buffer			
			
			/**** STEP 2-e ****/
			I2C1->CR1 |= 1<<10;  // Set the ACK bit to Acknowledge the data received
			
			remaining--;
		}
		
		// Read the SECOND LAST BYTE
		while (!(I2C1->SR1 & (1<<6)));  // wait for RxNE to set
		buffer[size-remaining] = I2C1->DR;
		
		/**** STEP 2-f ****/
		I2C1->CR1 &= ~(1<<10);  // clear the ACK bit 
		
		/**** STEP 2-g ****/
		I2C1->CR1 |= (1<<9);  // Stop I2C
		
		remaining--;
		
		// Read the Last BYTE
		while (!(I2C1->SR1 & (1<<6)));  // wait for RxNE to set
		buffer[size-remaining] = I2C1->DR;  // copy the data into the buffer
	}	
	
}
