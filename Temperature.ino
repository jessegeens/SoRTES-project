/* * * * * * * * * * * * * * * * * * 
 *          TEMPERATURE
 * * * * * * * * * * * * * * * * * */
void logTemp(void *pvParameters) {
  int delayS;
  TempEvent ev;
  for (;;) {
        xQueueReceive(readTempQueue, &delayS, portMAX_DELAY);
        ev.temperature = adcReadTemp();
        ev.sleepTime = delayS;
        xQueueSendFromISR(writeTempQueue, &ev.temperature, NULL);
        writeToDb(ev);
  }
}

int adcReadTemp()
{

    ADCSRA &= bit (ADEN);
    delay(20);
    //vTaskDelay(20 / portTICK_PERIOD_MS);

    ADMUX = (1<<REFS1) | (1<<REFS0) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0);
    ADCSRB |= (1<<MUX5);
    delay(4);
    // start the conversion
     ADCSRA |= bit(ADSC);

    // ADSC is cleared when the conversion finishes
    while (ADCSRA & bit(ADSC));

    uint8_t low  = ADCL;
    uint8_t high = ADCH;

    //discard first reading
    ADCSRA |= bit(ADSC);
    while (ADCSRA & bit(ADSC));
    low  = ADCL;
    high = ADCH;
    int a = (high << 8) | low;

     ADCSRA &= ~(1<<ADEN);

    return a - 236; //Calibrate here

      
}

 
//int8_t getTemperatureInternal() {
  /* Temperature  °C -45°C  +25°C  +85°C
     Voltage      mV 242 mV 314 mV 380 mV  */
/*
  // Select Temp Channel and 2.56V Reference
  ADMUX = (1<<REFS1) | (1<<REFS0) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0);
  ADCSRB |= (1<<MUX5);
  vTaskDelay((20)/portTICK_PERIOD_MS); //wait for internal reference to settle
  delay(4);
  // start the conversion
  ADCSRA |= bit(ADSC);

  // ADSC is cleared when the conversion finishes
  while (ADCSRA & bit(ADSC));

  uint8_t low  = ADCL;
  uint8_t high = ADCH;

  //discard first reading
  ADCSRA |= bit(ADSC);
  while (ADCSRA & bit(ADSC));
  low  = ADCL;
  high = ADCH;
  int a = (high << 8) | low;

  // Turn off ADC
  ADCSRA = 0;
  
  //return temperature in C
  return a - 272;
}*/

    /*byte ch = 32 + 7;
    ch &= 63;    //32U4 has 64 channels
    if (ch & 32) ADCSRB |= (1<<MUX5);
    ch &= 31;  //MUX0..4 are in ADMUX
    ADMUX = (3 << REFS0)|(ch << MUX0); //VREF=2.56V
    ADCSRA = (1<<ADEN)|(1<<ADSC)|(7<<ADPS0); //div128
    while (ADCSRA & (1<<ADSC)) ;
    uint16_t mV = (ADCW * 2560L) / 1024;
    adcsra_buf = ADCSRA;
    ADCSRA = 0; 
    return mV - 722; // Calibrate here*/
