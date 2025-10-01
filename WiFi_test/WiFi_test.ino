#include <WiFi.h>
const String SSID = "Vi@@";
const String PSWD = "vinicius";
unsigned long previousMillis = 0;
unsigned long interval = 10000;
void scanLocalworks();
void connectLocalworks();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  scanLocalworks();
  connectLocalworks();

  }

void loop() {
   unsigned long currentMillis = millis();
if((WiFi.status() != WL_CONNECTED)&& (currentMillis - previousMillis >=interval)){
  WiFi.disconnect();
  Serial.println("CONEXÃO PERDIDA");
  Serial.println("Re-Conectando ");
  WiFi.disconnect();
  WiFi.reconnect();
  previousMillis = currentMillis;
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("RE-CONECTADO");
  }
  }
}
void connectLocalworks(){
Serial.println("Iniciando conexão com rede WiFi");
  WiFi.begin(SSID,PSWD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
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