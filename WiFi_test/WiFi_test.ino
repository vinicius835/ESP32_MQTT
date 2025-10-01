#include <WiFi.h>
const String SSID = "Iphone";
const String PSWD = "iot_sul_123";
void scanLocalworks();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  scanLocalworks();
  Serial.println("Iniciando conexão com rede WiFi");
  WiFi.begin(SSID,PSWD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado!");
  }

void loop() {

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