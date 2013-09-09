#pragma once

#include "ofxOscSender.h"

#include "ofMain.h"

#define NUM 50
#define NUM2 7
#define PTNUM 62

struct Ball {
    int radius;
    int hue;
    ofPoint pos,vel,acc;
};

struct Ball2 {
    int size;
    int hue;
    ofPoint pos,vel,acc;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    
    // colors and stuff
    //-----------------------------
    int brightness;
    int hue;
    int huePoint[PTNUM];
    
    // pixel to osc
    //-----------------------------
    void pixelToOsc();
    ofxOscSender osc;
    ofImage img, img2;
    
    //unsigned char * pixels;
    ofPixels pixels;
    
    unsigned char r, g, b;
    int grabWidth, grabHeight;
    
    // draw functions
    //-----------------------------
    int drawFunction;
    
    void drawFunctions();
    
    void drawOff();
    void drawRainbow();
    void drawRainbowStripH();
    void drawRainbowStripV();
    void drawWaves();
    void drawWaveFade();
    void drawSun();
    void drawTriangles();
    void drawTileScroll();
    
    void drawText();
    ofTrueTypeFont font;
    float tX;
    
    void drawBalls();
    void updateBalls();
    void setupBalls();
    Ball balls[NUM];
    Ball2 balls2[NUM2];
    
    // counters
    //-----------------------------
    void scrollerUpdates();
    float counterShape;
    int pointCount;
};