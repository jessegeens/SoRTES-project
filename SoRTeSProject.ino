#include <SPI.h>
#include <LoRa.h>
#include <Arduino_FreeRTOS.h>
#include <EDB.h>
#include <EEPROM.h>
#include <semphr.h>
#include <avr/sleep.h>
#include <avr/power.h>


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
  uint16_t temperature;
} TempEvent;

EDB db(&writer, &reader); // Create an EDB object with the appropriate write and read handlers

int beaconCount = 0; // Keep track of received number of beacons
int numberOfBeacons = 5;

const byte interruptPin = 2;
volatile byte state = LOW;
byte adcsra_buf = ADCSRA;
uint16_t temp_buf = 0;
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
   *          Task                Human readable name           Stack Parameters  Priority  Handle */
  xTaskCreate(ReadSerialCommand,  "LoRa Receive",               128,  NULL,       2,        &readSerialHandle);
  xTaskCreate(logTemp,            "Write temp to db",           128,  NULL,       1,        NULL);
  xTaskCreate(LoRaWaitForNext,    "Wait for next LoRa signal",  128,  NULL,       3,        NULL);
  xTaskCreate(sendTemp,           "Send temperature to GW",     128,  NULL,       1,        NULL);
  xTaskCreate(enableDeepSleep,    "Turn on deep sleep",         128,  NULL,       0,        NULL);
  
  
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
  pinMode(interruptPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), wakeUpFromDeepSleep, CHANGE);
  enableLowPower();

  
  // Create database
  xSemaphoreGive(dbSemaphore);
  db.create(0, TABLE_SIZE, sizeof(TempEvent));

  // Discard first temperature read
  adc_read_temp();
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

 
