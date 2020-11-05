#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "cactus_io_BME280_I2C.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
 
// set up pins we are going to use to talk to the screen
#define TFT_DC     D8       // register select (stands for Data Control perhaps!)
#define TFT_RST   D4         // Display reset pin, you can also connect this to the Arduino reset
                            // in which case, set this #define pin to -1!
#define TFT_CS   D3       // Display enable (Chip select), if not enabled will not talk on SPI bus


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
BME280_I2C bme(0x76);

float temperature, humidity, pressure;

/*Put your SSID & Password*/
const char* ssid = "I come from a LAN down under";  // Enter SSID here
const char* password = "StemzellenBruecke74!";  //Enter Password here

ESP8266WebServer server(80);              

///int16_t c_x, c_y;
int16_t startXt, startYt, startXf, startYf, startXl, startYl;
 
void setup() {
  Serial.begin(9600);

  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip,
  tft.setRotation(3);
  tft.fillScreen(ST7735_BLACK);
  
  delay(100);
  
  bme.begin();   
  bme.setTempCal(-2);

  tft.setTextSize(1);
  tft.setCursor(0,0);
  tft.setTextColor(ST7735_WHITE);
  tft.print("Connecting to ");
  tft.print(ssid);
  Serial.println("\nConnecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  tft.print(".");
  Serial.print(".");
  }

  tft.println("");
  tft.println("WiFi connected!");
  tft.print("IP: "); tft.println(WiFi.localIP());
  tft.println();
  tft.getCursor(&startXt, &startYt);
  //c_y = startY;
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.println("IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

  tft.print("Temperatur: ");
  tft.getCursor(&startXt, &startYt);
  tft.print("\nLuftfeuchtigkeit: ");
  tft.getCursor(&startXf, &startYf);
  tft.print("\nLuftdruck: ");
  tft.getCursor(&startXl, &startYl);
}
void loop() {
  server.handleClient();
  bme.readSensor();
  temperature = bme.getTemperature_C();
  humidity = bme.getHumidity();
  pressure = bme.getPressure_MB();
  
  tft.fillRect(startXt, startYt, 100, 8, ST7735_BLACK);
  tft.fillRect(startXf, startYf, 60, 8, ST7735_BLACK);
  tft.fillRect(startXl, startYl, 110, 8, ST7735_BLACK);
  tft.setCursor(startXt, startYt);
  tft.print(String(temperature)+(char)247+"C");
  tft.setCursor(startXf, startYf);
  tft.print(String(humidity)+"%");
  tft.setCursor(startXl, startYl);
  tft.print(String(pressure)+"mb");
  delay(1000);
}

void handle_OnConnect() {
  bme.readSensor();
  temperature = bme.getTemperature_C();
  humidity = bme.getHumidity();
  pressure = bme.getPressure_MB();
  server.send(200, "text/html", SendHTML(temperature,humidity,pressure));
  /*String clientAddress = server.client().remoteIP().toString();
  uint16_t clientPort = server.client().remotePort();
  String con = clientAddress + ":" + clientPort + " con.!";
  if(c_y > 120)
    tft.setCursor(startX, startY);
  tft.getCursor(&c_x, &c_y);
  tft.fillRect(c_x, c_y, 160, 8, ST7735_BLACK);
  tft.println(con);
  tft.getCursor(&c_x, &c_y);*/
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float temperature,float humidity,float pressure){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Weather Station</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>Trockner Wetterstation</h1>\n";
  ptr +="<p>Temperatur: ";
  ptr +=temperature;
  ptr +="&deg;C</p>";
  ptr +="<p>Luftfeuchtigkeit: ";
  ptr +=humidity;
  ptr +="%</p>";
  ptr +="<p>Luftdruck: ";
  ptr +=pressure;
  ptr +="mb</p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
