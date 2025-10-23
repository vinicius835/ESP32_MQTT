
//Nome: Bruno Davi Dutka
//Turma: TDESI-V1
//Grupo: Bruno, Paulo e Vinicius Angelo

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128  //Largura da tela em pixels
#define SCREEN_HEIGHT 64  //Altura da tela em pixels

#define I2C_SCK 6
#define I2C_SDA 5

Adafruit_SSD1306 tela(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const String SSID = "Vini@@";
const String PSW = "viniciusA";

const String brokerURL = "test.mosquitto.org"; //URL do broker    (servidor)
const int porta = 1883;                        //Porta do broker  (servidor)

const String topic_1 = "Carrinhos/Cheios/1";

//Carrinhos/Cheios/1

JsonDocument doc;

const int y = 10;
const int g = 11;
int ledG = 0;
int ledY = 0;


WiFiClient espClient;                 //Criando Cliente WiFi
PubSubClient mqttClient(espClient);   //Criando Cliente MQTT

void conexaoWifi();
void conexaoBroker();
void reconexaoWifi();
void reconexaoBroker();
void mandarMensagem();

void setup() {
  Wire.begin(I2C_SDA, I2C_SCK);  //Inicia comunicação I2C
  tela.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  tela.clearDisplay();
  tela.setTextSize(1);
  tela.setTextColor(SSD1306_WHITE);
  tela.display();
  Serial.begin(115200);
  conexaoWifi();
  conexaoBroker();
  pinMode(g, OUTPUT);
  pinMode(y, OUTPUT);
  delay(1000);
  tela.clearDisplay();
  tela.display();
}

void loop() {
  mqttClient.loop();
  reconexaoWifi();
  reconexaoBroker();
}

void conexaoWifi(){
  Serial.println("Iniciando conexão com rede WiFi");
  while(WiFi.status() != WL_CONNECTED){
    WiFi.begin(SSID, PSW);
    Serial.print(".");
    delay(400);
    Serial.print(".");
    delay(400);
    Serial.println(".");
    delay(5000);
  }
  Serial.println("Rede de Wi-Fi conectada");
}

void reconexaoWifi(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Conexão de rede perdida!!!");
    conexaoWifi();
  }
}

void conexaoBroker(){
  Serial.println("Conectando ao broker...");
  mqttClient.setServer(brokerURL.c_str(), porta);
  String userId = "ESP_BRUNO";
  userId += String(random(0xffff), HEX);
  while(!mqttClient.connected()){
    mqttClient.connect(userId.c_str());
    Serial.print(".");
    delay(400);
    Serial.print(".");
    delay(400);
    Serial.println(".");
    delay(2000);
  }
  mqttClient.subscribe(topic_1.c_str());
  mqttClient.setCallback(callback);
  Serial.println("Conectado com sucesso!");
}

void callback(char* topic_1, byte* payload, unsigned long length){
  String resposta = "";
  for(int i = 0;  i < length; i++){
    resposta += (char) payload[i];
  }
  Serial.println(resposta);
  DeserializationError error = deserializeJson(doc, resposta);

  if(!error){
    String status = doc["status"];
    
    if(status == "na_linha"){
      tela.clearDisplay();
      tela.setCursor(40,20);
      tela.print("STATUS");
      tela.setCursor(20,40);
      tela.print("Robo na linha");
      tela.display();
      analogWrite(g, 255);
      analogWrite(y, 0);
    }
    else if(status == "fora_da_linha"){
      tela.clearDisplay();
      tela.setCursor(40,20);
      tela.print("STATUS");
      tela.setCursor(14,40);
      tela.print("Robo fora da linha");
      tela.display();
      analogWrite(g, 0);
      analogWrite(y, 255);
    }
    else{
      tela.clearDisplay();
      tela.setCursor(20,40);
      tela.print("Comando invalido!!");
      tela.display();
      analogWrite(g, 0);
      analogWrite(y, 0);
    }
  } 
}

void reconexaoBroker(){
  if(!mqttClient.connected()){
      Serial.println("Conexão com o broker perdida!!!");
      conexaoBroker();
    }
}