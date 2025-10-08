#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 //Largura da tela em px
#define SCREEN_HEIGHT 64 //Altura da tela em px

#define I2C_SCK 6
#define I2C_SDA 5

Adafruit_SSD1306 tela(SCREEN_WIDTH,SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA,I2C_SCK); //Inicia comunicação I2C
  tela.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  tela.clearDisplay();
  tela.setTextSize(0.1);
  tela.setTextColor(SSD1306_WHITE);
  tela.setCursor(0,0);
  tela.println("");
  // tela.println("Hello");
  // tela.setCursor(0,32);
  // tela.println("World");
  tela.display();
  delay(5000);

}

void loop() {
  tela.clearDisplay();
  tela.setCursor(40,20);

  tela.print(millis()/1000);
  tela.println("s");
  
  tela.display();


}
