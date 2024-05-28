#include <ESP8266WiFi.h>
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
#include <WiFiUdp.h>
#include <U8g2lib.h>  // Biblioteca para o Display Oled
#include <Wire.h>     // Biblioteca para o Display Oled

//defines de id mqtt e tópicos para publicação e subscribe
#define TOPICO_SUBSCRIBE "ProjectTecnoBrainEnvia"     //tópico MQTT de escuta
#define TOPICO_PUBLISH   "ProjectTecnoBrainRecebe"    //tópico MQTT de envio de informações para Broker
                                                   //IMPORTANTE: recomendamos fortemente alterar os nomes
                                                   //            desses tópicos. Caso contrário, há grandes
                                                   //            chances de você controlar e monitorar o NodeMCU
                                                   //            de outra pessoa.
#define ID_MQTT  "ProjectTecnoBrain"     //id mqtt (para identificação de sessão)
                               //IMPORTANTE: este deve ser único no broker (ou seja, 
                               //            se um client MQTT tentar entrar com o mesmo 
                               //            id de outro já conectado ao broker, o broker 
                               //            irá fechar a conexão de um deles).

// WIFI
//const char* SSID = "iPhone de ramon"; // SSID / nome da rede WI-FI que deseja se conectar
//const char* PASSWORD = "87654320"; // Senha da rede WI-FI que deseja se conectar
//const char* SSID = "brisa-3012659"; // SSID / nome da rede WI-FI que deseja se conectar
//const char* PASSWORD = "xs324hhh"; // Senha da rede WI-FI que deseja se conectar
const char* SSID = "Professor iCEV"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "1c3v@0sm3lh0r3s"; // Senha da rede WI-FI que deseja se conectar


// MQTT
const char* BROKER_MQTT = "broker.hivemq.com"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT

//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient

// Ponto de Acesso
const char* ssid = "ESP8266-AP";
const char* password = "password";
const int localUdpPort = 4210;  // Porta local para a comunicação UDP

WiFiUDP Udp;

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 12, U8X8_PIN_NONE);

void setup() {
  // Mostra informações no display
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14B_tr);
  u8g2.drawStr(45,20,"Master");
  u8g2.sendBuffer();

  // Inicializa o Monitor Serial
  Serial.begin(115200);

  // Inicializa o ponto de acesso (Access Point)
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println("Ponto de acesso (AP) iniciado:");
  Serial.print("Nome do SSID: ");
  Serial.println(ssid);
  Serial.print("Senha do AP: ");
  Serial.println(password);
  Serial.print("Endereço IP do mestre: ");
  Serial.println(WiFi.softAPIP());

  Udp.begin(localUdpPort);
  Serial.print("Porta local UDP: ");
  Serial.println(localUdpPort);
  
  // Inicializa uma conexão com Wifi
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

  // Inicializa a conexão com Broker MQTT
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
  while (!MQTT.connected()) {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}


void loop() {

  // Verifica se há dados recebidos
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // Lê os dados recebidos
    char packetBuffer[255];
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    // Imprime os dados recebidos na porta serial
    Serial.print("Dados recebidos do escravo: ");
    Serial.println(packetBuffer);

    // Envia 
    if (!MQTT.connected()){
      reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
    }
    MQTT.publish(TOPICO_PUBLISH, packetBuffer);
  }
  
  //keep-alive da comunicação com broker MQTT
  MQTT.loop();
  
  delay(10);
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
      Serial.print("* Tentando se conectar ao Broker MQTT: ");
      Serial.println(BROKER_MQTT);
      if (MQTT.connect(ID_MQTT)) 
      {
          Serial.println("Conectado com sucesso ao broker MQTT!");
          MQTT.subscribe(TOPICO_SUBSCRIBE); 
      } 
      else
      {
          Serial.println("Falha ao reconectar no broker.");
          Serial.println("Havera nova tentatica de conexao em 2s");
          delay(2000);
      }
  }
}