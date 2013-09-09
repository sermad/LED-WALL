/* 

Code modified from Paul Stoffregen
Copyright (c) 2013 Paul Stoffregen, PJRC.COM, LLC

*/

import processing.serial.*;
import java.awt.Rectangle;

import oscP5.*;
import netP5.*;
 
OscP5 oscP5;
NetAddress myRemoteLocation;

int ledWidth = 60;
int ledHeight = 8;
PImage img = createImage(ledWidth, ledHeight, RGB);

int numPorts=0;  // the number of serial ports in use
int maxPorts=24; // maximum number of serial ports
int locx, locy; // pixel location
int r, g, b;

Serial[] ledSerial = new Serial[maxPorts];     // each port's actual Serial port
Rectangle[] ledArea = new Rectangle[maxPorts]; // the area of the movie each port gets, in % (0-100)
boolean[] ledLayout = new boolean[maxPorts];   // layout of rows, true = even is left->right
PImage[] ledImage = new PImage[maxPorts];      // image sent to each port
int errorCount=0;
float framerate=30;

void setup() {
  
  // start oscP5, telling it to listen for incoming messages at port 5002 */
  oscP5 = new OscP5(this,5002);
 
  // set the remote location to be the localhost on port 5002
  myRemoteLocation = new NetAddress("127.0.0.1",5002);
  
  String[] list = Serial.list();
  delay(20);
  println("Serial Ports List:");
  println(list);
  
  serialConfigure("/dev/tty.usbmodem14761");  // change these to your port names
  if (errorCount > 0) exit();
  
  size(480, 400);
}

void update_serial() {
  
  //if (framerate == 0) framerate = m.getSourceFrameRate();
  framerate = 30.0; // TODO, how to read the frame rate???
  
  for (int i=0; i < numPorts; i++) {    
    // copy a portion of the movie's image to the LED image
    int xoffset = percentage(ledWidth, ledArea[i].x);
    int yoffset = percentage(ledHeight, ledArea[i].y);
    int xwidth =  percentage(ledWidth, ledArea[i].width);
    int yheight = percentage(ledHeight, ledArea[i].height);
    
    ledImage[i].copy(img, xoffset, yoffset, xwidth, yheight, 0, 0, ledImage[i].width, ledImage[i].height);
    
    // convert the LED image to raw data
    byte[] ledData =  new byte[(ledImage[i].width * ledImage[i].height * 3) + 3];
    image2data(ledImage[i], ledData, ledLayout[i]);
    if (i == 0) {
      ledData[0] = '*';  // first Teensy is the frame sync master
      int usec = (int)((1000000.0 / framerate) * 0.75);
      ledData[1] = (byte)(usec);   // request the frame sync pulse
      ledData[2] = (byte)(usec >> 8); // at 75% of the frame time
    } else {
      ledData[0] = '%';  // others sync to the master board
      ledData[1] = 0;
      ledData[2] = 0;
    }
    // send the raw data to the LEDs  :-)
    ledSerial[i].write(ledData); 
  }
}

//

// image2data converts an image to OctoWS2811's raw data format.
// The number of vertical pixels in the image must be a multiple
// of 8.  The data array must be the proper size for the image.
void image2data(PImage image, byte[] data, boolean layout) {
  int offset = 3;
  int x, y, xbegin, xend, xinc, mask;
  int linesPerPin = image.height / 8;
  int pixel[] = new int[8];
  
  for (y = 0; y < linesPerPin; y++) {
    if ((y & 1) == (layout ? 0 : 1)) {
      // even numbered rows are left to right
      xbegin = 0;
      xend = image.width;
      xinc = 1;
    } else {
      // odd numbered rows are right to left
      xbegin = image.width - 1;
      xend = -1;
      xinc = -1;
    }
    for (x = xbegin; x != xend; x += xinc) {
      for (int i=0; i < 8; i++) {
        // fetch 8 pixels from the image, 1 for each pin
        pixel[i] = image.pixels[x + (y + linesPerPin * i) * image.width];
        pixel[i] = colorWiring(pixel[i]);
      }
      // convert 8 pixels to 24 bytes
      for (mask = 0x800000; mask != 0; mask >>= 1) {
        byte b = 0;
        for (int i=0; i < 8; i++) {
          if ((pixel[i] & mask) != 0) b |= (1 << i);
        }
        data[offset++] = b;
      }
    }
  } 
}

// translate the 24 bit color from RGB to the actual
// order used by the LED wiring.  GRB is the most common.
int colorWiring(int c) {
  // return c;  // RGB
  return ((c & 0xFF0000) >> 8) | ((c & 0x00FF00) << 8) | (c & 0x0000FF); // GRB - most common wiring
}

// ask a Teensy board for its LED configuration, and set up the info for it.
void serialConfigure(String portName) {
  if (numPorts >= maxPorts) {
    println("too many serial ports, please increase maxPorts");
    errorCount++;
    return;
  }
  try {
    ledSerial[numPorts] = new Serial(this, portName);
    if (ledSerial[numPorts] == null) throw new NullPointerException();
    ledSerial[numPorts].write('?');
  } catch (Throwable e) {
    println("Serial port " + portName + " does not exist or is non-functional");
    errorCount++;
    return;
  }
  delay(50);
  String line = ledSerial[numPorts].readStringUntil(10);
  if (line == null) {
    println("Serial port " + portName + " is not responding.");
    println("Is it really a Teensy 3.0 running VideoDisplay?");
    errorCount++;
    return;
  }
  String param[] = line.split(",");
  if (param.length != 12) {
    println("Error: port " + portName + " did not respond to LED config query");
    errorCount++;
    return;
  }
  // only store the info and increase numPorts if Teensy responds properly
  ledImage[numPorts] = new PImage(Integer.parseInt(param[0]), Integer.parseInt(param[1]), RGB);
  ledArea[numPorts] = new Rectangle(Integer.parseInt(param[5]), Integer.parseInt(param[6]),
                     Integer.parseInt(param[7]), Integer.parseInt(param[8]));
  ledLayout[numPorts] = (Integer.parseInt(param[5]) == 0);
  numPorts++;
}

// scale a number by a percentage, from 0 to 100
int percentage(int num, int percent) {
  double mult = percentageFloat(percent);
  double output = num * mult;
  return (int)output;
}

// scale a number by the inverse of a percentage, from 0 to 100
int percentageInverse(int num, int percent) {
  double div = percentageFloat(percent);
  double output = num / div;
  return (int)output;
}

// convert an integer from 0 to 100 to a float percentage
// from 0.0 to 1.0.  Special cases for 1/3, 1/6, 1/7, etc
// are handled automatically to fix integer rounding.
double percentageFloat(int percent) {
  if (percent == 33) return 1.0 / 3.0;
  if (percent == 17) return 1.0 / 6.0;
  if (percent == 14) return 1.0 / 7.0;
  if (percent == 13) return 1.0 / 8.0;
  if (percent == 11) return 1.0 / 9.0;
  if (percent ==  9) return 1.0 / 11.0;
  if (percent ==  8) return 1.0 / 12.0;
  return (double)percent / 100.0;
}

void draw() {
  
  // then try to show what was most recently sent to the LEDs
  // by displaying all the images for each port.
  
  for (int i=0; i < numPorts; i++) {
    // compute the intended size of the entire LED array
    int xsize = percentageInverse(ledImage[i].width, ledArea[i].width);
    int ysize = percentageInverse(ledImage[i].height, ledArea[i].height);
    // computer this image's position within it
    int xloc =  percentage(xsize, ledArea[i].x);
    int yloc =  percentage(ysize, ledArea[i].y);
    // show what should appear on the LEDs
    image(ledImage[i], ledWidth - xsize / 2 + xloc, 10 + yloc);
  }

  // show the image in the pixel array
  image(img, 0, 0);
  
  // send the image to the serial port
  update_serial();
  
}

void oscEvent(OscMessage theOscMessage) {
  
  if (theOscMessage.checkAddrPattern("/ledwall/") == true) {
    
    r = theOscMessage.get(0).intValue();
    g = theOscMessage.get(1).intValue();
    b = theOscMessage.get(2).intValue();
    
    locx = theOscMessage.get(3).intValue();
    locy = theOscMessage.get(4).intValue();

    // set the pixels with the osc data
    img.loadPixels();
    img.pixels[locy*ledWidth+locx] = color(r, g, b);
    img.updatePixels();

    //println("r == " + r + " | " + "g == " + g + " | " + "b == " + b + " | " + "x == " + locx + " | " + "y == " + locy);
    
  }
  
}
