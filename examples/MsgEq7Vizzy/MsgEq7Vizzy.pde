import processing.io.I2C;
import processing.io.GPIO;

MsgEq7 Mq7;
ArrayList<Boid> Boids;
DeltaRegister[] Spectrum;
boolean debug = false;

void setup() {
  size(600,400);
  colorMode(HSB, 255);
  
  I2C i2c = new I2C(I2C.list()[0]);
  int i2CDeviceRstPin = 23;
  Mq7 = new MsgEq7(i2c, i2CDeviceRstPin);
  
  Boids = new ArrayList<Boid>();
  for(int x = 0; x < width; x += 3) {
    Boids.add(new Boid(x, height, random(5,15)));
  }
  
  Spectrum = new DeltaRegister[7];
  for(int i = 0; i < Spectrum.length; i++) {
    // Smaller buffers will be more reactive
    Spectrum[i] = new DeltaRegister(ceil(pow(2, i)) + 2);
  }
}

void draw() {
  background(0);
  if (debug) { println(frameRate); }
  
  //Read MsgEq7's bands -> Spectrum's registers
  for (int k = 0; k < Spectrum.length; k++) {
    byte[] data = Mq7.get();
    byte lsb = data[0];
    byte msb = data[1];
    I2CMessage msg = new I2CMessage(msb,lsb);
    Spectrum[msg.strobeIndex].push(msg.strobeValue);
    
    if (debug) { msg.printMsg(); }
  }
  
  for(int k = 0; k < Boids.size(); k++) {
    //Gimme a boid
    Boid particle = Boids.get(k);
    
    //Generate a lift by mapping the boids x co-ordinate to the Spectrum
    int particleBand = floor(map(particle.location.x, 0, width, 1, 7));
    particleBand = constrain(particleBand, 0, 6);
    float lift = Spectrum[particleBand].getDx();
    lift = (lift > 70) ? map(lift, 0, 1024, 0, -16) : 0;
    
    // We're done with this reading, let's cache it.
    Spectrum[particleBand].cache();
    
    // We might also make give them a little bleed.
    float jiggle = (random(2) - 1) / 4;
    
    // Lift & jiggle -> a force which we apply to our boid.
    PVector force = new PVector(jiggle, lift);
    particle.applyForce(force);
    
    // Do phyics and draw it to screen
    particle.physics();
    particle.paint();
  }
}
