/* * * * * * * * * * * * * * * * * * 
 *          DATABASE
 * * * * * * * * * * * * * * * * * */

void writer(unsigned long address, byte data){
  EEPROM.put(address, data);
}

byte reader(unsigned long address){
  return EEPROM.read(address);  
}

void writeToDb(TempEvent ev){
  // Take semaphore
  xSemaphoreTake(dbSemaphore, ( TickType_t ) 10 );
   
  // Write (critical section)
  db.appendRec(EDB_REC ev);
  
  // Give semaphore
   xSemaphoreGive(dbSemaphore);
}

void printOneToSerial(int id){
  if (id <= db.count() && id > 0){
    TempEvent ev;
    db.readRec(id, EDB_REC ev);
    Serial.println("ID  Sleep Temp");
    Serial.print(id); Serial.print("  ");
    //Serial.print(ev.beaconId); Serial.print("  ");
    Serial.print(ev.sleepTime); Serial.print("    ");
    Serial.println(ev.temperature);
  } else {
    Serial.println("No records found");
  }
}

void printAllToSerial(){
  int count = db.count();
  if (count == 0){
    Serial.println("No records found");
    return;
  };
  TempEvent ev;
  Serial.println("ID Sleep Temp");
  for (int recno = 1; recno <= count; recno++){
    db.readRec(recno, EDB_REC ev);
    Serial.print(recno); Serial.print("  ");
   // Serial.print(ev.beaconId); Serial.print("  ");
    Serial.print(ev.sleepTime); Serial.print("    ");
    Serial.println(ev.temperature);
  }
}
