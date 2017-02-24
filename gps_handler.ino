//#define DEBUG

void GPSSetup() {
  #ifdef DEBUG
    Serial.begin(115200);
    Serial.println("Starting GPS/SPI!");
  #endif
  
  pinMode(ublox_CSn_PIN, OUTPUT);
  pinMode(ublox_RESETn_PIN, OUTPUT);
  digitalWrite(ublox_RESETn_PIN, LOW);  // reset
  SPI.begin();
  digitalWrite(ublox_RESETn_PIN, HIGH); // not in reset
}

void GPSLoop() {
  static unsigned long last_update = 0;
  static bool time_sync = false;
  byte    in_byte = 0x0;
  uint8_t data;

  for(int i = 0; i < 20; i++)
  {
    /* Read SPI byte */
    digitalWrite(ublox_CSn_PIN, LOW);
    in_byte = SPI.transfer(0XFF);
    digitalWrite(ublox_CSn_PIN, HIGH);

    /* Decode NMEA */
    if (gps.decode(in_byte) == NMEAGPS::DECODE_COMPLETED) {
      fix_data = gps.fix();
      #ifdef DEBUG
        Serial.print("\r\nFIX: time valid: ");
        Serial.print(fix_data.valid.time);
        Serial.print(" time: ");
        Serial.print(fix_data.dateTime.hours);
        Serial.print(":");
        Serial.print(fix_data.dateTime.minutes);
        Serial.print(" lat: ");
        Serial.print(fix_data.latitude());
        Serial.print(" lon: ");
        Serial.print(fix_data.longitude());
        Serial.print("\r\n");
      #endif

      /* Sync clock */
      if (fix_data.valid.time)
      {
        /* Only update if > 1 hour since last update */
        if ((!time_sync) || (last_update > (millis() + 60ll * 60 * 1000)))
        {
          #ifdef DEBUG
            Serial.println("Setting time from GPS!");
          #endif
          last_update = millis();
          time_sync = true;
          setTime(fix_data.dateTime.hours, fix_data.dateTime.minutes, fix_data.dateTime.seconds, fix_data.dateTime.date, fix_data.dateTime.month, fix_data.dateTime.year);
        }
      }
    }
  }
}
