#include "mcu_support_package/inc/stm32f10x.h"
#include "spl/inc/stm32f10x_tim.h"
#include "math.h"
#include <stdlib.h>

#define PI 3.1415927

void vsyakieInit() {
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //Затактировали порт B
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);   //Тактирование TIM2
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //Тактирование внутреннего мультиплексора
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);     //Fullremap для TIM2 - TIM2CH3 соединен с B10

  TIM_TimeBaseInitTypeDef timer2;

  TIM_TimeBaseStructInit(&timer2);  //Заполнить поля структуры timer2 параметрами по умолчанию

  timer2.TIM_Prescaler = 0;
  timer2.TIM_CounterMode = TIM_CounterMode_Up;
  timer2.TIM_Period = 500;

  TIM_TimeBaseInit(TIM2, &timer2);  //Настройка TIM2 на частоту 144КГц и направлением счета на возрастание

  TIM_OCInitTypeDef chTimer3;       //Структура для третьего канала TIM2

  TIM_OCStructInit(&chTimer3);

  chTimer3.TIM_OCMode = TIM_OCMode_PWM1;
  chTimer3.TIM_OutputState = TIM_OutputState_Enable;

  TIM_OC3Init(TIM2, &chTimer3);    //Настройка таймерного канала

  GPIO_InitTypeDef dynamic;

  dynamic.GPIO_Mode = GPIO_Mode_AF_PP;
  dynamic.GPIO_Pin = GPIO_Pin_10;
  dynamic.GPIO_Speed = GPIO_Speed_10MHz;

  GPIO_Init(GPIOB, &dynamic);      //Пин 10 в порту B используются для динамика в режиме PP
}

int main(void) {
  vsyakieInit();

  //__disable_irq();
  //TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  //NVIC_EnableIRQ(TIM2_IRQn);		 //Разрешение прерываний от TIM2
  //__enable_irq();

  TIM_Cmd(TIM2, ENABLE);           //Запуск TIM2

  static int16_t sinMassive[360];
  for (int i = 0; i < 360; i++)
    sinMassive[i] = (500*sin((double)i*PI/180)+500)/2;

  int32_t degr1 = 0;
  int32_t degr2 = 0;
  int32_t degr3 = 0;
  int32_t sine1;
  int32_t sine2;
  int32_t sine3;
  int32_t signal;
  while(1) {
    sine1 = sinMassive[degr1/33];    //Числа 27, 30, 33 пока что абстрактные
    sine2 = sinMassive[degr2/30];    //Но они нужны, чтобы Sine i-тое принимали в один момент времени разные значения
    sine3 = sinMassive[degr3/27];
    signal = (sine1+sine2+sine3)/3;
    TIM_SetCompare3(TIM2, signal);   //Задание величины ШИМА
    degr1++;
    degr2++;
    degr3++;
    if(degr1 == 33*360)
      degr1 = 0;
    if(degr2 == 30*360)
      degr2 = 0;
    if(degr3 == 27*360)
      degr3 = 0;
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
