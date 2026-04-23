void new_effect2() {

  if ((max(Llenght, Rlenght) < vally) && strobeDwn_flag) {
    strobeDwn_flag = false;
    if (vally) magnitude = map(vally, 0, SPLIT_NUM_LEDS, 0, 255);
    //Serial.print(F("magnitude = ")); Serial.println(magnitude);
  } else {
    strobeDwn_flag = true;
  }

  vally = max(Llenght, Rlenght);


  if (timer_func()) { //100 50
    //++this_color; // по нарастающей
    this_color = random8(); // случайный цвет

    for (uint8_t i = 0; i < N_PEAKS; i++) {
      if (magnitude(i) == 0) {
        age(i) = 0;
        magnitude(i) = magnitude;
        baseColor(i) = this_color;
        magnitude = 0;
        break;
      }
    }
  }
  one_color_all();
  switch (mainSettings.cmu1_mode) {
    case DANCE_PARTY:         dance_party(SPLIT_NUM_LEDS + NUM_LEDS / 8);  break; //огоньки из центра
    case DANCE_PARTY_SINGLE:  dance_party(NUM_LEDS + NUM_LEDS / 4);        break; //огоньки из начала
    default:                  color_bars(60);                              break; // время жизни полосочки 60
  }
}

//======================================убежавшая вспышка=============================================

void dance_party(uint16_t MAX_AGE) {
  if (millis() - main_timer_dance > 12) { // таймер для убегающих вспышек
    main_timer_dance = millis();    // сбросить таймер
    uint16_t pos;
    uint16_t age_full;
    for (byte i = 0; i < N_PEAKS; i++) {
      age_full = (age(i) + 256 * rnd(i));
      if (magnitude(i) > 0) {
        if (magnitude(i) > 245)       pos = age_full;
        else                          pos = ((127 + (magnitude(i) / 2)) / 255.0) * age_full;
        if (mainSettings.cmu1_mode == DANCE_PARTY) {
          leds[SPLIT_NUM_LEDS + pos]        = CHSV(baseColor(i), thissat, map(age_full, 0, MAX_AGE, 255, 0));
          leds[SPLIT_NUM_LEDS - pos - 1]    = CHSV(baseColor(i), thissat, map(age_full, 0, MAX_AGE, 255, 0));
        } else {
          leds[pos]                         = CHSV(baseColor(i), thissat, map(age_full, 0, MAX_AGE, 255, 0));
        }

        if (age(i) == 255) rnd(i)++;  // Использование дополнительных битов для возраста
        age(i)++; //Прирост возраста

        if ((age_full + 1 > MAX_AGE) ||                                      // Если возраст огонька больше максимального
            ((pos + 1 >= SPLIT_NUM_LEDS) && (mainSettings.cmu1_mode == DANCE_PARTY)) ||   // Или позиция огонька выходит за пределы ленты в режиме из центра
            (pos + 1 >= NUM_LEDS)) {                                         // Или с краю
          magnitude(i) = 0;                                                  // Обнуляем магнитуду
          rnd(i) = 0;                                                        // Обнуляем дополнительный бит возраста
        }
      }
    }
  }
}

//------------------------ цветные полоски-----------------------------------------------
void color_bars(uint16_t MAX_AGE) {  // Визуальные пики назначаются одной из 15 цветных полосок.
  uint8_t j, k, oldest;
  float ageScale;
  uint8_t maxWidth = NUM_LEDS / nbars; // ширина полосочек 120 - 15,  180 - 22
  uint8_t width;

  for (uint8_t i = 0; i < N_PEAKS; i++) {
    if ((magnitude(i) > 0) && (age(i) == 0)) {
      // Поиск неиспользуемой полоски.
      j = random8(nbars);
      k = 0;
      oldest = j;
      while (k < nbars) {
        if (colorBars[j] == UNUSED) {
          colorBars[j] = i;
          break;
        } else {
          if (age(colorBars[j]) > age(colorBars[oldest])) oldest = j;
        }
        // Продолжить поиск неиспользуемой полоски.
        if ((i % 2) == 0) {
          j = (j + 1) % nbars;
        } else {
          if (j == 0) j = nbars;
          else        j = j - 1;
        }
        k++;
      }
      if (k == nbars) colorBars[oldest] = i; // Если не найдена неиспользуемая цветовая полоса, то используется самая старая.
    }
  }
  for (uint8_t i = 0; i < nbars; i++) {
    j = colorBars[i];
    if (j == UNUSED) continue;
    if (age(j) == 0)  ageScale = 1.0;       // Если пик новый - сделать его максимально ярким
    else              ageScale = 0.5 * (float)(1.0 - ((float)age(j) / (float)MAX_AGE)); // Иначе в диапазоне [0.0-0.5] в зависимости от возраста.

    uint8_t color_bright = 255.0 * ageScale;
    uint8_t color = baseColor(j);

    if (age(j) == MAX_AGE) colorBars[i] = UNUSED;             // Отметить цветовую полоску как неиспользуемую.

    if (mainSettings.cmu1_mode == COLOR_BARS) {     //для полосок без затухания
      width = maxWidth;
    } else {
      if (age(j) < 5) {                                       // Если новый то ширина максимальная
        width = maxWidth;
      } else {                                                // Затем уменьшаются в зависимости от возраста.
        width = map(age(j) - 5, 0, MAX_AGE - 4, maxWidth, 1);
        width -= width % 2;                                   // Ширина полоски должна быть кратна 2
      }
    }
    //Serial.print(F("color_bright = ")); Serial.println(color_bright);
    //Serial.print(F("color = ")); Serial.println(color);

    for (j = 0; j < (width / 2); j++) {
      // Регулируется яркость в зависимости от того, как далеко светодиод находится от "центра" полоски. Ярче в центре, тусклее скраю.
      color_bright = (float)color_bright * (float)(1.0 - (float)((j * 2) / width)); //
      k = (i * maxWidth) + ((maxWidth / 2) - 1 - j);
      //Serial.print(F("k1 = ")); Serial.println(k);
      //Serial.print(F("color_bright = ")); Serial.println(color_bright);
      leds[k % NUM_LEDS] = CHSV(color, thissat, color_bright);                    //
      k = k + 1 + (j * 2);
      //Serial.print(F("k2 = ")); Serial.println(k);
      leds[k % NUM_LEDS] = CHSV(color, thissat, color_bright);                    //
    }
  }
  for (uint8_t i = 0; i < N_PEAKS; i++) { // Прирост возраста
    if (magnitude(i) > 0) {
      age(i)++;
      if (age(i) > MAX_AGE) magnitude(i) = 0;
    }
  }
}
