# TFG_IoT_Hydroponics

IoT Solution prototype for monitoring water quality parameters in aquaculture, hydroponic, and aquaponic environments. This project is part of my thesis as an ICT Management Engineer and the motivation behind this project was to use technology on trying to solve the problems of tomorrow. Because of many factors, the way we harvest food may have to change and it needs to be more resilient and sustainable, some practices like the method mentioned before are fit to solve some challenges and this prototype aims to make water quality monitoring something feasible and easy. 

It should be noted that the most optimal would be to adapt a prototype for each of the cases and methods, the following solution could be considered the adaptation for hydroponics, the objective of this prototype is to read key parameters for the growth of plants in a hydroponic system.
<div align="center">
  <img src="https://github.com/Edusantosi/IoT_Water_Quality_Solution/assets/45486045/50708912-3abd-46e6-b5ab-3c60f64fe7d6" alt="Proyect Visualization" width="400"/>
  <img src="https://github.com/Edusantosi/IoT_Water_Quality_Solution/assets/45486045/e84bd3fa-0450-46b5-adfe-1c2cc5203a0c" alt="Implementation" width="400"/>
</div>

## Arquitecture
The node will read water parameters such as water temperature, pH, DO and TSD. In parallel, the air temperature and relative humidity will be captured to take into account data from the environment of the plants. With these parameters, the health and status of the plants is monitored and corrections can be made to the system to optimize plant growth.

Once all the parameters have been collected, the node will send the package to the cloud where it will be stored. Subsequently, these data will be processed to create notifications according to the defined alerts and the same platform will have display panels where the complete information of the hydroponic system can be viewed by farmers or operators.

![Sistem's Architecture](https://github.com/Edusantosi/IoT_Water_Quality_Solution/assets/45486045/0f0894ee-ff35-44fd-8d6e-03c61854ec8e)

The MQTT protocol has been chosen as the communication protocol. In this development, thethings.iO will be used as the cloud for the storage, processing and visualization of the information. To create the notification, an integration of thethings.iO with Twilio will be used to send an SMS to the desired phone number.

## Table of materials
### Hardware
| Material                                           | Price    | Quantity | Total   |
|----------------------------------------------------|----------|----------|---------|
| Arduino MKR 1000                                  | 43,55 €  | 1        | 43,55 € |
| Breadboard                                        | 4,79 €   | 1        | 4,79 €  |
| Gravity Analog Dissolved Oxygen Sensor/Meter Kit  | 168,27 € | 1        | 168,27 €|
| SHT20 I2C Temperature & Humidity Sensor (Waterproof Probe) | 22,40 €  | 1        | 22,40 € |
| Analog pH Sensor/Meter Kit V2                     | 39,33 €  | 1        | 39,33 € |
| Gravity Analog TDS Sensor/ Meter                  | 11,75 €  | 1        | 11,75 € |
| Waterproof DS18B20 Temperature Sensor Kit         | 7,47 €   | 1        | 7,47 €  |
|                                                    | **Total**|          | **297,56 €** |
### Software
https://thethings.io/

https://www.twilio.com/
## Circuit Diagram
<div align="center">
  <img src="https://github.com/Edusantosi/IoT_Water_Quality_Solution/assets/45486045/35df6438-7dad-47ce-8eed-0ab9533e09d1" alt="Circuit Diagram" width="400"/>
  <img src="https://github.com/Edusantosi/IoT_Water_Quality_Solution/assets/45486045/59ef4e45-0d4d-4cba-aba1-f1af7d38c7b8" alt="Implementation of Circuit Diagram" width="400"/>
</div>


## How to run it
### Arduino
To un the code in the Arduino MKR1000 just 

### Dependencies 
- **ArduinoMqttClient.h**
  - Enables and facilitates sending and receiving MQTT messages using Arduino. It is used to send the serialized JSON document to the MQTT broker of thethings.iO.

- **Wi-Fi101.h**
  - This library implements a network controller for devices based on the ATMEL WINC1500 Wi-Fi module, thus allowing and simplifying the Arduino MKR1000 board to connect to a Wi-Fi network.

- **ArduinoJson.h**
  - Allows serializing or deserializing JSON documents in a very intuitive and optimal way. It is used to serialize the JSON document containing all the information obtained from the sensors.

- **Wire.h**
  - This library allows communication with I2C/TWI devices. In this case, with the SHT20 I2C Temperature and Humidity sensor.

- **DFRobot_SHT20.h**
  - Library provided by the sensor suppliers, which allows initializing and checking the sensor and making temperature and humidity readings using the Wire.h library for I2C communication with the sensor.

- **OneWire.h**
  - Library that allows communication with digital sensors through the 1-Wire protocol. It is used to read the water temperature through the DS18B20 sensor.

- **DFRobot_PH.h**
  - Library provided by the sensor suppliers that allows initialization and calibration of the sensor and pH reading. This library depends on FlashAsEEPROM.h to be able to calibrate the sensor.

- **FlashAsEEPROM.h**
  - Library that emulates an EEPROM but in the non-volatile Flash memory of the microcontroller. It is used to store calibration data for the pH sensor.
