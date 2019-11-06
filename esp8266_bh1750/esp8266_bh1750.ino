#include <ESP8266WiFi.h>
#include <Wire.h>

// ================== defines and variables ========================
#define myPeriodic 60                            // in seconds – ThingSpeak pub is here every 60 sec

int BH1750_Device = 0x23;                    // I2C address for light sensor
unsigned int Lux, Scaled_FtCd;
float FtCd, Wattsm2;
const char* server = “api.thingspeak.com”;
String apiKey = “TQCRYN20XEJ9K6JX”;        // subsitute with your API key
const char* MY_SSID = “Minh”;    // subsitute with the SSID of your wifi network
const char* MY_PWD = “01654399912lth@”;    // subsitute with the password of your wifi network
int sent = 0;

void setup() {
Wire.begin ();
Serial.begin (9600);
Wire.beginTransmission(BH1750_Device);
Wire.write (0x10);                                // set resolution to 1 Lux
Wire.endTransmission ();
delay (200);
connectWifi ();
}

void loop() {

//char buffer[10];
int i;
Lux = BH1750_Read();
FtCd = Lux/10.764;
Wattsm2 = Lux/683.0;
Serial.print (Lux,DEC);
Serial.println (“[lx]”);
Serial.print (FtCd,2);
Serial.println (“[FC]”);
Serial.print (Wattsm2,4);
Serial.println (“[Watts/m^2]”);
Serial.print (String(sent)+” BH1750: “);
Serial.println (Wattsm2,4);

sendWattsm2 (Wattsm2);
int count = myPeriodic;
while(count–)
delay(1000);
}

unsigned int BH1750_Read() {
unsigned int i=0;
Wire.beginTransmission (BH1750_Device);
Wire.requestFrom (BH1750_Device, 2);
while(Wire.available ())
{
i <<=8;
i|= Wire.read();
}
Wire.endTransmission ();
return i/1.2;                                    // Convert to Lux
}

// ================== subroutines =====================================

void connectWifi()
{
Serial.print(“Connecting to “+*MY_SSID);
WiFi.begin(MY_SSID, MY_PWD);
while (WiFi.status() != WL_CONNECTED) {
delay(1000);
Serial.print(“.”);
}

Serial.println(“”);
Serial.println(“Connected”);
Serial.println(“”);
} //end connect

void sendWattsm2(float Wattsm2)
{
WiFiClient client;

if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
Serial.println(“WiFi Client connected “);

String postStr = apiKey;
postStr += “&field3=”;
postStr += String(Wattsm2);
postStr += “\r\n\r\n”;

client.print(“POST /update HTTP/1.1\n”);
client.print(“Host: api.thingspeak.com\n”);
client.print(“Connection: close\n”);
client.print(“X-THINGSPEAKAPIKEY: ” + apiKey + “\n”);
client.print(“Content-Type: application/x-www-form-urlencoded\n”);
client.print(“Content-Length: “);
client.print(postStr.length());
client.print(“\n\n”);
client.print(postStr);
delay(1000);
}

sent++;
client.stop();
}
