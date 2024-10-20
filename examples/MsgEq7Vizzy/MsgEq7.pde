class MsgEq7 {
  I2C i2c;
  
  MsgEq7(I2C comms, int resetPin) {
    GPIO.pinMode(resetPin, GPIO.OUTPUT);
    i2c = comms;
    reset(resetPin);
  }
  
  public byte[] get() {
    i2c.beginTransmission(0x08);    
    return i2c.read(2);
  }
  
  private void reset(int resetPin) {
    GPIO.digitalWrite(resetPin, GPIO.LOW);
    delay(1);
    GPIO.digitalWrite(resetPin, GPIO.HIGH);
  }
}

class I2CMessage {
  int strobeIndex;
  int strobeValue;
  
  I2CMessage(byte msb, byte lsb) {
    /* The i2c MsgEq7 device protocol is:
      xiii xxvv vvvv vvvv
    Where -> i = 3bit index (0b000, 0b110)
          -> v = 10bit value (0x000, 0x400)*/
    strobeIndex = msb >> 4 & 0x07;
    strobeValue = (msb & 0x03) << 8 | lsb;
  }
  
  public void printMsg() {
    // A nice text based spectroscope for debugging
    print(strobeIndex, ":");
    int stars = floor(map(strobeValue, 0, 1024, 0, 13));
    while (stars > 0) {
      print("*");
      stars = stars - 1;
    }
    println("*");
  }
}
