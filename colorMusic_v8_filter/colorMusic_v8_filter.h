// огненные эффекты
#define HUE_GAP 21        // заброс по hue
#define FIRE_STEP 20      // шаг огня
#define HUE_COEF 0.7    // коэффициент цвета огня (чем больше - тем дальше заброс по цвету)
#define MIN_BRIGHT_P 70   // мин. яркость огня (90)
#define MAX_BRIGHT_P 250  // макс. яркость огня
#define MIN_SAT 200       // мин. насыщенность
#define MAX_SAT 255       // макс. насыщенность
#define ZONE_AMOUNT 6  // количество зон
#define ZONE_K 0.08     // плавность движения зон (0.15)
#define ZONE_MAX_SHIFT 10
#define ZONE_SIZE NUM_LEDS / ZONE_AMOUNT
#define FOR_i(from, to) for(int i = (from); i < (to); i++)
#define FOR_j(from, to) for(int j = (from); j < (to); j++)
uint16_t idex = 0;                //-LED INDEX (0 to NUM_LEDS-1)
uint8_t ihue = 0;                //-HUE (0-255)
float zoneValues[ZONE_AMOUNT];
uint8_t zoneRndValues[ZONE_AMOUNT];
float zoneShift[ZONE_AMOUNT - 1];
int8_t zoneRND[ZONE_AMOUNT - 1];
unsigned long prevTime, prevTime2;
uint16_t counter = 0;


//------------------------LED EFFECT FUNCTIONS------------------------
// возвращает цвет огня для одного пикселя
CRGB getFireColor(uint16_t val_f) {
  // чем больше val_f, тем сильнее сдвигается цвет, падает насыщеность и растёт яркость
  return CHSV(
           map(val_f, 0, 255, 0, HUE_GAP),                                    // H
           constrain(map(val_f, 0, 255, MAX_SAT, MIN_SAT), 0, 255),           // S
           constrain(map(val_f, 0, 255, MIN_BRIGHT_P, MAX_BRIGHT_P), 0, 255)  // V
         );
}

// Функция делает всю ленту выбранным цветом
void one_color_all(uint8_t all_color = 0, uint8_t all_sat = 0, uint8_t all_bright = 0) {
  for (uint16_t i = 0 ; i < NUM_LEDS; i++ )  leds[i].setHSV(all_color, all_sat, all_bright);
}

void setPixelHeatColor (uint16_t Pixel, uint8_t heatramp) {

  if      (heatramp > 150)  leds[Pixel] = CHSV((random8(13, 18)), random8(225, 255), random8(240, 255)); // Горячо
  else if (heatramp > 75)   leds[Pixel] = CHSV((random8(7, 13)), random8(225, 255), random8(195, 240));  // Средне
  else if (heatramp > 0)    leds[Pixel] = CHSV((random8(0, 7)), random8(225, 255), heatramp * 3);        // Холодно
  else                      leds[Pixel] = CHSV(0, 0, 0);                                                                       // off
}
//============================================================================================================



// --- огонь из центра -------------------------------------
void Fire(uint16_t Cooling, uint16_t Sparking) {
  if (timer_func(mainSettings.COLOR_SPEED - 100)) {
    // Шаг 1.  Каждный пиксел немного остывает
    for ( int16_t i = 0; i < SPLIT_NUM_LEDS; i++) {
      heat[i] = qsub8(heat[i],  random8(0, ((Cooling * 10) / SPLIT_NUM_LEDS) + 2));
    }
    // Step 2.  Тепло от каждого пиксела дрейфует "вверх" и немного рассеивается
    for ( int16_t k = (SPLIT_NUM_LEDS) - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
      heat[NUM_LEDS - k] = (heat[NUM_LEDS - k + 1] + heat[NUM_LEDS - k + 2] + heat[NUM_LEDS - k + 2]) / 3;
    }
    // Шаг 3.  Беспорядочное зажигание новых "искр" у самого дна
    if ( random8() < Sparking ) {
      byte y = random8(7);
      heat[y] = random8(160, 255);
      //heat[y] = heat[y] + random8(160, 255);     
    }
    if ( random8() < Sparking ) {
      byte y = random8(7);
      heat[NUM_LEDS - y] = random8(160, 255);
    }
    // Шаг 4.  Конвертировать тепло в цвет
    for ( int16_t j = 0; j < SPLIT_NUM_LEDS; j++) {
      setPixelHeatColor(SPLIT_NUM_LEDS + j, heat[j]);
      setPixelHeatColor(SPLIT_NUM_LEDS - 1 - j, heat[j]);
    }
  }
}
// --- Плавная смена цвета всей ленты -----------------------------------------
void rainbow_fade() {
  if (timer_func(mainSettings.COLOR_SPEED)) {
    one_color_all(this_color++, thissat, 255);
  }
}

// --- безумие случайных вспышек -----------------------------------------------------------
void random_color_pop(int8_t amount_points, int8_t flag_clear, int8_t min_bright) {
  if (timer_func(mainSettings.COLOR_SPEED)) {
    if (flag_clear == 1) one_color_all();
    for (int8_t i = 0; i < amount_points; i++) {
      leds[random16(0, NUM_LEDS)] = CHSV(random8(), thissat, random8(min_bright, 255)); //color, ,bright
    }
  }
}
//============================================================================================================

// --- Конфетти - случайные цветные крапинки мигают и плавно исчезают ----------------------------------------
void confetti() {
  if (timer_func())  this_color++;
  fadeToBlackBy( leds, NUM_LEDS, 10);
  idex = random16(NUM_LEDS);
  leds[idex] += CHSV( this_color + random8(64), thissat, 255);
}
//============================================================================================================


// --- случайная вспышка и затухание полосок ----------------------------------------------------------
#define NM 8 //для экономии памяти, или меньше делитель на свой страх и риск!!! Чем меньше - тем больше огоньков (ставить не менее 2)
//цвет heat[SPLIT_NUM_LEDS + idex]
//яркость heat[idex]

void brights_dec() {
  if (timer_func()) {
    for (int8_t i = 0; i < NUM_LEDS / NM; i++) {
      if (heat[i] == 0) {
        do {  // т.к. точно есть один пустой пиксель, то ищем один из "пустых" пикселей рандомно...
          idex = random8(NUM_LEDS / NM);
        } while (heat[i] != 0);
        heat[SPLIT_NUM_LEDS + idex] = random8(); heat[idex] = random8(100, 255); // ...и зажигаем его.
        for (int8_t b = 0; b < NM; b++) {
          leds[idex * NM + b] = CHSV(heat[SPLIT_NUM_LEDS + idex], thissat, heat[idex]);
        }
      }
    }
    for (int8_t i = 0; i < NUM_LEDS / NM; i++) { //уменьшаем яркость всех горящих пикселей на 1
      if (heat[i] != 0) {
        heat[i] = heat[i] - 1;
        for (int8_t b = 0; b < NM; b++) leds[i * NM + b] = CHSV(heat[SPLIT_NUM_LEDS + i], thissat, heat[i]);
      }
    }
  }
}
// --- плавный огонь на шуме Перлина -----------------------------------------------------------
void fireTick() {
  // двигаем пламя
  if (millis() - prevTime > 100) {
    prevTime = millis();
    FOR_i(0, NUM_LEDS) {
      leds[i] = getFireColor((inoise8(i * FIRE_STEP, counter)));
    }
    counter += 20;
  }
}

// --- огонь с фиксированным количеством зон -----------------------------------------------------------
void randomizeZones() {
  // задаёт направление движения зон
  if (millis() - prevTime >= 800) {
    prevTime = millis();
    FOR_i(0, ZONE_AMOUNT - 1) {
      if (abs(zoneShift[i] - zoneRND[i]) < 2)
        zoneRND[i] = random(-ZONE_MAX_SHIFT, ZONE_MAX_SHIFT);
    }
  }

  // движение зон
  if (millis() - prevTime2 >= 50) { //50
    prevTime2 = millis();
    FOR_i(0, ZONE_AMOUNT - 1) {
      zoneShift[i] += (zoneRND[i] - zoneShift[i]) * ZONE_K;
    }
  }
}

void fireTick2() {
  // задаём направление движения огня
  if (millis() - prevTime > 100) {
    prevTime = millis();
    FOR_i(0, ZONE_AMOUNT) {
      zoneRndValues[i] = random(0, 10);
    }
  }

  // двигаем пламя
  if (millis() - prevTime2 > 20) { //20
    prevTime2 = millis();
    int16_t thisPos = 0, lastPos = 0;
    FOR_i(0, ZONE_AMOUNT) {
      zoneValues[i] = zoneValues[i] * (1 - mainSettings.SMOOTH_K) + zoneRndValues[i] * 10 * mainSettings.SMOOTH_K;
      if (i < ZONE_AMOUNT - 1) thisPos += ZONE_SIZE + zoneShift[i];
      else thisPos = NUM_LEDS;
      if (thisPos > NUM_LEDS) break;

      // вывести огонь
      int8_t zoneHalf = (thisPos - lastPos) / 2;
      float valStep = (float)1 / zoneHalf;
      byte count = 0;
      FOR_j(lastPos, lastPos + zoneHalf) {
        count++;
        if (j > NUM_LEDS) break;
        leds[j] = getFireColor(zoneValues[i] * count * valStep);
      }
      count = 0;
      FOR_j(lastPos + zoneHalf, thisPos) {
        count++;
        if (j > NUM_LEDS) break;
        leds[j] = getFireColor(zoneValues[i] * (zoneHalf - count) * valStep);
      }

      // вывести цветные полосы
      //FOR_j(lastPos, thisPos) leds[j] = CHSV(i * 70, 255, 150); // для тестирования, но эффект красивый. :)

      lastPos = thisPos;
    }
  }
}

void addGlitter(byte chanceOfGlitter) {   // искры для радуги
  if (random8() < chanceOfGlitter) leds[random16(NUM_LEDS)] += CRGB::White;
}

// ======================================== ВЫЗОВ ЭФФЕКТОВ ===================================================
void new_effect() {
  switch (mainSettings.effect_mode) {
    case 0: Fire(55, 120);  break;                    //1 огонь из центра
    case 1: random_color_pop(1, 0, 250);  break;      //2 случайная смена цветов точки... их все больше и больше. Максимальная нагрузка на БП
    case 2: rainbow_fade(); break;                    //3 Плавная смена цвета всей ленты
    case 3: rainbow_fade(); addGlitter(30); break;    //4 Плавная смена цвета всей ленты с глиттером
    case 4: random_color_pop(1, 1, 0);  break;        //5 безумие случайных вспышек
    case 5: random_color_pop(7, 1, 20); break;        //6 безумие случайных вспышек 2 / кол-во точек, выключение - включение, ограничение яркости
    case 6: brights_dec();  break;                    //7 случайная вспышка и затухание полосок
    case 7: fireTick(); break;                        //8 плавный огонь на шуме Перлина
    case 8: fireTick2(); randomizeZones(); break;     //9 огонь с фиксированным количеством зон
    case 9: confetti();  break;                       //10 конфети
  }
}
