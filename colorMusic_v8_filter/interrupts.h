#ifndef __interrupts_h__
#define __interrupts_h__

/* Версия 1.0 от 24.02.2023 */
/* прерывание окончания преобразования АЦП */
ISR(ADC_vect) {
  static byte muxch = 0;  // начинаекм с полосы 0

  switch (muxch) {
    case 0:                       // первое преобразование (A0)
      cFlag(ADCSRA, BIT(ADATE));  // выключаем автоматический триггер
      if (channels[a_channel]->isReading()) {
        // буфер защищён от изменения
        sFlag(ADCSRA, BIT(ADSC));  // начинаем новое измерение
        return;                    // ждём следующего срабатывания триггера
      }
      *buffer = ADC;
      ++buffer;                          // следующий элемент буфера                               ^buffer
      ++muxch;                           // переходим на следующую полосу                          ^band
      sFlag(ADMUX, 1);                   // записываем полосу в регистр
      channels[a_channel]->setActive();  // отмечаем, что буфер находится в состоянии изменения
      sFlag(ADCSRA, BIT(ADSC));          // начинаем новое измерение
      break;
    case 1:  // (A1)
    case 2:  // (A2)
    case 3:  // (A3)
    case 4:  // (A4)
    case 5:  // (A5)
    case 6:  // (A6)
      *buffer = ADC;
      ++buffer;           // следующий элемент буфера                               ^buffer
      ++muxch;            // переходим на следующую полосу                          ^band
      cFlag(ADMUX, 0xF);  // записываем полосу в регистр
      sFlag(ADMUX, muxch);
      sFlag(ADCSRA, BIT(ADSC));  // начинаем новое измерение
      break;
    case 7:  // (A7)
      *buffer = ADC;
      muxch = 0;
      cFlag(ADMUX, 0xF);                   // записываем полосу в регистр
      if (a_channel == 2) {                // если обрабатываем микрофон
        buffer = channels[2]->getBands();  // возвращаем указатель на начало буфера
        if (buffer[7] > 163) buffer[7] = 163;
        else if (buffer[7] < 32) buffer[7] = 32;
        if (buffer[0] < buffer[7]) buffer[0] = buffer[7];  // все измерения нужно вогнать в диапазон
        if (buffer[1] < buffer[7]) buffer[1] = buffer[7];  // эффективнее это сделать без for, так как вычисления будут осуществляться при компиляции, а не в run-time
        if (buffer[2] < buffer[7]) buffer[2] = buffer[7];  // программной памяти расходуется всего на несколько байт больше
        if (buffer[3] < buffer[7]) buffer[3] = buffer[7];
        if (buffer[4] < buffer[7]) buffer[4] = buffer[7];
        if (buffer[5] < buffer[7]) buffer[5] = buffer[7];
        if (buffer[6] < buffer[7]) buffer[6] = buffer[7];
        channels[2]->setReq();  // переводим канал в запрос декодирования
      } else {
        channels[a_channel]->offChannel();  // выключаем канал
        DIDR0 = 0;                          // включаем цифровую часть для портов A0 - A5
                                            // переводим порты A0 - A5 в состояние Output (pinMode(A0-A5, OUTPUT))
        sFlag(DDRC, BIT(DDC5) | BIT(DDC4) | BIT(DDC3) | BIT(DDC2) | BIT(DDC1) | BIT(DDC0));
        // digitalWrite(A0-A5, LOW)
        cFlag(PORTC, BIT(PORTC5) | BIT(PORTC4) | BIT(PORTC3) | BIT(PORTC2) | BIT(PORTC1) | BIT(PORTC0));
        buffer = channels[a_channel]->getBands();
        // нормализация данных
        if (buffer[7] > 163) buffer[7] = 163;
        else if (buffer[7] < 32) buffer[7] = 32;
        if (buffer[0] < buffer[7]) buffer[0] = buffer[7];  // все измерения нужно вогнать в диапазон
        if (buffer[1] < buffer[7]) buffer[1] = buffer[7];  // эффективнее это сделать без for, так как вычисления будут осуществляться при компиляции, а не в run-time
        if (buffer[2] < buffer[7]) buffer[2] = buffer[7];  // программной памяти расходуется всего на несколько байт больше
        if (buffer[3] < buffer[7]) buffer[3] = buffer[7];
        if (buffer[4] < buffer[7]) buffer[4] = buffer[7];
        if (buffer[5] < buffer[7]) buffer[5] = buffer[7];
        if (buffer[6] < buffer[7]) buffer[6] = buffer[7];
        channels[a_channel]->setReq();             // переводим канал в запрос декодирования
        a_channel ^= 1;                            // меняем канал
        buffer = channels[a_channel]->getBands();  //->buffer init
                                                   // переводим порт A0-A5 в состояние Input (pinMode(A0-A5, INPUT))
        cFlag(DDRC, BIT(DDC5) | BIT(DDC4) | BIT(DDC3) | BIT(DDC2) | BIT(DDC1) | BIT(DDC0));
        // запрещает использование входов ADC5 - ADC0 как цифровых
        sFlag(DIDR0, BIT(ADC5D) | BIT(ADC4D) | BIT(ADC3D) | BIT(ADC2D) | BIT(ADC1D) | BIT(ADC0D));
      }
      channels[a_channel]->onChannel();  // включаем канал
      sFlag(ADCSRA, BIT(ADATE));         // включаем автоматический триггер
      break;
  }
  sFlag(TIFR1, BIT(TOV1));  // cбрасываем признак переполнения
}

#endif