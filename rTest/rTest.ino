#include <EEPROM.h>

void setup() {

  //float f = 0.00f;   //Variable to store data read from EEPROM.
  //unsigned long f=0;
  char f[15];
  int eeAddress = 0; //EEPROM address to start reading from

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Read from EEPROM: ");

  //Get the float data from the EEPROM at position 'eeAddress'
  EEPROM.get(eeAddress, f);
  Serial.println(f);    //This may print 'ovf, nan' if the data inside the EEPROM is not a valid float.
}

void loop() {
  /* Empty loop */
}
