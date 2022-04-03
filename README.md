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

