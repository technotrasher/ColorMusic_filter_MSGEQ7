/* изм. 26.02.2023 */

void loop() {
  channels[0]->ilive();
  channels[1]->ilive();
  channels[2]->ilive();
  buttonTick();  // опрос и обработка кнопки
  remoteTick();  // опрос ИК пульта
  if (ONstate == true) {
    audio();       // главный цикл обработки и отрисовки
    eepromTick();  // проверка не пора ли сохранить настройки
  }
}
