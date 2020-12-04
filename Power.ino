void enableLowPower() {
  //ADCSRA = 0;               // Turn of ADC
  //ACSR |= _BV(ACD);    // Disable analog comparator
  wdt_disable();            // Disable Watchdog Timer
  power_timer1_disable();               // Disable Timer1
  power_timer2_disable();
  power_timer3_disable();
  power_twi_disable();      // Disable I2C
  //power_usart0_disable();
  //power_usart1_disable();

}

void enableLowPowerNoSerial() {
      Serial.println("Enabling LowPowerNoSerial");
      vTaskSuspend(readSerialHandle);
      listenSerialActive = false;
      Serial.end();
      power_usart0_disable();
      power_usart1_disable();
      power_usb_disable();  //FIXME: Uncomment for final version!
    
      USBCON |= (1 << FRZCLK);              // Freeze the USB Clock
      PLLCSR &= ~(1 << PLLE);               // Disable the USB Clock (PLL)
      USBCON &=  ~(1 << USBE  );            // Disable the USB
}

void enableDeepSleep(void *pvParameters) {
  for (;;) {
    if (xSemaphoreTake(sleepSemaphore, 0) && beaconCount >= numberOfBeacons) {
      Serial.println("Entering deep sleep mode...");
      Serial.end();                         // Stop Serial connection
      vTaskEndScheduler();                  // Stop Scheduler
      ACSR |= _BV(ACD); 
      ADCSRA = 0;                           // Disable ADC
      power_timer0_disable();               // Disable Timer0
      power_timer1_disable();               // Disable Timer1
      power_timer2_disable();               // Disable Timer2
      power_timer3_disable();               // Disable Timer3
      power_usart0_disable();               // Disable USART0
      power_usart1_disable();               // Disable USART1
      power_adc_disable();                  // Disable ADC
      power_usb_disable();                  // Disable USB
      power_twi_disable();                  // Disable I2C
      power_spi_disable();                  // Disable SPI
      USBCON |= (1 << FRZCLK);              // Freeze the USB Clock
      PLLCSR &= ~(1 << PLLE);               // Disable the USB Clock (PPL)
      USBCON &=  ~(1 << USBE  );            // Disable the USB
      noInterrupts ();
      //portENTER_CRITICAL();
      sleep_enable();
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_cpu();
    }
  }
}
