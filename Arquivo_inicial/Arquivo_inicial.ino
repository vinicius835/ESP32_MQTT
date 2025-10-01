#include <WiFi.h>
scanLocalworks();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Iniciando Scan de redes Wi- Fi");
  scanLocalworks();
}

void loop() {

}

void scanLocalworks(){
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