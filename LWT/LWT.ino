#include <WiFi.h>
#include <PubSubClient.h>
const String SSID = "Vini@@";
const String PSWD = "viniciusA";

const String brokerUrl = "test.mosquitto.org";              //URL do broker (servidor)
const int port = 1883;                                      //Porta do broker (servidor)

const String LWTTopic = "Placa1/status";
const String LWTMessage = "offline - A mimir";
const int  LWTQoS = 1;

const String topic_1 = "Carrinho/Cheios/1";

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
    mqttClient.publish(LWTTopic.c_str(),"online - AL 9",true);
    Serial.print("Conectado com sucesso!");
  }
