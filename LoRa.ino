/* * * * * * * * * * * * * * * * * * 
 *            LORA
 * * * * * * * * * * * * * * * * * */

void LoRaWaitForNext(void *pvParameters){
  int delayS = 0;
  for (;;) {
    xQueueReceive(receiveQueue, &delayS, portMAX_DELAY);
    vTaskDelay((delayS*1000 - 200)/portTICK_PERIOD_MS);
    if (LoRa.begin(BAND, PABOOST)){
      LoRa.receive();
    }
    else
      Serial.println("Starting LoRa failed!");
  }
}

void LoRaReceive(int packetSize){
    if (packetSize) {                       // received a packet
      if(listenSerialActive){
       enableLowPowerNoSerial();
      }
      Serial.print("Received packet '");
      String received = "";
      while (LoRa.available()) {            // read packet
        received += (char)LoRa.read();
      }
      int sec = 0;
      if(received.length() == 5){           // Check for the next wake-up time in seconds
        beaconCount++;
        Serial.println(beaconCount);
        char secChar = received[4];
        sec = secChar - '0'; //Subtract ASCII vals from each other
      }
      
      xQueueSendFromISR(readTempQueue, &sec, NULL);
      xQueueSendFromISR(receiveQueue, &sec, NULL);
      Serial.print(received);      
      Serial.print("' with RSSI ");
      Serial.println(LoRa.packetRssi());    // print RSSI of packet
    }
    LoRa.end();
}

void sendTemp(void *pvParameters) {
  int temp = 0;
  for (;;) {
    xQueueReceive(writeTempQueue, &temp, portMAX_DELAY);
     if (! LoRa.begin(BAND, PABOOST)) {
        Serial.println("Starting LoRa failed!");
    }
    LoRa.beginPacket();
    Serial.print("Sending temp: ");
    Serial.println(temp);
    LoRa.print(temp);
    LoRa.endPacket();
    LoRa.end();
    if (beaconCount >= numberOfBeacons){
      // Enter deep sleep mode
      xSemaphoreGive(sleepSemaphore);  
    } 
  }
}
