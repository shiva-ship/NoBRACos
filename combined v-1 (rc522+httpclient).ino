#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
ESP8266WiFiMulti WiFiMulti;

#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

String tag;

void setup() {
 
//COMON FOR BOTH RFID AND HHTPCLIENT
   Serial.begin(9600);
  // Serial.setDebugOutput(true);


////RC522 SETUP--------------------
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
///RC522 SETUP ENDS HERE-----------

///WIFI SETUP STARTS HERE+++++++++++++++++++++++
  Serial.println();
  Serial.println();
  Serial.println();
  
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("shiva2.4", "22oct2001");
//////WIFI SETUP ENDS HERE++++++++++++++++++++++


}

void rfidRead(){
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);
    httpClientAuthUser(tag);
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    
  }
}

void httpClientAuthUser(String tagUID){
if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, "http://192.168.1.141:8080/api/endpoint?rfid="+tagUID)) {  // HTTP

      Serial.print("http://192.168.1.141:8080/api/endpoint?rfid="+tagUID);
      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.printf("Rcvd from server: %s\n",payload);
          Serial.print(payload);
          if(payload=="Acces Granted"){
            Serial.print("same");
          }
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }

  delay(10000);
}

void loop() {
  
  rfidRead();
  
}
