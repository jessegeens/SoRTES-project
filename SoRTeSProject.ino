#include <SPI.h>
#include <LoRa.h>
#include <Arduino_FreeRTOS.h>
#include <EEPROM.h>
#include <semphr.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

/* * * * * * * * * * * * * * * * * * 
 *     CONFIGURATION SETTINGS
 * * * * * * * * * * * * * * * * * */

  // LoR32u4II 868MHz or 915MHz Settings
  #define SCK     15
  #define MISO    14
  #define MOSI    16
  #define SS      8
  #define RST     4
  #define DI0     7
  #define BAND    869300000  // 915E6
  #define PABOOST true 

  // Database settings
  #define TABLE_SIZE 512 // Arduino 168 or greater


/* * * * * * * * * * * * * * * * * * 
 *       DATA STRUCTS & VARS
 * * * * * * * * * * * * * * * * * */
typedef struct {
  //int beaconId;
  int sleepTime;
  int temperature;
} TempEvent;

int beaconCount = 0; // Keep track of received number of beacons
int numberOfBeacons = 20;


byte adcsra_buf = ADCSRA;
bool listenSerialActive = true;

/* * * * * * * * * * * * * * * * * * 
 *     FREE RTOS DEFINITIONS
 * * * * * * * * * * * * * * * * * */
// Idle Application Hook
void vApplicationIdleHook( void ); 

// Semaphore handles
SemaphoreHandle_t sleepSemaphore;
SemaphoreHandle_t dbSemaphore;

// Queues
QueueHandle_t receiveQueue;
QueueHandle_t readTempQueue;
QueueHandle_t writeTempQueue;

// Task handles
TaskHandle_t readSerialHandle = NULL;

/* * * * * * * * * * * * * * * * * * 
 *         INITIAL SETUP
 * * * * * * * * * * * * * * * * * */
void setup() {
  // Start serial
  Serial.begin(9600);

  /*                            FREERTOS Init
   *
   * Tasks
   * -------------------------------------------------------------------------------------
   *                    Task                              Human readable name         Stack Parameters  Priority  Handle */
  xTaskCreate(ReadSerialCommand,  "LoRa Receive",                   128,  NULL,       2,        &readSerialHandle);
  xTaskCreate(logTemp,                       "Write temp to db",               128,  NULL,       1,        NULL);
  xTaskCreate(LoRaWaitForNext,        "Wait for next LoRa signal", 128,  NULL,       3,        NULL);
  xTaskCreate(sendTemp,                    "Send temperature to GW", 128,  NULL,       1,        NULL);
  xTaskCreate(enableDeepSleep,         "Turn on deep sleep",          128,  NULL,       0,        NULL);
  
  
  //vTaskStartScheduler();
  /*
   * Semaphore creation + Queue creation
   * ------------------------------------------------------------------------------------- */
  vSemaphoreCreateBinary(sleepSemaphore);
  vSemaphoreCreateBinary(dbSemaphore);
  writeTempQueue = xQueueCreate(2, sizeof(TempEvent));
  readTempQueue = xQueueCreate(2, sizeof(int));
  receiveQueue = xQueueCreate(2, sizeof(int));

  // Power mgmt setup
  enableLowPower();

  delay(2000);
  // Create database
  xSemaphoreGive(dbSemaphore);
  initDb();

  // Discard first temperature read
  adcReadTemp();
  delay(2000);
  
  // Setup LoRa
  LoRa.setPins(SS,RST,DI0);
  if (!LoRa.begin(BAND,PABOOST )) {
    Serial.println("Starting LoRa failed!");
  }
  Serial.println("Started LoRa");
  LoRa.onReceive(LoRaReceive);
  LoRa.receive();
  
  Serial.println("Setup finished succesfully!");
}

/* * * * * * * * * * * * * * * * * * 
 *             LOOP
 * * * * * * * * * * * * * * * * * */
void loop() {

}

 
