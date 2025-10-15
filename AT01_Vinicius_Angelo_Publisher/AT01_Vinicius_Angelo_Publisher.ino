#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const String SSID = "Nome da Rede";
const String PSWD = "Senha da Rede";

const String brokerUrl = "test.mosquitto.org";              //URL do broker (servidor)
const int port = 1883;                                      //Porta do broker (servidor)

const String topic_1 = "Carrinho/Cheios/1";
WiFiClient espClient;  
PubSubClient mqttClient(espClient);
void scanLocalworks();
void connectLocalworks();
void connectBroker();
// MQTT
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 //Largura da tela em px
#define SCREEN_HEIGHT 64 //Altura da tela em px

#define I2C_SCK 6
#define I2C_SDA 5


// OLED

#include <ArduinoJson.h>


String movimento = "";
//JSON
const byte trigg_pin = 20;
const byte echo_pin = 8;
//SENSOR ULTRASONICO

int sensorPIR = 10; //Pino do sensor de presença
int leitura = 0; //Variável para armazenar a leitura do sensor
bool estadoSensor = false; //Variável para armazenar o estado do sensor
//PIR
const byte pot = 2;

//POTENCIOMETRO
unsigned long limite_atual = 0;
bool estado_funcionando = false;
void setup() {
  Serial.begin(115200); //Inicializa a comunicação serial
  pinMode(sensorPIR, INPUT); //Define sensorPin como entrada
   pinMode(pot, INPUT);

  delay(1000);
//JSON
  pinMode(echo_pin, INPUT);
  pinMode(trigg_pin, OUTPUT);
//SENSOR ULTRASONICO
  scanLocalworks();
  connectLocalworks();
  connectBroker();
//MQTT
  Wire.begin(I2C_SDA,I2C_SCK); //Inicia comunicação I2C
  delay(5000);
//OLED
}

void loop() {
  unsigned long atual = millis ();
  estado_funcionando = true;
  if(atual - limite_atual > 5000 && estado_funcionando == true){
    limite_atual = atual;
    String mensagem = "";
  
  // JsonDocument doc;

    leitura = digitalRead(sensorPIR); //Realiza a leitura do sensor de presença
  
    //PIR
    digitalWrite(trigg_pin, LOW); 
    delayMicroseconds(10);
    digitalWrite(trigg_pin, HIGH); 
    delayMicroseconds(10);
    digitalWrite(trigg_pin, LOW); 
    delayMicroseconds(10);

  // calcula o tempo da ida + volta do pulso sonoro
  
    unsigned long duracao = pulseIn(echo_pin, HIGH);
  
    int distancia_cm = ((duracao * 340)/2)/10000;
  
  //SENSOR ULTRASONICO


    int limiar_pot = analogRead(pot);

    if(leitura == HIGH){
      
      movimento = true;
      
    }else{
      
      movimento = false; 
      }
  
  //POTENCIOMETRO
    StaticJsonDocument<200> doc;
    doc["distancia_cm"] = distancia_cm;
    doc["movimento"] = movimento;
    doc["limiar_pot"] = limiar_pot;
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

//MQTT
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

    Serial.print("Conectado com sucesso!");
  }
