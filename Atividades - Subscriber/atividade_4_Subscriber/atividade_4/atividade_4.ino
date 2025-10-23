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

const String topic_1 = "Carrinho/Cheios/1";
const String topic_2 = "Carrinho/Cheios/2";

//Carrinho/Cheios/1

JsonDocument doc;

const int led = 10;
const byte pot = 2;

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
  pinMode(led, OUTPUT);
  delay(1000);
  tela.clearDisplay();
  tela.display();
  pinMode(pot, INPUT);
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
  int time = map(analogRead(pot), 0, 4096, 0, 30000);
  String resposta = "";
  for(int i = 0;  i < length; i++){
    resposta += (char) payload[i];
  }
  Serial.println(resposta);
  DeserializationError error = deserializeJson(doc, resposta);

  unsigned long ledTime = millis();

  if(!error){
    String evento = doc["evento"];
    
    if(evento == "movimento_detectado"){
      tela.clearDisplay();
      tela.setCursor(20,0);
      tela.print("!!!ALERTA!!!");
      tela.setCursor(0,20);
      tela.print("Movimento detectado!!");
      tela.setCursor(0, 40);
      tela.print("Delay:");
      tela.print(time);
      tela.display();
      analogWrite(led, 255);
      delay(time);

      StaticJsonDocument<200> doc;
      doc["comando"] = "reset";
      char buffer[200];
      serializeJson(doc, buffer);
      mqttClient.publish(topic_2.c_str(), buffer);

      Serial.println(time);
      Serial.println(buffer);
      tela.clearDisplay();
      tela.display();
      analogWrite(led, 0);
    }
    else if(evento == "movimento_insistente"){
      tela.clearDisplay();
      tela.setCursor(20,0);
      tela.print("!!!ALERTA!!!");
      tela.setCursor(0,20);
      tela.print("Movimento detectado!!");
      tela.setCursor(32, 40);
      tela.print("Delay:");
      tela.print(time);
      tela.display();
      analogWrite(led, 255);
      int sla = time / 300;
      for(int i = 0; i < sla; i++){
        unsigned long atual = millis();
        if(atual > ledTime + 300){
          digitalWrite(led,!digitalRead(led));
          ledTime = atual;
        }
      }
      Serial.println(sla);
      StaticJsonDocument<200> doc;
      doc["comando"] = "reset";
      char buffer[200];
      serializeJson(doc, buffer);
      mqttClient.publish(topic_2.c_str(), buffer);
      Serial.println("Mensagem enviada!!");
      tela.clearDisplay();
      tela.display();
      analogWrite(led, 0);
    }
    else if(evento == "nada_detectado"){
      tela.clearDisplay();
      tela.setCursor(0, 32);
      tela.print("Nenhum movimento!!!");
      tela.display();
      analogWrite(led, 0);
      }
  } 
}

void reconexaoBroker(){
  if(!mqttClient.connected()){
      Serial.println("Conexão com o broker perdida!!!");
      conexaoBroker();
    }
}