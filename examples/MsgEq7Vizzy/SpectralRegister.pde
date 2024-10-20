class Register {
  
  int[] bin;
  int size;
  
  Register(int sz) {
    /* A register is a rolling static buffer 
    of arbitray length.*/ 
    size = sz;
    bin = new int[size];
  }
  
  public void push(int val) {
    /* There were n in the bed and the 
    new kid said "roll over"... */
    for(int i = size-1; i > 0; i--) {
      bin[i] = bin[i-1];
    }
    bin[0] = val;
  }
  
  public float rms() {
    // Root mean square: an approximation of many samps
    int sumOfSquares = 0;
    for(int i = 0; i < size; i++) {
      sumOfSquares += bin[i] * bin[i];
    }
    return sqrt(sumOfSquares / size);
  }
}

class DeltaRegister extends Register {
  
  int[] delta;
  
  DeltaRegister(int sz) {
    /* this child will instead approximate the
    change in state of many samples.
    We might have, instead, considered the 'RMSE'*/
    super(sz);
    delta = new int[size];
  }
  
  float getDx() {
    return abs(rms() - rms(delta));
  }
  
  public void cache() {
    deltaPush(bin[size-1]);
  }
  
  private float rms(int[] other) {
    if (other.length != size) { return 0; }
    int sumOfSquares = 0;
    for(int i = 0; i < size; i++) {
      sumOfSquares += other[i] * other[i];
    }
    return sqrt(sumOfSquares / size);
  }
  
  public void deltaPush(int val) {
    for(int i = size-1; i > 0; i--) {
      delta[i] = delta[i-1];
    }
    delta[0] = val;
  }
  
}
