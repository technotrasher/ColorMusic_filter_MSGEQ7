void loop() {
  buttonTick();       // опрос и обработка кнопки
  remoteTick();      // опрос ИК пульта
  if (ONstate == true) {
    audio();          // главный цикл обработки и отрисовки
    eepromTick();     // проверка не пора ли сохранить настройки
  }
}
