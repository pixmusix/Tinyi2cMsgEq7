class Boid {
 
 PVector location;
 PVector velocity;
 PVector acceleration;
 
 float size;
 DynamicHue colour;
 float maxSpeed;
 float gravity;
 
 Boid(float x, float y, float sz) {
   maxSpeed = 15;
   gravity = 10;
   size = sz;
   
   int hue = ceil(map(x, 0, width, 0, 255));
   colour = new DynamicHue(hue, ceil(size*10), 255);
   
   acceleration = new PVector();
   velocity = new PVector();
   location = new PVector(x, y);
 }
   
 void physics() {
   applyForce(new PVector(0, dynamicGravity()));
   velocity.add(acceleration);
   velocity.limit(maxSpeed);
   location.add(velocity);
   acceleration.mult(0);
   outOfBounds();
 }
 
 void applyForce(PVector force) {
   /* Scale in-force by size because heavy particles 
   shoule be harder to accelerate*/
   force.div(size);
   acceleration.add(force);
 }
 
 float dynamicGravity() {
   // Our gravity is more aggressive as height increases.
   // This gives our animation a bounciness.
   float dynG = map(location.y, height, 0, 0, gravity);
   return dynG;
 }
 
 void outOfBounds() {
   // if we hit the walls bounce off elastically
   if (location.x < 0 | location.x > width) {
     velocity.x *= -1;
   }
   // if we hit the flor we semi-elastically bounce,
   if (location.y > height - size) {
     velocity.y *= -0.5;
     location.y = height - size;
   }
 }
 
 void paint() {
   // our colour is based on the rms of our x-coor
   stroke(1);
   fill(colour.pinch(ceil((location.x / width) * 215)));
   ellipse(location.x, location.y, size, size);
 }
 
}
