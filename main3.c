#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <string.h>
int change = 0;

void tim2_init(void){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	TIM2->PSC = 47;
	TIM2->ARR = 1000* 1000;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_CEN;


}

void gpio_init(){
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC->MODER |= 1 << 16;

}

void adc_init(void){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= 3;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->CR2 |= RCC_CR2_HSI14ON;
	while(!(RCC->CR2 & RCC_CR2_HSI14RDY));
	ADC1->CR |= ADC_CR_ADEN;
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
	while((ADC1->CR & ADC_CR_ADSTART));

	//Timer
}

//PA1 ADC_IN1, TIM2_CH2
/*
void TIM2_IRQHandler(){

	//Uncheck the interupt flag
	TIM2->SR &= ~TIM_SR_UIF;

	//Initiate conversion
	ADC1->CHSELR |= 1; //Set to proper channel
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
	ADC1->CR |= ADC_CR_ADSTART;
	while(!(ADC1->ISR & ADC_ISR_EOC));
	if(ADC1->DR > 4){
		if(GPIOA->ODR > 0){
			change = 0;
		}else{
			change++;
		}
	}else{
		if(GPIOA->ODR > 0){
			change++;
		}else{
			change = 0;
		}
	}
	if(change == 8){
		GPIOA->ODR ^= 2;
	}
}
*/

void TIM2_IRQHandler(){
	TIM2->SR &= ~TIM_SR_UIF;
	GPIOC->ODR |= 1 << 8;
}

int main(int argc, char ** argv){
	//adc_init();
	tim2_init();
	return 0;
}
