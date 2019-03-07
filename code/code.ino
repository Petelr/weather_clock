//This sketch connects to MichiganTechOpen WiFi and
//send request to darksky.net to get current weather information

// 11/30/2018
// adding time server

// 12/2/2018
// added ledcontrol

// added print temp

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "LedControl.h"
/* 
 * Create a new controller 
 * Params :
 * int dataPin    The pin on the Arduino where data gets shifted out
 * int clockPin   The pin for the clock
 * int csPin      The pin for selecting the device when data is to be sent
 * int numDevices The maximum number of devices that can be controlled
*/
//LedControl(int dataPin, int clkPin, int csPin, int numDevices);
int DIN = 16; // D0
int CS =  5;  // D1
int CLK = 4;  // D2

LedControl lc=LedControl(DIN,CLK,CS,4);

unsigned long previousMillis = 0; 
unsigned long r_previousMillis = 0; 
const long update_interval = 180*1000;
const long refresh_interval = 1000;
int state_flag = 0;
String var_temp;
const String darksky_fp = "EB:C2:67:D1:B1:C6:77:90:51:C1:4A:0A:BA:83:E1:F0:6D:73:DD:B8"; //darksky.net https fingerprint
const char* ssid = "MichiganTechOpen";
const char* host = "api.darksky.net";
const char* url = "/forecast/6064dd76fbb4f1f9c77120480a9d5ca9/47.1211,-88.5694?exclude=minutely,hourly,daily&lang=zh&units=si";
const String urlt = "https://api.darksky.net/forecast/6064dd76fbb4f1f9c77120480a9d5ca9/47.1211,-88.5694?exclude=minutely,hourly,daily&lang=zh&units=si";
StaticJsonBuffer<1116> jsonBuffer;

WiFiClientSecure client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const byte clock_digi4[][8] = {
{
  B00000000,
  B01111000,
  B11001100,
  B11011100,
  B11101100,
  B11001100,
  B11001100,
  B01111000
},{
  B00000000,
  B00110000,
  B00110000,
  B01110000,
  B00110000,
  B00110000,
  B00110000,
  B11111100
},{
  B00000000,
  B01111000,
  B11001100,
  B00001100,
  B00011000,
  B01100000,
  B11000000,
  B11111100
},{
  B00000000,
  B01111000,
  B11001100,
  B00001100,
  B00111000,
  B00001100,
  B11001100,
  B01111000
},{
  B00000000,
  B00011000,
  B00111000,
  B01011000,
  B10011000,
  B11111100,
  B00011000,
  B00011000
},{
  B00000000,
  B11111100,
  B11000000,
  B11111000,
  B00001100,
  B00001100,
  B11001100,
  B01111000
},{
  B00000000,
  B01111000,
  B11001100,
  B11000000,
  B11111000,
  B11001100,
  B11001100,
  B01111000
},{
  B00000000,
  B11111100,
  B11001100,
  B00011000,
  B00011000,
  B00110000,
  B00110000,
  B00110000
},{
  B00000000,
  B01111000,
  B11001100,
  B11001100,
  B01111000,
  B11001100,
  B11001100,
  B01111000
},{
  B00000000,
  B01111000,
  B11001100,
  B11001100,
  B01111100,
  B00001100,
  B11001100,
  B01111000
}};
const byte clock_digi3[][8] = {
{
  B00000000,
  B01111000,
  B11001101,
  B11011101,
  B11101100,
  B11001101,
  B11001101,
  B01111000
},{
  B00000000,
  B00110000,
  B00110001,
  B01110001,
  B00110000,
  B00110001,
  B00110001,
  B11111100
},{
  B00000000,
  B01111000,
  B11001101,
  B00001101,
  B00011000,
  B01100001,
  B11000001,
  B11111100
},{
  B00000000,
  B01111000,
  B11001101,
  B00001101,
  B00111000,
  B00001101,
  B11001101,
  B01111000
},{
  B00000000,
  B00011000,
  B00111001,
  B01011001,
  B10011000,
  B11111101,
  B00011001,
  B00011000
},{
  B00000000,
  B11111100,
  B11000001,
  B11111001,
  B00001100,
  B00001101,
  B11001101,
  B01111000
},{
  B00000000,
  B01111000,
  B11001101,
  B11000001,
  B11111000,
  B11001101,
  B11001101,
  B01111000
},{
  B00000000,
  B11111100,
  B11001101,
  B00011001,
  B00011000,
  B00110001,
  B00110001,
  B00110000
},{
  B00000000,
  B01111000,
  B11001101,
  B11001101,
  B01111000,
  B11001101,
  B11001101,
  B01111000
},{
  B00000000,
  B01111000,
  B11001101,
  B11001101,
  B01111100,
  B00001101,
  B11001101,
  B01111000
}};
const byte clock_digi2[][8] = {
{
  B00000000,
  B00011110,
  B10110011,
  B10110111,
  B00111011,
  B10110011,
  B10110011,
  B00011110
},{
  B00000000,
  B00001100,
  B10001100,
  B10011100,
  B00001100,
  B10001100,
  B10001100,
  B00111111
},{
  B00000000,
  B00011110,
  B10110011,
  B10000011,
  B00000110,
  B10011000,
  B10110000,
  B00111111
},{
  B00000000,
  B00011110,
  B10110011,
  B10000011,
  B00001110,
  B10000011,
  B10110011,
  B00011110
},{
  B00000000,
  B00000110,
  B10001110,
  B10010110,
  B00100110,
  B10111111,
  B10000110,
  B00000110
},{
  B00000000,
  B00111111,
  B10110000,
  B10111110,
  B00000011,
  B10000011,
  B10110011,
  B00011110
},{
  B00000000,
  B00011110,
  B10110011,
  B10110000,
  B00111110,
  B10110011,
  B10110011,
  B00011110
},{
  B00000000,
  B00111111,
  B10110011,
  B10000110,
  B00000110,
  B10001100,
  B10001100,
  B00001100
},{
  B00000000,
  B00011110,
  B10110011,
  B10110011,
  B00011110,
  B10110011,
  B10110011,
  B00011110
},{
  B00000000,
  B00011110,
  B10110011,
  B10110011,
  B00011111,
  B10000011,
  B10110011,
  B00011110
}};
const byte clock_digi1[][8] = {
{
  B00000000,
  B00011110,
  B00110011,
  B00110111,
  B00111011,
  B00110011,
  B00110011,
  B00011110
},{
  B00000000,
  B00001100,
  B00001100,
  B00011100,
  B00001100,
  B00001100,
  B00001100,
  B00111111
},{
  B00000000,
  B00011110,
  B00110011,
  B00000011,
  B00000110,
  B00011000,
  B00110000,
  B00111111
},{
  B00000000,
  B00011110,
  B00110011,
  B00000011,
  B00001110,
  B00000011,
  B00110011,
  B00011110
},{
  B00000000,
  B00000110,
  B00001110,
  B00010110,
  B00100110,
  B00111111,
  B00000110,
  B00000110
},{
  B00000000,
  B00111111,
  B00110000,
  B00111110,
  B00000011,
  B00000011,
  B00110011,
  B00011110
},{
  B00000000,
  B00011110,
  B00110011,
  B00110000,
  B00111110,
  B00110011,
  B00110011,
  B00011110
},{
  B00000000,
  B00111111,
  B00110011,
  B00000110,
  B00000110,
  B00001100,
  B00001100,
  B00001100
},{
  B00000000,
  B00011110,
  B00110011,
  B00110011,
  B00011110,
  B00110011,
  B00110011,
  B00011110
},{
  B00000000,
  B00011110,
  B00110011,
  B00110011,
  B00011111,
  B00000011,
  B00110011,
  B00011110
}};
const byte temp_char[][8] = {
{
  B00000000,
  B00111100,
  B01100110,
  B01101110,
  B01110110,
  B01100110,
  B01100110,
  B00111100
},{
  B00000000,
  B00011000,
  B00011000,
  B00111000,
  B00011000,
  B00011000,
  B00011000,
  B01111110
},{
  B00000000,
  B00111100,
  B01100110,
  B00000110,
  B00001100,
  B00110000,
  B01100000,
  B01111110
},{
  B00000000,
  B00111100,
  B01100110,
  B00000110,
  B00011100,
  B00000110,
  B01100110,
  B00111100
},{
  B00000000,
  B00001100,
  B00011100,
  B00101100,
  B01001100,
  B01111110,
  B00001100,
  B00001100
},{
  B00000000,
  B01111110,
  B01100000,
  B01111100,
  B00000110,
  B00000110,
  B01100110,
  B00111100
},{
  B00000000,
  B00111100,
  B01100110,
  B01100000,
  B01111100,
  B01100110,
  B01100110,
  B00111100
},{
  B00000000,
  B01111110,
  B01100110,
  B00001100,
  B00001100,
  B00011000,
  B00011000,
  B00011000
},{
  B00000000,
  B00111100,
  B01100110,
  B01100110,
  B00111100,
  B01100110,
  B01100110,
  B00111100
},{
  B00000000,
  B00111100,
  B01100110,
  B01100110,
  B00111110,
  B00000110,
  B01100110,
  B00111100
},{
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00111110,
  B00000000,
  B00000000,
  B00000000
},{
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
},{
  B00000000,
  B11000000,
  B11011110,
  B00110011,
  B00110000,
  B00110000,
  B00110011,
  B00011110
}};

void setup() {
  // put your setup code here, to run once:
  clear_displays();
  Serial.begin(115200);
  timeClient.begin();
  timeClient.setTimeOffset(-5*3600);
  init_wifi();
  var_temp = weather_request();
  timeClient.update();
  Serial.println(get_time());
  write_clock(String(get_time()));
}  
  
void loop() {
  
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= update_interval) {
    previousMillis = currentMillis;
    var_temp = weather_request();
    timeClient.update();
  }

  if (currentMillis - previousMillis < 0) {
    previousMillis = 0;
  }

  if (Serial.available() > 0) {
    Serial.println();
    Serial.println("Paused, press RST to reset.");
    delay(1000000);
  }

  if (currentMillis - r_previousMillis >= 3000) {
    r_previousMillis = currentMillis;
    if (state_flag == 0){
      clear_displays();
      write_clock(String(get_time()));
      state_flag = 1;
    }else{
      clear_displays();
      write_temp(var_temp);
      state_flag = 0;
    }
    
  }
}
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void clear_displays(){
  for(int i=0; i<4; i++){
    lc.shutdown(i,false);
    lc.setIntensity(i,3);
    lc.clearDisplay(i);
    }
}
////////////////////////////////////////////////////////////////////////////////////////
void write_clock(String t){
  int delay_time = 1;
  
  String t1 = t.substring(0,1);
  String t2 = t.substring(1,2);
  String t3 = t.substring(3,4);
  String t4 = t.substring(4,5);
  int num_feed = String(t1+t2+t3+t4).toInt();
//  Serial.println(num_feed);

  int num[4];
  num[3] = (num_feed%10000 - num_feed%1000)/1000;
  num[2] = (num_feed%1000 - num_feed%100)/100;
  num[1] = (num_feed%100 - num_feed%10)/10;
  num[0] = (num_feed%10 - num_feed%1)/1;

  for (int dev=0; dev<4; dev++)
  {
    
    switch(dev){
      case 0:
        for (int row=0; row<8; row++)
        {
          lc.setRow(dev, row, clock_digi1[num[dev]][row]);
          delay(delay_time);
         }
        break;
      case 1:
        for (int row=0; row<8; row++)
        {
          lc.setRow(dev, row, clock_digi2[num[dev]][row]);
          delay(delay_time);
         }
        break;
      case 2:
        for (int row=0; row<8; row++)
        {
          lc.setRow(dev, row, clock_digi3[num[dev]][row]);
          delay(delay_time);
         }
        break;
      case 3:
        for (int row=0; row<8; row++)
        {
          lc.setRow(dev, row, clock_digi4[num[dev]][row]);
          delay(delay_time);
         }
        break;
    }
   }
}
////////////////////////////////////////////////////////////////////////////////////////
void write_temp(String temp){
  int num[4];
  int t = round(temp.toFloat());
  if (t/10==0){
    num[3] = 11;
    if(t<0){
      num[2] = 10;  //negative, print '-'
    }
    else{
      num[2] = 11;  //positive, print nothing
    }
    num[1] = abs(t);
    num[0] = 12;    //print C sign  
  }
  else{
    if(t<0){
      num[3] = 10;  //negative, print '-'
    }
    else{
      num[3] = 11;  //positive, print nothing
    }
    num[2] = abs(t)/10;
    num[1] = abs(t)%10;
    num[0] = 12;    //print C sign 
  }
  
  
  for (int dev=0; dev<4; dev++){
    for (int row=0; row<8; row++){
      lc.setRow(dev, row, temp_char[num[dev]][row]);
//      delay(30);
     }
   }
}
////////////////////////////////////////////////////////////////////////////////////////
String get_time() {
//  String hours = String(timeClient.getHours());
//  String minutes = String(timeClient.getMinutes());
//  String str_time = String(hours+":"+minutes);
//  Serial.println(str_time);
  String str_time = timeClient.getFormattedTime();
  return str_time;
}
////////////////////////////////////////////////////////////////////////////////////////
void init_wifi() {
//  WiFi.mode(WIFI_STA);

  // Connecting to WiFi
  WiFi.begin(ssid);   Serial.print("Connecting to "); Serial.print(ssid);

  // Checking the connection status
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    // Print dots if not connected
    Serial.print(".");
  }
  Serial.println("Connected!");
  // Printing IP address
  Serial.print("IP: \t");
  Serial.println(WiFi.localIP());
  // Printing MAC address
  Serial.print("MAC: \t");
  Serial.println(WiFi.macAddress());
  
}
////////////////////////////////////////////////////////////////////////////////////////
String weather_request() {
  HTTPClient http;
  Serial.println("Begining HTTP request..");
  http.begin(urlt, darksky_fp);
  int httpCode = http.GET();
  Serial.print("HTTP Code is:\t");
  Serial.println(httpCode);
  
  if (httpCode > 0) {
    ////////////////////////////////////////////////////////////////////////////////////////
    // JSON parser code from https://arduinojson.org/v5/assistant/
    //////////////////////////////////////////////////////////////
    const size_t bufferSize = JSON_ARRAY_SIZE(11) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(19) + 560;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.parseObject(http.getString());
    // Parameters
    JsonObject& currently = root["currently"];
    const char* currently_icon = currently["icon"]; // "cloudy"
    float currently_temperature = currently["temperature"]; // -2.01
    //////////////////////////////////////////////////////////////
    // JSON parser code from https://arduinojson.org/v5/assistant/
    ////////////////////////////////////////////////////////////////////////////////////////
    
    Serial.print("Current condition:\t");
    Serial.println(currently_icon);
    Serial.print("Current Temperature:\t");
    Serial.println(currently_temperature);
    return String(currently_temperature);
  }
}
