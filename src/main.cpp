#include <Arduino.h>
#include <BluetoothSerial.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <time.h>
#include "LFS_HELPER.hpp"

#define DEVICE     1 // Transmissor -> 0; Base -> 1
#define FORMAT     0 // format on boot if set to 1

#if DEVICE == 0
  #define BTTOGGLE 2 // Bluetooth append to file toggle
  #define BTNAME   "Data Transmissor" // define bluetooth device name
  BluetoothSerial BT; // BluetoothSerial object creation
  const int BUFFER_SIZE = 255;
  char buf[BUFFER_SIZE]; // so Serial RX buffer is not overflown with data
  bool bt_append = false; // whether or not to append received BT data to LoremIpsum.txt
  AsyncWebServer server(80); // Server object creation
#endif

#define SSID     "Data Transmission" // define wifi network name
#define PW       "testpw123" // encrypt pw later

void setup() {

  #if DEVICE == 0
    pinMode(BTTOGGLE, INPUT);
    delay(50); // delay to give time for input pin to be set properly
  #endif

  // Setup Serial Monitor
  Serial.begin(115200);
  Serial.println();

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
      if (!LITTLEFS.exists("/storage/loremipsum.txt"))
      {
        File file = LITTLEFS.open("/storage/loremispum.txt", FILE_WRITE);
        file.print("");
        file.close();
      }
    }
  }

  
  #if DEVICE == 0
    // WiFi Station Setup
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PW);
    while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println(WiFi.localIP());

    // HTTP Async Server Setup
    server.on("/download", HTTP_GET, [] (AsyncWebServerRequest *request) {
      request->send(LITTLEFS, "/storage/loremipsum.txt", "text/plain", true);
    });
    server.begin();

  #else // if device is base, creates wifi network
    if (!WiFi.softAP(SSID, PW)) {
      Serial.println("WiFi Access Point not Started.");
    } else { 
      Serial.println("WiFi AP Created!");
      Serial.println(WiFi.softAPIP());
    }
    
  #endif
  
}


void loop() {
  #if DEVICE == 0
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
  #else
    if(WiFi.isConnected())
    {
      // setup to get connected devices list
      wifi_sta_list_t wifi_sta_list;
      tcpip_adapter_sta_list_t adapter_sta_list;
      memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
      memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));
      esp_wifi_ap_get_sta_list(&wifi_sta_list);
      tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);
      
      // connected devices by MAC address and IP address
      for (int i = 0; i < adapter_sta_list.num; i++)
      {
        tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
        Serial.print("MAC: ");
 
        for(int i = 0; i< 6; i++)
        {
          Serial.printf("%02X", station.mac[i]);  
          if(i<5)Serial.print(":");
        }

        Serial.print("\nIP: ");  
        Serial.println(ip4addr_ntoa(&(station.ip)));
        
        // setup for HTTP request for HTTP Server on the transmission boards
        HTTPClient http;
        http.begin((String)"http://"+ ip4addr_ntoa(&(station.ip)) + ":80/donwload");
        int httpCode = http.GET();
        if (httpCode > 0)
        {
          Serial.println((String)"[HTTP] GET... code: "+httpCode);

          if(httpCode == HTTP_CODE_OK) 
          {
            File file = LITTLEFS.open("/storage/loremipsum.txt" , FILE_APPEND);
            file.println((String)"IP: " + ip4addr_ntoa(&(station.ip)));
            http.writeToStream(&file);
            file.close();
          } else
          {
            Serial.println((String)"[HTTP] GET... failed, error: " + http.errorToString(httpCode).c_str());
          }
            
        }
        http.end();

      }
      

      
    }
  #endif
  
  
  
}