//library DHT22
#include <DHT.h>

//library esp
#include <ESP8266WiFi.h>

//library bmp180
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// Descomenta para usar un tipo u otro de sensor !

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Conecte el pin 1 ( izquierda ) del sensor a +5V
// NOTA: Si usa una plava con lógica a 3.3V conecte el pin 1
// a 3.3V en vez de 5V!
// Conecte el pin 2 dle sensor a DHTPIN ( pin digital asignado ).
// Conecte el pin 4 ( derecha )del sensor a GROUND.
// Conecte una resistencia de 10K desde el 2 (data) a el pin 1 (power) del sensor.

// Calibrar cada sensor con el valor en seco y mojado

const int seco = 840; // valor seco
const int mojado = 430; // valor mojada
const int DHTPin = 5;     // pin digital del DHT-22 pin 5 GPIO 5 D1
const int sensorVal = 0;
const int bomba = 4; // pin  GPIO 4 D2
DHT dht(DHTPin, DHTTYPE);

// Configuración API y WIFI,

String apiKey = "TU API KEY";             // Cambia con tu API Key Write Thingspeak Channel
const char* ssid = "TU SSID";                    // Pon tu nombre de red Wifi
const char* password = "TU PASSWORD";      // Pon tu password de la red Wifi

const char* server = "api.thingspeak.com";      // Dirección del servidor Thingspeak, "184.106.153.149" o api.thingspeak.com

WiFiClient client;



void setup() 
{
pinMode(4, OUTPUT);
digitalWrite(bomba, HIGH);


   
Serial.begin(9600);
dht.begin();

WiFi.begin(ssid, password);

Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
Serial.print("..........");
Serial.println();
WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);

}
Serial.println("WiFi connected");
Serial.println();

}

void loop()
{
  
delay(2000);
  
// Leemos sensor humedad tierra 
int sensorVal = analogRead(A0);

// Leemos temperatura y humedad ambiente
float h = dht.readHumidity();
float t = dht.readTemperature();

// comprobamos si ha leido los datos del dht-22
if ( isnan(h) || isnan(t))

{ 
  // comprobamos si ha leido los datos del dht-22
  
  Serial.println (" Error leyendo DHT-22 !! ");
 
  return;
 
  Serial.println (" Sensor DHT-22 conectado !!");
  
}



// extrapolamos valor mojado a seco con las lecturas y trasladamos a porcentaje.

 int PorcentajeHumedad = map(sensorVal, mojado, seco, 100,0);
 float m = PorcentajeHumedad;

// Imprimimos en el puerto serie los valores
 
 Serial.print(" Humedad Tierra ");
 Serial.print(PorcentajeHumedad);
 Serial.print(" % ");
 Serial.print("Humedad Ambiente: ");
 Serial.print(h);
 Serial.print(" %\t");
 Serial.print("Temperatura Ambiente: ");
 Serial.print(t);
 Serial.print(" ºC ");
 
//Serial.print (sensorVal);
Serial.println();
delay(100);

 if(PorcentajeHumedad <= 49)
  {
    Serial.println("Suelo seco, SE RIEGA!!!");
    digitalWrite(bomba, LOW);
    delay(2000);
    digitalWrite(bomba, HIGH);
    delay(1000);    
  }
 
  if(PorcentajeHumedad > 50 && PorcentajeHumedad <= 70)
  {
    Serial.println("Suelo Perfecto, NO SE RIEGA !!!");
  }
 
  if(PorcentajeHumedad > 71)
  {
    Serial.println("Suelo con exceso de humedad, NO SE RIEGA !!!");
  }
 
  Serial.println(" ");

//--------------------------thingspeak-------------------------

if (client.connect(server,80)) 

{ // "184.106.153.149" or api.thingspeak.com
String postStr = apiKey;
postStr +="&field1=";
postStr += String(m);
postStr +="&field2=";
postStr += String(h);
postStr +="&field3=";
postStr += String(t);
//postStr +="&field4=";
//postStr += String(event.pressure);
//postStr +="&field5=";
//postStr += String(temperature);
//postStr +="&field6=";
//postStr += String(sensorValue1);
//postStr +="&field7=";
//postStr += String(sensorValue2);
//postStr +="&field8=";
//postStr += String(bmp.pressureToAltitude(seaLevelPressure,event.pressure));
postStr += "\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n";

client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n\n\n\n\n\n\n");
client.print(postStr);

}
client.stop();


// Thingspeak necesita al menos 15 segundos de retardo en la actualización.

delay(20000);

}
