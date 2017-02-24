// sets tube display (without using "delay")
// should be called at a fixed rate, e.g. every 3 milliseconds
void WriteDisplay(int tube5, int tube4, int tube3, int tube2, int tube1, int tube0)
{
  static int display_status = 0;

  switch(display_status)
  {
    case 0:
      // Anode channel 2 - numerals 0,3
      digitalWrite(ledPin_a_4, LOW);
      //delay(100);
      DisplayBin(tube0, tube3);   
      digitalWrite(ledPin_a_2, HIGH);
      display_status++;
      break;

    case 1:
      // Anode channel 3 - numerals 1,4
      digitalWrite(ledPin_a_2, LOW);
      //delay(100);
      DisplayBin(tube1, tube4);   
      digitalWrite(ledPin_a_3, HIGH);   
      display_status++;
      break;

    case 2:
      // Anode channel 4 - numerals 2,5
      digitalWrite(ledPin_a_3, LOW);
      //delay(100);
      DisplayBin(tube2,tube5);   
      digitalWrite(ledPin_a_4, HIGH);   
      display_status=0;
      break;
  }
}

// Sets cathodes. Expects data in Bin/BCD format
// (native format of 74141 and DS1307 RTC module)
void DisplayBin(byte num1, byte num2) {
  // Write to output pins.
  digitalWrite(ledPin_0_a, bitRead(num2, 0));
  digitalWrite(ledPin_0_b, bitRead(num2, 1));
  digitalWrite(ledPin_0_c, bitRead(num2, 2));
  digitalWrite(ledPin_0_d, bitRead(num2, 3));   

  digitalWrite(ledPin_1_a, bitRead(num1, 0));
  digitalWrite(ledPin_1_b, bitRead(num1, 1));
  digitalWrite(ledPin_1_c, bitRead(num1, 2));
  digitalWrite(ledPin_1_d, bitRead(num1, 3));
}
