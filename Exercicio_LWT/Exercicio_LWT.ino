#include <WiFi.h>
#include <PubSubClient.h>
const String SSID = "Vini@@";
const String PSWD = "viniciusA";

const String brokerUrl = "test.mosquitto.org";              //URL do broker (servidor)
const int port = 1883;                                      //Porta do broker (servidor)

const String LWTTopic = "TDESI1v1/Placa100/status";
const String LWTTopic_2 = "TDESI1v1/Placa200/status";

const String LWTMessage = "Placa 1 : OFFLINE";
const int  LWTQoS = 1;
//OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 //Largura da tela em px
#define SCREEN_HEIGHT 64 //Altura da tela em px

#define I2C_SCK 8
#define I2C_SDA 10
Adafruit_SSD1306 tela(SCREEN_WIDTH,SCREEN_HEIGHT, &Wire, -1);
//OLED

WiFiClient espClient;                                       //Criando Cliente WiFi
PubSubClient mqttClient(espClient);                         //Criando Cliente MQTT
void scanLocalworks();
void connectLocalworks();
void connectBroker();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  scanLocalworks();
  connectLocalworks();
  connectBroker();
  
  Wire.begin(I2C_SDA,I2C_SCK); //Inicia comunicação I2C
  tela.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  tela.clearDisplay();
  tela.setTextSize(1.0);
  tela.setTextColor(SSD1306_WHITE);
  tela.setCursor(0,0);
  tela.println("Hello World!");
   tela.display();
   delay(100);
   tela.clearDisplay();
   delay(100);
}


void loop() {

if(WiFi.status() != WL_CONNECTED){
  Serial.print("Conexão Perdida\n");
  connectLocalworks(); 
}
if(!mqttClient.connected()){
  Serial.println("Erro de Conexão no Broker");
  connectBroker();
}
mqttClient.publish("AulaIoTSul/Chat","oi - alves9");
delay(1000);
mqttClient.loop();
tela.setCursor(10,10);
tela.println("OLÁ");

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
  
  while(!mqttClient.connected()){
    mqttClient.connect(userId.c_str(),"","",LWTTopic.c_str(),LWTQoS,true,LWTMessage.c_str());
    Serial.println(".");
    delay(200);
  }
    
    mqttClient.publish(LWTTopic.c_str(),"PLACA 1: ONLINE",true);
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

  delay(100);
  tela.setCursor(0,0);
  tela.println("Sistema de Manutenção");
  tela.setCursor(20,10);
  tela.println(resposta);
  tela.setCursor(30,5);
  tela.println(LWTMessage);
  tela.display();
  delay(100);
  tela.clearDisplay();

}
