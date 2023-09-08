/**
 * @brief Todo!
 */

#include "Arduino.h"
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <ArduinoLog.h>

#include "boardPins.h"

// Config
static const uint32_t GPSBaud = 9600;

// Objects
SoftwareSerial ss(RX_PIN, TX_PIN);
TinyGPSPlus gps;

// Headers
void displayInfo();

void setup()
{
  // Pins
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);

  // Begin
  Serial.begin(115200);
  ss.begin(GPSBaud);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  //
  Log.info("Starting version %s", REVISION);
}

void loop()
{
  /* TODO:
   * Lots of upgrades have to happen in here, namely:
   * - Move the CLOCK interface to its own object
   *  - Need to centralize components like safe power-on-reset (eeprom)
   *  - Need to ensure that we dont move unless we're 100% sure we can record what we're doing (so we never drift!)
   *  - RTOS functions like async gps reading, we need to be sure we dont race condition and loose time!
   */

  digitalWrite(DIR_PIN, gps.time.minute() % 2 == 0); // Set the dir if even/odd
  delay(250);                                        // Delay while dir is latched
  digitalWrite(EN_PIN, gps.time.second() < 10);      // Enable movement if first 10 seconds (Change this so that, on the minute mark, the hands have nearly finished moving)

  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true)
      ;
  }
}

void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}