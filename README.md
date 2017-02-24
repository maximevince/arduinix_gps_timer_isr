# arduinix_gps_timer_isr
Arduino code for using the ArduiNIX Nixie shield with a GPS for time synchronisation and timer interrupts for stable tube multiplexing/refreshing

## What is it
Arduino code for using the excellent ArduiNIX Nixie shield: http://arduinix.com/  
With an added GPS for time synchronisation  
And Timer1 interrupt used for Nixie tube refreshing / multiplexing, for a stable non-jittering tube display  
Non-blocking tube refreshing (no `delay()`-functions used)

![arduiNIX image](/img/arduinix.jpg?raw=true "arduiNIX")

## Pinout
Default ArduinNIX pinout for 6 nixie tubes:
#### SN74141 (1)
- ledPin_0_a: 2;
- ledPin_0_b: 3;
- ledPin_0_c: 4;
- ledPin_0_d: 5;

#### SN74141 (2)
- ledPin_1_a: 6;
- ledPin_1_b: 7;
- ledPin_1_c: 8;
- ledPin_1_d: 9;

#### Anode pins
- ledPin_a_1: 10
- ledPin_a_2: 11
- ledPin_a_3: 12
- ledPin_a_4: 13

#### Ublox Neo-6M GPS module using SPI 
- SPI pins on ICSP header (MISO, MOSI, SCK)
- ublox_CSn_PIN: A5
- ublox_RESETn_PIN: A4

