# Wireless Data Transfer Between two ESP32 - DOIT Devkit

* File upload via bluetooth available BUT VERY SLOW;
* The goal is to send a 500KB file from one ESP32 to another, atleast 100m apart from eachother;
* The PCB of the transmission board was designed considering up to eight I²C devices connected to it to works as a sensor DataLogger;
* The receiver PCB is just a normal DOIT Devkit 1 remake to learn how to use KiCad;
* This is just a concept, not all funcionalities will be developed/tested as the main goal is to send a file over a Wireless Conection.

## TODO : 
 * [x] Enable/Disable BT file transfer
 * [x] Write WiFi AP file transfer
 * [x] RTC sync to log time of [HTTP] GET request
 * [x] RTC sync to execute [HTTP] GET request every 5 minutes
 * [x] Test WiFi Long Range and Normal WiFi
 * [x] Successful test on cellphone hosted WiFi Hotspot - Download file via http://ESP-IP-ADDRESS/download
 * [x] Successful file transfer between two ESP32 boards

## Description:

** For more details, read Wireless_Data_Transfer.pdf**

ESP32 was the selected board because:
* it has an internal flash memory that can store more than 500kB of data
* it has both bluetooth and Wi-Fi onboard
* it has Wi-Fi Long Range, which is great for file transfers of up to 1Km
* it's low power mode has very low power consumption, which is great for battery powered projects

Both transmissor and receiver schematics can be found below:

![Transmissor](https://github.com/ViniBreda/Wireless-Data-Transfer---ESP32/blob/master/Esquematicos/Esquem%C3%A1tico%20do%20Transmissor-1.png

![Receiver](https://github.com/ViniBreda/Wireless-Data-Transfer---ESP32/raw/master/Esquematicos/Esquem%C3%A1tico%20do%20Receptor-1.png)

As there was no need for external components, the schematics would be fairly simple, so I decided to add some details on the transmissor, designing it to be a DataLogger that should read data from it's I²C. This required a logic level converter for I²C devices that need 5V for power and output 5V logic level signals, as ESP32's high level is 3.3V maximum.

As I was using KiCAD for the first time, designing both the transmissors and receivers was a great challenge and I'm happy with the final result.
