#include "mcu_support_package/inc/stm32f10x.h"

#define twinkling "<-this is working mode"

volatile int handler_counter=0;

void SysTick_Handler(void);

int main(void)
{
  #ifdef twinkling
  __disable_irq();
  SysTick_Config(16000000u);
  __enable_irq();
  int i;
  RCC->APB2ENR|=1<<4;//power on port C
  GPIOC->CRH&=~(0xFF);//setting last 8 bits to zero
  GPIOC->CRH|=0x11;//8th and 9th pins sets to output push pull at 10MHz
  #elif wave
    
  #else
    
  #endif
  while(1)
  {
    GPIOC->ODR|=0x1<<8;
    for(i=0;i<1000000;i++){}
    GPIOC->ODR&=~(1<<8);  
    for(i=0;i<1000000;i++){}
  }
  return 0;
}

void Systick_Handler()
{
  handler_counter++;
  if(handler_counter==5)
  {
    #ifdef twinkling
      GPIOC->ODR^=1<<8;//inversion of the 8th bit
    #elif wave
    
    #else
    
    #endif
    handler_counter=0;
  }
}