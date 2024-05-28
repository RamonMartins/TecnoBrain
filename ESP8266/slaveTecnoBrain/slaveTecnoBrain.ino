#include <ESP8266WiFi.h>
#include <WiFiUdp.h>  // Biblioteca para o Ponto de Acesso
#include <HCSR04.h>   // Biblioteca para o Sensor de Distancia
#include <U8g2lib.h>  // Biblioteca para o Display Oled
#include <Wire.h>     // Biblioteca para o Display Oled

// IDENTIFICADOR DE USUÁRIO
String num_matricula = "1";

const char* ssid = "ESP8266-AP";  // Mesmo SSID do AP do mestre
const char* password = "password"; // Mesma senha do AP do mestre
const char* masterIP = "192.168.4.1"; // Endereço IP do mestre
const int masterUdpPort = 4210; // Porta UDP do mestre

char buffer[50];    // Variável para auxiliar na conversão de INT para String

WiFiUDP Udp;

int buzz = 15;

int distancia;
UltraSonicDistanceSensor distanceSensor(4, 5);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 12, U8X8_PIN_NONE);

void setup() {
  pinMode(buzz, OUTPUT);
  digitalWrite(buzz, LOW);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Iniciando escravo...");
  
  u8g2.begin(); // Inicialize o display
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14B_tr);
  u8g2.drawStr(40,20,"Slave 01");
  u8g2.sendBuffer();
  
  // Conecta-se ao ponto de acesso (Access Point) do mestre
  WiFi.begin(ssid, password);
}

void loop() {
  distancia = distanceSensor.measureDistanceCm();
  Serial.println(distancia);

  if (WiFi.status() == 3){
      if(distancia >= 0 && distancia >= 10){
        String mensagem = num_matricula + " - Dentro da área & Sem uso do capacete";
        mensagem.toCharArray(buffer, sizeof(buffer));
        Serial.println(buffer);
        sendToMaster(buffer);
        digitalWrite(buzz, HIGH);
      }else if(distancia < 0) {
        String mensagem = num_matricula + " - Leitura do Sensor de distancia com defeito";
        mensagem.toCharArray(buffer, sizeof(buffer));
        Serial.println(buffer);
        sendToMaster(buffer);
        digitalWrite(buzz, LOW);
      }else{
        String mensagem = num_matricula + " - Dentro da área & Com uso do capacete";
        mensagem.toCharArray(buffer, sizeof(buffer));
        Serial.println(buffer);
        sendToMaster(buffer);
        digitalWrite(buzz, LOW);
      }
  }else{
    String mensagem = num_matricula + " - Fora da área";
    mensagem.toCharArray(buffer, sizeof(buffer));
    Serial.println(buffer);
    sendToMaster(buffer);
    digitalWrite(buzz, LOW);
  }

  //sendToMaster(itoa(2024, buffer, 10));
  delay(1000);
}

void sendToMaster(char* message) {
  // Envia dados para o mestre
  Udp.beginPacket(masterIP, masterUdpPort);
  Udp.write(message);
  Udp.endPacket();
}
