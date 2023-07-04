#include <LiquidCrystal_I2C.h>


#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

LiquidCrystal_I2C lcd(0x27,16,2);
float latitude, longitude;
// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1_Password", CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
    Serial.print("AP CHANNEL "); Serial.println(WiFi.channel());
  }
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Lat->");
  lcd.setCursor(0,1);
  lcd.print("Long->");

  Serial.println("ESPNow/Basic/Slave Example");
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Recv from: ");
  Serial.println(macStr);
  
  if (data_len >= sizeof(float) * 2) {
    memcpy(&latitude, data, sizeof(float));
    memcpy(&longitude, data + sizeof(float), sizeof(float));

    Serial.print("Last Packet Recv Data: ");
    Serial.print(latitude, 6);
    Serial.print(", ");
    Serial.println(longitude, 6);
  } else {
    Serial.println("Invalid data length");
  }
  
  Serial.println("");
}



void loop() {
  lcd.setCursor(3,0);
  lcd.print("[");
  lcd.print(latitude,6);
  lcd.print("]");
  lcd.setCursor(3,1);
  lcd.print("[");
  lcd.print(longitude,6);  
  lcd.print("]");

}
