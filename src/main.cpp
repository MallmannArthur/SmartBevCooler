#define BLYNK_TEMPLATE_ID "TMPLXbeVR6Ni" // Id do template, valor vindo do APP
#define BLYNK_DEVICE_NAME "Quickstart Template" //Nome do template, valor vindo do APP
#define BLYNK_AUTH_TOKEN "2jNRi3NAn20VXR-FKHfpKbwI150fq3hR" //Token de autenticacao, valor vindo do APP
#include <OneWire.h> //biblioteca para sensores digitais
#include <DallasTemperature.h> //biblioteca para sensor de temperatura
#include <ESP8266WiFi.h> //Usar ESP8266 como arduino
#include <BlynkSimpleEsp8266.h> //Biblioteca basica do Blynk
//Definicoes estaticas para uso das bibliotecas dos sensores e do Blynk
#define BLYNK_PRINT Serial 
// IMPORTANTE! Nao confundir GPIO com pinos. Ver imagem de pinagem da placa NodeMCU com o ESP8266
#define SENSOR_LEVEL10 12 //GPIO 12, pino ligado ao sensor é D6 e GND 
#define SENSOR_LEVEL50 14 //GPIO 14, pino ligado ao sensor é D5 e GND
#define ONE_WIRE_BUS 4    //GPIO  4, pino ligado ao atuador é D2
#define RELAY 2           //GPIO  2, pino de saída somente, pino D4
 

// Declaracao de variaveis globais
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

// Essa funcao e chamada sempre que o valor de V0 muda. V0 eh um pino virtual,
// e portanto existe fora da placa fisica, e dentro do APP do Blynk. Quando 
// valores sao escritos nesse pino, eles sao na verdade enviados via internet para
// o pino virtual que eh lido pelo APP
BLYNK_WRITE(V0)
{
  // Atribuir valor recem alterado do pino V0 a uma variavel de escopo
  int value = param.asInt();

  // Atualizando estado do pino
  Serial1.write(value);
}

// Essa funcao eh chamada sempre que o dispositivo eh conectado ao Blynk.Cloud
BLYNK_CONNECTED()
{
  // Muda a mensagem do botao virtual para  "Congratulations!" 
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
  // ainda ha muito do app original no que tempos no SmartBevCooler
  //------------ OBJETIVO-----------

  // Melhorar a interface do app, incluir imagem que indique que eh o software do SMC no lugar
  // da faixa de parabens
}


// Essa funcao funciona periodicamente lendo os valores de temperatura.
void myTimerEvent()
{
  // Os valores não podem ser atualizados a qualquer momento por haver um limite
  // de 10 envios de mensagem por segundo ao Blynk para garantia de funcionamento da conexao.
  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);
  if (t <= 24) { // gatilhos criados para facil teste a temperatura ambiente. Trocar valores posteriormente
    digitalWrite(RELAY, LOW); // Atuacao do Rele
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


void setup() //Setup padrao, inicia os servicos de cada classe
{
  
  Serial.begin(115200); // valor deve ser alterado para uso com a IDE do Arduino. No 
                        // PlatformIO, como foi utilizado, basta conversar com definicoes 
                        // no arquivo platformio.ini 
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