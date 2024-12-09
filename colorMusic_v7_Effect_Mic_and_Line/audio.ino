void audio() {
  int RsoundLevel, RsoundLevel_f, LsoundLevel, LsoundLevel_f;
  // главный цикл обработки аудио сигнала
  if (ONstate) {
    // первые два режима - громкость (VU meter)
    if (mainSettings.this_mode == 0 || mainSettings.this_mode == 1) {

      // сбрасываем значения
      RsoundLevel = 0;
      LsoundLevel = 0;
      Rlenght = 0;
      Llenght = 0;

      analyzeMSGEQ7();

      // складываем все частоты правого канала
      for (byte i = 0; i < 7; i++) {
        RsoundLevel = (RsoundLevel + vuMeterBands_R[i]);
      }
      RsoundLevel = RsoundLevel / 7; //усредняем
      // складываем все частоты левого канала
      for (byte i = 0; i < 7; i++) {
        LsoundLevel = (LsoundLevel + vuMeterBands_L[i]);
      }
      LsoundLevel = LsoundLevel / 7; //усредняем

      // фильтруем по нижнему порогу шумов
      RsoundLevel = map(RsoundLevel, mainSettings.VU_LOW_PASS_line_R, 1023, 0, 500); // 500 это ничего не значит. можно и 255
      LsoundLevel = map(LsoundLevel, mainSettings.VU_LOW_PASS_line_L, 1023, 0, 500);

      // возводим в степень (для большей чёткости работы)
      RsoundLevel = pow(RsoundLevel, EXP); // EXP = 1.4
      LsoundLevel = pow(LsoundLevel, EXP);

      //Serial.print(F("RsoundLevel = ")); Serial.print(RsoundLevel);
      //Serial.print ("  ");
      //Serial.print(F("LsoundLevel = ")); Serial.println(LsoundLevel);

      // фильтр
      RsoundLevel_f = RsoundLevel * mainSettings.SMOOTH + RsoundLevel_f * (1 - mainSettings.SMOOTH);
      LsoundLevel_f = LsoundLevel * mainSettings.SMOOTH + LsoundLevel_f * (1 - mainSettings.SMOOTH);

      //Serial.print(F("RsoundLevel_f = ")); Serial.print(RsoundLevel_f);
      //Serial.print ("  ");
      //Serial.print(F("LsoundLevel_f = ")); Serial.println(LsoundLevel_f);


      // расчёт общей средней громкости с обоих каналов, фильтрация.
      // Фильтр очень медленный, сделано специально для автогромкости
      averageLevel = (float)(RsoundLevel_f + LsoundLevel_f) / 2 * averK + averageLevel * (1 - averK);

      // принимаем максимальную громкость шкалы как среднюю, умноженную на некоторый коэффициент MAX_COEF
      maxLevel = (float)averageLevel * MAX_COEF;
      //Serial.print(F("maxLevel = ")); Serial.println(maxLevel);

      // преобразуем сигнал в длину ленты (где SPLIT_NUM_LEDS это половина количества светодиодов)
      Rlenght = map(RsoundLevel_f, 0, maxLevel + 1, 0, SPLIT_NUM_LEDS);
      Llenght = map(LsoundLevel_f, 0, maxLevel + 1, 0, SPLIT_NUM_LEDS);

      // ограничиваем до макс. числа светодиодов
      Rlenght = constrain(Rlenght, 0, SPLIT_NUM_LEDS);
      Llenght = constrain(Llenght, 0, SPLIT_NUM_LEDS);

      //Serial.print(F("Rlenght = ")); Serial.print(Rlenght);
      //Serial.print ("  ");
      //Serial.print(F("Llenght = ")); Serial.println(Llenght);

      //if (mainSettings.SILENCE)
      //if (Rlenght == 0 || Llenght == 0) {                  // проверка тишины
      //silence();
      //} else {
      //silence();
      //silence_timer = millis();
      //}

      if (mainSettings.SILENCE)
        silence();
      if (Rlenght > 0 || Llenght > 0) silence_timer = millis();  // проверка тишины

      animation();       // отрисовать
    }

    // 2-5 режим - цветомузыка
    if (mainSettings.this_mode == 2 || mainSettings.this_mode == 3 || mainSettings.this_mode == 4 || mainSettings.this_mode == 7) { //на самом деле 3  - 4 - 5 - 8

      analyzeMSGEQ7();

      for (byte i = 0 ; i < 7 ; i++) {
        colorMusic[i] = (vuMeterBands_L[i] + vuMeterBands_R[i]) / 2;  // сложил оба канала и усреднил
        colorMusic[i] = map(colorMusic[i], (mainSettings.VU_LOW_PASS_line_R + mainSettings.VU_LOW_PASS_line_L) / 2, 1023, 0, 255);
        colorMusic[i] = constrain(colorMusic[i], 0, 255);
      }

      /*
        Serial.print ("colorMusic =  ");
        for (int i = 0; i < 7; i++) {
        Serial.print (colorMusic[i]);
        Serial.print ("  ");
        }
        Serial.println ();
      */
      // colorMusic 63 Гц - 0, 160 Гц - 1, 400 Гц - 2, 1000 Гц - 3, 2500 Гц - 4, 6250 Гц - 5, 16000 Гц - 6


      for (byte i = 0; i < 7; i++) {
        colorMusic_aver[i] = colorMusic[i] * averK + colorMusic_aver[i] * (1 - averK);  // общая фильтрация
        colorMusic_f[i] = colorMusic[i] * mainSettings.SMOOTH_FREQ + colorMusic_f[i] * (1 - mainSettings.SMOOTH_FREQ);      // локальная

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

      if (mainSettings.SILENCE)
        silence();
      if (colorMusic[1] > 0) silence_timer = millis();  // проверка тишины
      /*
        for (byte i = 0; i < 7; i++) {
        Serial.print (F("thisBright")); Serial.print (thisBright[i]); Serial.print ("  ");
        Serial.print (F("colorMusic")); Serial.println (colorMusic[i]);
        //Serial.print (F("colorMusic_f =  "));    Serial.print (colorMusic_f[i]); Serial.print ("  "); Serial.println (i);
        //Serial.print (F("colorMusic_aver =  ")); Serial.print ((float)colorMusic_aver[i] * MAX_COEF_FREQ * MAX_COEF_FREQ_1[i]); Serial.print ("  "); Serial.println (i);
        }
      */

      animation();
    }

    // 9 режим - спектр
    if (mainSettings.this_mode == 8) {

      analyzeMSGEQ7();

      for (byte i = 0 ; i < 7 ; i++) {
        vuMeterBands_R[i] = map(vuMeterBands_R[i], mainSettings.VU_LOW_PASS_line_R, 1023, 0, 255);
        vuMeterBands_R[i] = constrain(vuMeterBands_R[i], 0, 255);
        vuMeterBands_L[i] = map(vuMeterBands_L[i], mainSettings.VU_LOW_PASS_line_L, 1023, 0, 255);
        vuMeterBands_L[i] = constrain(vuMeterBands_L[i], 0, 255);
      }

      // это для анализатора спектра
      float freq_max_R = 0;
      float freq_max_L = 0;

      for (byte i = 0; i < 7; i++) {
        if (vuMeterBands_R[i] > freq_max_R) freq_max_R = vuMeterBands_R[i]; // максимальное по всем частотам
        if (vuMeterBands_L[i] > freq_max_L) freq_max_L = vuMeterBands_L[i]; // максимальное по всем частотам
        if (freq_max_R < 5) freq_max_R = 5;
        if (freq_max_L < 5) freq_max_L = 5;
        //Serial.print(F("freq_max = ")); Serial.println(freq_max);

        if (freq_f_R[i] < vuMeterBands_R[i]) freq_f_R[i] = vuMeterBands_R[i]; // минимальное по всем частотам
        if (freq_f_R[i] > 1) freq_f_R[i] -= LIGHT_SMOOTH;
        else freq_f_R[i] = 0;

        if (freq_f_L[i] < vuMeterBands_L[i]) freq_f_L[i] = vuMeterBands_L[i]; // минимальное по всем частотам
        if (freq_f_L[i] > 1) freq_f_L[i] -= LIGHT_SMOOTH;
        else freq_f_L[i] = 0;
        //Serial.print(F("freq_f[i] = ")); Serial.println(freq_f[i]);
      }

      freq_max_f_R = freq_max_R * averK + freq_max_f_R * (1 - averK); // общая фильтрация
      freq_max_f_L = freq_max_L * averK + freq_max_f_L * (1 - averK); // общая фильтрация
      //Serial.print(F("freq_max_f = ")); Serial.println(freq_max_f);

      animation();
    }


    // стробоскоп
    if (mainSettings.this_mode == 5) {
      if (timer_func(mainSettings.STROBE_PERIOD)) {
        strobeUp_flag = true;
        strobeDwn_flag = false;
      }
      if (millis() - effect_timer > light_time) strobeDwn_flag = true; // light_time скважность вспышек
      if (strobeUp_flag) {                    // если настало время пыхнуть
        if (strobe_bright < 255)              // если яркость не максимальная
          strobe_bright += mainSettings.STROBE_SMOOTH;     // увелчить
        if (strobe_bright > 255) {            // если пробили макс. яркость
          strobe_bright = 255;                // оставить максимум
          strobeUp_flag = false;              // флаг опустить
        }
      }
      if (strobeDwn_flag) {                   // гаснем
        if (strobe_bright > 0)                // если яркость не минимальная
          strobe_bright -= mainSettings.STROBE_SMOOTH;     // уменьшить
        if (strobe_bright < 0) {              // если пробили мин. яркость
          strobe_bright = 0;                  // оставить 0
          strobeDwn_flag = false;             // флаг опустить
        }
      }
      animation();
    }

    if (mainSettings.this_mode == 6 || mainSettings.this_mode == 9) animation();

    if (!IRLremote.receiving())    // если на ИК приёмник не приходит сигнал (без этого НЕ РАБОТАЕТ!)
      FastLED.show();         // отправить значения на ленту
  }
}
