void buttonTickLed () {
  if (IRdata != BUTT_OK) fullLowPassSet = 0;
  //digitalWrite(LED_IR, HIGH);     // включить светодиод
  PORTB |= ( 1 << PB1);
  _delay_ms(50);
  //digitalWrite(LED_IR, LOW);      // выключить светодиод
  PORTB &= ~ ( 1 << PB1);
}

void buttonTick() {
  //Опрос кнопки OnOff на передней панели
  if (!digitalRead(BTN_OnOff) == 1) {
    _delay_ms(100);
    Proc_OnOff();
  }
}

void Proc_OnOff() {     //Включить/выключить
  ONstate = !ONstate; one_color_all(); FastLED.show();
  if (ONstate == true) {  //ВКЛЮЧИТЬ    
    for (byte i = 0; i <= 254; i++) {       //плавное включение и выключение диодов
      _delay_ms(1);
      analogWrite(LED_ON, i);
    }
  }
  else {                  //ВЫКЛЮЧИТЬ
    updateEEPROM(); // если выключаем то обновляем данные в памяти! при включении только читаем!
    for (byte i = 0; i <= 254; i++) {      //плавное включение и выключение диодов
      _delay_ms(1);
      analogWrite(LED_ON, 254 - i);
    }
    //digitalWrite(RELAYon, LOW);
    PORTD &= ~ ( 1 << PD4);
    //digitalWrite(LED_line, LOW);
    PORTB &= ~ ( 1 << PB0);
    settings_mode = false; // выключаем за собой свет газ и воду.
  }
}

void(* resetFunc) (void) = 0; //перезагрузка /0

void Proc_OK() {              // отработка нажатия 4 раза на кнопку OK
  digitalWrite(LED_OK, !settings_mode);
  settings_mode = !settings_mode;
  fullLowPassSet = fullLowPassSet + 1;
  //Serial.print(F("fullLowPassSet = ")); Serial.println(fullLowPassSet);
  if (fullLowPassSet == 4) {
    fullLowPassSet = 0;
    //digitalWrite(LED_line, HIGH);   // включить светодиод
    PORTB |= ( 1 << PB0);
    autoLowPass();            // измерить шумы
    //Serial.print(F("VU_LOW_PASS_line_R = ")); Serial.println(mainSettings.VU_LOW_PASS_line_R);
    //Serial.print(F("VU_LOW_PASS_line_L = ")); Serial.println(mainSettings.VU_LOW_PASS_line_L);
    //digitalWrite(LED_line, LOW);    // выключить светодиод
    PORTB &= ~(1 << PB0);
  }
}
