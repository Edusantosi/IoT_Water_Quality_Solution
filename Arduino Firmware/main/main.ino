//GENERAL
#include <ArduinoMqttClient.h> //File with Wifi credentials and MQTT TTiO topic
#include <WiFi101.h>
#include <ArduinoJson.h>
#include "secretos_arduino.h"
#define VREF 3.3  
#define ADC_RES 1024 //ADC Resolution

//Wi-Fi Connection
char ssid[] = SECRET_SSID;        // network SSID (name)
char pass[] = SECRET_PASS;    // network password 
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

//MQTT BROKER
const char broker[] = SECRET_BROKER;
int        port     = 1883;
const char topic[]  = SECRET_TOPIC;


//set interval for sending messages (milliseconds)
const long interval = 300000;
unsigned long previousMillis = 0;


//TEMP&HUM
#include <Wire.h>
#include "DFRobot_SHT20.h"
DFRobot_SHT20    sht20;

//WaterTEMP
#include <OneWire.h>
int DS18S20_Pin = 2; //Sensor DS18S20 waterTemp
OneWire ds(DS18S20_Pin);
float wTemp;

//TDS
#define TdsSensorPin A1
#define SCOUNT  30           // sum of sample 

int analogBuffer[SCOUNT];    // store the analog value of the tds sensor in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
float tdsAverageVoltage = 0;

//OD
#define DO_PIN A3

// Single-point calibration Mode=0
// Two-point calibration Mode=1
#define TWO_POINT_CALIBRATION 0

// Single point calibration needs to be filled CAL1_V and CAL1_T
#define CAL1_V (2030) //mv
#define CAL1_T (25)   //℃
// Two-point calibration needs to be filled CAL2_V and CAL2_T
// CAL1 High temperature point, CAL2 Low temperature point
#define CAL2_V (1300) //mv
#define CAL2_T (15)   //℃

const uint16_t DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};
int DO_ADC_Raw;
int odVoltage;

//pH
#include "DFRobot_PH.h"
#include <FlashAsEEPROM.h>
#define PH_PIN A5

float phVoltage,phValue;
DFRobot_PH ph;



void setup() {
  /*
  Serial.begin(9600);                                  //Initialize serial and wait for port to open, this can be optional
  while (!Serial) {
    ;                                                  // wait for serial port to connect. Needed for native USB port only
  }
  */

  connectToWiFi();
  connectToMqtt();

  setupSensors();
}

void loop() {
  
  mqttClient.poll();                                   // MQTT Client handling
  ph.calibration(phVoltage,wTemp);                     // calibration process by Serail CMD
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Network connection lost");
      connectToWiFi();
      connectToMqtt();
    }
    if (!mqttClient.connected()) {
      Serial.println("MQTT connection lost");
      connectToMqtt();
    }
     
    
    previousMillis = currentMillis;                     // save the last time a message was sent

    
    float temp =sht20.readTemperature();                // reading the air temperature and relative humidity of the SHT20 sensor
    float hum = sht20.readHumidity();                       

    wTemp = getTemp();                                  // reading of the water temperature of the DS18S20 sensor


    while (analogBufferIndex <= SCOUNT){                // 40 sample of the analog value of the TDS sensor
      analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);  
      analogBufferIndex++;
      delay(40);
    }
    analogBufferIndex = 0;
  
    for (int copyIndex = 0; copyIndex < SCOUNT; copyIndex++){
    analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    }

    tdsAverageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / ADC_RES;  // read the analog value more stably by the median filtering algorithm and convert it to a voltage value
    float compensationCoefficient = 1.0 + 0.02 * (temp - 25.0);  // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVoltage = tdsAverageVoltage / compensationCoefficient;  // temperature compensation
    float tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;  // convert voltage value to TDS value

    odVoltage = uint32_t(VREF*1000) * analogRead(DO_PIN) / ADC_RES; // lecture and analog conversion to voltage in mv
    float doValue = readDO(odVoltage, wTemp)/1000;  // DO value conversion from voltage with temperature compensation

    
    phVoltage = analogRead(PH_PIN)/ADC_RES*(VREF*1000); // analog lecture and analog conversion to voltage in mv
    phValue = ph.readPH(phVoltage,wTemp);  // ph value conversion from the voltage with temperature compensation

    // create a JSON document with all the readings from the sensors     
    StaticJsonDocument<320> doc;
    JsonArray values = doc.createNestedArray("values");
    JsonObject values_0 = values.createNestedObject();
    values_0["key"] = "temp";
    values_0["value"] = temp;
    JsonObject values_1 = values.createNestedObject();
    values_1["key"] = "hum";
    values_1["value"] = hum;
    JsonObject values_2 = values.createNestedObject();
    values_2["key"] = "wTemp";              
    values_2["value"] = wTemp;
    JsonObject values_3 = values.createNestedObject();
    values_3["key"] = "tds";
    values_3["value"] = tdsValue;
    JsonObject values_4 = values.createNestedObject();
    values_4["key"] = "od";
    values_4["value"] = doValue; 
    JsonObject values_5 = values.createNestedObject();
    values_5["key"] = "pH";
    values_5["value"] = phValue; 

    Serial.print("temp: ");
    Serial.println(temp);
    Serial.print("hum: ");
    Serial.println(hum);
    Serial.print("wTemp: ");
    Serial.println(wTemp);
    Serial.print("tds: ");
    Serial.println(tdsValue);
    Serial.print("do: ");
    Serial.println(doValue);
    Serial.print("ph: ");
    Serial.println(phValue);
    
    // send message, the Print interface can be used to set the message contents
    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    
    mqttClient.beginMessage(topic, (unsigned long)measureJson(doc)); // send message to the mqtt broker
    serializeJson(doc, mqttClient);
    mqttClient.endMessage();
    Serial.println();
    
  }
}

// function to connect Wi-Fi network
void connectToWiFi() {
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // Failed to connect, retry after delay 
    delay(5000);  
  }
  Serial.println("Connection to the network succesful");
}

// function to connect to the MQTT broker
void connectToMqtt(){
  Serial.println("Attempting to connect to thethings.iO's MQTT broker");
  while (!mqttClient.connect(broker, port)) {
    // Failed to connect, retry after delay 
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    delay(5000);  
  }
  Serial.println("Connection with thethings.iO's MQTT broker succesful");
}

// function to setup sensors
void setupSensors(){
    sht20.initSHT20();                                  // Init SHT20 Sensor
    delay(100);
    sht20.checkSHT20();                                 // Check SHT20 Sensor

    ph.begin();                                         // Initialize the pH sensor

    pinMode(TdsSensorPin,INPUT);  
}

// function to get water temperature
float getTemp() {
  // Returns the temperature from one DS18S20 in degrees Celsius

  byte data[12];
  byte addr[8];

  if (!ds.search(addr)) {
    // No more sensors on the chain, reset search
    ds.reset_search();
    return -1000;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return -1000;
  }

  if (addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.println("Device is not recognized");
    return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // Start conversion, with parasite power on at the end

  delay(1000); // Wait for the conversion to complete (adjust the delay if needed)

  ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // We need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); // Using two's complement
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;
}

// function to get the median value from an array
int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (int i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];

  for (int j = 0; j < iFilterLen - 1; j++)
  {
    for (int i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        int bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }

  if ((iFilterLen & 1) > 0)
    return bTab[(iFilterLen - 1) / 2];
  else
    return (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
}

// function to read dissolved oxygen depending on the point calibration selection
float readDO(uint32_t voltage_mv, uint8_t temperature_c){
if (TWO_POINT_CALIBRATION == 0){
  uint16_t V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature_c - (uint32_t)CAL1_T * 35;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
  }
else{
  uint16_t V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
  }
}
