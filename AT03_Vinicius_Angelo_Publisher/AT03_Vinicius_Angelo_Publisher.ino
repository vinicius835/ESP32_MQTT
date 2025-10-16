#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const String SSID = "Nome da Rede";
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
const byte trigg_pin = 20;
const byte echo_pin = 21;
//SENSOR ULTRASONICO
String status  = "";
bool estado_millis = false;
bool estado_millis_loop = false;
unsigned long negativo_atual = 0;
unsigned long negativo_atual_loop = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //Inicializa a comunicação serial
  
   pinMode(pot, INPUT);

//JSON

  scanLocalworks();
  connectLocalworks();
  connectBroker();
//MQTT


 pinMode(echo_pin, INPUT);
  pinMode(trigg_pin, OUTPUT);
//Sensor UltraSonico


}

void loop() {
  // put your main code here, to run repeatedly:
    unsigned long atual =millis();
    unsigned long atual_loop =millis();
    if(atual_loop - negativo_atual_loop >1000 && estado_millis_loop == true){
    negativo_atual_loop = atual;
    digitalWrite(trigg_pin, LOW); 
    delayMicroseconds(10);
    digitalWrite(trigg_pin, HIGH); 
    delayMicroseconds(10);
    digitalWrite(trigg_pin, LOW); 
    delayMicroseconds(10);

    unsigned long duracao = pulseIn(echo_pin, HIGH);

    int sonico = ((duracao * 340)/2)/10000;

  //Sensor UltraSonico
   int distancia_det_pot = map(analogRead(pot),15,4095,15,400);
   Serial.printf("distancia feita pelo potenciometro",distancia_det_pot);
   Serial.println(distancia_det_pot);
   Serial.println();
    if(sonico < distancia_det_pot){
    //if(sonico < 15){
  
    Serial.print("Distancia Detectada");
    negativo_atual = atual;
    status = "proximidade_detectada";
    //}else if(sonico > 15 && atual - negativo_atual > 3000 && estado_millis == true){
      }else if(sonico > distancia_det_pot && atual - negativo_atual > 3000 && estado_millis == true){

  
      negativo_atual = atual;
      estado_millis = false;
      status = "area_livre";
      
    
      Serial.println("Area Libre");
    }

    StaticJsonDocument<200> doc;
    doc["status"] = status;
    doc["distancia"] = sonico;
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
    estado_millis = true;
  }
    estado_millis_loop = true;
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
   
    Serial.print("Conectado com sucesso!");
  }
