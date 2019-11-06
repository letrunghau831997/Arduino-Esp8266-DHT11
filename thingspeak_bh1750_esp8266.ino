
// Code to use SoftwareSerial
#include <SoftwareSerial.h>
SoftwareSerial espSerial(2,3);      // arduino RX pin=2  arduino TX pin=3    connect the arduino RX pin to esp8266 module TX pin   -  connect the arduino TX pin to esp8266 module RX pin



#include <BH1750.h>

BH1750 lightMeter;

String apiKey = "TQCRYN20XEJ9K6JX";     // replace with your channel's thingspeak WRITE API key

String ssid="Minh";    // Wifi network SSID
String password ="01654399912lth@";  // Wifi network password

boolean DEBUG=true;

//======================================================================== showResponce
void showResponse(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (espSerial.available()){
        c=espSerial.read();
        if (DEBUG) Serial.print(c);
      }
    }
                   
}

//========================================================================
boolean thingSpeakWrite(float value3){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  // TCP connection
  cmd += "184.106.153.149";                               // api.thingspeak.com
  cmd += "\",80";
  espSerial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  
  
  String getStr = "GET /update?api_key=";   // prepare GET string
  getStr += apiKey;
  
  getStr +="&field3=";
  getStr += String(value3);
  getStr += "\r\n\r\n";
  
  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  espSerial.println(cmd);
  if (DEBUG)  Serial.println(cmd);
  
  delay(100);
  if(espSerial.find(">")){
    espSerial.println("AT+CIPCLOSE");
    // alert user
    if (DEBUG)   Serial.println("AT+CIPCLOSE");
    return false;
  }
  else{
    espSerial.print(getStr);
    if (DEBUG)  Serial.print(getStr);

  }
  return true;
  delay(5000);
}
//================================================================================ setup
void setup() {                
  DEBUG=true;           // enable debug serial
  Serial.begin(115200);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();
  // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);

  lightMeter.begin();         // Start DHT sensor
  
  espSerial.begin(115200);  // enable software serial
                          // Your esp8266 module's speed is probably at 115200. 
                          // For this reason the first time set the speed to 115200 or to your esp8266 configured speed 
                          // and upload. Then change to 9600 and upload again
  espSerial.println("AT+CIOBAUD=115200");
  showResponse(1000);
  
  espSerial.println("AT+RST");         // Enable this line to reset the module;
  showResponse(1000);

  espSerial.println("AT+UART_CUR=115200,8,1,0,0");         
  showResponse(1000);
  
  espSerial.println("AT+CWMODE=1");   // set esp8266 as client
  showResponse(1000);

  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  // set your home router SSID and password
  showResponse(1000);

  espSerial.println("AT+CIFSR"); 
  showResponse(1000);
  if(DEBUG) Serial.println("Setup completed");
}


// ====================================================================== loop
void loop() {

  // Read sensor values
   float lux = lightMeter.readLightLevel();
        if (isnan(lux)) {
        if (DEBUG) Serial.println("Failed to read from BH1750");
      }
      else {
          if (DEBUG)  Serial.println("Light: "+String(lux)+" lux");
           thingSpeakWrite(lux);                                      // Write values to thingspeak
      }
  
    
  // thingspeak needs 15 sec delay between updates,     
  delay(16000);  
}
