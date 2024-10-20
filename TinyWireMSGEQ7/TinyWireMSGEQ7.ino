#include <TinyWireS.h>
#include "i2cMessage.h"
#include "MsgEq7.h"

const byte READ_EQ_PIN = A3;
const byte STROBE_EQ_PIN = 4;
const byte EQ_RESET_PIN = 1;

const byte RESET_WORD = 0xB0;
const byte SET_STROBE_WORD = 0xC0;
const byte WRITE_CYCLE_WORD = 0xF0;
const byte WRITE_DELTA_WORD = 0xD0;
const byte WRITE_IMMEDIATE_WORD = 0xA0;

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
  byte command_word;
  byte argument;
  while (0 < TinyWireS.available()) {
    byte received = TinyWireS.read();
    command_word = received & 0xF0;
    argument = received & 0x0F;
  }

  switch (command_word) {
    case RESET_WORD:
      eq.reset();
      break;

    case SET_STROBE_WORD:
      eq.strobeTo(argument);
      break;

    case WRITE_CYCLE_WORD:
      for(int b = 0; b < eq.num_bands; b++) {
        punchBandAt(b);
      }
      break;

    case WRITE_IMMEDIATE_WORD:
      punchBandAt(argument);
      break;

    case WRITE_DELTA_WORD:
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