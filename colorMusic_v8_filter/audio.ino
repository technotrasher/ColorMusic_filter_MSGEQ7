/* изм. 27.02.2023 */
// целочисленное возведение в степень 1,4 любого числа от 0 до 1024
#define rpowmul(myval) ((myval) > 154 ? ((myval) > 448 ? ((myval) > 669 ? ((myval) > 800 ? 15 : 14) : ((myval) > 552 ? 13 : 12)) : ((myval) > 278 ? ((myval) > 357 ? 11 : 10) : (myval > 210 ? 9 : 8))) : ((myval) > 23 ? ((myval) > 43 ? ((myval) > 107 ? 7 : 6) : ((myval) > 43 ? 5 : 4)) : ((myval) > 9 ? 3 : ((myval) > 3 ? 2 : 1))))
#define rpow(myval) (myval) * rpowmul(myval)


void audio() {
  static int RsoundLevel = 0, LsoundLevel = 0;
  // главный цикл обработки аудио сигнала
  if (ONstate) {
    // первые два режима - громкость (VU meter)
    if (mainSettings.this_mode == 0 || mainSettings.this_mode == 1) {

      bool __isChanged = false;
      if (channels[0]->isReady()) {
        uint16_t *_tpointer = (uint16_t *)channels[0]->getBands();
        int __level = *_tpointer + *(++_tpointer) + *(++_tpointer) + *(++_tpointer) + *(++_tpointer) + *(++_tpointer) + *(++_tpointer);
        // отсчёты [0-1023], сумма [0-7161]
        __level = rpow(__level >> 3);  // результат [0-895] -> [0-13425]
        LsoundLevel = __level * mainSettings.SMOOTH + LsoundLevel * (1 - mainSettings.SMOOTH);
        __isChanged = true;
        channels[0]->clReading();
      }
      if (channels[1]->isReady()) {
        uint16_t *_tpointer = (uint16_t *)channels[1]->getBands();
        int __level = *_tpointer + *(++_tpointer) + *(++_tpointer) + *(++_tpointer) + *(++_tpointer) + *(++_tpointer) + *(++_tpointer);
        // отсчёты [0-1023], сумма [0-7161]
        __level = rpow(__level >> 3);  // результат [0-895] -> [0-13425]
        RsoundLevel = __level * mainSettings.SMOOTH + RsoundLevel * (1 - mainSettings.SMOOTH);
        __isChanged = true;
        channels[1]->clReading();
      }
      if (channels[2]->isReady()) {
        uint16_t *_tpointer = (uint16_t *)channels[2]->getBands();
        int __level = *_tpointer + *(++_tpointer) + *(++_tpointer) + *(++_tpointer) + *(++_tpointer) + *(++_tpointer) + *(++_tpointer);
        __level = rpow(__level >> 3);  // результат [0-895] -> [0-13425]
        RsoundLevel = __level * mainSettings.SMOOTH + RsoundLevel * (1 - mainSettings.SMOOTH);
        LsoundLevel = RsoundLevel;  // режим моно
        __isChanged = true;
        channels[2]->clReading();
      }
      if (__isChanged) {
        __isChanged = false;

        //Serial.print(F("RsoundLevel = "));
        //Serial.print(RsoundLevel);
        //Serial.print("  ");
        //Serial.print(F("LsoundLevel = "));
        //Serial.println(LsoundLevel);

        // расчёт общей средней громкости с обоих каналов, фильтрация.
        // Фильтр очень медленный, сделано специально для автогромкости
        averageLevel = (float)(RsoundLevel + LsoundLevel) / 2 * averK + averageLevel * (1 - averK);
        // принимаем максимальную громкость шкалы как среднюю, умноженную на некоторый коэффициент MAX_COEF
        maxLevel = (float)averageLevel * MAX_COEF;
        // преобразуем сигнал в длину ленты (где SPLIT_NUM_LEDS это половина количества светодиодов)
        Rlenght = map(RsoundLevel, 0, maxLevel + 1, 0, SPLIT_NUM_LEDS);
        Llenght = map(LsoundLevel, 0, maxLevel + 1, 0, SPLIT_NUM_LEDS);
        // ограничиваем до макс. числа светодиодов
        Rlenght = constrain(Rlenght, 0, SPLIT_NUM_LEDS);
        Llenght = constrain(Llenght, 0, SPLIT_NUM_LEDS);
        animation();  // отрисовать
      }

      if (mainSettings.SILENCE) silence();
      if (Rlenght > 0 || Llenght > 0) silence_timer = millis();  // проверка тишины
    }

    // 2-5 режим - цветомузыка
    if (mainSettings.this_mode == 2 || mainSettings.this_mode == 3 || mainSettings.this_mode == 4 || mainSettings.this_mode == 7) {  //на самом деле 3  - 4 - 5 - 8
      bool __isChanged = false;
      if (channels[0]->isReady()) {
        uint16_t *_tpointer = (uint16_t *)channels[0]->getBands();
        memcpy((void *)vuMeterBands_L, (void *)_tpointer, 7 * sizeof(uint16_t));
        __isChanged = true;
        channels[0]->clReading();
      }
      if (channels[1]->isReady()) {
        uint16_t *_tpointer = (uint16_t *)channels[1]->getBands();
        memcpy((void *)vuMeterBands_R, (void *)_tpointer, 7 * sizeof(uint16_t));
        __isChanged = true;
        channels[1]->clReading();
      }
      if (channels[2]->isReady()) {
        uint16_t *_tpointer = (uint16_t *)channels[2]->getBands();
        memcpy((void *)vuMeterBands_L, (void *)_tpointer, 7 * sizeof(uint16_t));
        memcpy((void *)vuMeterBands_R, (void *)_tpointer, 7 * sizeof(uint16_t));
        __isChanged = true;
        channels[2]->clReading();
      }
      if (__isChanged) return;  //if (!__isChanged) return;
      __isChanged = false;

      for (uint8_t i = 0; i < 7; i++) {
        Serial.print(F("BandR "));
        Serial.print(i);
        Serial.print(F(": "));
        Serial.print(vuMeterBands_R[i]);
        Serial.print("  ");
        Serial.print(F("BandL "));
        Serial.print(i);
        Serial.print(F(": "));
        Serial.print(vuMeterBands_L[i]);
        Serial.print("  ");
      }
      Serial.println();

      for (byte i = 0; i < 7; i++) {
        colorMusic[i] = (vuMeterBands_L[i] + vuMeterBands_R[i]) / 2;  // сложил оба канала и усреднил
        colorMusic[i] = map(colorMusic[i], 0, 1023, 0, 255);
        colorMusic[i] = constrain(colorMusic[i], 0, 255);
      }

      // colorMusic 63 Гц - 0, 160 Гц - 1, 400 Гц - 2, 1000 Гц - 3, 2500 Гц - 4, 6250 Гц - 5, 16000 Гц - 6
      for (byte i = 0; i < 7; i++) {
        colorMusic_aver[i] = colorMusic[i] * averK + colorMusic_aver[i] * (1 - averK);                                  // общая фильтрация
        colorMusic_f[i] = colorMusic[i] * mainSettings.SMOOTH_FREQ + colorMusic_f[i] * (1 - mainSettings.SMOOTH_FREQ);  // локальная

        //if (colorMusic_f[i] > (colorMusic_aver[i] * MAX_COEF_FREQ_1[i])) {
        if (colorMusic_f[i] > colorMusic_aver[i]) {
          //thisBright[i] = 255;
          thisBright[i] = colorMusic[i];
          colorMusicFlash[i] = true;
          running_flag[i] = true;
        } else colorMusicFlash[i] = false;

        if (thisBright[i] > (EMPTY_BRIGHT + SMOOTH_STEP)) thisBright[i] -= SMOOTH_STEP;
        else {
          thisBright[i] = EMPTY_BRIGHT;
          running_flag[i] = false;
        }
      }

      if (mainSettings.SILENCE) silence();
      if (colorMusic[1] > 0) silence_timer = millis();  // проверка тишины
      animation();
    }

    // 9 режим - спектр
    if (mainSettings.this_mode == 8) {
      bool __isChanged = false;
      if (channels[0]->isReady()) {
        uint16_t *_tpointer = (uint16_t *)channels[0]->getBands();
        memcpy((void *)vuMeterBands_L, (void *)_tpointer, 7 * sizeof(uint16_t));
        __isChanged = true;
        channels[0]->clReading();
      }
      if (channels[1]->isReady()) {
        uint16_t *_tpointer = (uint16_t *)channels[1]->getBands();
        memcpy((void *)vuMeterBands_R, (void *)_tpointer, 7 * sizeof(uint16_t));
        __isChanged = true;
        channels[1]->clReading();
      }
      if (channels[2]->isReady()) {
        uint16_t *_tpointer = (uint16_t *)channels[2]->getBands();
        memcpy((void *)vuMeterBands_L, (void *)_tpointer, 7 * sizeof(uint16_t));
        memcpy((void *)vuMeterBands_R, (void *)_tpointer, 7 * sizeof(uint16_t));
        __isChanged = true;
        channels[2]->clReading();
      }
      if (!__isChanged) return;
      __isChanged = false;

      for (uint8_t i = 0; i < 7; i++) {
        vuMeterBands_R[i] = map(vuMeterBands_R[i], 0, 1023, 0, 255);
        vuMeterBands_R[i] = constrain(vuMeterBands_R[i], 0, 255);
        vuMeterBands_L[i] = map(vuMeterBands_L[i], 0, 1023, 0, 255);
        vuMeterBands_L[i] = constrain(vuMeterBands_L[i], 0, 255);
      }

      // это для анализатора спектра
      float freq_max_R = 0;
      float freq_max_L = 0;

      for (uint8_t i = 0; i < 7; i++) {
        if (vuMeterBands_R[i] > freq_max_R) freq_max_R = vuMeterBands_R[i];  // максимальное по всем частотам
        if (vuMeterBands_L[i] > freq_max_L) freq_max_L = vuMeterBands_L[i];  // максимальное по всем частотам
        if (freq_max_R < 5) freq_max_R = 5;
        if (freq_max_L < 5) freq_max_L = 5;
        //Serial.print(F("freq_max = ")); Serial.println(freq_max);

        if (freq_f_R[i] < vuMeterBands_R[i]) freq_f_R[i] = vuMeterBands_R[i];  // минимальное по всем частотам
        if (freq_f_R[i] > 1) freq_f_R[i] -= LIGHT_SMOOTH;
        else freq_f_R[i] = 0;

        if (freq_f_L[i] < vuMeterBands_L[i]) freq_f_L[i] = vuMeterBands_L[i];  // минимальное по всем частотам
        if (freq_f_L[i] > 1) freq_f_L[i] -= LIGHT_SMOOTH;
        else freq_f_L[i] = 0;
        //Serial.print(F("freq_f[i] = ")); Serial.println(freq_f[i]);
      }

      freq_max_f_R = freq_max_R * averK + freq_max_f_R * (1 - averK);  // общая фильтрация
      freq_max_f_L = freq_max_L * averK + freq_max_f_L * (1 - averK);  // общая фильтрация
      //Serial.print(F("freq_max_f = ")); Serial.println(freq_max_f);

      if (mainSettings.SILENCE) silence();
      if (colorMusic[1] > 0) silence_timer = millis();  // проверка тишины
      animation();
    }


    // стробоскоп
    if (mainSettings.this_mode == 5) {
      if (timer_func(mainSettings.STROBE_PERIOD)) {
        strobeUp_flag = true;
        strobeDwn_flag = false;
      }
      if (millis() - effect_timer > light_time) strobeDwn_flag = true;  // light_time скважность вспышек
      if (strobeUp_flag) {                                              // если настало время пыхнуть
        if (strobe_bright < 255)                                        // если яркость не максимальная
          strobe_bright += mainSettings.STROBE_SMOOTH;                  // увелчить
        if (strobe_bright > 255) {                                      // если пробили макс. яркость
          strobe_bright = 255;                                          // оставить максимум
          strobeUp_flag = false;                                        // флаг опустить
        }
      }
      if (strobeDwn_flag) {                             // гаснем
        if (strobe_bright > 0)                          // если яркость не минимальная
          strobe_bright -= mainSettings.STROBE_SMOOTH;  // уменьшить
        if (strobe_bright < 0) {                        // если пробили мин. яркость
          strobe_bright = 0;                            // оставить 0
          strobeDwn_flag = false;                       // флаг опустить
        }
      }
      animation();
    }

    if (mainSettings.this_mode == 6 || mainSettings.this_mode == 9) animation();

    if (!IRLremote.receiving())  // если на ИК приёмник не приходит сигнал (без этого НЕ РАБОТАЕТ!)
      FastLED.show();            // отправить значения на ленту
  }
}
