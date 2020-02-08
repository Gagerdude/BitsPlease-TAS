#include "avr/io.h"

#define DELAY_CYCLES(n) __builtin_avr_delay_cycles(n);

#define DATA_PIN PINB
#define DATA_MASK 0x01

#define WRITE_HIGH PORTB |= 0x01
#define WRITE_LOW PORTB &= 0xFE

byte command = 0x00;
void readCommand();

void setup() {
  Serial.begin(9600);
  Serial.println("Started!");
  DDRB |= B00000001;
}

byte arr[] { 0x00, 0x02 };
size_t arrayLen = 2;

void loop() {
//  digitalWrite(8,HIGH);
//  Serial.println(PORTB, HEX);
//  Serial.print('\n');
//  Serial.println(0x00, BIN);
//  sendByte(0x00);
//  delay(1000);
//  Serial.print('\n');
//  Serial.println(0x01, BIN);
  sendBytes(arr, arrayLen);
//  delay(1000);
//  Serial.print('\n');
//  Serial.println(0x02, BIN);
//  delay(1000);
//  Serial.print('\n');
//  Serial.println(0x03, BIN);
//  sendByte(0x03);
//  delay(1000);
}

byte readBit() {
  byte thisBit = 0x00;

  // guestimating 3 cycles for each? 1 and, 1 shift, one or compound
  thisBit |= (DATA_PIN & DATA_MASK) << 3;
  DELAY_CYCLES(13);

  thisBit |= (DATA_PIN & DATA_MASK) << 2;
  DELAY_CYCLES(13);

  thisBit |= (DATA_PIN & DATA_MASK) << 1;
  DELAY_CYCLES(13);

  thisBit |= (DATA_PIN & DATA_MASK);

  return thisBit;
}

byte readByte() {
  byte inByte = 0x00;
  short bitsLeftToRead = 8;

  while(bitsLeftToRead){        // 3 cycles when true
    bitsLeftToRead -= 1;        // 1 cycle
    byte thisBit = readBit();

    if(thisBit == 0x07){        // 3 cycles when true
      inByte |= (0x01 << bitsLeftToRead); // 2 cycles
      DELAY_CYCLES(6);
    } else {                    // 2 cycles when false
      DELAY_CYCLES(10);
    }
  }

  return inByte;
}

void readCommand() {
  command = readByte();

  switch (command) {
    case 0x00: // STATUS
      // sendByte(0x05);
      // sendByte(0x00);
      // sendByte(bitFlags);
      Serial.println("Got Status");
      break;
    case 0x02: // POLL
      // send button and stick state
      Serial.println("Got Poll");
      break;
    case 0x03: // READ
      Serial.println("Got Read");
      break;
    case 0x04: // WRITE
      Serial.println("Got Write");
      break;
    default:
      // some kind of error handling?
      Serial.print("Got Something Else: ");
      Serial.println(command, HEX);
      break;
  }
}

void sendBytes(const byte* b, size_t n) {
  byte mask;
  while (n!=0) {
    WRITE_LOW; // 2 cycles
    mask = 0b10000000;
    // write start bit
    while (mask!=0) {
      DELAY_CYCLES(4); // 11 cycles
      if ((*b) & mask) { // 1 cycle (AND)
        WRITE_HIGH; // 2 cycles
        mask >>= 1; // 1 cc
        DELAY_CYCLES(33);
      }
      else {
        mask >>= 1; // 1 cc
        DELAY_CYCLES(30); // 31 cycles
        WRITE_HIGH; // 2 cycles
      }
      if (mask!=0) {
        DELAY_CYCLES(10);
        WRITE_LOW; // 2 cycles
        DELAY_CYCLES(2);
      }
    }  // 3 cycles
    ++b;
    --n;
  }
}
