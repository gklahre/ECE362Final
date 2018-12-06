#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <string.h>
int change = 0;
int turns = 0;
int speedturns = 0;
int currentState = 0;
int wheelSize = 222;
char line[21];
char line2[21];
int speedArray[5] = {0, 0, 0, 0, 0};
int arrLoc = 0;

static void cmd(char b);
static void init_lcd();
static void display1(const char * string);
static void display2(const char * string);
static void nano_wait(int t);


static void nano_wait(int t) {
    asm("       mov r0,%0\n"
        "repeat:\n"
        "       sub r0,#83\n"
        "       bgt repeat\n"
        : : "r"(t) : "r0", "cc");
}

void tim2_init(void){
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	TIM2->PSC = 47;
	TIM2->ARR = 1000-1;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_CEN;
	NVIC->ISER[0] |= 1 << TIM2_IRQn;


}
/*
void tim3_init(void){
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	TIM3->PSC = 48-1;
	TIM3->ARR = (1000 * 1000) - 1;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 |= TIM_CR1_CEN;
	NVIC->ISER[0] |= 1 << TIM3_IRQn;


}
*/
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

void TIM2_IRQHandler(){

	//Uncheck the interupt flag
	TIM2->SR &= ~TIM_SR_UIF;

	//Initiate conversion
	ADC1->CHSELR |= 2; //Set to proper channel
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
	ADC1->CR |= ADC_CR_ADSTART;
	while(!(ADC1->ISR & ADC_ISR_EOC));
	float f = (ADC1->DR * 3/ 4095.0);
	if(f > .030){
		if(currentState == 1){
			change = 0;
		}else{
			change++;
		}
	}else{
		if(currentState == 1){
			change++;
		}else{
			change = 0;
		}
	}
	if(change == 6){
		if(currentState == 0){
			currentState = 1;
			turns++;
			speedturns++;
		}
		if(currentState == 1){
			currentState = 0;
		}

	}
	//sprintf(line, "Turns : %2.2f, %d",f,turns);
	sprintf(line2,"Kilometers: %2.3f",.00222 * turns);
	//display2(line);
	display1(line2);
	sprintf(line,"Turns: %d",turns);
	display2(line);
}
/*
void TIM3_IRQHandler(){
	TIM3->SR &= ~TIM_SR_UIF;

	//display2("Here?");
	int i;
	int totalTurns = 0;
	speedArray[arrLoc] = speedturns;

	if(arrLoc == 4){
		arrLoc = 0;
	}else{
		arrLoc++;
	}
	for(i=0;i < 5; i++){
		totalTurns += speedArray[i];
	}
	sprintf(line,"Speed: %2.1f kmh",(720 * .00222 * totalTurns));
	display2(line);
	speedturns = 0;

}
*/
/*
void TIM2_IRQHandler(){
	TIM2->SR &= ~TIM_SR_UIF;
	GPIOC->ODR ^= (1 << 8);

	turns++;
	sprintf(line, "Turns : %d",turns);
	display2(line);

}
*/
int main(int argc, char ** argv){
	adc_init();
	init_lcd();
	//tim3_init();
	tim2_init();
	gpio_init();
	//display1("Heyo");
	while(1){};
	return 0;
}

static void init_lcd(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~((3<<(2*12)) | (3<<(2*13)) | (3<<(2*15)) );
    GPIOB->MODER |=   (2<<(2*12)) | (2<<(2*13)) | (2<<(2*15));
    GPIOB->AFR[1] &= ~( (0xf<<(4*(12-8))) | (0xf<<(4*(13-8))) | (0xf<<(4*(15-8))) );

    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;

    SPI2->CR1 &= ~SPI_CR1_BR;
    SPI2->CR1 |= SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE | SPI_CR1_MSTR;
    SPI2->CR1 |= SPI_CR1_BR;
    SPI2->CR2 = SPI_CR2_SSOE | SPI_CR2_NSSP | SPI_CR2_DS_3 | SPI_CR2_DS_0;
    SPI2->CR1 |= SPI_CR1_SPE;

    nano_wait(100000000); // Give it 100ms to initialize
    cmd(0x38);  // 0011 NF00 N=1, F=0: two lines
    cmd(0x0c);  // 0000 1DCB: display on, no cursor, no blink
    cmd(0x01);  // clear entire display
    nano_wait(6200000); // clear takes 6.2ms to complete
    cmd(0x02);  // put the cursor in the home position
    cmd(0x06);  // 0000 01IS: set display to increment
}

static void cmd(char b) {
    while((SPI2->SR & SPI_SR_TXE) != SPI_SR_TXE);
    SPI2->DR = b;
}
static void data(char b) {
    while((SPI2->SR & SPI_SR_TXE) != SPI_SR_TXE);
    SPI2->DR = 0x200 | b;
}
static void display1(const char *s) {
    cmd(0x02); // put the cursor on the beginning of the first line.
    int x;
    for(x=0; x<16; x+=1)
        if (s[x])
            data(s[x]);
        else
            break;
    for(   ; x<16; x+=1)
        data(' ');
}
static void display2(const char *s) {
    cmd(0xc0); // put the cursor on the beginning of the second line.
    int x;
    for(x=0; x<16; x+=1)
        if (s[x] != '\0')
            data(s[x]);
        else
            break;
    for(   ; x<16; x+=1)
        data(' ');
}

