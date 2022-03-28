#include <Arduino.h>
#include <BluetoothSerial.h>
#include "LFS_HELPER.hpp"

#define FORMAT 0 // format on boot if set to 1

BluetoothSerial BT;

const int BUFFER_SIZE=250;
char buf[BUFFER_SIZE];

void setup() {
  // Setup Serial Monitor
  Serial.begin(115200);
  Serial.println();

  // Setup Bluetooth Server
  if(!BT.begin("Data Transmissor")){
    Serial.println("BT Device not started");
  } else {
    Serial.println("BT Started!");
  }

  // Setup File System
  if(!LITTLEFS.begin())
    Serial.println("LittleFS not Mounted");
  else{
    Serial.println("LittleFS Mounted!");
    if(FORMAT){ // Format drive if needed
      Serial.println("Formatting Drive...");
      Serial.println((String)"Previously Used Bytes: "+LITTLEFS.usedBytes()+"/"+LITTLEFS.totalBytes());
      LITTLEFS.format();
      Serial.println((String)"After formatting: "+LITTLEFS.usedBytes()+"/"+LITTLEFS.totalBytes());
    }
    //createDir(LITTLEFS, "/storage"); // Create directory for file to be stored
    readFile(LITTLEFS, "/storage/loremipsum.txt");
  }

}


void loop() {
  if(BT.available()){
    Serial.println("Writing to File loremimpsum.txt...");
    BT.readBytes(buf, BUFFER_SIZE);
    appendFile(LITTLEFS, "/storage/loremipsum.txt", buf);
  }


}




// #include <Arduino.h>
// #include <BluetoothSerial.h>
// #include <LITTLEFS.h>

// BluetoothSerial SerialBT;

// void setup() {
//   Serial.begin(115200);
//   SerialBT.begin("ESP32test"); //Bluetooth device name
//   Serial.println("The device started, now you can pair it with bluetooth!");
// }

// void loop() {
//   if (Serial.available()) {
//     SerialBT.write(Serial.read());
//   }
//   if (SerialBT.available()) {
//     Serial.write(SerialBT.read());
//   }
//   delay(20);
// }