  //--------------------------------------------WAPOSAT----------------------------------------------------
#include <EEPROM.h>                 //Allows to save data in the EEPROM memory
#include <SPI.h>                    //Include the software serial library 
#include <Ethernet.h>               //Includes all the funtions needed to stablish communication with the server
#include <SoftwareSerial.h>         //Extends the Serial ports in the Arduino
//#include <stdio.h>               
#define rx 2                        //define what pin rx is going to be
#define tx 3                        //define what pin tx is going to be
#define NUMSENSORS 4                //CHANGE IN ACCORD TO THE NUMBER OF SENSORS
#define NUMSTATION 14               //CHANGE FOR EVERY ARDUINO STATION
//#define INITEMPKEY
#define MACL 5
//---------------------------comunicacion serial con sensor-------------------------------------------
SoftwareSerial myserial(rx, tx);   //define how the soft serial port is going to work.
int Pin_x = 5;                     //Arduino pin 5 to control pin X
int Pin_y = 4;                     //Arduino pin 4 to control pin Y

//---------------------------datos de sensor----------------------------------------------------------
char sensorData[NUMSENSORS][30]={"5","5","5","5"}; //A 30 byte character array to hold incoming data from the sensors
byte sensor_bytes_received[NUMSENSORS]={0}; //We need to know how many characters bytes have been received

//----------------------------configuracion shell ethernet----------------------------------------------------------
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE };
byte ip[] = { 172, 16, 13, 62 };
byte gateway[] = { 172, 16, 13, 254 };
byte server[] = { 45, 55, 150, 245 };// IP Publico del servidor WAPOSAT
//byte server[] = { 172, 16, 13, 11 }; // IPServidor LAN
//byte server[] = { 172, 16, 13, 15 };   //My own computer

//----------------------------Functions----------------------------------------------------------
void open_channel1(void);
void open_channel2(void);
void open_channel3(void);
void open_channel4(void);
void LecturaSensores(void);
void httpRequest(void);
void (*pF[])(void)={open_channel1, open_channel2, open_channel3, open_channel4};
String convert2String(byte[]);
//----------------------------Connnections States----------------------------------------------------------
boolean lastConnected = false;                 // state of the connection last time through the main loop
EthernetClient client;
int addr=0;
char iNITEMPKEY[15]; 
String sTempKey;
int activatLectk=false;
//----------------------------------------------------------------------------------------------------
void setup()
{
//-----------------configuracion de serial pc---------------------------------------------------------  
  //Ethernet.begin(mac); // inicializa ethernet shield para un DHCP
  //Ethernet.begin(mac, ip, dns, gateway); // inicializa ethernet shield
  Ethernet.begin(mac, ip, dns, gateway);
  Serial.begin(9600);
  while(!Serial){
      //wait for serial
  }
//-----------------configuracion de serial sensor-----------------------------------------------------
  pinMode(Pin_x, OUTPUT);           //Set the digital pin as output.
  pinMode(Pin_y, OUTPUT);          //Set the digital pin as output.
  myserial.begin(9600);             //Set the soft serial port to 9600
  EEPROM.get(addr, iNITEMPKEY);
  Serial.println("first read of initemkey "+String(iNITEMPKEY));
  //sTempKey = "$"+String(iNITEMPKEY);
  //EEPROM.put(addr, "$1457538768");
  delay(1000); // espera 1 segundo despues de inicializar

}

void loop()
{   
//-----------------------conectando al servidor--------------------------------------------------------  
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  //JUST FOR PRINT SERVER RESPONSE
  if (client.available()) {
    char c = client.read();
    if(c == '$'){
      activatLectk = !activatLectk;
    }
    if(activatLectk){
      sTempKey = sTempKey+String(c);
    }
    Serial.print(c);
  }
  
  //IF THERE IS NO DATA TO READ AND THE LAS TIME THERE WAS THEN STOP CLIENT
  if (!client.connected() && lastConnected) {
    sTempKey = sTempKey.substring(1);
    Serial.println("Finalizando respuesta del servidor, sTempKey "+sTempKey);
    sTempKey.toCharArray(iNITEMPKEY, 15);
    Serial.println("Saving in EEProm" + String(iNITEMPKEY));
    EEPROM.put(addr, iNITEMPKEY);
   // sTempKey="";
    Serial.println("disconnecting.");
    Serial.println("");
    Serial.println("");
    Serial.println("");
    client.stop();
    delay(10000);
  }


 //IF THERE IS NO DATA TO READ
  if(!client.connected()) {  
    delay(5000);
    Serial.println("iNITEMPKEY before "+ String(iNITEMPKEY));
    //Serial.println("sTempKey before"+sTempKey);
    Serial.println("Start request");
    httpRequest();
    //char send2[15];
    //Serial.println("After sTempKey "+sTempKey);
    //Serial.println("After send2 "+String(send2));
    //Serial.println("Saving in EEProm" + String(send2));
   // EEPROM.put(addr, send2);
    sTempKey="";
    Serial.println("Blanking sTempKey tu read new tempkey "+sTempKey);
    Serial.println("Aqui comienza la RESPUESTA SERVIDOR");
    Serial.println("");
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
  
}
//---------------------------------------------------------------------------------------------------------------------

// this method makes a HTTP connection to the server:
void httpRequest() {
  //client.stop();
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    LecturaSensores();

  
    //Converting char* sensorDAta into String objects which can be concatenated
    String Sensors[NUMSENSORS];
    for(int i=0; i<NUMSENSORS;i++){
        Sensors[i]=String(sensorData[i]);
      }
    //Concatenate information of sensors adding '|' in str2Send
    String str2Send = String(NUMSTATION)+"|"+ convert2String(mac)+"|"+String(iNITEMPKEY);
    //sTempKey = sTempKey.substring(1);    
    //String str2Send = String(NUMSTATION)+"|"+ convert2String(mac)+"|"+sTempKey;
    
    for(int i=0; i<NUMSENSORS; i++){
        str2Send = str2Send + "|" + Sensors[i];
      }
      
    //client.print("GET /insertData.php?monitor="+str2Send+" HTTP/1.0 \r\n\r\n");

    client.println("GET /insertData.php?monitor="+str2Send+" HTTP/1.0");
    client.println("Host: estaciones.waposat.com");
    client.println("User-Agent: arduino-ethernet");
    client.println();
    client.flush();
    
    Serial.println("GET /insertData.php.php?monitor="+str2Send+" HTTP/1.0");
   // Serial.println("Problem");
    /*client.println("POST /TEST/index.php HTTP/1.0");
    client.println("Content-type: multipart/form-data");
    client.println("Content-Length: 37");
    client.println("");
    client.println("monitor=0");*/
    myserial.flush();

    delay(1000);

  } 
  else {
// if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    Serial.println("");
    Serial.println("");
    Serial.println("");
    client.stop();
  }
}


//----------------funcion de coneccion al canal 1------------------------------------------------------------------
void open_channel1(){                                //This function controls what UART port is opened.                                                  
         myserial.flush();
         digitalWrite(Pin_x, LOW);                   //Pin_x and pin_y control what channel opens 
         digitalWrite(Pin_y, LOW);                   //Pin_x and pin_y control what channel opens 
      }
void open_channel2(){
         myserial.flush();
         digitalWrite(Pin_x, LOW);                   //Pin_x and pin_y control what channel opens 
         digitalWrite(Pin_y, HIGH);                   //Pin_x and pin_y control what channel opens 
      }
void open_channel3(){
         myserial.flush();
         digitalWrite(Pin_x, HIGH);                   //Pin_x and pin_y control what channel opens 
         digitalWrite(Pin_y, LOW);                   //Pin_x and pin_y control what channel opens 
      }
void open_channel4(){
         myserial.flush();
         digitalWrite(Pin_x, HIGH);                   //Pin_x and pin_y control what channel opens 
         digitalWrite(Pin_y, HIGH);                   //Pin_x and pin_y control what channel opens 
      }
      

void LecturaSensores() {

  for(int i=0; i<NUMSENSORS; i++)
  {
    itoa(random(NUMSTATION/2,NUMSTATION), sensorData[i], 10);
  }
  
  
  //-----------------------sensoR------------------------------------------------------------------  
  /*    Serial.println("Sensing data");
      for(int i=0; i<NUMSENSORS; i++){
        (pF[i])();
        delay(1000);
        myserial.print('r');                      //Send the command from the computer to the Atlas Scientific device using the softserial port                                     
        myserial.print("\r");

        sensor_bytes_received[i]=myserial.readBytesUntil(13,sensorData[i],30); //we read the data sent from the Atlas Scientific device until we see a <CR>. We also count how many character have been received 
        sensorData[i][sensor_bytes_received[i]]=0;            //we add a 0 to the spot in the array just after the last character we received. This will stop us from transmitting incorrect data that may have been left in the buffer
        Serial.println(sensorData[i]);
      }    */
}

String convert2String(byte a[]){
  
  String b="";
  for(int i=0; i<MACL;i++){
         b = b + String(a[i], HEX);
  }
  
  return b;
    
}
