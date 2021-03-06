#include "mcu_support_package/inc/stm32f10x.h"
#include "spl/inc/stm32f10x_tim.h"
#include "math.h"
#include <stdlib.h>

#define PI 3.1415927

#define freqC6 1046
#define freqE6 1318
#define freqG6 1568

/*
Создан массив на 360 элементов, содержащий значения от 0 до 500
каждый элемент расчитывается как (sin(i)+1)/2*500
500 - величина периода для TIM2
каждый синус приравнивается к элементу под номером (freq*360*time/144000)%360
где time/144000 - истинное время, freq*360(v*2PI, но так как работаем в
градусах, то вместо 2PI - 360 град)
*/

int16_t sine1;
int16_t sine2;
int16_t sine3;

volatile uint32_t time;
volatile uint16_t pb10=0;

void TIM2_IRQHandler() {
  time++;
  if(time > 144000) {  //144000 Гц частота ШИМА на TIM2
    time = 0; //по проществию секунды time обнуляется
  }
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
}

void allInit() {
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
  allInit();
  __disable_irq();
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  NVIC_EnableIRQ(TIM2_IRQn);		   //Разрешение прерываний от TIM2
  __enable_irq();

  TIM_Cmd(TIM2, ENABLE);           //Запуск TIM2

  static int16_t sinMassive[360];
  for (int i = 0; i < 360; i++) {
    sinMassive[i] = (500*sin((double)i*PI/180)+500)/2;
  }
	int16_t last_time;
  int16_t signal;
  while(1) {
		if(time != last_time){
			sine1 = sinMassive[(int)(((freqC6*time)/400)%360)];    //400 = 144000/360
			sine2 = sinMassive[(int)(((freqE6*time)/400)%360)];    //time/144000 - истинное время
			sine3 = sinMassive[(int)(((freqG6*time)/400)%360)];    //freq*360 - это (f*2PI)
			signal = (sine1+sine2+sine3)/3;
			TIM_SetCompare3(TIM2, signal);   //Задание величины ШИМА
			last_time = time;
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
