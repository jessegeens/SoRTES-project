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
