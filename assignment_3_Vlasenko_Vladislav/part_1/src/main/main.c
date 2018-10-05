#include "mcu_support_package/inc/stm32f10x.h"
#include "spl/inc/stm32f10x_tim.h"
#include "math.h"
#include <stdlib.h>

volatile int16_t step = 0; //Шажок ШИМа
volatile int16_t pc8 =0;

void TIM3_IRQHandler() {
  step = 1;
  TIM_ClearFlag(TIM3, TIM_FLAG_Update);
}

void allInit() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //Затактировали порт C
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);   //Тактирование TIM3
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //Тактирование внутреннего мультиплексора
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);     //Fullremap для TIM3 - TIM3CH3 соединен с PC8

  TIM_TimeBaseInitTypeDef timer3;

  TIM_TimeBaseStructInit(&timer3);  //Заполнить поля структуры timer3 параметрами по умолчанию

  timer3.TIM_Prescaler = 7199;
  timer3.TIM_CounterMode = TIM_CounterMode_Up;
  timer3.TIM_Period = 100;
	
  TIM_TimeBaseInit(TIM3, &timer3);  //Настройка TIM43на частоту 200Гц и направлением счета на возрастание

  TIM_OCInitTypeDef chTimer3;       //Структура для третьего канала TIM3

  TIM_OCStructInit(&chTimer3);

  chTimer3.TIM_OCMode = TIM_OCMode_PWM1;
  chTimer3.TIM_OutputState = TIM_OutputState_Enable;

  TIM_OC3Init(TIM3, &chTimer3);    //Настройка таймерного канала

  GPIO_InitTypeDef lamp;

  lamp.GPIO_Mode = GPIO_Mode_AF_PP;
  lamp.GPIO_Pin = GPIO_Pin_8;
  lamp.GPIO_Speed = GPIO_Speed_10MHz;

  GPIO_Init(GPIOC, &lamp);        //Пин 8 в порту C используются для динамика в режиме PP
}

int main(void) {
  allInit();
  __disable_irq();
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  NVIC_EnableIRQ(TIM3_IRQn);		 //Разрешение прерываний от TIM2
  __enable_irq();

  TIM_Cmd(TIM3, ENABLE);           //Запуск TIM2

  int16_t triangle = 0;
  int16_t signal = 0;

  while(1) {
		pc8=GPIOC->IDR&(1<<8);
    if(step) {
      if(triangle >= 100) {
        triangle = -100;
      }
      signal = abs(triangle);
      TIM_SetCompare3(TIM3, signal);
      triangle++;
      step = 0;
    }
  }
  return 0;
}

// классический ассерт для STM32
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t * file, uint32_t line) {
  /* User can add his own implementation to report the file name and line number,
   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  (void)file;
  (void)line;

  __disable_irq();
  while(1) {
    // это ассемблерная инструкция "отладчик, стой тут"
    // если вы попали сюда, значит вы ошиблись в параметрах. Смотрите в call stack
    __BKPT(0xAB);
  }
}
#endif
