#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const String SSID = "Rede Wifi";
const String PSWD = "Senha";

const String brokerUrl = "test.mosquitto.org";              //URL do broker (servidor)
const int port = 1883;                                      //Porta do broker (servidor)

const String topic_1 = "Carrinho/Cheios/1";
WiFiClient espClient;  
PubSubClient mqttClient(espClient);
void scanLocalworks();
void connectLocalworks();
void connectBroker();
// MQTT
#include <ArduinoJson.h>
String movimento = "";
//JSON

const byte pot = 2;
int angulo = 0;
//POTENCIOMETRO

unsigned long limite_atual = 0;
bool estado_funcionando = false;
void setup() {
  Serial.begin(115200); //Inicializa a comunicação serial

   pinMode(pot, INPUT);

//JSON

  scanLocalworks();
  connectLocalworks();
  connectBroker();
//MQTT



}

void loop() {
unsigned long atual = millis();
estado_funcionando = true;
if(atual - limite_atual >1500 && estado_funcionando == true){
    limite_atual = atual;
    int angulo = map(analogRead(pot),0,4095,0,180);


    StaticJsonDocument<200> doc;
    doc["angulo"] = angulo;
    char buffer[200];
    serializeJson(doc,buffer);

  
  //JSON - ENVIAR
    mqttClient.publish("Carrinho/Cheios/1",buffer);
    Serial.println("mensagem enviada");
    Serial.println(buffer);
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Conexão Perdida\n");
      connectLocalworks(); 
    }
    if(!mqttClient.connected()){
      Serial.println("Erro de Conexão no Broker");
      connectBroker();
    }
    mqttClient.loop();
estado_funcionando = false;
}
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
  
  }
}
void connectBroker(){
  Serial.println("Conectando ao broker");
  mqttClient.setServer(brokerUrl.c_str(), port);
  String userId = "ESP-alves9";
  userId += String(random(0xffff), HEX);
  
  while(!mqttClient.connected()){
    mqttClient.connect(userId.c_str());
    Serial.println(".");
    delay(200);
  }
    mqttClient.subscribe("Carrinho/Cheios/1");
82
