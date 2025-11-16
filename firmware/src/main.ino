#include <Arduino.h>
#include <WiFi.h>
#include <driver/adc.h>
#include <driver/gpio.h>
#include <DHT.h>

// Wi-Fi credentials (replace with provisioning flow or secrets manager later)
constexpr char WIFI_SSID[] = "YOUR_WIFI_SSID";
constexpr char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";
constexpr uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000; // 15 seconds

// GPIO assignments captured in docs/gpio-pinout.md
constexpr uint8_t ROOM1_DHT_PIN = 4;
constexpr uint8_t ROOM2_DHT_PIN = 5;
constexpr uint8_t BACKUP_VBAT_PIN = 1; // ADC1_CH0

#define DHTTYPE DHT11

DHT room1Dht(ROOM1_DHT_PIN, DHTTYPE);
DHT room2Dht(ROOM2_DHT_PIN, DHTTYPE);

constexpr gpio_num_t RELAY_PINS[] = {
  GPIO_NUM_33,
  GPIO_NUM_34,
  GPIO_NUM_35,
  GPIO_NUM_36,
  GPIO_NUM_37,
  GPIO_NUM_38,
};

constexpr int RELAY_INACTIVE_LEVEL = 0; // board ships active-high, so drive low at boot

constexpr float ADC_REF_V = 3.3f;
constexpr uint16_t ADC_MAX_VALUE = 4095;
constexpr float BACKUP_VBAT_DIVIDER_RATIO = (56.0f + 10.0f) / 10.0f; // 56k/10k divider -> 6.6:1 scaling

float countsToVoltage(uint16_t rawCounts, float dividerRatio) {
  const float adcVolts = (static_cast<float>(rawCounts) / ADC_MAX_VALUE) * ADC_REF_V;
  return adcVolts * dividerRatio;
}

float celsiusToFahrenheit(float tempC) {
  return (tempC * 9.0f / 5.0f) + 32.0f;
}

void initRelays() {
  gpio_config_t config = {};
  uint64_t mask = 0;
  for (const auto &pin : RELAY_PINS) {
    mask |= (1ULL << pin);
  }
  config.pin_bit_mask = mask;
  config.mode = GPIO_MODE_OUTPUT;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  config.pull_up_en = GPIO_PULLUP_DISABLE;
  config.intr_type = GPIO_INTR_DISABLE;
  gpio_config(&config);

  for (const auto &pin : RELAY_PINS) {
    gpio_set_level(pin, RELAY_INACTIVE_LEVEL);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Swift Cabin Controller booting...");

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(true);
  WiFi.disconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.printf("Connecting to WiFi SSID '%s'...\n", WIFI_SSID);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED &&
         (millis() - wifiStart) < WIFI_CONNECT_TIMEOUT_MS) {
    delay(100);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("WiFi connected successfully. IP: %s\n",
                  WiFi.localIP().toString().c_str());
  } else {
    Serial.println("WiFi connection timed out; continuing in offline mode.");
  }

  initRelays();
  room1Dht.begin();
  room2Dht.begin();
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
}

void loop() {
  float room1TempC = room1Dht.readTemperature();
  float room1Humidity = room1Dht.readHumidity();
  float room2TempC = room2Dht.readTemperature();
  float room2Humidity = room2Dht.readHumidity();

  if (isnan(room1TempC) || isnan(room1Humidity)) {
    Serial.println("Room 1 DHT read error");
  } else {
    Serial.printf(
        "Room 1: %.1f°F / %.1f%% RH\n",
        celsiusToFahrenheit(room1TempC),
        room1Humidity);
  }

  if (isnan(room2TempC) || isnan(room2Humidity)) {
    Serial.println("Room 2 DHT read error");
  } else {
    Serial.printf(
        "Room 2: %.1f°F / %.1f%% RH\n",
        celsiusToFahrenheit(room2TempC),
        room2Humidity);
  }

  uint16_t backupRaw = analogRead(BACKUP_VBAT_PIN);
  float backupVoltage = countsToVoltage(backupRaw, BACKUP_VBAT_DIVIDER_RATIO);
  Serial.printf("Backup battery: %.2f V (raw=%u)\n", backupVoltage, backupRaw);

  delay(1000);
}
