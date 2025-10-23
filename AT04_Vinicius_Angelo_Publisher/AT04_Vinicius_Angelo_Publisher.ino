#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const String SSID = "Vini@@";
const String PSWD = "viniciusA";

const String brokerUrl = "test.mosquitto.org";              //URL do broker (servidor)
const int port = 1883;                                      //Porta do broker (servidor)

const String topic_1 = "Carrinho/Cheios/1";
WiFiClient espClient;  
PubSubClient mqttClient(espClient);
void scanLocalworks();
void connectLocalworks();
void connectBroker();
// MQTT
byte pino_PIR = 15;
byte leitura_PIR = 0; //Variável para armazenar a leitura do sensor
const byte pino_LED = 20;
//PIR

String evento ="";
unsigned long atual_ciclo = 0;
bool estado_millis = false;
byte ciclo = 0;
byte alerta = 0;
//VARIAVEIS
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pino_PIR,INPUT);
  pinMode(pino_LED, OUTPUT);
  digitalWrite(pino_LED, LOW);

}

void loop() {
  unsigned long atual = millis();
  // put your main code here, to run repeatedly:
  if(atual - atual_ciclo > 400 && estado_millis == true ){
  atual_ciclo = atual;
  estado_millis = false;
  leitura_PIR = digitalRead(pino_PIR);
  if(leitura_PIR == HIGH){
    
    Serial.println("movimento detectado");
    evento = "movimento_detectado";
    digitalWrite(pino_LED, HIGH);
    delay(500);
    alerta = alerta +1;
  } else if(alerta >4){
    pisca_pisca();
    delay(500);
    pisca_pisca();
    Serial.println("movimento insistente");
    evento = "movimento_insistente";
  }else{
    Serial.println("nada detectado");
    evento = "nada_detectado";
    delay(100);
 
  }

  StaticJsonDocument<200>doc;
    doc["evento"] = evento;
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
  }
  ciclo= ciclo +1;
  if(ciclo > 15 ){
    alerta = 0;
    ciclo = 0;
    delay(10);
  }
  estado_millis = true;
  delay(100);
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
    mqttClient.subscribe("Carrinho/Cheios/2");
    mqttClient.setCallback(callback);
    Serial.print("Conectado com sucesso!");
  }
  void callback(char* topic, byte* payload, unsigned long length){
  String resposta = "";
  for(int i = 0; i < length;i++){
    // ele vai pegar um byte e  e transformar em letra
    resposta += (char) payload[i];
  }
 StaticJsonDocument<200>doc;
 DeserializationError error = deserializeJson(doc, resposta);
  if(!error){
    
    String limiar = doc["comando"];
    Serial.println("...");
    if(limiar == "reset"){
      digitalWrite(pino_LED, LOW);
      delay(500);
      Serial.println("Led Desligado");
    }
  
}
}
void pisca_pisca(){
  digitalWrite(pino_LED,HIGH);
  delay(500);
  digitalWrite(pino_LED,LOW);
  delay(500);
  digitalWrite(pino_LED,HIGH);
  delay(500);
  digitalWrite(pino_LED,LOW);
  delay(500);
  digitalWrite(pino_LED,HIGH);
  delay(500);
  digitalWrite(pino_LED,LOW);
  delay(500);
}
