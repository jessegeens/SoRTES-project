void ReadSerialCommand(void *pvParameters){
  char option;
  for (;;) {
    while (Serial.available() > 0) {
      option = (char)Serial.read();
      if (option == '1'){
        // read the latest temperature value and beacon details fromdatabase
        printLatest();
      } else if (option == '2'){
        // read all temperature values and beacon details from database
        printAllToSerial();
      } else if (option == '3'){ 
        // enable low power operation mode
        beaconCount = numberOfBeacons;
        xSemaphoreGive(sleepSemaphore);
      } else if (option == '4'){ 
        clearDb();
      } else if (option == '5'){ 
        Serial.println("---------------------");
        Serial.println("DEBUGGING INFORMATION");
        Serial.println("---------------------");
        Serial.print("beaconCount:  "); Serial.println(beaconCount);
        Serial.print("db.curr_addr: "); Serial.println(curr_addr);
        Serial.print("db.counter:   "); Serial.println(counter);
        Serial.println("---------------------");
      } 
    }
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}
