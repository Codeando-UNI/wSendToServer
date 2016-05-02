

#include <EEPROM.h>

void setup() {
  
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  //float f = 123.456f;  //Variable to store in EEPROM.
  //unsigned long f=1457538768;
  char f[15]="1461966702";
  int eeAddress = 0;   //Location we want the data to be put.
  
  //One simple call, with the address first and the object second.
  EEPROM.put(eeAddress, f);

  Serial.println("Written data!"+String(f));
}

void loop() {
  /* Empty loop */
}
