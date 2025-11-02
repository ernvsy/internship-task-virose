#include <Arduino.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <esp_err.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "main.h"
#include "controller_fp.h"

uint8_t mac_addresses[MAC_ADDRESS_TOTAL][MAC_ADDRESS_LENGTH] = {
    {0x24, 0x0A, 0xC4, 0x0A, 0x10, 0x10},  // Fauzan Firdaus
    {0x24, 0x0A, 0xC4, 0x0A, 0x10, 0x11},  // Africha Sekar wangi
    {0x24, 0x0A, 0xC4, 0x0A, 0x11, 0x10},  // Rafaina Erin Sadia
    {0x24, 0x0A, 0xC4, 0x0A, 0x11, 0x11},  // Antonius Michael Yordanis Hartono
    {0x24, 0x0A, 0xC4, 0x0A, 0x12, 0x10},  // Dinda Sofi Azzahro
    {0x24, 0x0A, 0xC4, 0x0A, 0x12, 0x11},  // Muhammad Fahmi Ilmi
    {0x24, 0x0A, 0xC4, 0x0A, 0x13, 0x10},  // Dhanishara Zaschya Putri Syamsudin
    {0x24, 0x0A, 0xC4, 0x0A, 0x13, 0x11},  // Irsa Fairuza
    {0x24, 0x0A, 0xC4, 0x0A, 0x14, 0x10},  // Revalinda Bunga Nayla Laksono
    {0x24, 0x0A, 0xC4, 0x0A, 0x21, 0x11},  // BISMA
    {0x24, 0x0A, 0xC4, 0x0A, 0x21, 0x10},  // JSON
    {0x24, 0x0A, 0xC4, 0x0A, 0x20, 0x11},  // FARUG
};

const char* mac_names[MAC_ADDRESS_TOTAL] = {
    "Fauzan Firdaus",                      // 0
    "Africha Sekar Wangi",                 // 1
    "Rafaina Erin Sadia",                  // 2
    "Antonius Michael Yordanis Hartono",   // 3
    "Dinda Sofi Azzahro",                  // 4
    "Muhammad Fahmi Ilmi",                 // 5
    "Dhanishara Zaschya Putri Syamsudin",  // 6
    "Irsa Fairuza",                        // 7
    "Revalinda Bunga Nayla Laksono",       // 8
    "BISMA",                               // 9
    "JSON",                                // 10
    "FARUG",                               // 11
};

typedef struct {
  bool W,A,S,D,Q,E,C,V,R,F;
  float inc;
} ControlMsg;

void onRecv(const uint8_t* mac, const uint8_t* data, int len) {
  if (len == sizeof(ControlMsg)) {
    ControlMsg m; memcpy(&m, data, len);
   Serial.printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%.3f\n", m.W, m.A, m.S, m.D, m.Q, m.E, m.C, m.V, m.R, m.F, m.inc);
  }
}



const int RECEIVER_MAC_INDEX = 2;
void setup() {
    Serial.begin(115200);
    Serial.println("ESP RECIEVER");
    WiFi.mode(WIFI_STA);
    if (esp_wifi_set_mac(WIFI_IF_STA, mac_addresses[RECEIVER_MAC_INDEX]) != ESP_OK) {
    Serial.println("!!! Gagal mengatur MAC Address !!!");
    return;
    }
    esp_now_init();
    esp_now_register_recv_cb(onRecv);

    Serial.println("ESP-NOW Receiver siap");
}


void loop() {
}