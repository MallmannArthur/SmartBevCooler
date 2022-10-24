#define BLYNK_TEMPLATE_ID "TMPLXbeVR6Ni"
#define BLYNK_DEVICE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "2jNRi3NAn20VXR-FKHfpKbwI150fq3hR"
#include <OneWire.h>
#include <DallasTemperature.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define SENSOR_LEVEL10 12 //pino ligado ao sensor é D4 e GND
#define SENSOR_LEVEL50 14 //pino ligado ao sensor é D5 e GND
#define ONE_WIRE_BUS 4//pino ligado ao atuador é D2
#define RELAY 2
 

// Setting up Global variables
bool flag_nivel10_baixo = false;
bool flag_nivel10_acima = false;
bool flag_nivel50_baixo = false;
bool flag_nivel50_acima = false;
unsigned long time_nivel10 = 0;
unsigned long lasttime_nivel10 = 0;
unsigned long time_nivel50 = 0;
unsigned long lasttime_nivel50 = 0;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "arthur"; 
char pass[] = "arthur01";
BlynkTimer timer;
WidgetLED led50(V5);
WidgetLED led10(V6);

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Serial1.write(value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);
  if (t <= 24) {
    digitalWrite(RELAY, LOW);
  } 
  if (t>= 25 ){
    digitalWrite(RELAY, HIGH);
  }
  Blynk.virtualWrite(V4, t);
}

 void IRAM_ATTR level10ISR() { //interrupt para detectar mudança, apenas levanta flag, prints e dalays fora para não causar wdt timeout
  time_nivel10 = millis(); //milis retorna tempo de execução atual reseta em ~50d
  if (time_nivel10 - lasttime_nivel10 > 100) {
    if (digitalRead(SENSOR_LEVEL10) == HIGH) {
      flag_nivel10_baixo = true;
      led10.on();
    } else {
    flag_nivel10_acima = true;
    led10.off();
  }
  lasttime_nivel10 = time_nivel10;
  }
 }

  void IRAM_ATTR level50ISR() {
 time_nivel50 = millis();
 if (time_nivel50 - lasttime_nivel50 > 100) {
 if (digitalRead(SENSOR_LEVEL50) == HIGH) {
 flag_nivel50_baixo = true;
 led50.on();
 } else {
 flag_nivel50_acima = true;
 led50.off();
 }
 lasttime_nivel50 = time_nivel50;
 }
 }


void setupLevelSensors() {
 pinMode(SENSOR_LEVEL10, INPUT_PULLUP);
 attachInterrupt(SENSOR_LEVEL10, level10ISR, CHANGE);
 pinMode(SENSOR_LEVEL50, INPUT_PULLUP);
 attachInterrupt(SENSOR_LEVEL50, level50ISR, CHANGE);
 }

 void checkLevelSensors() {
 if (flag_nivel10_baixo == true) {
 flag_nivel10_baixo = false;
 Serial.println("abaixo 10");
 }
 if (flag_nivel10_acima == true) {
 flag_nivel10_acima = false;
 Serial.println("acima 10");
 }
 if (flag_nivel50_baixo == true) {
 flag_nivel50_baixo = false;
 Serial.println("abaixo 50");
 }
 if (flag_nivel50_acima == true) {
 flag_nivel50_acima = false;
 Serial.println("acima 50");
 }
 }


void setup()
{
  
  Serial.begin(115200);
  void setupLevelSensors();
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  checkLevelSensors();
}