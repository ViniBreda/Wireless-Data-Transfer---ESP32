#include <Arduino.h>
#include <BluetoothSerial.h>
#include <WiFi.h>
#include <time.h>
#include "LFS_HELPER.hpp"


#define FORMAT   0 // format on boot if set to 1
#define SEND     4 // SEND button GPIO
#define BTTOGGLE 2 // Bluetooth append to file toggle
#define BTNAME   "Data Transmissor" // define bluetooth device name

// // receiver tests
// #define SSID     "Data Transmissor" // define wifi network name
// #define PW       "testpw123" // encrypt pw later
// String header; // string to store HTTP requests
// WifiServer server(80);

BluetoothSerial BT;

const int BUFFER_SIZE = 255;
char buf[BUFFER_SIZE]; // so Serial RX buffer is not overflown with data
bool send_signal = false; // to detect whether the software is supposed to send data or not
bool bt_append = false; // whether or not to append received BT data to LoremIpsum.txt

void setup() {
  pinMode(SEND, INPUT);
  pinMode(BTTOGGLE, INPUT);
  digitalWrite(BTTOGGLE, HIGH);
  // Setup Serial Monitor
  Serial.begin(115200);
  Serial.println();

  // // Setup WiFi AP - receiver tests
  // if (!WiFi.softAP(SSID, PW)){
  //   Serial.println("WiFi not Started");
  // } else {
  //   Serial.println("WiFi Started!");
  //   IPAddress IP = WiFi.softAPIP();
  //   Serial.println((String)"AP IP address: " + IP);
  //   server.begin();
  // }
  

  // Setup Bluetooth Server
  Serial.println("BT Disabled - Append Disabled");

  // Setup File System
  if (!LITTLEFS.begin())
    Serial.println("LittleFS not Mounted");
  else{
    Serial.println("LittleFS Mounted!");
    if (FORMAT){ // Format drive if needed
      Serial.println("Formatting Drive...");
      Serial.println((String)"Previously Used Bytes: " + LITTLEFS.usedBytes() + "/" + LITTLEFS.totalBytes());
      LITTLEFS.format();
      Serial.println((String)"After formatting: " + LITTLEFS.usedBytes() + "/" + LITTLEFS.totalBytes());
      createDir(LITTLEFS, "/storage"); // Create directory for file to be stored on first boot
    }
    //readFile(LITTLEFS, "/storage/loremipsum.txt");
  }
  delay(50); // delay to give time for input pin to be set properly
}


void loop() {
  // ==============================   R E A D   B L U E T O O T H   D A T A   ==============================
  if (!digitalRead(BTTOGGLE)) { // debounce circuit output high, command trigger on LOW
    Serial.println("BTTOGGLE");
    bt_append = !bt_append;     // toggle append enable and disable
    Serial.println(bt_append);
    if (bt_append == false)
    {
      BT.end();
      Serial.println("BT Disabled - Append Disabled");
    } else {
      if (!BT.begin(BTNAME)){
        Serial.println("BT Device not Started");
      } else {
        Serial.println("BT Enabled - Append Enabled");
        if (BT.available()) {
          Serial.println("Writing to File loremimpsum.txt...");
          BT.readBytes(buf, BUFFER_SIZE);
          appendFile(LITTLEFS, "/storage/loremipsum.txt", buf);
        }
      }
    }
    
  }
  

  // receiver tests
  // WiFiClient client = server.available(); // listen for incoming clients
  // if (client){
  //   Serial.println("New Client!");
  //   String currentLine = "";
  //   while (client.connected()) {
  //     if (client.available()) {
  //       char c = client.read();
  //       Serial.write(c);
  //       header += c;
  //       if (c == '\n') {
  //         if (currentLine.length() == 0) {

  //         }
  //       }
  //     }
  //   }
  // }

/* TODO : 
 - Enable/Disable BT file transfer - DONE
 - Write WiFi AP file transfer
 - Test WiFi Long Range and Normal WiFi
*/
}