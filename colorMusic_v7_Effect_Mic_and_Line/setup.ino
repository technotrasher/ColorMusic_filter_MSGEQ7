void setup() {
  //Serial.begin(115200); // включить для теста и вывода информации. отключать, для экономии памяти.

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(CORRECTION);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  IRLremote.begin(IR_PIN);
  //pinMode(BTN_OnOff, INPUT_PULLUP); // кнопка на минус
  bitClear (DDRB, 5);
  bitSet   (PORTB, 5);

  //pinMode(LED_line, OUTPUT); pinMode(LED_IR, OUTPUT); pinMode(LED_ON, OUTPUT); pinMode(LED_OK, OUTPUT);
  DDRB |= ( 1 << PB0) | ( 1 << PB1) | ( 1 << PB2) | ( 1 << PB3);
  //pinMode(RESET_R, OUTPUT);
  //pinMode(STROBE_R, OUTPUT);
  //pinMode(RESET_L, OUTPUT);
  //pinMode(STROBE_L, OUTPUT);
  DDRD |= ( 1 << PD3) | ( 1 << PD4) | ( 1 << PD5) | ( 1 << PD6) | ( 1 << PD7);
  //pinMode (SOUND_R_LINE, INPUT);
  //pinMode (SOUND_L_LINE, INPUT);
  DDRC &= ~(( 1 << PC2) | ( 1 << PC3));

  //Помигаем светодиодами на передней панели...
  for (byte i = 0; i < 2; i++) {
    PORTB |= ( 1 << PB2); _delay_ms(100); PORTB &= ~ (1 << PB2); _delay_ms(100);
    PORTB |= ( 1 << PB0); _delay_ms(100); PORTB &= ~ (1 << PB0); _delay_ms(100);
    PORTB |= ( 1 << PB1); _delay_ms(100); PORTB &= ~ (1 << PB1); _delay_ms(100);
    PORTB |= ( 1 << PB3); _delay_ms(100); PORTB &= ~ (1 << PB3); _delay_ms(100);

    //digitalWrite(LED_ON, HIGH);   _delay_ms(100); digitalWrite(LED_ON, LOW);  _delay_ms(100);
    //digitalWrite(LED_line, HIGH); _delay_ms(100); digitalWrite(LED_line, LOW);  _delay_ms(100);
    //digitalWrite(LED_IR, HIGH);   _delay_ms(100); digitalWrite(LED_IR, LOW);  _delay_ms(100);
    //digitalWrite(LED_OK, HIGH);   _delay_ms(100); digitalWrite(LED_OK, LOW);  _delay_ms(100);
  }

  FastLED.setBrightness(0); // погасить ленту
  FastLED.show();           // отправить значения на ленту
  FastLED.setBrightness(mainSettings.BRIGHTNESS);

  // в 1 ячейке хранится число 1. Если нет - "0" - значит это первый запуск системы
  uint8_t st;
  EEPROM_ReadBlock(0, &st, sizeof(&st));
  //Serial.print(F("First start")); Serial.println(st);
  if (st != 1) {
    st = 1;
    EEPROM_UpdateBlock(0, &st, sizeof(st));
    updateEEPROM();
  } else {
    readEEPROM();
  }

  //-------------------запуск АЦП----------------------------// вместо analogRead
  // убеждаемся, что на АЦП подано питание
  PRR &= ~(1 << PRADC);
  // Включаем АЦП, режим однократного преобразования, прерывание АЦП разрешено, устанавливаем предделитель преобразователя на 32
  ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0);
  // переключение между входами А2 и А3 будем делать инвертированием байта. старт с А2
  // Выставляем выставляем опорное напряжение Vcc, результат преобразования по левому краю, пин ADC2 (A2)
  ADMUX |= (0 << REFS1) | (1 << REFS0) | (1 << ADLAR) | (0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (0 << MUX0);
  // Выставляем выставляем опорное напряжение Vcc, результат преобразования по левому краю, пин ADC3 (A3)
  //ADMUX = (0 << REFS1) | (1 << REFS0) | (1 << ADLAR) | (0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (1 << MUX0);
  DIDR0 |= (1 << ADC2D) | (1 << ADC3D); //запрещают использование входов ADC2 - ADC3 как цифровых
}
