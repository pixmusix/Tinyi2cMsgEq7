#include <FastLED.h>

const byte NUM_LEDS = 13;
const byte LED_DATA_PIN = 2;
const byte READ_EQ_PIN = A3;
const byte STROBE_EQ_PIN = 4;
const byte EQ_RESET_PIN = 1;

struct MsgEq7 {
  static const int num_bands = 7;
  int bands[num_bands];
  int index;
  byte pinStrobe;
  byte pinReset;
  byte pinReader;

  int get() {
    return bands[index];
  }

  int get(int k) {
   return bands[k];
  }


  int set(int k) {
    bands[index] = k;
  }

  int readIt() {
    delayMicroseconds(36);            // to : output stabilization
    return analogRead(pinReader);
  }

  int readNextBand() {
    strobeDown();			                // request and output
    set(readIt());			              // cache our data
    strobeUp();                       // index increments
    return get(index - 1);		        // return the data we cached
  }

  int readBandAt(int target) {
    strobeTo(target);
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
  }

  int incrementIndex() {
    index += 1;
    index = index % num_bands;
    return index;
  }

  void strobeTo(int target) {
    target = target % num_bands;
    while(index != target) {
      strobeUp();
      strobeDown();
    }
  }

  void strobeUp() {
    digitalWrite(pinStrobe, HIGH);
    delayMicroseconds(18);          // ts : strobe pulse width
  }

  void strobeDown() {
    digitalWrite(pinStrobe, LOW);
    delayMicroseconds(72);          // tss : strobe to strobe
    incrementIndex();               // last band has spent; next band ready!
  }
};

MsgEq7 makeMsgEq7(byte rdd, byte str, byte rst) {
  MsgEq7 eq;
  eq.index = 0;
  eq.pinStrobe = str;
  eq.pinReset = rst;
  eq.pinReader = rdd;

  pinMode(eq.pinStrobe, OUTPUT);
  pinMode(eq.pinReset, OUTPUT);
  pinMode(eq.pinReader, INPUT);

  eq.reset();

  return eq;
}

MsgEq7 eq;
CRGB leds[NUM_LEDS];

void cylon() {
  int pos = NUM_LEDS * (-1);
  while (pos <= NUM_LEDS) { 
    FastLED.clear();
    int k = NUM_LEDS - abs(pos);
    leds[k] = CRGB(0,25,0);
    FastLED.show();
    delay(50);
    pos += 1;
  }
}

void setup() {
  pinMode(LED_DATA_PIN, OUTPUT);
  eq = makeMsgEq7(READ_EQ_PIN, STROBE_EQ_PIN, EQ_RESET_PIN);
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);

  //cylon the Leds as feedback that rainbow is working
  int pos = NUM_LEDS * (-1);
  while (pos <= NUM_LEDS) { 
    FastLED.clear();
    int k = NUM_LEDS - abs(pos);
    leds[k] = CRGB(0,25,0);
    FastLED.show();
    delay(50);
    pos += 1;
  }
}

void loop() {
 /* Populate eq.bands with new data */
 do {                             
   eq.readNextBand();
 } while (eq.index != 0);

 /* holds our interpolated data */
 double ledLum[NUM_LEDS];       
 for (int i=0; i < NUM_LEDS; i++) {

   /* where to sample from */
   const int idx = i * (eq.num_bands - 1) / NUM_LEDS;
   /* where to write our sample to */
   const int mod = i * (eq.num_bands - 1) % NUM_LEDS;

   /* interpolation */
   const double q = double(mod) / double(NUM_LEDS);
   ledLum[i] = eq.bands[idx] * (1.0-q) + eq.bands[idx+1] * q;

   /* convert from 10bit -> 8bit data */
   ledLum[i] = ledLum[i] / 1024.0 * 255.0;
   /* map the luminosity to a colour on our rainbow */
   leds[i] = CRGB(ledLum[i], 2, 25);
 }
 /* bit bang the colours to the hardware */
 FastLED.show();
}
 