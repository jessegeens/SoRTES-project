/* * * * * * * * * * * * * * * * * * 
 *          DATABASE
 * * * * * * * * * * * * * * * * * */
// Initial address
int offset = 2*sizeof(int);
int curr_addr = 4;
int counter = 0;
int tempSize = sizeof(TempEvent);

/***** LAYOUT ****
| 0         | 2       | 4     | ... | 80
| curr_addr | counter | temp0 | ... | temp19
*/
void initDb(){
    EEPROM.get(0, curr_addr);
    EEPROM.get(2, counter);
}

void clearDb(){
  // Take semaphore
  xSemaphoreTake(dbSemaphore, ( TickType_t ) 10 );

  curr_addr = offset;
  counter = 0;
  EEPROM.put(0, curr_addr);
  EEPROM.put(2, counter);
  Serial.println("Erased database");
  // Give semaphore
  xSemaphoreGive(dbSemaphore);
}

void writeToDb(TempEvent ev){
  // Take semaphore
  xSemaphoreTake(dbSemaphore, ( TickType_t ) 10 );
  // Write (critical section)
   EEPROM.put(curr_addr,ev); 
   if (counter < numberOfBeacons){
    counter++;
    EEPROM.put(2, counter);
   }

   curr_addr = curr_addr + sizeof(TempEvent);
   if(curr_addr > (numberOfBeacons) * tempSize + offset){
    curr_addr = 4;
   }
   EEPROM.put(0, curr_addr);
  
  // Give semaphore
   xSemaphoreGive(dbSemaphore);
}

void printLatest(){
  if (counter > 0){
    TempEvent ev;
    EEPROM.get(curr_addr - sizeof(TempEvent), ev);
    Serial.println("ID  Sleep Temp");
    Serial.print(counter - 1); Serial.print("  ");
    Serial.print(ev.sleepTime); Serial.print("    ");
    Serial.println(ev.temperature);
  } else {
    Serial.println("No records found");
  }
}

void printAllToSerial(){
  if (counter == 0){
    Serial.println("No records found");
    return;
  };
  TempEvent ev;
  Serial.println("ID Sleep Temp");
  for (int recno = 0; recno < counter; recno++){
    int addr = offset + recno * sizeof(TempEvent);
    EEPROM.get(addr, ev);
    Serial.print(recno); Serial.print("  ");
    Serial.print(ev.sleepTime); Serial.print("    ");
    Serial.println(ev.temperature);
  }
}
