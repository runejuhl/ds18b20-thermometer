#include <OneWire.h>

// DS18S20 Temperature chip i/o
OneWire ds(10);                 // on pin 10

void setup(void) {
  // initialize inputs/outputs
  // start serial port
  Serial.begin(9600);
}

/* Taken from
   https://forum.arduino.cc/index.php?topic=46320.msg335376#msg335376 */
void printBits(byte myByte){
 for(byte mask = 0x80; mask; mask >>= 1){
   if(mask  & myByte)
       Serial.print('1');
   else
       Serial.print('0');
 }
}

void printTemp(byte* bs) {
  byte lsb = bs[1];
  byte msb = bs[0];

  int t=0;

  t += 2^6 * (msb ^ 0b0000100);
  t += 2^5 * (msb ^ 0b0000010);
  t += 2^4 * (msb ^ 0b0000001);
  t += 2^3 * (lsb ^ 0b1000000);
  t += 2^2 * (lsb ^ 0b0100000);
  t += 2^1 * (lsb ^ 0b0010000);
  t += 2^0 * (lsb ^ 0b0001000);
  t += 2^-1 * (lsb ^ 0b0001000);
  t += 2^-2 * (lsb ^ 0b0000100);
  t += 2^-3 * (lsb ^ 0b0000010);
  t += 2^-4 * (lsb ^ 0b0000001);

  boolean s = msb ^ 0b1000000;
  if (!s) {
    t *= -1;
  }

  Serial.print("Temp: ");
  Serial.print(String((float)t / 100.0, 2));
  Serial.println(" degrees C");
}

void loop(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  byte bs[2];

  ds.reset_search();
  if ( !ds.search(addr)) {
    Serial.print("No more addresses.\n");
    ds.reset_search();
    return;
  }

  Serial.print("R=");
  for( i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.print("CRC is not valid!\n");
    return;
  }

  if ( addr[0] == 0x10) {
    Serial.print("Device is a DS18S20 family device.\n");
  }
  else if ( addr[0] == 0x28) {
    Serial.print("Device is a DS18B20 family device.\n");
  }
  else {
    Serial.print("Device family is not recognized: 0x");
    Serial.println(addr[0],HEX);
    return;
  }

  while(1) {
    ds.reset();
    ds.select(addr);

    ds.write(0x44,1);         // start conversion, with parasite power on at the end

    delay(1000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.

    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE);         // Read Scratchpad

    Serial.print("P=");
    Serial.print(present,HEX);
    Serial.print(" ");
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = ds.read();
      /* Serial.print(data[i], HEX); */
      /* Serial.print(" "); */
    }
    bs[0] = data[0];
    bs[1] = data[1];
    printBits(data[1]);
    printBits(data[0]);

    Serial.print(" CRC=");
    byte crc = OneWire::crc8( data, 8);

    if (crc < 0x10) {
      /* pad with zero */
      Serial.print('0');
    }
    Serial.print(crc, HEX);
    Serial.print(" ");

    printTemp(bs);
  }
}
