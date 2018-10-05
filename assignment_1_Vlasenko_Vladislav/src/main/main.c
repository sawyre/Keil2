#define FlashingLED							//- Мигание светодиодом каждые две секунды
//#define RunningFIRE     			  //- Бегущий огонек
//#define BouncePROTECTION				//- Защита от дребезга контактов
#include "mcu_support_package/inc/stm32f10x.h"
#include <stdlib.h>

volatile uint32_t ucount=0;
#ifdef RunningFIRE
volatile int32_t count=0;
#endif

void SysTick_Handler(void) {
	ucount++;
}

#ifdef FlashingLED
void EveryTwoSecond() {
	RCC->APB2ENR|=1<<4;  		//Подать питание на порт C
	GPIOC->CRH&=0xFFFFFFF0; //Настроить режим контакта 8 в порту C
	GPIOC->CRH|=0x2;		 		//на режим General purpose output push pull
	__disable_irq();
	SysTick_Config(14400000);			//Таймер для прерывания на 0.2 с
	__enable_irq();
	while(1) {
		if(ucount > 9) {       //Каждые 2 секунды
			GPIOC->ODR^= 1<<8;
			ucount = 0;
		}
	}
}
#endif

#ifdef RunningFIRE
void RunningFire() {
	count = -9;
	RCC->APB2ENR|=1<<2;  					//Подать питание на порт A
	GPIOA->CRL&=0x0000000F; 			//Настроить режим контакта 1-7 в порту A
	GPIOA->CRH&=0xFFFFF000; 			//Настроить режим контакта 8-10 в порту A
	GPIOA->CRL|=0x22222220;		 		//на режим General purpose output push pull
	GPIOA->CRH|=0x00000222;		 		//на режим General purpose output push pull
	while(1) {
		__disable_irq();
		SysTick_Config(9000000);		//Таймер для прерывания на 0.125 с
		__enable_irq();
		while(ucount<2);						//Ожидать 0.25 секунд
		GPIOA->ODR&=0xFFFFF801;
		GPIOA->ODR|=1<<(abs(count)+1);
		//count = (-9)*(1^(((count-8)>>31)&1))+(count+1)*(((count-8)>>31)&1);  //count = -9 if count >= 8, count = count+1 if count < 8
		count=(count==8)?(-9):(count+1);
		ucount = 0;
	}
}
#endif	

#ifdef BouncePROTECTION
void BounceProtection() {
	RCC->APB2ENR|=1<<4 | 1<<2;  		//Подать питание на порт C
	GPIOC->CRH&=0xFFFFFFF0;				  //Настроить режим контакта PC8
	GPIOC->CRH|=0x2;		 						//на режим General purpose output push pull
	GPIOA->CRL&=0xFFFFFFF0; 				//Настроить режим контакта PA0
	GPIOA->CRL|=0x4;		 						//на режим floating input
	while(1) {
		ucount=0;
		__disable_irq();
		SysTick_Config(1440000);		  	//Таймер для прерывания на 0.02 с
		__enable_irq();
		while(GPIOA->IDR&1) {
			if(ucount > 99)							//По прошествию примерно 2 секунд после нажатия
				GPIOC->ODR|= 1<<8;
		}
		ucount=0;
		__disable_irq();
		SysTick_Config(1440000);		  //Таймер для прерывания на 0.02 с
		__enable_irq();
		while((GPIOA->IDR&1) == 0) {
			if(ucount > 149)						//По прошествию примерно 3 секунд после отжатия
				GPIOC->ODR&= ~(1<<8);
		}
	}
}
#endif


int main(void) {
#ifdef FlashingLED
	EveryTwoSecond();			//1-ое задание
#endif
#ifdef RunningFIRE
	RunningFire();				//2-ое задание
#endif
#ifdef BouncePROTECTION
	BounceProtection();		//3-ье задание
#endif
	return 0;
}
