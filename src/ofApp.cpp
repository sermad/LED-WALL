#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // display stuff
    //----------------------------------
    ofBackground(0, 0, 0);        // background color is black
    ofEnableSmoothing();
    ofSetVerticalSync(true);  // disabled for now...
    ofSetFrameRate(30);
    
    // initial variables
    //----------------------------------
    grabWidth = 60;         // width of data
    grabHeight = 8;         // height of data
    brightness = 255;       // brightness
    counterShape = 0.0f;    // shape sin
    drawFunction = 1;         // initial draw function
    
    // initial setups
    //----------------------------------
    setupBalls();

    // osc send to server and port
    //----------------------------------
    osc.setup("127.0.0.1", 5002);
    
    // create the texture that we will transmit via osc
    img.allocate(grabWidth, grabHeight, OF_IMAGE_COLOR);
    img2.allocate(grabWidth, grabHeight, OF_IMAGE_COLOR);
    
    // load font for txt function
    font.loadFont("uni05_54.ttf", 6);
    
}

//--------------------------------------------------------------
void ofApp::setupBalls() {
    
    for (int i = 0; i < NUM; i++) {
        //set the defaults for the balls...
        balls[i].radius = ofRandom(1, 6);
        balls[i].hue = ofRandom(0,254);
		balls[i].pos = ofPoint(ofRandom(0,60), ofRandom(0,7));
		balls[i].vel = ofPoint(ofRandomf() * 0.08f, ofRandomf() * 0.18f);
        balls[i].acc = ofPoint(0,0,0);
    }
    
    int b2size = 4;
    for (int i = 0; i < NUM2; i++) {
        //set the defaults for the balls...
        balls2[i].size = b2size;
        balls2[i].hue = ofRandom(0,254);
        balls2[i].pos = ofPoint(b2size*(i*2), grabHeight/2);
		balls2[i].vel = ofPoint(-2, 0);
        balls2[i].acc = ofPoint(0,0,0);
    }
    
}

//--------------------------------------------------------------
void ofApp::updateBalls() {
    
    // balls mode
    for (int i = 0; i < NUM; i++) {
		// horizontal boundaries
        //----------------------------------
		if (!ofInRange(balls[i].pos.x, 0, grabWidth)) {
			balls[i].vel.x *= -1;
			balls[i].pos.x = ofClamp(balls[i].pos.x, 0, grabWidth);
		}
        
		// vertical boundaries
        //----------------------------------
		if (!ofInRange(balls[i].pos.y, 0, grabHeight)) {
			balls[i].vel.y *= -1;
			balls[i].pos.y = ofClamp(balls[i].pos.y, 0, grabHeight);
		}
        
        // velocity
        //----------------------------------
        balls[i].vel += balls[i].acc;
		
		// position
        //----------------------------------
		balls[i].pos += balls[i].vel;
    }
    
    // used for our nightrider mode...
    for (int i = 0; i < NUM2; i++) {
		// horizontal boundaries
        //----------------------------------
		if (!ofInRange(balls2[i].pos.x, 0, grabWidth)) {
			balls2[i].vel.x *= -1;
			balls2[i].pos.x = ofClamp(balls2[i].pos.x, 0, grabWidth);
		}
        
		// vertical boundaries
        //----------------------------------
		if (!ofInRange(balls2[i].pos.y, 0, grabHeight)) {
			balls2[i].vel.y *= -1;
			balls2[i].pos.y = ofClamp(balls2[i].pos.y, 0, grabHeight);
		}
        
        // velocity
        //----------------------------------
        balls2[i].vel += balls2[i].acc;
		
		// position
        //----------------------------------
		balls2[i].pos += balls2[i].vel;
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    pixelToOsc();       // our pixel conversion
    updateBalls();      // updates the balls
    scrollerUpdates();  // all of our scrollers/counters...
    
}

//--------------------------------------------------------------
void ofApp::scrollerUpdates(){
    
    // hue scroller
    hue++;
    if (hue > 255) {    // openFrameworks hsb is 0-255, not 0-360 !!
        hue = 0;
    }
    
    // counters
    counterShape = counterShape + 0.058f;
    
}

//--------------------------------------------------------------
void ofApp::pixelToOsc(){
    
    // make a copy of the area we're sending via osc
    img.grabScreen(0, 240, grabWidth, grabHeight);
    
    // get the pixels via ref
    pixels = img.getPixelsRef();
    
    // TODO: Fix this line
    img2.setFromPixels(pixels, grabWidth, grabHeight, OF_IMAGE_COLOR);
    
    // Bundle not working
    //ofxOscBundle oB;
    
    for (int y = 0; y < grabHeight; y++) {
        for (int x = 0; x < grabWidth; x++) {
            
            // create temp colour
            ofColor tempc = pixels.getColor(x,y);
            
            // don't like this
            //int loc = (x + y*grabWidth)*3;
            
            //r = pixels[loc];    // reds
            //g = pixels[loc+1];  // greens
            //b = pixels[loc+2];  // blues
            
            // create osc message
            ofxOscMessage oM;
            
            oM.setAddress("/ledwall/");

            // set the r,g,b of the pixel
            oM.addIntArg(tempc.r);
            oM.addIntArg(tempc.g);
            oM.addIntArg(tempc.b);
            
            // set the x,y of the pixel
            oM.addIntArg(x);
            oM.addIntArg(y);
            
            // add message to bundle
            // not working
            //oB.addMessage( oM );
            
            // send the pixel via osc
            osc.sendMessage(oM);
            
            
        }
    }
    
    //osc.sendBundle( oB );
    //osc.sendMessage( oM );
    
    //for(int m=0; m<mB.getBundleCount(), m++;) {
        
    //string tempc = mB.getMessageAt(m);
    
    //cout << "osc - " << tempc << endl;
        
    //}
    
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // xxl pixel view
    //----------------------------------
    ofPushMatrix();
    ofTranslate(0, 80);
    ofScale(10, 10);
    drawFunctions();
    ofPopMatrix();
    // xxl border masks
    ofSetColor(0, 0, 0);
    ofRect(0, 0, ofGetWidth(), 80);
    ofRect(0, 160, ofGetWidth(), 260);
    
    // original view
    //----------------------------------
    ofPushMatrix();
    ofTranslate(0, 240);
    drawFunctions();
    ofPopMatrix();
    
    // osc image
    //----------------------------------
    int brightOSC = ofMap(brightness, 0, 255, 0, 60);
    ofSetColor(brightOSC);
    img2.draw(0, 0);
    
    
    // view descriptions
    //----------------------------------
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("XXL", 10, 180);
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Osc pixels", 10, 28);
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Original", 10, 268);
    
    
    // system brightness level + notes
    //----------------------------------
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("brightness == " + ofToString(brightness), 10, 360);
    ofDrawBitmapString("0-9 - draw modes // b = off", 10, 380);
}

void ofApp::drawFunctions(){
//--------------------------------------------------------------

    switch (drawFunction) {
        case 0:
            drawOff();
            break;
            
        case 1:
            drawRainbow();
            break;
            
		case 2:
            drawRainbowStripH();
            break;
            
        case 3:
            drawRainbowStripV();
            break;
            
        case 4:
            drawWaves();
            break;
            
        case 5:
            drawWaveFade();
            break;
            
        case 6:
            drawSun();
            break;
            
        case 7:
            drawTriangles();
            break;
            
        case 8:
            drawText();
            break;
            
        case 9:
            drawTileScroll();
            break;
            
        case 10:
            drawBalls();
            break;
    }
}

//--------------------------------------------------------------
void ofApp::drawOff(){
    // draw nothing... turns off LEDssssssssss
}

//--------------------------------------------------------------
void ofApp::drawRainbow(){
    
    // all pixels, the same color, scrolls through hue
    for (int x = 0; x < grabWidth; x++) {
        for (int y = 0; y < grabHeight; y++) {
            ofSetColor(ofColor::fromHsb(hue, 255, brightness));
            ofRect(x, y, 1, 1);
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::drawRainbowStripH(){
    
    // horizontal strips of rainbow goodness
    for (int i = 0; i < grabHeight; i++) {
        int huemap = ofMap(i, 0, grabHeight-1, 0, 200);
        ofSetColor(ofColor::fromHsb(huemap, 255, brightness));
        ofRect(0, i, 60, 0.5);
    }
    
}

//--------------------------------------------------------------
void ofApp::drawRainbowStripV(){
    
    // vertials strips of rainbow goodness
    for (int i = 0; i < grabWidth; i++) {
        int huemap = ofMap(i, 0, grabWidth-1, 0, 200);
        ofSetColor(ofColor::fromHsb(huemap, 255, brightness));
        ofRect(i, 0, 0.5, 8);
    }
    
}

//--------------------------------------------------------------
void ofApp::drawWaves() {
        
    // back layer is white
    float k = 0.0;
    for(int i = 0; i < grabWidth; i+=3)
    {
        ofSetColor(ofColor::fromHsb(255, 0, brightness));
        ofRect(i, 8, 3, -3 * (sin(counterShape-k)+1.0) - 2);
        k+=0.5;
    }
    
    // front layer black, masks back layer
    float kk = 0.0;
    for(int i = 0; i < grabWidth; i+=3)
    {
        ofSetColor(ofColor::fromHsb(255, 0, 0));
        ofRect(i, 10, 3, -3 * (sin(counterShape-kk)+1.0) - 2);
        kk+=0.5;
    }
    
}

//--------------------------------------------------------------
void ofApp::drawWaveFade() {
        
    float k = 0.0;
    for(int i = 0; i < grabWidth; i++)
    {
        for (int j = 0; j < grabHeight; j++) {
            
            int brightFade = ofMap(j, 0, grabHeight-1, brightness, 0);
            
            ofSetColor(ofColor::fromHsb(255, 0, brightFade));
            ofRect(i, 8, 1, -5 * (sin(1.4*counterShape-k)+1.0) - 0);
            k+=0.4;
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::drawSun() {
    
    ofSetCircleResolution(120); // high res circle
    
    ofNoFill();
    for(int i = 0; i < pointCount; i++){
        ofSetColor(ofColor::fromHsb(huePoint[i], 255, brightness));
        ofCircle(grabWidth/2, grabHeight/2, i);
    }
    pointCount++;
    huePoint[pointCount] = hue;
    
    if (pointCount > grabWidth) {
        pointCount = -2;
    }
    ofFill();
}

//--------------------------------------------------------------
void ofApp::drawTriangles() {
    
    ofSetCircleResolution(4);   // diamond shape
    
    ofNoFill();
    for(int i = 0; i < pointCount; i+=4){
        ofSetColor(ofColor::fromHsb(huePoint[i], 255, brightness));
        ofCircle(grabWidth/2, grabHeight/2, i);
    }
    
    pointCount++;
    huePoint[pointCount] = hue;
    
    if (pointCount > grabWidth) {
        pointCount = -2;
    }
    ofFill();
}

//--------------------------------------------------------------
void ofApp::drawText() {
    
    // text scroller
    string txt = "BBDO";
    
    ofSetColor(ofColor::fromHsb(255, 255, brightness));
    int txtWidth = font.stringWidth(txt);
    int txtHeight = font.stringHeight(txt);
    
    font.drawString(txt, tX, txtHeight+txtHeight/4);
    
    tX+=0.25f;
    if (tX > grabWidth) {
        tX = -txtWidth;
    }
}

//--------------------------------------------------------------
void ofApp::drawTileScroll() {

    // knight rider-ish effect...
    ofSetRectMode(OF_RECTMODE_CENTER);
    for (int i = NUM2-1; i > -1; i--) {
        ofSetColor(ofColor::fromHsb(hue, 255, brightness/((i+1)*2)));
        ofRect(balls2[i].pos.x, balls2[i].pos.y, balls2[i].size, balls2[i].size);
    }
    ofSetRectMode(OF_RECTMODE_CORNER);
    
}

//--------------------------------------------------------------
void ofApp::drawBalls() {
    
    // bouncing balls
    for (int i = 0; i < NUM; i++) {
        ofSetColor(ofColor::fromHsb(balls[i].hue, 255, brightness));
        ofRect(balls[i].pos.x, balls[i].pos.y, balls[i].radius, balls[i].radius);
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key) {
        // number keys change the draw functions
        case 'b':
            drawFunction = 0;   // draws only black / LEDs OFF !!
            break;
            
        case '1':
            drawFunction = 1;
            break;
        case '2':
            drawFunction = 2;
            break;
        case '3':
            drawFunction = 3;
            break;
        case '4':
            drawFunction = 4;
            break;
        case '5':
            drawFunction = 5;
            break;
        case '6':
            drawFunction = 6;
            break;
        case '7':
            drawFunction = 7;
            break;
        case '8':
            drawFunction = 8;
            break;
        case '9':
            drawFunction = 9;
            break;
        case '0':
            drawFunction = 10;
            break;
            
        // control the brightness via left/right arrow keys
        case OF_KEY_UP:
            brightness++;
            if (brightness > 255) {
                brightness = 255;
            }
            break;
            
        case OF_KEY_DOWN:
            brightness--;
            if (brightness < 0) {
                brightness = 0;
            }
            break;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
