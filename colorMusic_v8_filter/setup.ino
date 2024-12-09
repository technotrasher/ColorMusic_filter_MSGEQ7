/* изм. 26.02.2023 */

void setup() {
  Serial.begin(115200);  // включить для теста и вывода информации. отключать, для экономии памяти.

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(CORRECTION);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  IRLremote.begin(IR_PIN);
  //pinMode(BTN_OnOff, INPUT_PULLUP); // кнопка на минус
  bitClear(DDRB, 5);
  bitSet(PORTB, 5);

  // установка портов ручного управление АРУ в стандартное положение
  // pinMode(REF_FORCE_LOW, OUTPUT);
  sFlag(FORCE_LOW_SETUP, BIT(REF_FORCE_LOW));
  // pinMode(REF_FORCE_HIGH, OUTPUT);
  sFlag(FORCE_HIGH_SETUP, BIT(REF_FORCE_HIGH));
  // digitalWrite(REF_FORCE_LOW, HIGH);
  sFlag(FORCE_LOW_PORT, BIT(REF_FORCE_LOW));
  // digitalWrite(REF_FORCE_HIGH, LOW);
  cFlag(FORCE_HIGH_PORT, BIT(REF_FORCE_HIGH));


  //pinMode(IR_PIN, INPUT);
  DDRD &= ~(1 << PD2);
  //pinMode(LED_line, OUTPUT); pinMode(LED_IR, OUTPUT); pinMode(LED_ON, OUTPUT); pinMode(LED_OK, OUTPUT);
  DDRB |= (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4);

  /* включение начального канала */
  channels[a_channel]->onChannel();
  // задержка для установки правильных напряжений
  _delay_ms(20);

  /* устанавливаем таймер периодичности измерений 16MHz / 256 (prescaler) / 1000 (OCR1A) = 62.5 Гц (16 мс) */
  cli();
  /* не используем ШИМ (COM1A1, COM1A0, COM1B1, COM1B0 */
  TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0) | (1 << WGM11) | (1 << WGM10);
  /* не используем захват входного сигнала (выкл. фильтр шума ICNC1 и фронт/спад ICES1), Fast PWM+OCR1A (WGM10-WGM13), prescaler 256 (CS10 - CS12)*/
  TCCR1B = (0 << ICNC1) | (0 << ICES1) | (1 << WGM13) | (1 << WGM12) | (1 << CS12) | (0 << CS11) | (0 << CS10);
  /*  устанавливаем OCR1A */
  OCR1A = 1000;
  sei();

  cFlag(PRR, BIT(PRADC));  // убеждаемся, что на АЦП подано питание
  ADCSRA = 0;
  sFlag(ADCSRA, BIT(ADEN));  // ADCSRA.ADEN = включаем АЦП

  // ADMUX.REFS[1:0] = опорное напряжение AREF
  // ADMUX.ADLAR = результат преобразования по правому краю (для 10-бит данных) (ADCL, затем ADCH)
  // ADMUX.MUX[3:0] = 0 - выбранный вход АЦП
  ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR);
  // запрещают использование входов ADC5 - ADC0 как цифровых
  sFlag(DIDR0, BIT(ADC5D) | BIT(ADC4D) | BIT(ADC3D) | BIT(ADC2D) | BIT(ADC1D) | BIT(ADC0D));
  ADCSRB = (1 << ADTS2) | (1 << ADTS1) | (0 << ADTS0);  // триггер - Timer/Counter1 Overflow

  // ADCSRA.ADSC = не запускаем преобразование до срабатывания триггера
  // ADCSRA.ADATE = автоматический запуск преобразования
  // ADCSRA.ADIF = игнорируем признак состоявшегося прерывания
  // ADCSRA.ADIE = разрешаем прерывания
  // ADCSRA.ADPS[2:0] = устанавливаем делитель на 64
  // частота ADC clock будет 250кГц, один такт будет соответствовать 4мкс, время одного преобразования - 60мкс
  ADCSRA |= (0 << ADSC) | (1 << ADATE) | (0 << ADIF) | (1 << ADIE) | (1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0);

  //Помигаем светодиодами на передней панели...
  for (byte i = 0; i < 2; i++) {
    PORTB |= (1 << PB1);
    _delay_ms(100);
    PORTB &= ~(1 << PB1);
    _delay_ms(100);
    PORTB |= (1 << PB2);
    _delay_ms(100);
    PORTB &= ~(1 << PB2);
    _delay_ms(100);
    PORTB |= (1 << PB3);
    _delay_ms(100);
    PORTB &= ~(1 << PB3);
    _delay_ms(100);
    PORTB |= (1 << PB4);
    _delay_ms(100);
    PORTB &= ~(1 << PB4);
    _delay_ms(100);

    //digitalWrite(LED_ON, HIGH);   _delay_ms(100); digitalWrite(LED_ON, LOW);  _delay_ms(100);
    //digitalWrite(LED_line, HIGH); _delay_ms(100); digitalWrite(LED_line, LOW);  _delay_ms(100);
    //digitalWrite(LED_IR, HIGH);   _delay_ms(100); digitalWrite(LED_IR, LOW);  _delay_ms(100);
    //digitalWrite(LED_OK, HIGH);   _delay_ms(100); digitalWrite(LED_OK, LOW);  _delay_ms(100);
  }

  /*************** выключаем оранжевый, синий и зелёный белый светодиоды *******************/
  PORTB &= ~(1 << PB1);
  PORTB &= ~(1 << PB2);
  PORTB &= ~(1 << PB3);
  PORTB &= ~(1 << PB4);

  FastLED.setBrightness(0);  // погасить ленту
  FastLED.show();            // отправить значения на ленту
  FastLED.setBrightness(mainSettings.BRIGHTNESS);

  // в 0 ячейке хранится число 1. Если нет - "0" - значит это первый запуск системы
  if (EEPROM_ReadByte(0) != 1) {  // первый запуск
    Serial.print(F("First start  "));
    EEPROM_UpdateByte(0, 1);
    updateEEPROM();
  } else {
    readEEPROM();
  }
}