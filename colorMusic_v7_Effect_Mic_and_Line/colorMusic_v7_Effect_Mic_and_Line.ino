
/*
  Управление:
  - Однократное нажатие кнопки: смена режима
  - Калибровка шума.
  - Выбор источника сигнала кнопкой: микрофон/линейный вход.
  - Подсветка в режиме тишины.
  Режимы работы (переключаются кнопкой #):
  - VU meter (столбик громкости): от зелёного к красному
  - VU meter (столбик громкости): плавно бегущая радуга
  - VU meter (столбик громкости): огонь
  - Цветомузыка - убежавшие вспышки
  - Цветомузыка - вспышки
  - Светомузыка по частотам: 3 полосы, 4 полосы, 7 полос
  - Светомузыка по частотам: 1 полоса
  - Стробоскоп
  - Подсветка
  - Бегущие частоты
  - анализатор спектра
  - все настройки сохраняются в памяти и не сбрасываются при перезагрузке
  - Сохранение настроек происходит при выключении кнопкой звёздочка (*)
  - А также через 30 секунд после последнего нажатия на любую кнопку ИК пульта

  НАСТРОЙКА НИЖНЕГО ПОРОГА ШУМА
  - Включаем систему
  - Ставим музыку на паузу
  - Жмём кнопку OK 4 раза подряд на пульте
  Значения шумов будут записаны в память и САМИ загружаться при последующем запуске! Всё!

  Настройка своего пульта:
  найти в скетче строку //Serial.println(IRdata, HEX); в функции remoteTick()
  уберите //, компилируете и загружаете скетч, открываете монитор, нажимаете кнопки на пульте и записываете их в скетч в нужное место.

  Пульт:
  - Цифры (0 - 9) активируют режимы
  - Звёздочка (*): включить/выключить систему
  - Кнопка ОК: переключение между локальными и глобальными настройками)
  - Решётка (#): смена подрежима
  - Глобальные настройки (горит красный светодиод на плате):
  + Стрелки ← →: общая яркость горящих светодиодов
  + Стрелки ↑: включить микрофон
  + Стрелки ↓: выключить микрофон
  + 1, 2: +- пусто
  + 4, 5: +- нижний порог шумов режим спектра (SPEKTR_LOW_PASS_line)
  - Локальные настройки (у каждого режима свои):
  1 - Шкала громкости (градиент)
  повтороное нажатие 1 - индикация от краёв
  Стрелки ← →: плавность анимации
  Стрелки ↑ ↓: скорость радуги
  Подрежимы #: зелёный градиент / радуга / огонь
  2 - Цветомузыка - убежавшие вспышки
  Подрежимы #: из середины ленты кааак побегут!
  2.1 - бегут, но сначала ленты
  Стрелки ← →: скорость анимации
  2.2 - полосочки
  Стрелки ← →: скорость анимации
  2.3 - полосочки, стягиваются внутырь
  Стрелки ← →: скорость анимациим
  3 - Цветомузыка - вспышки
  Подрежимы #
  3.1 - Цветомузыка - вспышки убегают
  Стрелки ← →: плавность анимации
  3.2 - Цветомузыка - вспышки убегают радужные
  Стрелки ← →: плавность анимации
  4 - Цветомузыка (3 полосы)
  Подрежимы #:
  4.1 - 4 полосы
  4.2 - 7 полос
  Стрелки ← →: плавность анимации
  5 - Цветомузыка (1 полоса)
  Стрелки ← →: плавность анимации
  6 - Стробоскоп
  Стрелки ← →: плавность вспышек
  Стрелки ↑ ↓: частота вспышек
  Подрежимы # - изменение цвета
  цвет и насыщенность так же меняется в режиме Цветная подсветка. стробоскоп будет такой-же 
  7 - Цветная подсветка
  Подрежимы #:
  7.1 Постоянный: 
  Стрелки ← →: цвет
  Стрелки ↑ ↓: насыщенность
  7.2 - Бегущая радуга
  стрелки ← →: скорость
  стрелки ↑ ↓: шаг радуги
  8 - “Бегущие частоты”
  Подрежимы #:
  8.1 от начала
  Стрелки ← →: скорость
  9 - Анализатор спектра
  Стрелки ← →: шаг цвета
  Стрелки ↑ ↓: цвет
  0 - Цветовые эффекты 10 шт.
  Стрелки ← →: скорость
  Стрелки ↑ ↓: плавность огня

  Общие настройки кнопка OK.
  Стрелки ← →: Общая яркость горящих светодиодов. баг - при яркости больше 60 отваливается "радуга"
  кнопка 0: включение - отключение микрофона
  Кнопка «ОК» 4 раза подряд
  в режимах цветомузыки:   авто настройка нижнего порога шума (линейный выход/микрофон)
  Звёздочка (*) в режиме настройки - сброс и перезагрузка!!!!!!!!!!!!!!!!!!!!
  Кнопка 9 переключает режим тишины (вкл-выкл), который потом запоминается
  и остается при последующих включениях.
**************************************************
  Особая благодарность Slenk. без его доработок не было бы очень интересного режима работы!!!
  А без Sergo_ST - вообще бы ничего не получилось.
*/
// --------------------------- НАСТРОЙКИ ---------------------------

// лента
#define NUM_LEDS 180        // количество светодиодов 180 - 1 метр
const byte SPLIT_NUM_LEDS = NUM_LEDS / 2; // половина ленты
#define CURRENT_LIMIT 2500  // лимит по току в МИЛЛИАМПЕРАХ, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит
#define CORRECTION 0xFFB0D0 // Корректировка оттенка (что бы белый был БЕЛЫМ). Ниже есть стандартные варианты. 0xFFB0D0
/*  Типовые значения:
          TypicalSMD5050      = 0xFFB0F
          Typical8mmPixel     = 0xFFE08C
          TypicalLEDStrip     = 0xFFB0F0 - стоял этот, но на мой взгляд он "синит"
          TypicalPixelString  = 0xFFE08C
          UncorrectedColor    = 0xFFFFFF
*/

// пины
#define SOUND_R_LINE A3    // ЛИНЕЙНЫЙ пин вход аудио, правый канал и микрофон
#define SOUND_L_LINE A2    // ЛИНЕЙНЫЙ пин вход аудио, левый канал

#define BTN_OnOff 13       // Пин кнопка On/Off (PIN --- КНОПКА --- GND)
#define RELAYon 4          // Пин реле (отключение питания с микрофона)

#define LED_PIN 12        // пин DI светодиодной ленты

#define IR_PIN 2          // пин ИК приёмника

#define LED_ON 10         //пин светодиода ON (красный)
#define LED_OK 11        //пин светодиода Off (зеленый)и пин светодиода режима настроек, кнопка OK
#define LED_IR 9          //пин светодиода IR (синий). сопротивление 100 Ом  - так как он только работает на вспышки. на 220 Ом - не будет его видно.
#define LED_line 8        //пин светодиода "ЛИНЕНЫЙ ВХОД" (оранжевый)

//////// MSGEQ7 ////////
#define STROBE_R 5       // вывод STROBE MSGEQ7 правый канал
#define RESET_R  3       //  вывод RESET MSGEQ7 правый канал
#define STROBE_L 6       // вывод STROBE MSGEQ7 левый канал
#define RESET_L  7       //  вывод RESET MSGEQ7 левый канал
int vuMeterBands_R[7];    // переменная для хранения показаний MSGEQ7. по каждой частоте правого канала
int vuMeterBands_L[7];    // переменная для хранения показаний MSGEQ7. по каждой частоте левого канала
int colorMusic[7];    // переменная для хранения показаний MSGEQ7. средняя с двух каналов

// режим тишины
#define SILENCE_TIME 180000   // Время тишины (в миллисекундах)
boolean silence_flag = false;

// нижний порог шумов
#define LOW_PASS_VU_ADD 20     // "добавочная" величина к нижнему порогу, для надёжности (режим VU)

// шкала громкости
#define EXP 1.4              // Степень усиления сигнала (для более "резкой" работы) (по умолчанию 1.4)
#define MAX_COEF 1.8        // коэффициент громкости (максимальное равно срднему * этот коэф) (по умолчанию 1.8)

// режим цветомузыки
//const float MAX_COEF_FREQ_1[7] = {1, 1, 1, 1, 1, 1, 1};  // отдельные коэффициенты для впышек по частотам (0.75, 0.9, 1.05, 1.2, 1.25, 1.35, 1.5)
#define SMOOTH_STEP 20             // шаг уменьшения яркости в режиме цветомузыки (чем больше, тем быстрее гаснет)

#define thissat 255                         // Насыщенность  saturation для подсветки, 7 режим и стробоскоп настраивается
#define EMPTY_BRIGHT 0                      // яркость "не горящих" светодиодов 0
#define EMPTY_COLOR   HUE_PURPLE            // цвет "не горящих" светодиодов. Будет чёрный, если яркость 0
#define LOW_COLOR_0   HUE_RED               // цвет низких частот.      канал 0 
#define LOW_COLOR_1   HUE_ORANGE            // цвет низких-средних.     канал 1
#define MID_COLOR_2   HUE_YELLOW            // цвет средних.            канал 2 
#define MID_COLOR_3   HUE_GREEN             // цвет средних - средних.  канал 3
#define MID_COLOR_4   HUE_AQUA              // цвет средних-высоких.    канал 4
#define HIGH_COLOR_5  HUE_BLUE              // цвет высоких.            канал 5 
#define HIGH_COLOR_6  HUE_PURPLE            // цвет высоких-высоких.    канал 6

//uint32_t freq_color[7] = {0xFF0000, 0xFFFF00, 0x00FF00, 0x#007800, 0xFF00FF, 0x0000FF, 0x9400D3};
// цвета по частотам :      RED       YELLOW    GREEN    DeepGREEN  DeepSkyBlue   BLUE   purple

// режим стробоскопа
//#define STROBE_DUTY 50            // скважность вспышек (1 - 99) - отношение времени вспышки ко времени темноты
#define light_time 20 // 10-100

// режим анализатора спектра
#define LIGHT_SMOOTH 2    // Скорость затухания 2
const float freq_to_stripe = NUM_LEDS / 14; // /2 так как симметрия, и 14 так как 7 частот

// Переменные для работы шкалы громкости
byte peakR = 0;     // Используется для падения точки правый канал
byte peakL = 0;
byte dotCount = 0; // Счетчик кадров для задержки скорости падения точек
boolean CMU_CENTER = true; // false - к центру, true - из центра
#define PEAK_FALL 5 // скорость падения пика 0-10

// режим вспышки
#define P_SPEED 2       // скорость движения


//новые эффекты
const byte nbars = NUM_LEDS / 8;  // колличество полосочек
byte heat[NUM_LEDS + 1]; // массив для подсветки
#define N_PEAKS NUM_LEDS / 5
#define age(x)        (heat[x])
#define magnitude(x)  (heat[x + N_PEAKS])
#define baseColor(x)  (heat[x + N_PEAKS * 2])
#define rnd(x)        (heat[x + N_PEAKS * 3])
#define UNUSED 0
byte colorBars[nbars];
byte vally;
byte magnitude;
#define DANCE_PARTY             0
#define DANCE_PARTY_SINGLE      1
#define COLOR_BARS              2
#define COLOR_BARS2             3


/*  Цвета для HSV
      HUE_RED    = 0
      HUE_ORANGE = 32
      HUE_YELLOW = 64
      HUE_GREEN  = 96
      HUE_AQUA   = 128
      HUE_BLUE   = 160
      HUE_PURPLE = 192
      HUE_PINK   = 224     */
// --------------------------- НАСТРОЙКИ ---------------------------

// ----- КНОПКИ ПУЛЬТА WAVGAT -----
#define BUTT_UP     0xF39EEBAD
#define BUTT_DOWN   0xC089F6AD
#define BUTT_LEFT   0xE25410AD
#define BUTT_RIGHT  0x14CE54AD
#define BUTT_OK     0x297C76AD
#define BUTT_1      0x4E5BA3AD
#define BUTT_2      0xE51CA6AD
#define BUTT_3      0xE207E1AD
#define BUTT_4      0x517068AD
#define BUTT_5      0x1B92DDAD
#define BUTT_6      0xAC2A56AD
#define BUTT_7      0x5484B6AD
#define BUTT_8      0xD22353AD //0xD22353AD
#define BUTT_9      0xDF3F4BAD
#define BUTT_0      0xF08A26AD
#define BUTT_STAR   0x68E456AD
#define BUTT_HASH   0x151CD6AD
//#define BUTT_LINE   0xD22354AD  //Переключение "микрофон" <-> "линейный вход" если на пульте есть дополнительные кнопки

struct Settings { // структура для запоминаемых переменных
  uint8_t this_mode = 0;
  uint8_t freq_strobe_mode = 0;
  uint8_t light_mode = 0;
  uint8_t cmu_mode = 0;
  uint8_t cmu1_mode = 0;
  uint8_t cmu2_mode = 0;
  uint8_t cmu3_mode = 0;
  uint8_t effect_mode = 0;
  byte RAINBOW_STEP = 8;    // шаг изменения цвета радуги 1-30
  byte BRIGHTNESS = 100;    // яркость ленты (0 - 255)
  byte LIGHT_SAT = 255;     // Насыщенность  saturation. Если 0 - цвет будет БЕЛЫЙ при любом цвете (0 - 255)
  byte STROBE_PERIOD = 140; // период вспышек, миллисекунды
  byte STROBE_SMOOTH = 255; // скорость нарастания/угасания вспышки (0 - 255)
  byte FLASH_PERIOD = 20;   // период вспышек, миллисекунды - режим бегущих вспышек
  byte HUE_STEP = 10;       // Шаг цвета
  byte HUE_START = 0;       // начальный цвет огня (0 красный, 80 зелёный, 140 молния, 190 розовый)
  byte COLOR_SPEED = 140;   // Скорость смены цветов - настройки радуги
  byte LIGHT_COLOR = 0;     // начальный цвет подсветки и цвет стробоскопа 0
  byte RUNNING_SPEED = 20;  // режим бегущих частот
  float SMOOTH = 0.5;       // коэффициент плавности анимации VU (по умолчанию 0.5)
  float SMOOTH_K = 0.15;    // коэффициент плавности огня (0.15) 0.8
  float SMOOTH_FREQ = 0.8;  // коэффициент плавности анимации частот (по умолчанию 0.8)
  // сохранить значения шумов из памяти
  uint16_t VU_LOW_PASS_line_R = 100; // нижний порог шумов правый канал режим VU
  uint16_t VU_LOW_PASS_line_L = 100; // нижний порог шумов левый канал режим VU
  boolean SILENCE = true;      // Отслеживание тишины (включение подсветки после n секунд тишины)
} mainSettings;

#include "EEPROM.h"
#include <util/delay.h>
#include "FastLED.h"
CRGB leds[NUM_LEDS]; // Объявить ленту

#include "IRLremote.h" // Подключить библиотеку для работы с ИК пультом
CHashIR IRLremote; // Объявить ИК управление
uint32_t IRdata; // Переменная для ИК команд

// градиент-палитра от зелёного к красному //https://www.color-hex.com https://colorscheme.ru/color-converter.html
// https://colorscheme.ru/html-colors.html
// градиент-палитра от зелёного к красному
const TProgmemRGBPalette16 DEFINE_GRADIENT_PALETTE PROGMEM =
{
  CRGB::Green,
  CRGB::Green,
  CRGB::Yellow,
  CRGB::Orange,
  //CRGB::OrangeRed,
  CRGB::Red
};
CRGBPalette16 myPal = DEFINE_GRADIENT_PALETTE;

byte Rlenght, Llenght;
int maxLevel;
float averageLevel;
byte hue;
unsigned long main_timer, eeprom_timer, effect_timer, main_timer_dance, silence_timer, MSGEQ7_timer;
const float averK = 0.006;
float colorMusic_f[7], colorMusic_aver[7];
boolean colorMusicFlash[7], strobeUp_flag, strobeDwn_flag;
byte strobe_bright, thisBright[7];
volatile boolean ir_flag;
boolean settings_mode = false;
boolean ONstate = false;        // true - включено при подаче питания, false - выключено
boolean line_mode = false;      // true - включен микрофон, false - выключен микрофон
float freq_max_f_R, freq_max_f_L;
byte freq_f_R[7], freq_f_L[7];
byte this_color, rainbow_steps;
boolean running_flag[7], eeprom_flag;
byte effect_delay = 20;    //20 задержка
bool timer_func (int val = effect_delay);
byte this_bright;
byte HUEindex;

#include "colorMusic_v7_Effect_Mic_and_Line.h" //Набор различных эффектов
byte fullLowPassSet = 0;          //Счетчик нажатия клавиши ОК на пульте. При 4 нажатиях подряд вызвать fullLowPass

// используется в remoteTick
// вспомогательная функция, изменяет величину value на шаг incr в пределах minimum. maximum
int smartIncr(int value, int incr_step, int mininmum, int maximum) {
  int val_buf = value + incr_step;
  val_buf = constrain(val_buf, mininmum, maximum);
  return val_buf;
}

float smartIncrFloat(float value, float incr_step, float mininmum, float maximum) {
  float val_buf = value + incr_step;
  val_buf = constrain(val_buf, mininmum, maximum);
  return val_buf;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool timer_func (int val = effect_delay) {    // Таймер общий
  if (millis() - effect_timer > val) {
    effect_timer = millis();
    return  true;
  }
  else return false;
}

byte getBrightCRT(byte val) { // для 8 бит ШИМ
  return ((long)val * val * val + 130305) >> 16;
}

void autoLowPass() {
  uint16_t thisMax_L = 0;
  uint16_t thisMax_R = 0;
  uint16_t thisLevel_R = 0;
  uint16_t thisLevel_L = 0;

  for (byte j = 0; j < 100; j++) {  // 100 измерений
    analyzeMSGEQ7();                  //опрашиваем MSGEQ7

    for (byte i = 0; i < 7; i++) {
      thisMax_L = max(thisMax_L, vuMeterBands_L[i]); // теперь ищем максимумы на левом канале и запоминаем.
    }
    if (thisMax_L > thisLevel_L) thisLevel_L = thisMax_L;

    for (byte i = 0; i < 7; i++) {
      thisMax_R = max(thisMax_R, vuMeterBands_R[i]); // ищем максимумы на правом канале и запоминаем
    }
    if (thisMax_R > thisLevel_R) thisLevel_R = thisMax_R;
  }
  mainSettings.VU_LOW_PASS_line_R = thisLevel_R + LOW_PASS_VU_ADD;    // нижний порог как максимум тишины + некая величина для VU
  mainSettings.VU_LOW_PASS_line_L = thisLevel_L + LOW_PASS_VU_ADD;    // нижний порог как максимум тишины + некая величина для VU

  updateEEPROM();
}

void analyzeMSGEQ7() {
  if (millis() - MSGEQ7_timer > 50) { //  20 задержка измерения
    MSGEQ7_timer = millis();            // сбросить таймер

    PORTD |= ( 1 << PD3) | ( 1 << PD7); // reset в 1
    PORTD |= ( 1 << PD5) | ( 1 << PD6); // strobe в 1
    _delay_us(1);
    PORTD &= ~(( 1 << PD3) | ( 1 << PD7)); // reset в 0
    _delay_us(74);
    for (byte i = 0; i < 7; i++) {     // 7 раз, для каждого канала
      PORTD &= ~(( 1 << PD5) | ( 1 << PD6)); // strobe в 0
      _delay_us(40);
      ADCSRA |= (1 << ADIF);      // сброс флага окончания замера
      ADCSRA |= (1 << ADSC);      // запуск преобразования
      while ((ADCSRA & (1 << ADIF)) == 0); // ждем окончания измерения
      vuMeterBands_L[i] = (ADCL >> 6) | (ADCH << 2);  // считываем данные ADC2 (A2) 0-1023// в setup настроенно сразу на A2!!
      //vuMeterBands_L[i] = ADCH;   // считываем данные ADC2 (A2) 0-255
      ADMUX |= (1 << MUX0);      // присваиваем биту mux0 - 1, переключаем на ADC3 (А3)
      ADCSRA |= (1 << ADIF);      // сброс флага окончания замера
      ADCSRA |= (1 << ADSC);      // запуск преобразования
      while ((ADCSRA & (1 << ADIF)) == 0); // ждем окончания измерения
      vuMeterBands_R[i] = (ADCL >> 6) | (ADCH << 2);  // считываем данные A3 0-1023
      //vuMeterBands_R[i] = ADCH;   // считываем данные A3 0-255
      ADMUX &= ~ (1 << MUX0);       //присваиваем биту mux0 - 0 переключаем на ADC2 (А2)
      if (line_mode == true) vuMeterBands_R[i] = vuMeterBands_L[i];
      PORTD |= ( 1 << PD5) | ( 1 << PD6); // strobe в 1
      _delay_us(40);

      //digitalWrite(RESET_R, HIGH);         // reset в 1
      //digitalWrite(RESET_L, HIGH);        // reset в 1
      //_delay_us(1);               // ждем 1 мкс (min 0.1)
      //digitalWrite(RESET_R, LOW);         // reset в 0
      //digitalWrite(RESET_L, LOW);         // reset в 0
      //_delay_us(74);               // ждем 1 мкс (min 72)
      //for (byte i = 0; i < 7; i++) {     // 7 раз, для каждого канала
      //digitalWrite(STROBE_R, LOW);      // strobe в 0 - запуск MSGEQ7 на измерение канала
      //digitalWrite(STROBE_L, LOW);      // strobe в 0 - запуск MSGEQ7 на измерение канала
      //_delay_us(40);  // ждем 40 мкс (min 36)
      //vuMeterBands_R[i] = analogRead(SOUND_R_LINE);
      //vuMeterBands_L[i] = analogRead(SOUND_L_LINE);
      //if (line_mode == true) vuMeterBands_L[i] = vuMeterBands_R[i];
      //digitalWrite(STROBE_R, HIGH);   // strobe в 1 - перевод MSGEQ7 на следующий канал
      //digitalWrite(STROBE_L, HIGH);   // strobe в 1 - перевод MSGEQ7 на следующий канал
      //_delay_us(40);  // ждем 40 мкс (min 36)
    }
    /*
      Serial.print (F("vuMeterBands_L =  "));
      for (int i = 0; i < 7; i++) {
      Serial.print (F(vuMeterBands_L[i]));
      Serial.print (F("  "));
      }
      //Serial.println ();
      Serial.print (F("vuMeterBands_R =  "));
      for (int i = 0; i < 7; i++) {
      Serial.print (F(vuMeterBands_R[i]));
      Serial.print (F("  "));
      }
      Serial.println ();
    */
  }
}
/*
  void  print() {
  // Вывод для тестирования
  Serial.print(F("this_mode = ")); Serial.println(mainSettings.this_mode);
  Serial.print(F("freq_strobe_mode = ")); Serial.println(mainSettings.freq_strobe_mode);
  Serial.print(F("light_mode = ")); Serial.println(mainSettings.light_mode);
  Serial.print(F("cmu_mode = ")); Serial.println(mainSettings.cmu_mode);
  Serial.print(F("cmu1_mode = ")); Serial.println(mainSettings.cmu1_mode);
  Serial.print(F("cmu2_mode = ")); Serial.println(mainSettings.cmu2_mode);
  Serial.print(F("cmu3_mode = ")); Serial.println(mainSettings.cmu3_mode);
  Serial.print(F("RAINBOW_STEP = ")); Serial.println(mainSettings.RAINBOW_STEP);
  Serial.print(F("BRIGHTNESS = ")); Serial.println(mainSettings.BRIGHTNESS);
  Serial.print(F("LIGHT_SAT = ")); Serial.println(mainSettings.LIGHT_SAT);
  Serial.print(F("STROBE_PERIOD = ")); Serial.println(mainSettings.STROBE_PERIOD);
  Serial.print(F("STROBE_SMOOTH = ")); Serial.println(mainSettings.STROBE_SMOOTH);
  Serial.print(F("FLASH_PERIOD = ")); Serial.println(mainSettings.FLASH_PERIOD);
  Serial.print(F("HUE_STEP = ")); Serial.println(mainSettings.HUE_STEP);
  Serial.print(F("COLOR_SPEED = ")); Serial.println(mainSettings.COLOR_SPEED);
  Serial.print(F("LIGHT_COLOR = ")); Serial.println(mainSettings.LIGHT_COLOR);
  Serial.print(F("RUNNING_SPEED = ")); Serial.println(mainSettings.RUNNING_SPEED);
  Serial.print(F("effect_mode = ")); Serial.println(mainSettings.effect_mode);
  Serial.print(F("HUE_START = ")); Serial.println(mainSettings.HUE_START);
  Serial.print(F("SMOOTH = ")); Serial.println(mainSettings.SMOOTH);
  Serial.print(F("SMOOTH_K = ")); Serial.println(mainSettings.SMOOTH_K);
  Serial.print(F("SMOOTH_FREQ = ")); Serial.println(mainSettings.SMOOTH_FREQ);
  Serial.print(F("VU_LOW_PASS_line_R = ")); Serial.println(mainSettings.VU_LOW_PASS_line_R);
  Serial.print(F("VU_LOW_PASS_line_L = ")); Serial.println(mainSettings.VU_LOW_PASS_line_L);
  //Serial.print(F("settings_mode = ")); Serial.println(settings_mode);
  //Serial.print(F("line_mode = ")); Serial.println(line_mode);
  //Serial.print(F("ONstate = ")); Serial.println(ONstate);
  //Serial.print(F("effect_delay = ")); Serial.println(effect_delay);
  //Serial.print(F("eeprom_flag = ")); Serial.println(eeprom_flag);
  //Serial.print(F("Rlenght = ")); Serial.println(Rlenght);
  //Serial.print(F("Llenght = ")); Serial.println(Llenght);

  }
*/
void updateEEPROM() {
  //Serial.println(F("updateEEPROM"));
  EEPROM_UpdateBlock(1, &mainSettings, sizeof(mainSettings));
}

void readEEPROM() {
  //Serial.println(F("readEEPROM"));
  EEPROM_ReadBlock(1, &mainSettings, sizeof(mainSettings));
}

void eepromTick() { // сохранение в память при бездействии 30 с.
  if (eeprom_flag)
    if (millis() - eeprom_timer > 30000) {    // 30 секунд после последнего нажатия с пульта
      eeprom_flag = false;
      updateEEPROM();
      //Serial.print(F("Настройки сохранены 30с eeprom_flag = false"));
    }
}

void silence() { // в режиме тишины запуск подсветки
  if (millis() - silence_timer > SILENCE_TIME) {
    if (silence_flag == false) {
      silence_flag = true;
      //Serial.println(F("test1"));
    }
  } else {
    if (silence_flag) {
      silence_flag = false;
      //Serial.println(F("test2"));
    }
  }
}

//-----------------------------------сброс массива--------------------------------------
void reset_arr() {
  memset(heat, 0, NUM_LEDS);
  memset(colorBars, 0, nbars);
}
