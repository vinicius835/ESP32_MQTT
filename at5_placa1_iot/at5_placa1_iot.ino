#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

const char* SSID = "Vini@@";
const char* PSWD = "viniciusA";

const char* brokerUrl = "test.mosquitto.org";
const int port = 1883;
const char* topicStatus = "Carrinhos/Cheios/1";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// sensor linha
const int pinoSensorLinha = 5;   // Sensor de linha (D0)
String statusSensor = "";

// servo
const byte pinoServo = 18;        // Servo motor
Servo servo;

void setup() {
  Serial.begin(115200);

  // pinMode(pinoSensorLinha, INPUT);

  servo.setPeriodHertz(50);
  servo.attach(pinoServo, 500, 2400);

  scanLocalworks();
  connectLocalworks();
  connectBroker();
}

void loop() {
  mqttClient.loop(); 
  int valorLinha = digitalRead(pinoSensorLinha);
  if (valorLinha == HIGH) {
    statusSensor = "na_linha";
    servo.write(0);
  } else {
    statusSensor = "fora_da_linha";
    servo.write(90);
  }

  // JSON
  StaticJsonDocument<200> doc;
  doc["status"] = statusSensor;
  char buffer[200];
  serializeJson(doc, buffer);

  // Enviar para o broker
  mqttClient.publish(topicStatus, buffer);
  

  // Verificar conexões
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado, reconectando...");
    connectLocalworks();
  }

  if (!mqttClient.connected()) {
    Serial.println("MQTT desconectado, reconectando...");
    connectBroker();
  }
  delay(500);
  Serial.println(statusSensor);

}

void connectLocalworks() {
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(SSID, PSWD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWiFi conectado!");
}

void scanLocalworks() {
  Serial.println("Buscando redes Wi-Fi...");
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("Nenhuma rede encontrada!");
  } else {
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s (%d dBm)\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
    }
  }
  delay(2000);
}

void connectBroker() {
  Serial.println("Conectando ao broker MQTT...");
  mqttClient.setServer(brokerUrl, port);

  String clientId = "ESP32C6-";
  clientId += String(random(0xffff), HEX);

  while (!mqttClient.connected()) {
    Serial.print(".");
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("\nConectado ao broker MQTT!");
    } else {
      Serial.print("Falha (rc=");
      Serial.print(mqttClient.state());
      Serial.println("). Tentando novamente...");
      delay(2000);
    }
  }

  mqttClient.subscribe(topicStatus);
}
