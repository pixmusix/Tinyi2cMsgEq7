class DynamicHue {
  
  int hue;
  int saturation;
  int brightness;
  
  Register hueHistory;
  
  DynamicHue(int h, int s, int b) {
    hue = h;
    saturation = s;
    brightness = b;
    hueHistory = new Register(32);
  }
  
  color pinch(int h) {
    /* A dynamic hue returns the rms of it's last 32 hues
    this smooths out the hue accross pinches of time */
    hue = h;
    hueHistory.push(h);
    return color(hueHistory.rms(), saturation, brightness);
  }
  
}
