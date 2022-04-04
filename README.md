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

ESP32 was the selected board because:
* it has an internal flash memory that can store more than 500kB of data;
* it has both bluetooth and Wi-Fi onboard;
* it has Wi-Fi Long Range, which is great for file transfers of up to 1Km;
* it's low power mode has very low power consumption, which is great for battery powered projects;
* great market availability, atleast in Brazil;
* highest clock rate;
* internal RTC module;
* cheap price;
* 15 analog pins on DOIT ESP32 DevKit 1 that are great for reading data from sound and light sensors, for example;
* Possibility to code using Arduino syntax, which I'm more familiar to;

Both transmissor and receiver schematics can be found below:

![Transmissor](https://github.com/ViniBreda/Wireless-Data-Transfer---ESP32/blob/master/Esquematicos/Esquem%C3%A1tico%20do%20Transmissor-1.png)

![Receiver](https://github.com/ViniBreda/Wireless-Data-Transfer---ESP32/raw/master/Esquematicos/Esquem%C3%A1tico%20do%20Receptor-1.png)

As there was no need for external components, the schematics would be fairly simple, so I decided to add some details on the transmissor, designing it to be a DataLogger that should read data from it's I²C. This required a logic level converter for I²C devices that need 5V for power and output 5V logic level signals, as ESP32's high level is 3.3V maximum. Also, I used as an example the schematics for the original DOIT ESP32 Devkit 1, changing some of the voltage regulators for ones with lower dropout voltage.

As I was using KiCAD for the first time, designing both the transmissors and receivers was a great challenge and I'm happy with the final result. Common 9V batteries can be used for thisn project and will power the device for many hours.

The transmissor design sacrifices one of the GPIOs with less functionality to have it as I²C enable for the logic level converter. I²C will work normally with this pin on LOW, but I²C for 5V power 5V logic level signal devices this pin has to be put on high while data is being sent and received. It's a great measure to ensure a lower power draw.

The board design for both transmissor and receiver can be found below:

<p align="left">
  <img width="400" height="560" src="https://github.com/ViniBreda/Wireless-Data-Transfer---ESP32/raw/master/Placas%20de%20Circuito%20Impresso/PCI%20Transmissor.png">
  <img width="400" height="560" src="https://github.com/ViniBreda/Wireless-Data-Transfer---ESP32/raw/master/Placas%20de%20Circuito%20Impresso/PCI%20Transmissor-3D.png">
</p>

<p align="left">
  <img width="400" height="560" src="https://github.com/ViniBreda/Wireless-Data-Transfer---ESP32/raw/master/Placas%20de%20Circuito%20Impresso/PCB-Receptor.png">
  <img width="400" height="560" src="https://github.com/ViniBreda/Wireless-Data-Transfer---ESP32/raw/master/Placas%20de%20Circuito%20Impresso/PCB-Receptor-3D.png">
</p>

The code written in this project only simulates the transfer of an existing file stored via bluetooth to folder /storage/loremipsum.txt

But it's easily adaptable to work as it was designed to, as a Data Logger.

* All KiCAD files are stored at **Arquivos KiCAD**;
* All Schematic images are stored at **Esquematicos**;
* All PCB images are stored at **Placas de Circuito Impresso**.

### For more details, read Wireless_Data_Transfer.pdf (in portuguese)