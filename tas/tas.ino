#include "avr/io.h"

#define DELAY_CYCLES(n) __builtin_avr_delay_cycles(1000*n);

#define DATA_PIN PINB
#define DATA_MASK 0x01

#define WRITE_HIGH PORTB |= 0x01
#define WRITE_LOW PORTB &= 0xFE

byte command = 0x00;
void readCommand();
void sendByte(byte b);

void setup() {
  Serial.begin(9600);
  Serial.println("Started!");
  DDRB |= B00000001;
}


void loop() {
//  digitalWrite(8,HIGH);
//  Serial.println(PORTB, HEX);
  Serial.print('\n');
  Serial.println(0x00, BIN);
  sendByte(0x00);
  delay(1000);
  Serial.print('\n');
  Serial.println(0x01, BIN);
  sendByte(0x01);
  delay(1000);
  Serial.print('\n');
  Serial.println(0x02, BIN);
  sendByte(0x02);
  delay(1000);
  Serial.print('\n');
  Serial.println(0x03, BIN);
  sendByte(0x03);
  delay(1000);
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

void sendByte(byte b) {
  byte remainingBits = 8;
  byte mask = 0b10000000;
  // write start bit
  do {
    WRITE_LOW; // 2 cycles
    DELAY_CYCLES(11); // 11 cycles
//    Serial.print(PINB&0x01, BIN);
    // If bit is a 1
    if (b & mask) { // 1 cycle (AND)
      // true -> 2 cycles (BREQ)
      WRITE_HIGH; // 2 cycles
      // Subtotal: 11 + 1 + 2 + 2 = 16 CC
      DELAY_CYCLES(30);
   //      Serial.print(PINB&0x01, BIN);
 //      Serial.print(PINB&0x01, BIN);
      // Subtotal = 16 + 30 = 46 cc
    }
    else {
      // false -> 1 cycle (BREQ)
      DELAY_CYCLES(31); // 31 cycles
      //Serial.print(PINB&0x01, BIN);
  //      Serial.print(PINB&0x01, BIN);
      WRITE_HIGH; // 2 cycles
      // Subtotal: 11 + 1 + 1 + 31 + 2 = 46 cc
    }
 //    Serial.print(PINB&0x01, BIN);
    remainingBits--; // 1 cycle
    if (remainingBits==0) { // 2 cc if true
    } else { // 1 cc if false
      mask >>= 1; // 1 cc
    }
    //    Serial.print("\n");
  } while (remainingBits); // 3 cycles
}
