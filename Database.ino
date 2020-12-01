/* * * * * * * * * * * * * * * * * * 
 *          DATABASE
 * * * * * * * * * * * * * * * * * */
// Initial address
int curr_addr = 1;
int counter = 0;

/***** LAYOUT ****
| 0               | 1           | 2         | ... | 21
| curr_addr | counter | temp0 | ... | temp19
*/
void initDb(){
    EEPROM.get(0, curr_addr);
    EEPROM.get(1, counter);
}

void writeToDb(TempEvent ev){
  // Take semaphore
  xSemaphoreTake(dbSemaphore, ( TickType_t ) 10 );
   
  // Write (critical section)
   EEPROM.put(curr_addr,ev); 
   if (counter < numberOfBeacons){
    counter++;
    EEPROM.put(1, counter);
   }
   curr_addr = (curr_addr + 1) % numberOfBeacons + 1;
   EEPROM.put(0, curr_addr);
   
  
  // Give semaphore
   xSemaphoreGive(dbSemaphore);
}

void printLatest(){
  if (counter > 0){
    TempEvent ev;
    EEPROM.get(curr_addr - 1, ev);
    Serial.println("ID  Sleep Temp");
    Serial.print(counter); Serial.print("  ");
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
  for (int recno = 0; recno <= counter; recno++){
   EEPROM.get(recno + 2, ev);
    Serial.print(recno); Serial.print("  ");
    Serial.print(ev.sleepTime); Serial.print("    ");
    Serial.println(ev.temperature);
  }
}
