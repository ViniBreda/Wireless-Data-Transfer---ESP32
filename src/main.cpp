#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "LFS_HELPER.hpp"

#define DEVICE 1  // Transmissor -> 0; Base -> 1
#define FORMAT 1 // format on boot if set to 1

#if DEVICE == 0

  #include <ESPAsyncWebServer.h>
  #include <BluetoothSerial.h>
  #define BTTOGGLE 2 // Bluetooth append to file toggle
  #define BTNAME   "Data Transmissor" // define bluetooth device name
  BluetoothSerial BT; // BluetoothSerial object creation
  const int BUFFER_SIZE = 255;
  char buf[BUFFER_SIZE]; // so Serial RX buffer is not overflown with data
  bool bt_append = false; // whether or not to append received BT data to LoremIpsum.txt
  AsyncWebServer server(80); // Server object creation
#else
  #include <HTTPClient.h>
  #include <time.h>
  #define LED 2 // using board LED to indicate file was received
  struct tm data; // create struct that contains time information
  #define MINUTES 2 // define get request time if there are stations connected to the softAP
  time_t tt = time(NULL); // set current time in seconds for the first time
#endif

#define SSID "Data Transmission" // define wifi network name
#define PW   "testpw123" // encrypt pw later

void setup() {

  #if DEVICE == 0
    pinMode(BTTOGGLE, INPUT);
    delay(50); // delay to give time for input pin to be set properly
  #else
    pinMode(LED, OUTPUT);
    timeval tv;
    tv.tv_sec = 1648783914; // set up RTC unix time
    settimeofday(&tv, NULL); // keeps unix time updated
  #endif

  // Setup Serial Monitor
  Serial.begin(115200);
  Serial.println();

  // Setup File System
  if (!LITTLEFS.begin())
    Serial.println("LittleFS not Mounted");
  else{
    Serial.println("LittleFS Mounted!");
    if (FORMAT)
    { // Format drive if needed
      Serial.println("Formatting Drive...");
      Serial.println((String)"Previously Used Bytes: " + LITTLEFS.usedBytes() + "/" + LITTLEFS.totalBytes());
      LITTLEFS.format();
      Serial.println((String)"After formatting: " + LITTLEFS.usedBytes() + "/" + LITTLEFS.totalBytes());
      createDir(LITTLEFS, "/storage"); // Create directory for file to be stored on first boot
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
      WiFi.setTxPower(WIFI_POWER_19_5dBm); // set WiFi Power to Highest Level
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
    Serial.println((String)( (long int)tt + " " + time(NULL)));
    if( ( WiFi.softAPgetStationNum() > 0 ) && ( ( (long int)tt + (MINUTES * 60) ) <= ( time(NULL) ) ) ) // if there's something connected and two minutes have passed do get request
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
        http.begin((String)"http://"+ ip4addr_ntoa(&(station.ip)) + "/donwload");
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
            digitalWrite(LED, HIGH); // turn on OnBoard LED if file was received
          } else
          {
            Serial.println((String)"[HTTP] GET... failed, error: " + http.errorToString(httpCode).c_str());
          }
            
        }
        http.end();
        tt = time(NULL); // updates time variable so it can run again
      }
    }
  #endif
  
  
  
}