#include <Arduino.h>
// This sketch provide the functionality of OTA Firmware Upgrade
#include "WiFi.h"
#include "HttpsOTAUpdate.h"
#include "esp_https_ota.h"

unsigned long last_ping = 0;

static const char *url_fw_bin = "https://raw.githubusercontent.com/RnD-VIL/OTA_Firmware_Jyoti/main/Control_FW.bin";
const char *host = "https://raw.githubusercontent.com";
const int httpsPort = 443;

// server certificates
extern const char certificate_pem_crt_start[] asm("_binary_src_certificate_pem_crt_start");
extern const char certificate_pem_crt_end[] asm("_binary_src_certificate_pem_crt_end");

const char *Version = "3.1";

const char *ssid = "VerticalInv";
const char *password = "@VIL@123";

int incomingByte;

void reconnect()
{
  int i = 0;
  // Loop until we're reconnected
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
      if (i == 10)
      {
        ESP.restart();
      }
      i++;
    }
    Serial.println("Connected to AP");
  }
}

void FirmwareUpdate(void)
{
  Serial.println("Starting OTA\nPlease Wait it takes some time ...");
  esp_http_client_config_t ota_client_config = {
      .url = url_fw_bin,
      .cert_pem = certificate_pem_crt_start,
  };
  esp_err_t ret = esp_https_ota(&ota_client_config);
  if (ret == ESP_OK)
  {
    printf("OTA done, restarting...\n");
    esp_restart();
  }
  else
  {
    printf("OTA failed...\n");
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.print("Active Firmware Version:");
  Serial.println(Version);
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");

    int i = 0;
    if (i == 10)
    {
      ESP.restart();
    }
    i++;
  }
  Serial.println("Connected To Wifi");
}

void loop()
{
  if (millis() - last_ping > 2000)
  {
    last_ping = millis();
    Serial.println(Version);
  }
  // put your main code here, to run repeatedly:
  if (WiFi.status() != WL_CONNECTED)
  {
    reconnect();
  }

  if (Serial.available() > 0)
  {
    incomingByte = Serial.read();
    if (incomingByte == 'U')
    {
      Serial.println("Firmware Update In Progress..");
      FirmwareUpdate();
    }
  }
  // delay(1000);
}