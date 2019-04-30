//CODE BY PRASUN GUCHHAIT prasunguchhait1997@gmail.com

/*Pin Diagram of Project
********************************************
  Esp8266 Pin Layout (required one only)
    o     o      o     o
    GND         io      RX
    
    TX    CH_PD        VCC
    o     o      o     o
********************************************
  DHT11 PIN lAYOUR (REQUIRED ONLY)
    o     o     o     o
    VCC   DATA        GND
********************************************
  PIN OUT DIAGRAM OF THE PROJECT

  Arduino 5V       --> DHT11(VCC)& ESP8266(VCC & CH_PD) //ESP8266 WORKS WITH 3.3V BUT 5V ALSO WORKS FINE
  Arduino GND      --> DHT11(GND)& ESP8266(GND)
  Arduino Pin 12   --> DHT11(DATA)
  Arduino Pin 3    --> ESP8266(RX)
  Arduino Pin 2    --> ESP8266(TX)
  Arduino Pin A0   --> LDR
  Arduino pin A1   --> Soil Moisture Sensor Analog
*/
//*****************************************CODING****************************************************

#include"dht.h"
#include<SoftwareSerial.h>

#define ESP8266_TX 2
#define ESP8266_RX 3
#define LDRpin A0 // pin where we connected the LDR and the resistor
#define ledPin 9  // pin for LED connectio
#define ledPinI 13

SoftwareSerial WiFi_Serial(ESP8266_TX,ESP8266_RX);

#define WIFI_NAME "sensoponics"
#define WIFI_PASSWORD "0000000000"
#define port 80
#define server "api.thingspeak.com"
#define route "TA"
#define msgtype "1"
#define dht_dpin 12 
#define LDRpin A0
#define Soilpin A1
#define RELAY1 7
#define WATER  A2

dht DHT;
int humi,tem;
int LDRValue = 0;
int SOILValue = 0;
int val=0;
int wl=0;

boolean No_IP=false;

String data="";
String request_header_1="";
String request_header_2=""; 

String response="";

void setup() 
{
      Serial.begin(9600);  // sets serial port for communication
        
      Serial.println("Calibrating Waterlevel sensor....");
      pinMode(RELAY1, OUTPUT);    
      
      Serial.println("Calibrating LED System....");
      for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) 
      {
          // sets the value (range from 0 to 255):
          analogWrite(ledPin, fadeValue);
          // wait for 30 milliseconds to see the dimming effect
          delay(30);
      }
      // fade out from max to min in increments of 5 points:
      for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) 
      {
          // sets the value (range from 0 to 255):
          analogWrite(ledPin, fadeValue);
          // wait for 30 milliseconds to see the dimming effect
          delay(30);
      }
      delay(200);
      
      pinMode(RELAY1, OUTPUT);  
      Serial.println("Calibrating Sumbersible Module....");
      digitalWrite(RELAY1,1);          
      Serial.println("Motor ON");
      delay(2000);
      digitalWrite(RELAY1,0);          
      Serial.println("Motor OFF");
     
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~NEXT MODULE
      Serial.println("Preparing Wifi Module"); 
      WiFi_Serial.begin(9600);
      wifi_init();
      Serial.println("System Ready and connected to WiFi network");
      analogWrite(ledPinI, 150);
      delay(100);  
}
void loop() 
{
      analogWrite(ledPinI, 0);
      DHT.read11(dht_dpin);
      humi=DHT.humidity;
      tem=DHT.temperature;
      LDRValue = analogRead(LDRpin);
      SOILValue = analogRead(Soilpin);
      val=analogRead(WATER);
      
      if(LDRValue<100)
          analogWrite (ledPin, 255);
      else if(LDRValue>=100 && LDRValue<300)
          analogWrite (ledPin, 150);
      else if(LDRValue>=300 && LDRValue<500)
          analogWrite (ledPin, 80);
      else if(LDRValue>=500 && LDRValue<800)
          analogWrite (ledPin, 50);
      else
          analogWrite (ledPin, 0);
      Serial.println("LED is ON");

      if(SOILValue > 400)
      {
          if(val > 500)
          {
              digitalWrite(RELAY1,1);          
              Serial.println("Motor ON");
              wl=1;
          }
          else
          {
              digitalWrite(RELAY1,0);          
              Serial.println("Motor OFF water level is low");
              wl=0;
          }
      }
      else
      {
          digitalWrite(RELAY1,0); 
          Serial.println("Soil is moist");
      }
      
      Serial.println("Sending Sensor data to Thingspeak"); 
      String uri="/update?api_key=096M6HTDUHZHEWWZ&"+String("field1=")+String(humi)+String("&")+String("field2=")+String(tem)+String("&")+String("field3=")+String(LDRValue)+String("&")+String("field4=")+String(SOILValue)+String("&")+String("field5=")+String(wl);
      sendIOT(uri);
  
  analogWrite(ledPinI, 150);
  delay(200);                    // wait a little
  
}

void sendIOT(String uri)
{
      Serial.println("Connecting to IOT Gateway");
      response="";
      
      request_header_1= "GET " + uri + " HTTP/1.1\r\n";
      request_header_2= "Host: " + String(server) + ":" + String(port) + "\r\n\r\n";



      Serial.println("AT+CIPSTART=\"TCP\",\"" + String(server)+"\","+ String(port));    
      WiFi_Serial.println("AT+CIPSTART=\"TCP\",\"" + String(server)+"\","+ String(port));
      delay(5000);
      
      if(WiFi_Serial.available())
      {
          response="";
          
          // read the data into a variable as long as the
          while(WiFi_Serial.available())  
            response+= (char)WiFi_Serial.read();
          
          Serial.println(response);
          if(WiFi_Serial.find("CONNECT"))
            Serial.print("AT+CIPSEND=");
          

          WiFi_Serial.print("AT+CIPSEND=");
          WiFi_Serial.println(request_header_1.length() + request_header_2.length() + data.length());
          delay(1000);
          
          if(WiFi_Serial.available());
            response="";

          // read the data into a variable as long as the 
          while(WiFi_Serial.available()) 
            response+= (char)WiFi_Serial.read();
           
          Serial.println(response);

          Serial.println(data);  
          if(WiFi_Serial.find(">"));
          {
            WiFi_Serial.print(request_header_1);
            WiFi_Serial.print(request_header_2);
            delay(5000);
          }
          
          if(WiFi_Serial.available())
          {
            delay(100);
            String response="";
            
            // read the data into a variable as long as the 
            while(WiFi_Serial.available()) 
              response+= (char)WiFi_Serial.read();

            Serial.println(response);
            if(WiFi_Serial.find("200"));
            {
              Serial.println("RESPONSE: 200");
              delay(1000);
            }
          }
      }
      else
      {
          Serial.println("Error in Posting");
          delay(1000);
      }     
}


void wifi_init()
{
      connect_wifi("AT",100);
      connect_wifi("AT+CWMODE=3",100);
      connect_wifi("AT+CWQAP",100);  
      connect_wifi("AT+RST",5000);
      check4IP(5000);
      if(!No_IP)
        connect_wifi("AT+CWJAP=\""+String(WIFI_NAME)+"\",\""+String(WIFI_PASSWORD)+"\"",7000);        
}

void connect_wifi(String cmd, int t)
{
  int temp=0,i=0;
  while(1)
  {
    Serial.println(cmd);
    WiFi_Serial.println(cmd); 
    while(WiFi_Serial.available())
    {
      if(WiFi_Serial.find("OK"))
      i=8;
    }
    delay(t);
    if(i>5)
    break;
    i++;
  }
  if(i==8)
  Serial.println("OK");
  else
  Serial.println("Error");
}



void check4IP(int t1)
{
  int t2=millis();
  while(t2+t1>millis())
  {
    while(WiFi_Serial.available()>0)
    {
      if(WiFi_Serial.find("WIFI GOT IP"))
      {
        No_IP=true;
      }
    }
  }
}



































