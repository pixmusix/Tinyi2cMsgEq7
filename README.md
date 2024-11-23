# Tinyi2cMsgEq7
Interface with [Mixed Signal Intergrations](https://mix-sig.com/) [MsgEq7](https://mix-sig.com/index.php/msgeq7-seven-band-graphic-equalizer-display-filter) via i2c.
The MsgEq7 is a 7 band spectral analyser with analogue filters, peak detection, and a multiplexor.
The [AtTiny85](https://www.microchip.com/en-us/product/attiny85) microprocessor is used as a slave device to interface between the i2c Bus and the MsgEq7.
A Trs 3.5mm jack is used as an audio input.

![Alt text](assets/board.jpg)

### Inits

The TinyI2cMsgEq7 joins an i2c bus at address 0x08.
When it first joins the bus it's ready to send data.
The MsgEq7's internal multiplexor is primed to output the value of the first filter.
Simply requesting/reading data, usi

### Basic Use
```cpp
#define WIRE Wire
#define SLAVE_ADDR 0x08

int graph[7];

void drawBand(int lsb, int msb) {
  int strobe = ((msb >> 4) & 0x07);
  Serial.print(strobe);
  Serial.print(":");
  int val = ((msb & 0x03) << 8) | (lsb);
  graph[strobe] = map(val, 0, 1024, 0, 12);
  for(int star = 0; star < graph[strobe]; star++) {
    Serial.print("*");
  }
  Serial.println("*");
}

void printNextBand() {
  Wire.requestFrom(SLAVE_ADDR, 2);
  int msb = 0;
  int lsb = 0;
  if (Wire.available()) {
    lsb = Wire.read();
    msb = Wire.read();
  }
  drawBand(lsb,msb);
}
```

### Return Protocol

The Tinyi2cMsgEq7 transmit data back to an i2c Master in two bytes.

Each 16bit word contains in order :
 1. a void bit
 2. a 3 bit index representing one of the seven filters from the MsgEq7s Multiplexor (MSB)
 3. 2 void bits
 4. a 10bit value representing the output of the multiplexor at that index. (LSB)

The data is transmitted little endian.

![Alt text](assets/MsgEq7%20Reading%20i2c%20Chunck%20Syntax.jpg)



For more information, see my complete writeup on Core-Electronics Australia's Website.
