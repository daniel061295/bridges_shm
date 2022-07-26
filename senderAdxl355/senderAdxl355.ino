#include <SPI.h>
#include <LoRa.h>

//######################DECLARANDO REGISTROS Y VARIABLES ADXL355############################################
//#define SCK 5
//#define MISO 19
//#define MOSI 27

//float t = 0;

// Memory register addresses:
const String id_nodo = "0";
const int Ts = 50;
const int SF = 7; // Spreading factor
const int Freq = 915E6; // Center frequency
const int CHIP_SELECT_PIN = 17; // PARA EL ACELEROMETRO


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

// Pins used for the connection with the sensor



//###################################################################
int counter = 0;
const int csPin = 18;          // LoRa radio chip select
const int resetPin = 14;        // LoRa radio reset
const int irqPin = 26;          // change for your board; must be a hardware interrupt pin

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
  LoRa.setPins(csPin, resetPin, irqPin);

  
  Serial.println("LoRa Sender");

  //Configure ADXL355:

  
  // Give the sensor time to set up:  
  delay(100);

  if (!LoRa.begin(Freq)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //LoRa.setSpreadingFactor(SF); 
}

void loop() {

//#################READING FROM ADXL 355######################
  int axisAddresses[] = {XDATA1, XDATA2, XDATA3, YDATA1, YDATA2, YDATA3, ZDATA1, ZDATA2, ZDATA3};
  int axisMeasures[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int dataSize = 9;

  // Read accelerometer data
  readMultipleData(axisAddresses, dataSize, axisMeasures);

  // Split data
  //int xdata = (axisMeasures[0] >> 4) + (axisMeasures[1] << 4) + (axisMeasures[2] << 12);
  //int ydata = (axisMeasures[3] >> 4) + (axisMeasures[4] << 4) + (axisMeasures[5] << 12);
  int zdata = (axisMeasures[6] >> 4) + (axisMeasures[7] << 4) + (axisMeasures[8] << 12);

  
  // extend the sign bit from bit 19 to bit 31
  //xdata = (xdata<<12)>>12;
  //ydata = (ydata<<12)>>12;
  zdata = (zdata<<12)>>12;

  // LBS to g
  //double xdata_g = double(xdata)/256000;
  //double ydata_g = double(ydata)/256000;
  double zdata_g   = double(zdata)/256000; // DATO EN G
  String zdata_str = String(zdata);
  //Serial.println(id_nodo);
  //Serial.println(zdata_str);
  //Serial.println(id_nodo + zdata_str);
  String message = zdata_str + id_nodo;
  //Serial.print(zdata_g,12);
  //Serial.print("\n");
  
//################SENDING#############################
  Serial.print("Sending packet: ");
  //Serial.println(zdata);
  Serial.println(zdata_g,12);
  //Serial.println(message);
  //Serial.print("From node: ");
  //Serial.println(id_nodo);
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
  
  
  counter ++;

  delay(Ts);
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
