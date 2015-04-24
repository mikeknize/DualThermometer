#include <LiquidCrystal.h>
#include <OneWire.h>
#include <Wire.h>

LiquidCrystal lcd(4, 3, 5);
#define LCD_WIDTH 16
#define LCD_HEIGHT 2

// change depending on pin probe data connected to
OneWire ds1(11);
OneWire ds2(12);

void setup(void) 
{
  lcd.begin(LCD_WIDTH, LCD_HEIGHT,1);
  lcd.setBacklight(HIGH);
}

int HighByte1, LowByte1, TReading1, SignBit1, Tc_100x, Whole1, Fract1,
HighByte2, LowByte2, TReading2, SignBit2, Tc_100y, Whole2, Fract2;
char buf[20], buff[20];
void loop(void) {
  byte i;
  byte j;
  byte present1 = 0;
  byte data1[12];
  byte addr1[8];
  byte present2 = 0;
  byte data2[12];
  byte addr2[8];

  if ( !ds1.search(addr1)) {
    lcd.setCursor(0,0);
    ds1.reset_search();
  }
  if ( !ds2.search(addr2)) {
    lcd.setCursor(1,0);
    ds2.reset_search();
  }

  if ( OneWire::crc8( addr1, 7) != addr1[7]) {
    lcd.setCursor(0,0);
      lcd.print("CRC is not valid!\n");
      return;
  }
  if ( OneWire::crc8( addr2, 7) != addr2[7]) {
    lcd.setCursor(0,0);
      lcd.print("CRC is not valid!\n");
      return;
  }

  ds1.reset();
  ds1.select(addr1);
  ds1.write(0x44,1);         // start conversion, with parasite power on at the end
  ds2.reset();
  ds2.select(addr2);
  ds2.write(0x44,1);
  delay(1000);     
  
  present1 = ds1.reset();
  ds1.select(addr1);    
  ds1.write(0xBE);         // Read Scratchpad
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data1[i] = ds1.read();
  }
  present2 = ds2.reset();
  ds2.select(addr2);    
  ds2.write(0xBE);         // Read Scratchpad
  for ( j = 0; j < 9; j++) {           // we need 9 bytes
    data2[j] = ds2.read();
  }
  
  //Left Probe
  LowByte1 = data1[0];
  HighByte1 = data1[1];
  TReading1 = (HighByte1 << 8) + LowByte1;
  SignBit1 = TReading1 & 0x8000;  // test most sig bit
  if (SignBit1) // negative
  {
    TReading1 = (TReading1 ^ 0xffff) + 1; // 2's comp
  }
  Tc_100x = (6 * TReading1) + TReading1 / 4;    // multiply by (100 * 0.0625) or 6.25

  Whole1 = Tc_100x / 100;  // separate off the whole and fractional portions
  Fract1 = Tc_100x % 100;
  Whole1 = (Whole1 * 9/5) + 32; //Farenheit

  sprintf(buf, "<<  %c%d.%d\337F     ",SignBit1 ? '-' : '+', Whole1, Fract1 < 10 ? 0 : Fract1);
  lcd.setCursor(0,0);
  lcd.print(buf);
  
 // Rt Probe 
  LowByte2 = data2[0];
  HighByte2 = data2[1];
  TReading2 = (HighByte2 << 8) + LowByte2;
  SignBit2 = TReading2 & 0x8000;  // test most sig bit
  if (SignBit2) // negative
  {
    TReading2 = (TReading2 ^ 0xffff) + 1; // 2's comp
  }
  Tc_100y = (6 * TReading2) + TReading2 / 4;    // multiply by (100 * 0.0625) or 6.25

  Whole2 = Tc_100y / 100;  // separate off the whole and fractional portions
  Fract2 = Tc_100y % 100;
  Whole2 = (Whole2 * 9/5) + 32; //Farenheit

  sprintf(buff, "    %c%d.%d\337F  >>",SignBit2 ? '-' : '+', Whole2, Fract2 < 10 ? 0 : Fract2);
  lcd.setCursor(0,1);
  lcd.print(buff);
}
