void remoteTick() {
  if (IRLremote.available())  {
    auto data = IRLremote.read();
    IRdata = data.command;
    ir_flag = true;
  }
  if (ir_flag) {                  // если данные пришли
    eeprom_timer = millis();
    eeprom_flag = true;

    //Serial.println(IRdata, HEX);  // раскомментировать для определения кодов пульта

    if ((IRdata) == BUTT_STAR) {
      buttonTickLed ();
      if (settings_mode == true) {
        uint8_t st = 0;
        EEPROM_UpdateBlock(0, &st, sizeof(st)); //в 0 ячейку пишем нуль для отметки о первом запуске
        resetFunc();
      }
      Proc_OnOff();
    }

    if (ONstate == false) goto TickExit;

    switch (IRdata) {  //режимы и подрежимы

      case BUTT_HASH: buttonTickLed ();
        switch (mainSettings.this_mode) {
          case 0: if (++mainSettings.cmu_mode > 2)  mainSettings.cmu_mode = 0;  break;
          case 1: reset_arr(); if (++mainSettings.cmu1_mode > 3) mainSettings.cmu1_mode = 0;  break;
          case 2: if (++mainSettings.cmu2_mode > 2) mainSettings.cmu2_mode = 0; break;
          case 3: if (++mainSettings.cmu3_mode > 2) mainSettings.cmu3_mode = 0; break;
          case 5: mainSettings.LIGHT_COLOR = mainSettings.LIGHT_COLOR + 20;
            if (mainSettings.LIGHT_COLOR > 255) mainSettings.LIGHT_COLOR = 0; break;
          case 6: if (++mainSettings.light_mode > 1) mainSettings.light_mode = 0; break;
          case 7: if (++mainSettings.freq_strobe_mode > 1) mainSettings.freq_strobe_mode = 0; break;
          case 9: if (++mainSettings.effect_mode > 9) mainSettings.effect_mode = 0; one_color_all(); break;
        }
        break;

      case BUTT_OK: buttonTickLed (); Proc_OK();  break;

      case BUTT_1: buttonTickLed ();
        if (mainSettings.this_mode == false) {
          CMU_CENTER = !CMU_CENTER;
        }
        else mainSettings.this_mode = 0;
        break;
      case BUTT_2: buttonTickLed (); reset_arr(); mainSettings.this_mode = 1; break;
      case BUTT_3: buttonTickLed (); mainSettings.this_mode = 2; break;
      case BUTT_4: buttonTickLed (); mainSettings.this_mode = 3; break;
      case BUTT_5: buttonTickLed (); mainSettings.this_mode = 4; break;
      case BUTT_6: buttonTickLed (); mainSettings.this_mode = 5; break;
      case BUTT_7: buttonTickLed (); mainSettings.this_mode = 6; break;
      case BUTT_8: buttonTickLed (); mainSettings.this_mode = 7; break;
      
      case BUTT_9: buttonTickLed ();
        if (settings_mode) {
          mainSettings.SILENCE = !mainSettings.SILENCE; // отключение - включение режима тишины
        }
        else mainSettings.this_mode = 8;
        break;

      case BUTT_0: buttonTickLed ();
        if (settings_mode) {
          if (line_mode == false) {
            //line_mode = true; digitalWrite(LED_line, HIGH); digitalWrite(RELAYon, HIGH);  // включить микрофон
            line_mode = true; PORTB |= ( 1 << PB0); PORTD |= ( 1 << PD4);  // включить микрофон
          }
          else {
            //line_mode = false;  digitalWrite(LED_line, LOW);  digitalWrite(RELAYon, LOW);  // выключить микрофон
            line_mode = false; PORTB &= ~ (1 << PB0); PORTD &= ~ (1 << PD4); // выключить микрофон
          }
        }
        else mainSettings.this_mode = 9;
        break;

      //case BUTT_LINE: buttonTickLed (); // включить микрофон
      //  if (line_mode == false) {
      //    line_mode = true; digitalWrite(LED_line, HIGH); digitalWrite(RELAYon, HIGH);
      //  }
      //  else {
      //    line_mode = false; digitalWrite(LED_line, LOW); digitalWrite(RELAYon, LOW);
      //  }
      //  break;

      case BUTT_UP: buttonTickLed ();
        if (settings_mode) {
          // ВВЕРХ общие настройки
          //EMPTY_BRIGHT = smartIncr(EMPTY_BRIGHT, 5, 0, 255);
        } else {
          switch (mainSettings.this_mode) {
            case 0: //режим 1 Шкала громкости
              switch (mainSettings.cmu_mode) {
                case 1: mainSettings.RAINBOW_STEP = smartIncr(mainSettings.RAINBOW_STEP, 1, 0, 20);  break;   //Радуга
              }
              break;
            case 1: break;  //режим 2
            case 2: mainSettings.HUE_STEP = smartIncr(mainSettings.HUE_STEP, 10, 1, 255);  break;  //режим 3
            case 3: break;//режим 4
            case 4: break;//режим 5
            case 5: mainSettings.STROBE_PERIOD = smartIncr(mainSettings.STROBE_PERIOD, 20, 1, 255); break;  //режим 6 стробоскоп
            case 6: //режим 7
              switch (mainSettings.light_mode) {
                case 0:  mainSettings.LIGHT_SAT = smartIncr(mainSettings.LIGHT_SAT, 20, 0, 255);  break; // подсветка
                case 1: mainSettings.RAINBOW_STEP = smartIncr(mainSettings.RAINBOW_STEP, 1, 0, 20);  break; // радуга
              }
              break;
            case 7: break;  //режим 8
            case 8: mainSettings.HUE_START = smartIncr(mainSettings.HUE_START, 20, 0, 255); break;  //режим 9
            case 9: mainSettings.SMOOTH_K = smartIncrFloat(mainSettings.SMOOTH_K, 0.05, 0.1, 1);  break; //режим 10 эффекты плавность огня
          }
        }
        break;

      case BUTT_DOWN: buttonTickLed ();
        if (settings_mode) {
          // ВНИЗ общие настройки
          //EMPTY_BRIGHT = smartIncr(EMPTY_BRIGHT, -5, 0, 255);
        } else {
          switch (mainSettings.this_mode) {
            case 0: //режим 1
              switch (mainSettings.cmu_mode) {
                case 1: mainSettings.RAINBOW_STEP = smartIncr(mainSettings.RAINBOW_STEP, -1, 0, 20); break;      //Радуга
              }
              break;
            case 1: break;  //режим 2
            case 2: mainSettings.HUE_STEP = smartIncr(mainSettings.HUE_STEP, -10, 1, 255);  break;  //режим 3
            case 3: break;//режим 4
            case 4: break;//режим 5
            case 5: mainSettings.STROBE_PERIOD = smartIncr(mainSettings.STROBE_PERIOD, -20, 1, 255);  break;      //режим 6 стробоскоп
            case 6: //режим 7
              switch (mainSettings.light_mode) {
                case 0: mainSettings.LIGHT_SAT = smartIncr(mainSettings.LIGHT_SAT, -20, 0, 255);  break;
                case 1: mainSettings.RAINBOW_STEP = smartIncr(mainSettings.RAINBOW_STEP, -1, 0, 20); break;
              }
              break;
            case 7: break;   //режим 8
            case 8: mainSettings.HUE_START = smartIncr(mainSettings.HUE_START, -20, 0, 255);  break;
            case 9: mainSettings.SMOOTH_K = smartIncrFloat(mainSettings.SMOOTH_K, -0.05, 0.1, 1); break;  //режим 10 эффекты плавность огня
          }
        }
        break;

      case BUTT_LEFT: buttonTickLed ();
        if (settings_mode) {
          // ВЛЕВО общие настройки
          mainSettings.BRIGHTNESS = smartIncr(mainSettings.BRIGHTNESS, -5, 5, 255);  //установка общей яркости уменьшить
          FastLED.setBrightness(mainSettings.BRIGHTNESS);
        } else {  // установка по режимам плавность анимации уменьшить
          switch (mainSettings.this_mode) {
            case 0: mainSettings.SMOOTH = smartIncrFloat(mainSettings.SMOOTH, -0.05, 0.05, 1);  break;
            case 1: effect_delay = smartIncr(effect_delay, 2, 0, 100); break;
            case 2: mainSettings.FLASH_PERIOD = smartIncr(mainSettings.FLASH_PERIOD, 10, 1, 255);  break;
            case 3:
            case 4: mainSettings.SMOOTH_FREQ = smartIncrFloat(mainSettings.SMOOTH_FREQ, -0.05, 0.05, 1);  break;
            case 5: mainSettings.STROBE_SMOOTH = smartIncr(mainSettings.STROBE_SMOOTH, -10, 0, 255);  break;
            case 6:
              switch (mainSettings.light_mode) {
                case 0: mainSettings.LIGHT_COLOR = smartIncr(mainSettings.LIGHT_COLOR, -20, 0, 255);  break;
                case 1: mainSettings.RUNNING_SPEED = smartIncr(mainSettings.RUNNING_SPEED, 5, 5, 100);  break;
              }
              break;
            case 7:
              switch (mainSettings.freq_strobe_mode) { //бегущие частоты
                case 0:
                case 1: mainSettings.RUNNING_SPEED = smartIncr(mainSettings.RUNNING_SPEED, 5, 5, 100);  break;
              }
            case 8: mainSettings.HUE_STEP = smartIncr(mainSettings.HUE_STEP, -10, 1, 255); break;
            case 9: mainSettings.COLOR_SPEED = smartIncr(mainSettings.COLOR_SPEED, 10, 10, 255); break;  //режим 10 эффекты
          }
        }
        break;

      case BUTT_RIGHT: buttonTickLed ();
        if (settings_mode) {
          // ВПРАВО общие настройки
          mainSettings.BRIGHTNESS = smartIncr(mainSettings.BRIGHTNESS, 5, 5, 255);
          FastLED.setBrightness(mainSettings.BRIGHTNESS);
        } else {
          switch (mainSettings.this_mode) {
            case 0: mainSettings.SMOOTH = smartIncrFloat(mainSettings.SMOOTH, 0.05, 0.05, 1); break;
            case 1: effect_delay = smartIncr(effect_delay, -2, 0, 100);  break;
            case 2: mainSettings.FLASH_PERIOD = smartIncr(mainSettings.FLASH_PERIOD, -10, 1, 255); break;
            case 3:
            case 4: mainSettings.SMOOTH_FREQ = smartIncrFloat(mainSettings.SMOOTH_FREQ, 0.05, 0.05, 1); break;
            case 5: mainSettings.STROBE_SMOOTH = smartIncr(mainSettings.STROBE_SMOOTH, 10, 0, 255); break;
            case 6:
              switch (mainSettings.light_mode) {
                case 0: mainSettings.LIGHT_COLOR = smartIncr(mainSettings.LIGHT_COLOR, 20, 0, 255); break;
                case 1: mainSettings.RUNNING_SPEED = smartIncr(mainSettings.RUNNING_SPEED, -5, 5, 100); break;
              }
              break;
            case 7:
              switch (mainSettings.freq_strobe_mode) { //бегущие частоты
                case 0:
                case 1: mainSettings.RUNNING_SPEED = smartIncr(mainSettings.RUNNING_SPEED, -5, 5, 100); break;
              }
            case 8: mainSettings.HUE_STEP = smartIncr(mainSettings.HUE_STEP, 10, 1, 255);  break;
            case 9: mainSettings.COLOR_SPEED = smartIncr(mainSettings.COLOR_SPEED, -10, 10, 255);  break;
          }
        }
        break;
    }
TickExit:
    ir_flag = false;
    //print();  //вывод данных для тестирования
  }
}
