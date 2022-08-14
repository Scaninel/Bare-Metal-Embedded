#include "stm32f4xx.h"                  // Device header



void TIM6_Config (void){

	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	
	
	TIM6->PSC = 90-1;  							// 90MHz/90 = 1 MHz ~~ 1 uS delay  !!!this value must be changed according to the main clock!!!
	TIM6->ARR = 0xffff; 
	
	TIM6->CR1 |= (1<<0); 						// Enable the Counter
	while (!(TIM6->SR & (1<<0))); 	//This bit is set by hardware when the registers are updated

}

void Delay_us (uint16_t us)
{
	TIM6->CNT = 0;
	while (TIM6->CNT < us);
}

void Delay_ms (uint16_t ms)
{
	for (uint16_t i=0; i<ms; i++)
	{
		Delay_us (1000); // delay of 1 ms
	}
}
