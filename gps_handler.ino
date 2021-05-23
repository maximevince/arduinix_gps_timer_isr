//#define DEBUG
#define GPS_CLOCK_SYNC_RATE   (60ll * 60 * 1000)

static bool time_sync = false;

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

void printDateTime(time_t t, const char *tz)
{
    char buf[32];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
        hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
    Serial.println(buf);
}

static time_t GPSMakeTime(NeoGPS::time_t &dt)
{
#ifdef DEBUG
    char buf[64];
    sprintf(buf, "maketime %.2d:%.2d:%.2d, %.2d %.2d %.2d",
        dt.hours, dt.minutes, dt.seconds, dt.date, dt.month, dt.year);
    Serial.println(buf);
#endif

    tmElements_t tm;

    tm.Year = dt.full_year() - 1970;
    tm.Month = dt.month;
    tm.Day = dt.date;

    tm.Hour = dt.hours;
    tm.Minute = dt.minutes;
    tm.Second = dt.seconds;

    return makeTime(tm);
}

void GPSLoop() {
  static unsigned long last_update = 0;
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
      if (fix_data.valid.time && fix_data.valid.date) {
        /* Only update if > 1 hour since last update */
        if ((!time_sync) || (last_update > (millis() + GPS_CLOCK_SYNC_RATE)))
        {
          #ifdef DEBUG
            Serial.println("Setting time from GPS!");
          #endif
          last_update = millis();
          time_sync = true;
          time_t gpstime = GPSMakeTime(fix_data.dateTime);
          setTime(myTimeZone.toLocal(gpstime, &tcr));
          printDateTime(gpstime, tcr -> abbrev);
        }
      }
    }
  }
}

bool GPSIsTimeSynced(void)
{
  return time_sync;
}
