#include <WiFi.h>
#include <PubSubClient.h>
const String SSID = "Vi@@";
const String PSWD = "vinicius";

const String brokerUrl = "test.mosquitto.org";              //URL do broker (servidor)
const int port = 1883;                                      //Porta do broker (servidor)

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
if(Serial.available() > 0){
  String mensagem = Serial.readStringUntil('\n');
  mensagem = "Vinicius A: " + mensagem;
  mqttClient.publish("AulaIoTSul/Chat",mensagem.c_str());

}
if(WiFi.status() != WL_CONNECTED){
  Serial.print("Conexão Perdida\n");
  connectLocalworks(); 
}
if(!mqttClient.connected()){
  Serial.println("Erro de Conexão no Broker");
  connectBroker();
}


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
    mqttClient.connect(userId.c_str());
    Serial.println(".");
    delay(200);
  }
    mqttClient.subscribe("AulaIoTSul/Chat");
    mqttClient.setCallback(callback);

    Serial.print("Conectado com sucesso!");
  }
void callback(char* topic, byte* payload, unsigned long length){
  String resposta = "";
  for(int i = 0; i < length;i++){
    // ele vai pegar um byte e  e transformar em letra
    resposta += (char) payload[i];
  }
  // Serial.println("Recebido: " + resposta);
  Serial.println(resposta);
}