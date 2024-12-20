struct MsgEq7 {

  static const int num_bands = 7;
  int index;                          // next filter from multiplexor 
  int lastIndex;                      // last filter from multiplexor

  byte pinStrobe;                     // output : multiplexor control
  byte pinReset;                      // multiplexor reset
  byte pinReader;                     // filter data from multiplexor

  int getVal() {
    delayMicroseconds(36);            // to : output stabilization
    return analogRead(pinReader);
  }

  int readNextBand() {
    strobeDown();			                // request and output
    int w = getVal();                 // Get band from multiplexor
    strobeUp();                       // index increments
    return w;
  }

  int readBandAt(int target) {
    target = target % num_bands;
    strobeTo(target);                 // get to the target band
    return readNextBand();
  }

  void reset() {
    strobeDown();
    digitalWrite(pinReset, HIGH);     // reset
    delayMicroseconds(1);             // tr: reset pulse width
    digitalWrite(pinReset, LOW);
    delayMicroseconds(10);            // just good manners
    strobeUp();                       // arm the strobe
    delayMicroseconds(72);            // trs: reset to strobe
    index = 0;                        // set multiplexor pos
    lastIndex = 6;
  }

  int incrementIndex() {
    lastIndex = index;                // what did we just read?
    index += 1;                       // what we will read next?
    index = index % num_bands;        // we're counting base 7
    return index;
  }

  void strobeTo(int target) {
    target = target % num_bands;
    while(target != index) {
      strobeDown();                   
      strobeUp();                     // index increments here
    }
  }

  void strobeUp() {
    digitalWrite(pinStrobe, HIGH);
    delayMicroseconds(18);            // ts : strobe pulse width
    incrementIndex();                 // last band has spent 
    /* next band ready! */
  }

  void strobeDown() {
    digitalWrite(pinStrobe, LOW);
    delayMicroseconds(72);            // tss : strobe to strobe
    /* filter live on output */
  }
};

MsgEq7 makeMsgEq7(byte rdd, byte str, byte rst) {
  MsgEq7 eq;

  /* necessary IO */
  eq.pinStrobe = str;
  eq.pinReset = rst;
  eq.pinReader = rdd;

  pinMode(eq.pinStrobe, OUTPUT);
  pinMode(eq.pinReset, OUTPUT);
  pinMode(eq.pinReader, INPUT);

  eq.reset();                        // init MsgEq7

  return eq;
}