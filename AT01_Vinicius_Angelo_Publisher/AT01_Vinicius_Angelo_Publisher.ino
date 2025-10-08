#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const String SSID = "Paulo";
const String PSWD = "cxos9674";

const String brokerUrl = "test.mosquitto.org";              //URL do broker (servidor)
const int port = 1883;                                      //Porta do broker (servidor)

const String topic_1 = "Carrinho/Cheio/1";
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

Adafruit_SSD1306 tela(SCREEN_WIDTH,SCREEN_HEIGHT, &Wire, -1);
// OLED
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#define LEDPIN 8
#define LEDNUM 1
#define LEDMODEL NEO_GRB + NEO_KHZ800
Adafruit_NeoPixel Neo(LEDNUM,LEDPIN,LEDMODEL);
//JSON
const byte trigg_pin = 20;
const byte echo_pin = 8;
//SENSOR ULTRASONICO

int sensorPIR = 20; //Pino do sensor de presença
int leitura = 0; //Variável para armazenar a leitura do sensor
bool estadoSensor = false; //Variável para armazenar o estado do sensor
//PIR
const byte pot = 2;

//POTENCIOMETRO

void setup() {
  Serial.begin(115200); //Inicializa a comunicação serial
  pinMode(sensorPIR, INPUT); //Define sensorPin como entrada
   pinMode(pot, INPUT);

  Neo.begin();
  Neo.show();
  Serial.begin(115200);
  while (!Serial)
  continue;
  JsonDocument doc;
  

  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
//JSON
  pinMode(echo_pin, INPUT);
  pinMode(trigg_pin, OUTPUT);
//SENSOR ULTRASONICO
  scanLocalworks();
  connectLocalworks();
  connectBroker();
//MQTT
  Wire.begin(I2C_SDA,I2C_SCK); //Inicia comunicação I2C
  tela.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  tela.clearDisplay();
  tela.setTextSize(1.5);
  tela.setTextColor(SSD1306_WHITE);
  tela.setCursor(0,0);
  tela.println("I ALWAYS COME BACK!!!!!!!");
  tela.display();
  delay(5000);
//OLED
}

void loop() {
  leitura = digitalRead(sensorPIR); //Realiza a leitura do sensor de presença
  //PIR
  unsigned long duracao = pulseIn(echo_pin, HIGH);
  int distancia = ((duracao * 340)/2)/10000;
  //SENSOR ULTRASONICO
   Serial.println(analogRead(pot));
  //POTENCIOMETRO
  if(Serial.available() > 0){
  String mensagem = leitura.c_str();

  mqttClient.publish("Carrinho/Cheio/1",mensagem.c_str());

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
//MQTT

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
    mqttClient.subscribe("Carrinho/Cheio/1");
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
  tela.clearDisplay();
  tela.setCursor(0,0);
  tela.println(resposta);
  tela.display();
}