# Срок сдачи: 11 марта 2018, 01:00
# Задание 1 - Таймер SysTick

При определении временных интервалов в симуляторе ориентируйтесь на поле "Sec." в симуляторе; помните, симулированное время идет медленнее, чем реальное.

![Поле sec](.img/sec.png)  

Не забудьте поставить галочку View->Periodic window update (в режиме отладки), чтобы во время отладки все окна обновлялись автоматически.

Не используйте пустые циклы для задания или измерения временных интервалов!

Задание состоит из трех частей, для каждой части, пожалуйста, напишите отдельную функцию, которая вызывается из main или используйте условную компиляцию (#ifdef'ы). Пожалуйста, прокомментируйте, какой кусок кода соответствует каждой части задания.

За использование прерываний от SysTick'a во всех трех частях к оценке добавляется полбалла. Если ваша программа (все три части) продолжает корректно работать при включении оптимизации -О3, к оценке добавляется еще полбалла.

Не забывайте, что код с неправильными отступами не принимается!

Подробнее о том, как сдавать домашние задания, смотрите: https://github.com/spbstu-microprocessors/lectures#assignments

## Часть 1 - Мигание

К выводу PC.8 подключен светодиод, который загорается при подаче логической 1. Необходимо с помощью таймера SysTick мигать этим светодиодом раз в две секунды (т.е. две секунды горит - две секунды не горит).

## Часть 2 - Бегущий огонек

К выводам порта А с 1-го по 10-й подключено 10 светодиодов. Напишите программу, которая будет поджигать зажигать первый светодиод, гасить его, зажигать второй, гасить его и так далее. После 10-го светодиода должен загорется девятый, потом восьмой и т. д.
Т.е. "огонек" должен "бежать" по светодиодам туда-сюда. Каждый светодиод должен гореть 0,25 секунды.

## Часть 3 - Защита от дребезга контактов

### Нажатие на кнопку в идеальном мире
![Идеальная картинка](.img/ideal.png)  

### Нажатие на кнопку в жестокой реальности
![Реальная картинка](.img/bounce.png)  

Причина дребезга в том, что нажатие на кнопку - это упругое взаимодействие твердых тел; контакты отскакивают друг от друга, а микроконтроллер работает достаточно быстро, чтобы зафиксировать несколько перепадов напряжения.

К сожалению, нельзя избавиться от дребезга, просто добавив конденсатор или резистор. Дребезг необходимо отфильтровывать программно.

К выводу РА0 подключена кнопка. К выводу РС8 подключен светодиод. Необходимо, чтобы светодиод загорался, только если кнопка нажата две секунды подряд (или более). Гаснуть светодиод должен только если кнопка не нажата три секунды подряд (или более).  
Кнопка считается нажатой при наличии высокого уровня на контакте и не нажатой при наличии низкого уровня.  
В симуляторе нажатие на кнопку соответствует установке галочки в поле Pins соответствующего окна GPIO.