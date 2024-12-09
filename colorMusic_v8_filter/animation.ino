void animation() {

  // согласно режиму
  switch (mainSettings.this_mode) {

    // 1 Режим: шкала громкости (градиент)
    case 0: {
        uint16_t idex2 = 0;
        one_color_all(); //заливаем "подложку"  EMPTY BRIGHT
        if (timer_func(200)) hue = hue + mainSettings.RAINBOW_STEP;

        uint16_t heightL = Llenght;
        uint16_t heightR = Rlenght;
        if (heightL > peakL) peakL = heightL; // Держит точку "пик" вверху
        if (heightR > peakR) peakR = heightR; // Держит точку "пик" вверху

        if (CMU_CENTER) { // Only from center
          if (peakL > 0 && peakL <= SPLIT_NUM_LEDS - 1) {
            leds[SPLIT_NUM_LEDS + peakL] = ColorFromPalette(RainbowColors_p, (map(peakL, 0, SPLIT_NUM_LEDS, 0, 255)));   // пик слева
          }
          if (peakR > 0 && peakR <= SPLIT_NUM_LEDS - 1) {
            leds[SPLIT_NUM_LEDS - peakR - 1] = ColorFromPalette(RainbowColors_p, (map(peakR, 0, SPLIT_NUM_LEDS, 0, 255))); // пик справа
          }
        }

        // Каждые несколько кадров уменьшает пиковый пиксель на 1:
        if (++dotCount >= PEAK_FALL) { // Скорость падения
          if (peakR > 0) peakR--;
          if (peakL > 0) peakL--;
          dotCount = 0;
        }

        for (byte j = 0; j < 2; j++) {
          for (uint16_t i = 0; i < (Rlenght * !j + Llenght * j); i++) {
            if (CMU_CENTER) {
              idex2 = SPLIT_NUM_LEDS - 1 - i;
              if (j) idex2 = SPLIT_NUM_LEDS + i;
            }
            else {
              idex2 = i;
              if (j) idex2 = NUM_LEDS - 1 - i;
            }

            switch (mainSettings.cmu_mode) {
              // ПОДРежим: шкала громкости (зеленая-красная)
              case 0: leds[idex2] = ColorFromPalette(myPal, i); break;
              // ПОДРежим: шкала громкости (радуга)
              case 1: leds[idex2] = ColorFromPalette(RainbowColors_p, (map(i, 0, SPLIT_NUM_LEDS, 0, 255)) / 2 - hue); break;
              // ПОДРежим: шкала громкости (Огонь)
              case 2: setPixelHeatColor (idex2, map(i, 0, Rlenght * !j + Llenght * j, 195, 1)); break;
            }
            //leds[idex2] %= (map(i, 0, SPLIT_NUM_LEDS, 255, 0));
            leds[idex2] %= getBrightCRT((map(i, 0, SPLIT_NUM_LEDS, 255, 0)));
          }
        }
        if (silence_flag) one_color_all(0, 0, 40); // Включить подсветку через n секунд тишины
      }
      break;

    // 2 Режим: точки, полоски
    case 1: {
        new_effect2();
        if (silence_flag) one_color_all(0, 0, 40); // Включить подсветку через n секунд тишины
      }
      break;

    // 3 Режим: вспышки
    case 2: {
        switch (mainSettings.cmu2_mode) {
          case 0:
            if (timer_func(mainSettings.FLASH_PERIOD)) {
              for (byte i = 0; i < 7; i++) {
                uint16_t idex1 = random(NUM_LEDS - 14);
                leds[idex1] = CHSV(LOW_COLOR_0, thissat, thisBright[0]);
                leds[idex1 + 2] = CHSV(LOW_COLOR_1, thissat, thisBright[1]);
                leds[idex1 + 4] = CHSV(MID_COLOR_2, thissat, thisBright[2]);
                leds[idex1 + 7] = CHSV(MID_COLOR_3, thissat, thisBright[3]);
                leds[idex1 + 9] = CHSV(MID_COLOR_4, thissat, thisBright[4]);
                leds[idex1 + 12] = CHSV(HIGH_COLOR_5, thissat, thisBright[5]);
                leds[idex1 + 14] = CHSV(HIGH_COLOR_6, thissat, thisBright[6]);
              }
            }
            break;

          case 1: // вспышки бегут...          
            if (timer_func(mainSettings.FLASH_PERIOD)) {              
              for (uint8_t i = (NUM_LEDS - 1); i != 0; i--) {
                leds[i] = leds[i - 1];
                //leds[0] = CHSV(LOW_COLOR_1, thissat, thisBright[1]);
                if (colorMusicFlash[0]) leds[0] = CHSV(LOW_COLOR_0, thissat,  thisBright[0]);
                else if (colorMusicFlash[3]) leds[0] = CHSV(MID_COLOR_3, thissat,  thisBright[3]);
                else if (colorMusicFlash[5]) leds[0] = CHSV(HIGH_COLOR_5, thissat,  thisBright[5]);
                else leds[0] = CHSV(EMPTY_COLOR, thissat, EMPTY_BRIGHT);
              }
            }
            break;

          case 2: // вспышки бегут2...
            if (timer_func(mainSettings.FLASH_PERIOD)) {
              //for (byte l = 0; l < 7; l++) {
              // перематываем массив светодиодов на P_SPEED вправо
              for (byte k = 0; k < P_SPEED; k++) {
                for (uint16_t i = NUM_LEDS - 1; i > 0; i--) leds[i] = leds[i - 1];
              }
              if (colorMusicFlash[1]) HUEindex += mainSettings.HUE_STEP;
              CRGB color = CHSV(HUEindex, 255, thisBright[1]);
              //CRGB color = CHSV(HUEindex + thisBright[l] / 5, 255 - thisBright[l], thisBright[l]);
              for (uint8_t i = 0; i < P_SPEED; i++) leds[i] = color;
              //}
            }
            break;
        }
        if (silence_flag) one_color_all(0, 0, 40); // Включить подсветку через n секунд тишины
      }
      break;

    // 4 Режим: цветомузыка 3-4-7 полос (подрежимы: в cmu3_mode)
    case 3: {
        switch (mainSettings.cmu3_mode) {
          case 0:
            // три канала
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
              if (i < NUM_LEDS / 3)           leds[i] = CHSV(LOW_COLOR_0, thissat,  thisBright[0]); //R
              else if (i < NUM_LEDS * 2 / 3)  leds[i] = CHSV(MID_COLOR_3, thissat,  thisBright[2]); //G
              else if (i < NUM_LEDS)          leds[i] = CHSV(HIGH_COLOR_5, thissat, thisBright[4]); //B
            }
            break;
          // 4 канала
          case 1:
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
              if (i < NUM_LEDS / 4)          leds[i] = CHSV(HIGH_COLOR_5, thissat, thisBright[5]); //B
              else if (i < NUM_LEDS * 2 / 4) leds[i] = CHSV(MID_COLOR_2, thissat,  thisBright[2]); //Y
              else if (i < NUM_LEDS * 3 / 4) leds[i] = CHSV(MID_COLOR_3, thissat,  thisBright[3]); //G
              else if (i < NUM_LEDS)         leds[i] = CHSV(LOW_COLOR_0, thissat,  thisBright[0]); //R
            }
            break;
          // 7 каналов
          case 2:
            for (uint16_t i = 0; i < NUM_LEDS; i++) {
              if (i < NUM_LEDS / 7)                  leds[i] = CHSV(LOW_COLOR_0, thissat,  thisBright[0]); //R
              else if (i < NUM_LEDS * 2 / 7)         leds[i] = CHSV(LOW_COLOR_1, thissat,  thisBright[1]); //O
              else if (i < NUM_LEDS * 3 / 7)         leds[i] = CHSV(MID_COLOR_2, thissat,  thisBright[2]); //Y
              else if (i < NUM_LEDS * 4 / 7)         leds[i] = CHSV(MID_COLOR_3, thissat,  thisBright[3]); //G
              else if (i < NUM_LEDS * 5 / 7)         leds[i] = CHSV(MID_COLOR_4, thissat,  thisBright[4]); //A
              else if (i < NUM_LEDS * 6 / 7)         leds[i] = CHSV(HIGH_COLOR_5, thissat, thisBright[5]); //B
              else if (i < NUM_LEDS)                 leds[i] = CHSV(HIGH_COLOR_6, thissat, thisBright[6]); //P
            }
            break;
        }
        if (silence_flag) one_color_all(0, 0, 40); // Включить подсветку через n секунд тишины
      }
      break;

    // 5 Режим: цветомузыка 1 полоса
    case 4:
      if (colorMusicFlash[5])      one_color_all(HIGH_COLOR_5, thissat, thisBright[5]);
      else if (colorMusicFlash[2]) one_color_all(MID_COLOR_2, thissat, thisBright[2]);
      else if (colorMusicFlash[3]) one_color_all(MID_COLOR_3, thissat, thisBright[3]);
      else if (colorMusicFlash[0]) one_color_all(LOW_COLOR_0, thissat, thisBright[0]);
      else one_color_all(); // SILENCE
      if (silence_flag) one_color_all(0, 0, 40); // Включить подсветку через n секунд тишины
      break;

    // 6 Режим: стробоскоп
    case 5:
      if (strobe_bright > 0)
        one_color_all(mainSettings.LIGHT_COLOR, mainSettings.LIGHT_SAT, strobe_bright);
      else one_color_all(); // SILENCE
      break;

    // 7 режим: цветовая подсветка
    case 6: {
        switch (mainSettings.light_mode) {
          case 0: one_color_all(mainSettings.LIGHT_COLOR, mainSettings.LIGHT_SAT, 255); break;
          case 1:
            if (timer_func(mainSettings.RUNNING_SPEED)) {
              //this_color -= 1;
              rainbow_steps -= 1; // rainbow_steps++ counter меняется от 0 до 255 (тип данных byte)
            }
            //fill_rainbow( leds, NUM_LEDS, this_color, mainSettings.RAINBOW_STEP );   // mainSettings.RAINBOW_STEP 1 - 30
            for (uint16_t i = 0; i < NUM_LEDS; i++ ) {         // от 0 до первой трети
              leds[i] = CHSV((rainbow_steps + i) * mainSettings.RAINBOW_STEP, thissat, 255);// HSV. Увеличивать HUE (цвет)// умножение i уменьшает шаг радуги
            }
            break;
        }
      }
      break;

    // 8 режим: бегущие частоты из середины
    case 7: {
        switch (mainSettings.freq_strobe_mode) {
          case 0:
            if (running_flag[0]) leds[SPLIT_NUM_LEDS] = CHSV(LOW_COLOR_0, thissat, thisBright[0]);
            else if (running_flag[2]) leds[SPLIT_NUM_LEDS] = CHSV(MID_COLOR_2, thissat, thisBright[2]);
            else if (running_flag[3]) leds[SPLIT_NUM_LEDS] = CHSV(MID_COLOR_3, thissat, thisBright[3]);
            else if (running_flag[5]) leds[SPLIT_NUM_LEDS] = CHSV(HIGH_COLOR_5, thissat, thisBright[5]);
            else leds[SPLIT_NUM_LEDS] = CHSV(EMPTY_COLOR, thissat, EMPTY_BRIGHT);
            leds[(SPLIT_NUM_LEDS) - 1] = leds[SPLIT_NUM_LEDS];
            if (timer_func(mainSettings.RUNNING_SPEED)) {
              for (uint16_t i = 0; i < SPLIT_NUM_LEDS - 1; i++) {
                leds[i] = leds[i + 1];
                leds[NUM_LEDS - i - 1] = leds[i];
              }
            }
            break;

          case 1: // бегущие частоты из начала
            if (running_flag[0]) leds[0] = CHSV(LOW_COLOR_0, thissat, thisBright[0]);
            else if (running_flag[2]) leds[0] = CHSV(MID_COLOR_2, thissat, thisBright[2]);
            else if (running_flag[3]) leds[0] = CHSV(MID_COLOR_3, thissat, thisBright[3]);
            else if (running_flag[5]) leds[0] = CHSV(HIGH_COLOR_5, thissat, thisBright[5]);
            else leds[0] = CHSV(EMPTY_COLOR, thissat, EMPTY_BRIGHT);
            leds[1] = leds[0];
            if (timer_func(mainSettings.RUNNING_SPEED)) {
              for ( uint16_t i = NUM_LEDS - 1; i >= 2; i--) {
                leds[i] = leds[i - 1];
              }
            }
            break;
        }
        if (silence_flag) one_color_all(0, 0, 40); // Включить подсветку через n секунд тишины
      }
      break;

    // 9 режим: анализатор спектра
    case 8: {
        HUEindex = mainSettings.HUE_START;
        //Serial.print(F("this_bright =  ")); Serial.println(this_bright);
        for (uint16_t i = 0; i < SPLIT_NUM_LEDS; i++) {
          uint8_t this_bright_R = map(freq_f_R[(uint16_t)floor((SPLIT_NUM_LEDS - i) / freq_to_stripe)], 0, freq_max_f_R, 0, 255);
          uint8_t this_bright_L = map(freq_f_L[(uint16_t)floor((SPLIT_NUM_LEDS - i) / freq_to_stripe)], 0, freq_max_f_L, 0, 255);
          leds[i] = CHSV(HUEindex, thissat, this_bright_R); // правый канал
          //leds[NUM_LEDS - i - 1] = leds[i];
          leds[NUM_LEDS - i - 1] = CHSV(HUEindex, thissat, this_bright_L); // левый канал
          HUEindex += mainSettings.HUE_STEP;
          if (HUEindex > 255) HUEindex = 0;
        }
      }
      break;

    // 10 режим: эффекты
    case 9: new_effect(); break;

  }
}
