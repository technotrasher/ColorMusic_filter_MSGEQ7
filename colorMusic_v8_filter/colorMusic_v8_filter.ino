/* изм. 26.02.2023 */
/*
  Управление:
  - Однократное нажатие кнопки: смена режима
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
  + Стрелки ↑ ↓: общий таймер effect_delay
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
  Стрелки ↑ ↓: общий таймер effect_delay
  Звёздочка (*) в режиме настройки - сброс и перезагрузка!!!!!!!!!!!!!!!!!!!!
  Кнопка 9 переключает режим тишины (вкл-выкл), который потом запоминается
  и остается при последующих включениях.
**************************************************
  Особая благодарность Slenk. без его доработок не было бы очень интересного режима работы!!!
  А без Sergo_ST - вообще бы ничего не получилось.
*/
// --------------------------- НАСТРОЙКИ ---------------------------

// лента
#define NUM_LEDS 180                       // количество светодиодов 180 - 1 метр
const byte SPLIT_NUM_LEDS = NUM_LEDS / 2;  // половина ленты
#define CURRENT_LIMIT 2500                 // лимит по току в МИЛЛИАМПЕРАХ, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит
#define CORRECTION 0xFFB0D0                // Корректировка оттенка (что бы белый был БЕЛЫМ). Ниже есть стандартные варианты. 0xFFB0D0
/*  Типовые значения:
          TypicalSMD5050      = 0xFFB0F
          Typical8mmPixel     = 0xFFE08C
          TypicalLEDStrip     = 0xFFB0F0 - стоял этот, но на мой взгляд он "синит"
          TypicalPixelString  = 0xFFE08C
          UncorrectedColor    = 0xFFFFFF
*/

// пины
#define LED_line 9    //пин светодиода "ЛИНЕНЫЙ ВХОД" (оранжевый) PB1
#define LED_IR 10     //пин светодиода IR (синий). PB2
#define LED_ON 11     //пин светодиода ON (зеленый) PB3
#define LED_OK 12     //пин светодиода OK (белый) PB4
#define BTN_OnOff 13  // Пин кнопка On/Off (PIN --- КНОПКА --- GND)
#define IR_PIN 2      // пин ИК приёмника PD2
#define LED_PIN 3     // пин DI светодиодной ленты PD3

#define SOUND_1_LINE A0   // ЛИНЕЙНЫЙ пин вход аудио 1 канал 60Гц
#define SOUND_2_LINE A1   // ЛИНЕЙНЫЙ пин вход аудио 2 канал 150Гц
#define SOUND_3_LINE A2   // ЛИНЕЙНЫЙ пин вход аудио 3 канал 400Гц
#define SOUND_4_LINE A3   // ЛИНЕЙНЫЙ пин вход аудио 4 канал 1кГц
#define SOUND_5_LINE A4   // ЛИНЕЙНЫЙ пин вход аудио 5 канал 2,5кГц
#define SOUND_6_LINE A5   // ЛИНЕЙНЫЙ пин вход аудио 6 канал 6кГц
#define SOUND_7_LINE A6   // ЛИНЕЙНЫЙ пин вход аудио 7 канал 15кГц
#define LEVEL        A7   // фиксированное напряжение 0,16В

uint16_t vuMeterBands_R[7];  // переменная для хранения показаний по каждой частоте правого канала
uint16_t vuMeterBands_L[7];  // переменная для хранения показаний по каждой частоте левого канала

/* ---- изменение для прерываний ADC ---- */
#define CHPORT          PORTD           // порт пинов (4-6)
#define REF_FORCE_LOW     PD7           // пин ручной корректировки уровня AREF (7)
                                        // норма = HIGH, подача импульса уменьшает референсное напряжение
                                        // (не действует, если референсное напряжение = 1В)
#define FORCE_LOW_PORT  PORTD           // порт пина (7)
#define FORCE_LOW_SETUP  DDRD           // регистр управления пина (7)

#define REF_FORCE_HIGH    PB0           // PB0 норма = LOW, подача импульса увеличивает референсное напряжение
                                        // (не действует, если референсное напряжение выше 4,5В) (8)
#define FORCE_HIGH_PORT PORTB           // порт пина (8)
#define FORCE_HIGH_SETUP DDRB           // регистр управления пина (8)

#define MIC               PD4           // PD4 ключ подключения микрофона
#define KEY_R             PD5           // PD5 ключ правого канала
#define KEY_L             PD6           // PD6 ключ левого канала
//#define REF_FORCE_LOW 7   // PD7 норма = HIGH, подача импульса уменьшает референсное напряжение (не действует, если референсное напряжение = 1В)
//#define REF_FORCE_HIGH 8  // PB0 норма = LOW, подача импульса увеличивает референсное напряжение (не действует, если референсное напряжение выше 4,5В)

#include "classes.h"                    // реализация основных классов
result *channels[] = { new result(KEY_L), new result(KEY_R), new result(MIC) };
/* переменные, используемые в прерывании */
volatile byte a_channel   = 0;          // начинаем измерять с этого канала в массиве channels
volatile uint16_t *buffer = channels[0]->getBands();


#include "interrupts.h"                 // обработчики прерываний
/* -------------------------------------- */

uint16_t colorMusic[7];      // переменная для хранения показаний средняя с двух каналов

// режим тишины
#define SILENCE_TIME 180000  // Время тишины (в миллисекундах)
boolean silence_flag = false;

// шкала громкости
#define MAX_COEF 1.8  // коэффициент громкости (максимальное равно срднему * этот коэф) (по умолчанию 1.8)

// режим цветомузыки
//const float MAX_COEF_FREQ_1[7] = {1, 1, 1, 1, 1, 1, 1};  // отдельные коэффициенты для впышек по частотам (0.75, 0.9, 1.05, 1.2, 1.25, 1.35, 1.5)
#define SMOOTH_STEP 20  // шаг уменьшения яркости в режиме цветомузыки (чем больше, тем быстрее гаснет)

#define thissat 255              // Насыщенность  saturation для подсветки, 7 режим и стробоскоп настраивается
#define EMPTY_BRIGHT 0           // яркость "не горящих" светодиодов 0
#define EMPTY_COLOR HUE_PURPLE   // цвет "не горящих" светодиодов. Будет чёрный, если яркость 0
#define LOW_COLOR_0 HUE_RED      // цвет низких частот.      канал 0
#define LOW_COLOR_1 HUE_ORANGE   // цвет низких-средних.     канал 1
#define MID_COLOR_2 HUE_YELLOW   // цвет средних.            канал 2
#define MID_COLOR_3 HUE_GREEN    // цвет средних - средних.  канал 3
#define MID_COLOR_4 HUE_AQUA     // цвет средних-высоких.    канал 4
#define HIGH_COLOR_5 HUE_BLUE    // цвет высоких.            канал 5
#define HIGH_COLOR_6 HUE_PURPLE  // цвет высоких-высоких.    канал 6

//uint32_t freq_color[7] = {0xFF0000, 0xFFFF00, 0x00FF00, 0x#007800, 0xFF00FF, 0x0000FF, 0x9400D3};
// цвета по частотам :      RED       YELLOW    GREEN    DeepGREEN  DeepSkyBlue   BLUE   purple

// режим стробоскопа
//#define STROBE_DUTY 50            // скважность вспышек (1 - 99) - отношение времени вспышки ко времени темноты
#define light_time 20  // 10-100

// режим анализатора спектра
#define LIGHT_SMOOTH 2                       // Скорость затухания 2
const float freq_to_stripe = NUM_LEDS / 14;  // /2 так как симметрия, и 14 так как 7 частот

// Переменные для работы шкалы громкости
uint8_t peakR = 0;  // Используется для падения точки правый канал
uint8_t peakL = 0;
uint8_t dotCount = 0;    // Счетчик кадров для задержки скорости падения точек
bool CMU_CENTER = true;  // false - к центру, true - из центра
#define PEAK_FALL 5      // скорость падения пика 0-10

// режим вспышки
#define P_SPEED 2  // скорость движения

//новые эффекты
const uint8_t nbars = NUM_LEDS / 8;  // колличество полосочек
uint8_t heat[NUM_LEDS + 1];          // массив для подсветки
#define N_PEAKS NUM_LEDS / 5
#define age(x) (heat[x])
#define magnitude(x) (heat[x + N_PEAKS])
#define baseColor(x) (heat[x + N_PEAKS * 2])
#define rnd(x) (heat[x + N_PEAKS * 3])
#define UNUSED 0
uint8_t colorBars[nbars];
uint8_t vally;
uint8_t magnitude;
#define DANCE_PARTY 0
#define DANCE_PARTY_SINGLE 1
#define COLOR_BARS 2
#define COLOR_BARS2 3


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
#define BUTT_UP 0xF39EEBAD
#define BUTT_DOWN 0xC089F6AD
#define BUTT_LEFT 0xE25410AD
#define BUTT_RIGHT 0x14CE54AD
#define BUTT_OK 0x297C76AD
#define BUTT_1 0x4E5BA3AD
#define BUTT_2 0xE51CA6AD
#define BUTT_3 0xE207E1AD
#define BUTT_4 0x517068AD
#define BUTT_5 0x1B92DDAD
#define BUTT_6 0xAC2A56AD
#define BUTT_7 0x5484B6AD
#define BUTT_8 0xD22353AD  //0xD22353AD
#define BUTT_9 0xDF3F4BAD
#define BUTT_0 0xF08A26AD
#define BUTT_STAR 0x68E456AD
#define BUTT_HASH 0x151CD6AD
//#define BUTT_LINE   0xD22354AD  //Переключение "микрофон" <-> "линейный вход" если на пульте есть дополнительные кнопки

#include "EEPROM.h"
#include <util/delay.h>
#include "FastLED.h"
CRGB leds[NUM_LEDS];  // Объявить ленту

#include "IRLremote.h"  // Подключить библиотеку для работы с ИК пультом
CHashIR IRLremote;      // Объявить ИК управление
uint32_t IRdata;        // Переменная для ИК команд

struct Settings {  // структура для запоминаемых переменных
  uint8_t this_mode = 0;
  uint8_t freq_strobe_mode = 0;
  uint8_t light_mode = 0;
  uint8_t cmu_mode = 0;
  uint8_t cmu1_mode = 0;
  uint8_t cmu2_mode = 0;
  uint8_t cmu3_mode = 0;
  uint8_t effect_mode = 0;
  uint8_t RAINBOW_STEP = 8;     // шаг изменения цвета радуги 1-30
  uint8_t BRIGHTNESS = 100;     // яркость ленты (0 - 255)
  uint8_t LIGHT_SAT = 255;      // Насыщенность  saturation. Если 0 - цвет будет БЕЛЫЙ при любом цвете (0 - 255)
  uint8_t STROBE_PERIOD = 140;  // период вспышек, миллисекунды
  uint8_t STROBE_SMOOTH = 255;  // скорость нарастания/угасания вспышки (0 - 255)
  uint8_t FLASH_PERIOD = 20;    // период вспышек, миллисекунды - режим бегущих вспышек
  uint8_t HUE_STEP = 10;        // Шаг цвета
  uint8_t HUE_START = 0;        // начальный цвет огня (0 красный, 80 зелёный, 140 молния, 190 розовый)
  uint8_t COLOR_SPEED = 140;    // Скорость смены цветов - настройки радуги
  uint8_t LIGHT_COLOR = 0;      // начальный цвет подсветки и цвет стробоскопа 0
  uint8_t RUNNING_SPEED = 20;   // режим бегущих частот
  float SMOOTH = 0.5;           // коэффициент плавности анимации VU (по умолчанию 0.5)
  float SMOOTH_K = 0.15;        // коэффициент плавности огня (0.15) 0.8
  float SMOOTH_FREQ = 0.8;      // коэффициент плавности анимации частот (по умолчанию 0.8)
  bool SILENCE = true;          // Отслеживание тишины (включение подсветки после n секунд тишины)
} mainSettings;

// градиент-палитра от зелёного к красному //https://www.color-hex.com https://colorscheme.ru/color-converter.html
// https://colorscheme.ru/html-colors.html
// градиент-палитра от зелёного к красному
const TProgmemRGBPalette16 DEFINE_GRADIENT_PALETTE PROGMEM = {
  CRGB::Green,
  CRGB::Green,
  CRGB::Yellow,
  CRGB::Orange,
  //CRGB::OrangeRed,
  CRGB::Red
};
CRGBPalette16 myPal = DEFINE_GRADIENT_PALETTE;

uint8_t Rlenght, Llenght;
uint16_t maxLevel;
float averageLevel;
uint8_t hue;
unsigned long main_timer, eeprom_timer, effect_timer, main_timer_dance, silence_timer;
const float averK = 0.006;
float colorMusic_f[7], colorMusic_aver[7];
bool colorMusicFlash[7], strobeUp_flag, strobeDwn_flag;
uint8_t strobe_bright, thisBright[7];
volatile boolean ir_flag;
bool settings_mode = false;
bool ONstate = false;    // true - включено при подаче питания, false - выключено
bool line_mode = true;  // false - включен микрофон, true - выключен микрофон
float freq_max_f_R, freq_max_f_L;
uint8_t freq_f_R[7], freq_f_L[7];
uint8_t this_color, rainbow_steps;
bool running_flag[7], eeprom_flag;
uint8_t effect_delay = 50;  //20 задержка
bool timer_func(uint8_t val = effect_delay);
uint8_t this_bright;
uint8_t HUEindex;

#include "colorMusic_v8_filter.h"  //Набор различных эффектов
uint8_t fullLowPassSet = 0;        //Счетчик нажатия клавиши ОК на пульте. При 4 нажатиях подряд вызвать fullLowPass

// используется в remoteTick
// вспомогательная функция, изменяет величину value на шаг incr в пределах minimum. maximum
uint16_t smartIncr(uint16_t value, uint16_t incr_step, uint16_t mininmum, uint16_t maximum) {
  uint16_t val_buf = value + incr_step;
  val_buf = constrain(val_buf, mininmum, maximum);
  return val_buf;
}

float smartIncrFloat(float value, float incr_step, float mininmum, float maximum) {
  float val_buf = value + incr_step;
  val_buf = constrain(val_buf, mininmum, maximum);
  return val_buf;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool timer_func(uint8_t val) {  // Таймер общий
  if (millis() - effect_timer > val) {
    effect_timer = millis();
    return true;
  } else return false;
}

uint8_t getBrightCRT(uint8_t val) {  // для 8 бит ШИМ
  return ((long)val * val * val + 130305) >> 16;
}

///*
  void  print() {
  // Вывод для тестирования
  //Serial.print(F("this_mode = ")); Serial.println(mainSettings.this_mode);
  //Serial.print(F("freq_strobe_mode = ")); Serial.println(mainSettings.freq_strobe_mode);
  //Serial.print(F("light_mode = ")); Serial.println(mainSettings.light_mode);
  //Serial.print(F("cmu_mode = ")); Serial.println(mainSettings.cmu_mode);
  //Serial.print(F("cmu1_mode = ")); Serial.println(mainSettings.cmu1_mode);
  //Serial.print(F("cmu2_mode = ")); Serial.println(mainSettings.cmu2_mode);
  //Serial.print(F("cmu3_mode = ")); Serial.println(mainSettings.cmu3_mode);
  //Serial.print(F("RAINBOW_STEP = ")); Serial.println(mainSettings.RAINBOW_STEP);
  //Serial.print(F("BRIGHTNESS = ")); Serial.println(mainSettings.BRIGHTNESS);
  //Serial.print(F("LIGHT_SAT = ")); Serial.println(mainSettings.LIGHT_SAT);
  //Serial.print(F("STROBE_PERIOD = ")); Serial.println(mainSettings.STROBE_PERIOD);
  //Serial.print(F("STROBE_SMOOTH = ")); Serial.println(mainSettings.STROBE_SMOOTH);
  //Serial.print(F("FLASH_PERIOD = ")); Serial.println(mainSettings.FLASH_PERIOD);
  //Serial.print(F("HUE_STEP = ")); Serial.println(mainSettings.HUE_STEP);
  //Serial.print(F("COLOR_SPEED = ")); Serial.println(mainSettings.COLOR_SPEED);
  //Serial.print(F("LIGHT_COLOR = ")); Serial.println(mainSettings.LIGHT_COLOR);
  //Serial.print(F("RUNNING_SPEED = ")); Serial.println(mainSettings.RUNNING_SPEED);
  //Serial.print(F("effect_mode = ")); Serial.println(mainSettings.effect_mode);
  //Serial.print(F("HUE_START = ")); Serial.println(mainSettings.HUE_START);
  //Serial.print(F("SMOOTH = ")); Serial.println(mainSettings.SMOOTH);
  //Serial.print(F("SMOOTH_K = ")); Serial.println(mainSettings.SMOOTH_K);
  //Serial.print(F("SMOOTH_FREQ = ")); Serial.println(mainSettings.SMOOTH_FREQ);
  //Serial.print(F("settings_mode = ")); Serial.println(settings_mode);
  //Serial.print(F("line_mode = ")); Serial.println(line_mode);
  //Serial.print(F("ONstate = ")); Serial.println(ONstate);
  //Serial.print(F("effect_delay = ")); Serial.println(effect_delay);
  //Serial.print(F("eeprom_flag = ")); Serial.println(eeprom_flag);
  //Serial.print(F("Rlenght = ")); Serial.println(Rlenght);
  //Serial.print(F("Llenght = ")); Serial.println(Llenght);
  }
//*/
void updateEEPROM() {
  //Serial.println(F("updateEEPROM"));
  EEPROM_UpdateBlock((uint16_t)&mainSettings, 1, sizeof(mainSettings));
}

void readEEPROM() {
  //Serial.println(F("readEEPROM"));
  EEPROM_ReadBlock((uint16_t)&mainSettings, 1, sizeof(mainSettings));
}

void eepromTick() {  // сохранение в память при бездействии 30 с.
  if (eeprom_flag)
    if (millis() - eeprom_timer > 30000) {  // 30 секунд после последнего нажатия с пульта
      eeprom_timer = millis();
      eeprom_flag = false;
      updateEEPROM();
      //Serial.print(F("Настройки сохранены - eeprom_flag = false"));
    }
}

void silence() {  // в режиме тишины запуск подсветки
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
