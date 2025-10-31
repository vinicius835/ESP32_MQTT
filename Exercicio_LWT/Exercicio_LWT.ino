#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


const String SSID = "Vini@@";
const String PSWD = "viniciusA";

const String brokerUrl = "test.mosquitto.org";              //URL do broker (servidor)
const int port = 1883;                                      //Porta do broker (servidor)

const String LWTTopic = "TDESI1v1/Placa1/vini_paulo/status";
const String LWTTopic_2 = "TDESI1v1/Placa2/bruno/status";

const String LWTMessage = "Offline";
const int  LWTQoS = 1;
bool estado_status = false;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"pool.ntp.org",3600,60000);


//OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 //Largura da tela em px
#define SCREEN_HEIGHT 64 //Altura da tela em px

#define I2C_SCK 22
#define I2C_SDA 21
Adafruit_SSD1306 tela(SCREEN_WIDTH,SCREEN_HEIGHT, &Wire, -1);
//OLED
// String placa2 = "[NADA_CONECTADO]";

WiFiClient espClient;                                       //Criando Cliente WiFi
PubSubClient mqttClient(espClient);                         //Criando Cliente MQTT
void scanLocalworks();
void connectLocalworks();
void connectBroker();

StaticJsonDocument<200>doc;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  scanLocalworks();
  connectLocalworks();
  connectBroker();
  
  Wire.begin(I2C_SDA,I2C_SCK); //Inicia comunicação I2C
  tela.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  tela.clearDisplay();
  tela.setTextSize(0.1);
  tela.setTextColor(SSD1306_WHITE);
  tela.setCursor(0,0);
  tela.println("Aguarde...");
   tela.display();
   delay(100);

   timeClient.begin();
   timeClient.setTimeOffset(-10800);
}


void loop() {
timeClient.update();

if(WiFi.status() != WL_CONNECTED){
  Serial.print("Conexão Perdida\n");
  connectLocalworks(); 
}
if(!mqttClient.connected()){
  Serial.println("Erro de Conexão no Broker");
  connectBroker();
}

delay(1000);
mqttClient.loop();


}


void connectLocalworks(){
Serial.println("Iniciando conexão com rede WiFi");

  while (WiFi.status() != WL_CONNECTED){

    WiFi.begin(SSID,PSWD);
    Serial.print(".");
    delay(2000);
    
  }
  Serial.println("\nConectado!");
}
void scanLocalworks(){
  Serial.println("Iniciando Scan de redes Wi- Fi");
  int number = WiFi.scanNetworks();
  delay (500);
  if(number == -1)  {
    Serial.println("ERRO!. Não deu boa");
  }
  else{
    for(int net = 0; net < number; net++){
      Serial.printf("cont: %d", net);
      Serial.printf("---- %d - %s | %d db\n",net,WiFi.SSID(net),WiFi.RSSI(net));
      
    }
    // Serial.printf("Número de redes encontradas: %d\n", number);
  }
}
void connectBroker(){
  Serial.println("Conectando ao broker");
  mqttClient.setServer(brokerUrl.c_str(), port);
  String userId = "ESP-alves9";
  userId += String(random(0xffff), HEX);

  doc["status"] = LWTMessage;
  char buffer[200];
  serializeJson(doc,buffer);

  while(!mqttClient.connected()){
    mqttClient.connect(userId.c_str(),"","",LWTTopic.c_str(),LWTQoS,true,buffer);
     
    Serial.println(".");
    delay(200);
  }
    
    doc["status"] = "Online";
    serializeJson(doc, buffer);

    mqttClient.publish(LWTTopic.c_str(),buffer,true);
    Serial.println("Conectado com sucesso no Broker! - Placa 1");
    mqttClient.subscribe(LWTTopic_2.c_str(),1);
    mqttClient.setCallback(callback);
    Serial.print("Conectado com sucesso!");
  }
    
    void callback(char* LWTtopic_2, byte* payload, unsigned long length){
  String resposta = "";
  for(int i = 0; i < length;i++){
    // ele vai pegar um byte e  e transformar em letra
    resposta += (char) payload[i];
  }
  StaticJsonDocument<200>doc;
  DeserializationError error = deserializeJson(doc, resposta);
  if(!error){
    
    String placa2 = doc["status"];
    oled(placa2);
    if(!mqttClient.connected() || WiFi.status() != WL_CONNECTED){
    Serial.print("Foi");
    }
    else{
      Serial.print("Não Foi");
    }

  }
}


void oled(String placa2){
 Serial.println("...");
    tela.clearDisplay();
    timeClient.update();
    delay(100);
    tela.setCursor(0,0);
    tela.print("Sistema Monitoramento");
    tela.setCursor(0,15);
    tela.print("Placa 1: ");
    if(!mqttClient.connected() || WiFi.status() != WL_CONNECTED){
      tela.print("Offline");
    }
    else{tela.print("Online");
    }

    tela.setCursor(0,25);
    tela.print("Placa 2: ");
    tela.println(placa2);
    tela.setCursor(0,35);
    tela.print("-----------");
    tela.setCursor(0,45);
    tela.print("Ultima Atualizacao");
    tela.setCursor(0,55);
    tela.print(timeClient.getFormattedTime());
    tela.display();
    delay(700);

    tela.display();
}
