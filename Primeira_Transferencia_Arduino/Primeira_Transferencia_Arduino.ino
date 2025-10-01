<#include WiFi.h>
const byte trigg_pin = 20;
const byte echo_pin = 8;

const byte led_sema_green = 4;
const byte led_sema_yellow = 2;
const byte led_sema_red = 15;

const byte led_ped_red = 5;
const byte led_ped_green = 17;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(led_sema_green, OUTPUT);
  pinMode(led_sema_yellow, OUTPUT);
  pinMode(led_sema_red, OUTPUT);

  pinMode(led_ped_red, OUTPUT);
  pinMode(led_ped_green, OUTPUT);

}

void loop() {
  if(Serial.available() > 0){
    String message = Serial.readStringUntil('\n');
  }
  int distancia = lerDistancia();
  if(distancia < 100){
      Serial.println("entrou");
      digitalWrite(led_sema_red, HIGH);
      digitalWrite(led_sema_green, LOW);
      digitalWrite(led_sema_yellow, LOW);
      delay(500);
      digitalWrite(led_ped_green, HIGH);
      delay(2500);
      digitalWrite(led_ped_green, LOW);
      delay(200);
      digitalWrite(led_ped_red, HIGH);
      delay(1000);
    }
  
      digitalWrite(led_sema_green, HIGH);
      digitalWrite(led_sema_yellow, LOW);
      digitalWrite(led_sema_red, LOW);

      digitalWrite(led_ped_red, HIGH);
      digitalWrite(led_ped_green, LOW);
      Serial.println("Passou");
      delay(1500);
      digitalWrite(led_sema_green, LOW);
      for(int i = 0; i < 3; i++){
        digitalWrite(led_sema_yellow, HIGH);
        delay(500);
        digitalWrite(led_sema_yellow, LOW);
        delay(500);
      }
      delay(10);
      digitalWrite(led_sema_red, HIGH);
      digitalWrite(led_ped_red, LOW);
      digitalWrite(led_ped_green, HIGH);
      delay(1500);
      digitalWrite(led_sema_green, HIGH);
      digitalWrite(led_sema_red, LOW);
      digitalWrite(led_ped_red, HIGH);
      digitalWrite(led_ped_green, LOW);
      delay(500);
    
}
int lerDistancia(){
  digitalWrite(trigg_pin, LOW); 
  delayMicroseconds(10);
  digitalWrite(trigg_pin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigg_pin, LOW); 
  delayMicroseconds(10);

  // calcula o tempo da ida + volra do pulso sonoro
  unsigned long duracao = pulseIn(echo_pin, HIGH);
  int distancia = ((duracao * 340)/2)/10000;
  Serial.printf("Distancia medida: %d\n", distancia);
  delay(100);
}