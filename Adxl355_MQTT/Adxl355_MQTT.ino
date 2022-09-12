#include <SPI.h>
// Pins used for the connection with the sensor

#ifdef ESP8266
 #include <ESP8266WiFi.h>  // Pins for board ESP8266 Wemos-NodeMCU
 #else
 #include <WiFi.h>  
#endif

#include "time.h" 
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

//######################DECLARANDO REGISTROS Y VARIABLES ADXL355############################################
//#define SCK 5
//#define MISO 19
//#define MOSI 27

//float t = 0;

// Memory register addresses:
const String id_nodo = "13";
const int Ts = 50;
const int CHIP_SELECT_PIN = 17; // PARA EL ACELEROMETRO
const int csPin = 18;          // LoRa radio chip select

//---- WiFi settings
const char* ssid = "APTO_508_2.4";
const char* password = "Pocholo0203D";

//---- MQTT Broker settings
const char* mqtt_server = "a33454d332054780b8feaf83950ed54a.s2.eu.hivemq.cloud";  // replace with your broker url
const char* mqtt_username = "danielcardenaz";
const char* mqtt_password = "Manzana2132881";
const int mqtt_port =8883;
const char* topic_mqtt= "SHM_PROYECTO/13";

//---- Time server settings
const char* ntpServer = "pool.ntp.org"; //Servidor para tiempo
const long  gmtOffset_sec = -18000;    //GTM pais  -5*60-60    
const int   daylightOffset_sec = 3600;  //Delay de hora
char dateTime[50];
bool timeflag = false;


const int XDATA3 = 0x08;
const int XDATA2 = 0x09;
const int XDATA1 = 0x0A;
const int YDATA3 = 0x0B;
const int YDATA2 = 0x0C;
const int YDATA1 = 0x0D;
const int ZDATA3 = 0x0E;
const int ZDATA2 = 0x0F;
const int ZDATA1 = 0x10;
const int RANGE = 0x2C;
const int POWER_CTL = 0x2D;

// Device values
const int RANGE_2G = 0x01;
const int RANGE_4G = 0x02;
const int RANGE_8G = 0x03;
const int MEASURE_MODE = 0x06; // Only accelerometer

// Operations
const int READ_BYTE = 0x01;
const int WRITE_BYTE = 0x00;





WiFiClientSecure espClient;   // for no secure connection use WiFiClient instead of WiFiClientSecure 
//WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
//
//
//int sensor1 = 0;
//float sensor2 = 0;
//int command1 =0;
//
//const char* sensor1_topic= "sensor1";
//const char*  sensor2_topic="sensor2";
////const char*  sensor2_topic="sensor3";
//
//const char* command1_topic="command1";
////const char* command1_topic="command2";


static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

//###################################################################
int counter = 0;

//==========================================
void setup_wifi() {
  delay(10);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("\nWiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //printLocalTime();
  
}


//=====================================
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";   // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");

      client.subscribe(topic_mqtt);   // subscribe the topics here
      //client.subscribe(command2_topic);   // subscribe the topics here
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");   // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  while (!Serial);
  pinMode(CHIP_SELECT_PIN, OUTPUT);
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
  ADXL355_init();
  writeRegister(RANGE, RANGE_2G); // 2G
  writeRegister(POWER_CTL, MEASURE_MODE); // Enable measure mode
  SPI.endTransaction();
  //Configure ADXL355:  
  // Give the sensor time to set up:  
  delay(100);
  while (!Serial) delay(1);
  setup_wifi();
//  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output


  #ifdef ESP8266
    espClient.setInsecure();
  #else   // for the ESP32
    espClient.setCACert(root_ca);      // enable this line and the the "certificate" code for secure connection
  #endif
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  }
  //LoRa.setSpreadingFactor(SF); 


void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  

//#################READING FROM ADXL 355######################
  int axisAddresses[] = {XDATA1, XDATA2, XDATA3, YDATA1, YDATA2, YDATA3, ZDATA1, ZDATA2, ZDATA3};
  int axisMeasures[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int dataSize = 9;

  // Read accelerometer data
  readMultipleData(axisAddresses, dataSize, axisMeasures);

  // Split data
  int xdata = (axisMeasures[0] >> 4) + (axisMeasures[1] << 4) + (axisMeasures[2] << 12);
  int ydata = (axisMeasures[3] >> 4) + (axisMeasures[4] << 4) + (axisMeasures[5] << 12);
  int zdata = (axisMeasures[6] >> 4) + (axisMeasures[7] << 4) + (axisMeasures[8] << 12);

  
  // extend the sign bit from bit 19 to bit 31
  xdata = (xdata<<12)>>12;
  ydata = (ydata<<12)>>12;
  zdata = (zdata<<12)>>12;

  // LBS to g
  double xdata_g = double(xdata)/256000;
  double ydata_g = double(ydata)/256000;
  double zdata_g   = double(zdata)/256000; // DATO EN G
  
  String xdata_str = String(xdata);
  String ydata_str = String(ydata);
  String zdata_str = String(zdata);
  //Serial.println(id_nodo);
  //Serial.println(zdata_str);
  //Serial.println(id_nodo + zdata_str);
  //String message = zdata_str + id_nodo;
  //Serial.print(zdata_g,12);
  //Serial.print("\n");
  
//################SENDING#############################
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
  Serial.println("Failed to obtain time");
    //return;
  }
  //Serial.println(&timeinfo, "%Y-%b-%d %H:%M:%S");
  strftime(dateTime,50, "%Y-%m-%d %H:%M:%S", &timeinfo);
  //===============================================================================================
  if ((strcmp(dateTime,"2022-09-11 22:56:00") ==  0)&&(timeflag == false)) {
    Serial.println("Triggered!");
    timeflag = true;
    }
  //===============================================================================================

//  int node = 1;
//  int dato = 256000;
  String mensaje = String(dateTime) + "," + String(id_nodo) +","+ xdata_str +","+ ydata_str +","+ zdata_str;
//  String message = zdata_str + id_nodo;
  //---- example: how to publish sensor values every 5 sec

  unsigned long now = millis();
  if (now - lastMsg > Ts) {
    lastMsg = now;
//    sensor1= random(50);       // replace the random value with your sensor value
//    sensor2= 20+random(80);    // replace the random value  with your sensor value
  Serial.print("Sending packet: ");
  //Serial.println(zdata);
//  Serial.println(zdata_g,12);
  Serial.println(mensaje);
  //Serial.print("From node: ");
  //Serial.println(id_nodo);
    publishMessage(topic_mqtt,String(mensaje),true);    
//    publishMessage(topic_mqttt,String(sensor2),true);
  
    
  }

  
  
  counter ++;

//  delay(Ts);
}


void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
  
  Serial.println("Message arrived ["+String(topic)+"]"+incommingMessage);
  
  //--- check the incomming message
//    if( strcmp(topic,command1_topic) == 0){
//     if (incommingMessage.equals("1")) digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on 
//     else digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off 
//  }

   //  check for other commands
 /*  else  if( strcmp(topic,command2_topic) == 0){
     if (incommingMessage.equals("1")) {  } // do something else
  }
  */
}

void publishMessage(const char* topic, String payload , boolean retained){
  if (client.publish(topic, payload.c_str(), true))
      Serial.println("Message publised ["+String(topic)+"]: "+payload);
}



//##############################FUNCIONES ADXL355######################################################

void ADXL355_init() {
  pinMode(CHIP_SELECT_PIN, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(200000,MSBFIRST,SPI_MODE0));
  digitalWrite(CHIP_SELECT_PIN, HIGH);
 }



/* 
 * Write registry in specific device address
 */
void writeRegister(byte thisRegister, byte thisValue) {
  byte dataToSend = (thisRegister << 1) | WRITE_BYTE;
  
  digitalWrite(CHIP_SELECT_PIN, LOW);
  SPI.transfer(dataToSend);
  SPI.transfer(thisValue);
  digitalWrite(CHIP_SELECT_PIN, HIGH);
   
}

/* 
 * Read registry in specific device address
 */
unsigned int readRegistry(byte thisRegister) {
  unsigned int result = 0;
  byte dataToSend = (thisRegister << 1) | READ_BYTE;
  
  digitalWrite(CHIP_SELECT_PIN, LOW);
  SPI.transfer(dataToSend);
  result = SPI.transfer(0x00);
  digitalWrite(CHIP_SELECT_PIN, HIGH);
  
  return result;
}

/* 
 * Read multiple registries
 */
void readMultipleData(int *addresses, int dataSize, int *readedData) {
  
  digitalWrite(CHIP_SELECT_PIN, LOW);
  for(int i = 0; i < dataSize; i = i + 1) {
      digitalWrite(CHIP_SELECT_PIN, LOW);  
    byte dataToSend = (addresses[i] << 1) | READ_BYTE;
    SPI.transfer(dataToSend);
    readedData[i] = SPI.transfer(0x00);
    digitalWrite(CHIP_SELECT_PIN, HIGH);
     
  }

}

//######################################################################################
