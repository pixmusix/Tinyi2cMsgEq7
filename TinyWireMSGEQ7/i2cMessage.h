struct i2cMessage {
  byte lsb;
  byte msb;
};

i2cMessage makei2cMsg(byte msb, byte lsb) {
  i2cMessage msg;
  msg.msb = msb;
  msg.lsb = lsb;
  return msg;
}

class i2cBuffer {

  protected:
    static const int bufSize = 128;
    i2cMessage postables[bufSize];
    int writeMark;
    int readMark;

  public:

    i2cBuffer() {
      writeMark = 0;
      readMark = 0;
    }

    bool isEmpty() {
      return (readMark == writeMark);
    };

    void add(i2cMessage msg) {
      postables[writeMark] = msg;
      writeMark = incrementMarker(writeMark);
    }

    i2cMessage getNext() {
      i2cMessage msg = postables[readMark];
      readMark = incrementMarker(readMark);
      return msg;
    } 

    int incrementMarker(int mark) {
      mark = mark + 1;
      mark = mark % bufSize;
      return mark;
    }
};