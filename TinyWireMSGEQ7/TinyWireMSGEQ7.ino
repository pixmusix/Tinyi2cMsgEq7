#include <TinyWireS.h>
#include "i2cMessage.h"
#include "MsgEq7.h"

const byte READ_EQ_PIN = A3;
const byte STROBE_EQ_PIN = 4;
const byte EQ_RESET_PIN = 1;

const byte RESET_BYTE = 0xB0;             // resets MsgEq7
const byte SET_STROBE_BYTE = 0xC0;        // force position of multiplexor
const byte WRITE_CYCLE_BYTE = 0xF0;       // output all filters 1-7.
const byte WRITE_DELTA_BYTE = 0xD0;       // punch an batch of filters
const byte WRITE_IMMEDIATE_BYTE = 0xA0;   // punch strobe at index

MsgEq7 eq = makeMsgEq7(READ_EQ_PIN, STROBE_EQ_PIN, EQ_RESET_PIN);
i2cBuffer i2cOuts = i2cBuffer();

void setup() {
  TinyWireS.begin(0x08);
  TinyWireS.onReceive(receiveEvent); 
  TinyWireS.onRequest(requestEvent);
}

void loop() {}

void punchNextBand() {
  int idx = eq.index;
  int dat = eq.readNextBand();
  appendToI2COutput(idx, dat);
}

void punchBandAt(int i) {
  int dat = eq.readBandAt(i);
  int idx = eq.lastIndex;
  appendToI2COutput(idx, dat);
}

void appendToI2COutput(int idx, int val) {
  /* 
  protocol : 0b.iii ..vv vvvv vvvv
  where : 
    i = multiplexor pos (0b000-0b110)
    v = peak amplitude of band (0x000-0x3FF)
  */
  byte lsb = val & 0xff;
  byte msb = (val >> 8) | (idx << 4);

  i2cMessage msg = makei2cMsg(msb, lsb);
  i2cOuts.add(msg);
}

void writeToi2c(i2cMessage msg) {
  /* dispatch in little endian */
  TinyWireS.write(msg.lsb);
  TinyWireS.write(msg.msb);
}

 void receiveEvent(byte numBytes) {
  byte instruction;
  byte argument;
  while (0 < TinyWireS.available()) {
    /*
    protocol 0b cccc aaaa
    where :
      c = action to take
      a = optional argument
    */
    byte received = TinyWireS.read();
    instruction = received & 0xF0;
    argument = received & 0x0F;
  }

  switch (instruction) {
    case RESET_BYTE:
      eq.reset();
      break;

    case SET_STROBE_BYTE:
      eq.strobeTo(argument);
      break;

    case WRITE_CYCLE_BYTE:
      for(int b = 0; b < eq.num_bands; b++) {
        punchBandAt(b);
      }
      break;

    case WRITE_IMMEDIATE_BYTE:
      punchBandAt(argument);
      break;

    case WRITE_DELTA_BYTE:
      for(int b = 0; b < argument; b++) {
        punchNextBand();
      }
      break;

    default: 
      break;
  }
 }

 void requestEvent() {
  if (i2cOuts.isEmpty()) {
    punchNextBand();
  }
  while(!i2cOuts.isEmpty()) {
    writeToi2c(i2cOuts.getNext());
  }
 }