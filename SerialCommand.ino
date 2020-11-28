void ReadSerialCommand(void *pvParameters){
  char option;
  for (;;) {
    while (Serial.available() > 0) {
      option = (char)Serial.read();
      if (option == '1'){
        // read the latest temperature value and beacon details fromdatabase
        printOneToSerial(db.count());
      } else if (option == '2'){
        // read all temperature values and beacon details from database
        printAllToSerial();
      } else if (option == '3'){ 
        // enable low power operation mode
        beaconCount = numberOfBeacons;
        xSemaphoreGive(sleepSemaphore);
      } 
    }
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}
