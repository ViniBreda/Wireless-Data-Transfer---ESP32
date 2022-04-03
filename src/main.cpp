#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "LFS_HELPER.hpp"

#define DEVICE 0 // Transmissor -> 0; Base -> 1
#define LONGRANGE 1 // Long Range -> 1; Common Range 0

#define SSID "Data Transmission" // define wifi network name
#define PW   "testpw123" // encrypt pw later

#if DEVICE == 0
  #include <ESPAsyncWebServer.h>
  #include <BluetoothSerial.h>
  #define BTTOGGLE 4 // Bluetooth append to file toggle
  #define BTNAME   "Data Transmissor" // define bluetooth device name
  BluetoothSerial BT; // BluetoothSerial object creation
  const int BUFFER_SIZE = 255;
  char buf[BUFFER_SIZE]; // so Serial RX buffer is not overflown with data
  bool bt_append = false; // whether or not to append received BT data to LoremIpsum.txt
  AsyncWebServer server(80); // Server object creation
  void WiFi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("Disconnected from WIFI access point");
    Serial.print("WiFi lost connection. Reason: ");
    Serial.println(info.disconnected.reason);
    Serial.println("Reconnecting...");
    WiFi.begin(SSID, PW);
}
#else
  #include <HTTPClient.h>
  #include <time.h>
  #define LED 2 // using board LED to indicate file was received
  struct tm data; // create struct that contains time information
  #define MINUTES 0.1 // define get request time if there are stations connected to the softAP
  time_t tt;
#endif

void setup() {

  #if DEVICE == 0
    pinMode(BTTOGGLE, INPUT);
    delay(50); // delay to give time for input pin to be set properly
  #else
    pinMode(LED, OUTPUT);
    tt = time(NULL); // set current time in seconds for the first time
    timeval tv;
    tv.tv_sec = 1648783914; // set up RTC unix time
    settimeofday(&tv, NULL); // keeps unix time updated
  #endif

  // Setup Serial Monitor
  Serial.begin(115200);

  // Setup File System
  if (!LITTLEFS.begin(true))
    Serial.println("LittleFS not Mounted");
  else
  {
    Serial.println("LittleFS Mounted!");
    createDir(LITTLEFS, "/storage"); // Create directory for file to be stored on first boot
  }

  #if DEVICE == 0
    // WiFi Station Setup
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PW);
    while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println(WiFi.localIP());
    WiFi.onEvent(WiFi_disconnected, SYSTEM_EVENT_STA_DISCONNECTED);

    #if LONGRANGE == 1
      esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR );
    #endif

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
      #if LONGRANGE == 1
        esp_wifi_set_protocol( WIFI_IF_AP, WIFI_PROTOCOL_LR );
      #endif
    }
  #endif
}

void loop() {
  #if DEVICE == 0
    // ==============================   R E A D   B L U E T O O T H   D A T A   ==============================
    if (!digitalRead(BTTOGGLE)) { // debounce circuit output high, command trigger on LOW
      bt_append = !bt_append;     // toggle append enable and disable
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
    if( ( WiFi.softAPgetStationNum() > 0 ) && ( ( tt + (MINUTES * 60) ) <= ( time(NULL) ) )) // if there's something connected and two minutes have passed do get request
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
        http.begin((String)"http://"+ ip4addr_ntoa(&(station.ip)) + ":80/download");
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
            //readFile(LITTLEFS, "/storage/loremipsum.txt");
            digitalWrite(LED, HIGH); // turn on OnBoard LED if file was received
          } 
          else
          {
            Serial.println((String)"[HTTP] GET... failed, error: " + http.errorToString(httpCode).c_str());
          }
            
        }
        http.end();
      }
      tt = time(NULL); // updates time variable so it can run again
    }
  #endif
}