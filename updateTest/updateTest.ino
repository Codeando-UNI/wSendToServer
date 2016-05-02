#include<EEPROM.h>

int eeAddress = 0;
char f[15];

void setup() {
  Serial.begin(9600);

  while(!Serial){
      //wait for serial
  }
}

void loop() {
  
  EEPROM.get(eeAddress, f);

  Serial.println("Read from EEPROM "+String(f));

  delay(5000);

  unsigned long ul0 = strtoul(f, NULL, 0);
  ul0 ++;
  sprintf(f, "%ul", ul0 );
  EEPROM.put(eeAddress, f);
  Serial.println("in EEprom "+String(f));
  delay(5000);
  

}
