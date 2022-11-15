#include "ofApp.h"

// app_karplus
// Nicola Pisanti, GPLv3 License, 2016

// midi controlled polysynth example
// you need a midi keyboard for trying this! 
// remember also to select the right midi port and audio output device, as ususal.

//--------------------------------------------------------------
void ofApp::setup(){
    
    //patching-------------------------------

#ifdef OFAPP_KARPLUS_MIDI
    midiKeys.setPolyMode(8, 1);
    int voicesNum = midiKeys.getVoicesNumber();    
#else
    keyboard.setPolyMode(8);
    int voicesNum = keyboard.getVoicesNumber();       
#endif

    ksynth.setup(voicesNum);
    
    for( size_t i=0; i<ksynth.size(); ++i ){
#ifdef OFAPP_KARPLUS_MIDI
        midiKeys.outs_trig[i]  >> ksynth.in_trig( i );
        midiKeys.outs_pitch[i] >> ksynth.in_pitch( i );
#else
        keyboard.outs_trig[i]  >> ksynth.in_trig( i );
        keyboard.outs_pitch[i] >> ksynth.in_pitch( i );
#endif
    }
    
    ksynth.ch(0) >> chorus.ch(0) >> engine.audio_out(0);
    ksynth.ch(1) >> chorus.ch(1) >> engine.audio_out(1);

  
    // graphic setup---------------------------
    ofSetVerticalSync(true);
    ofDisableAntiAliasing();
    ofBackground(0);
    ofSetColor(ofColor(0,100,100));
    ofNoFill();
    ofSetLineWidth(1.0f);
    ofSetFrameRate(24);
    
#ifdef OFAPP_KARPLUS_MIDI     
    ofSetWindowShape(230, 370);
#else
    ofSetWindowShape(230, 530);
#endif 

    // GUI -----------------------------------
    gui.setHeaderBackgroundColor(ofColor(0,40,40));
    gui.setBorderColor(ofColor(0,40,40));
    
    gui.setDefaultFillColor(ofColor(0,90,90));
    gui.setDefaultBackgroundColor(ofColor(0,0,0));

    gui.setup("panel");
    gui.setName("karplus synth gui");
    gui.add( ksynth.parameters );
    gui.add( chorus.parameters );
    gui.setPosition(15, 20);
    
    
    // audio / midi setup----------------------------
#ifdef OFAPP_KARPLUS_MIDI    
    //get MIDI control
    midiIn.listPorts();
    midiIn.openPort(2); //set the right port !!!
    // for our midi controllers to work we have to add them to the engine, so it know it has to process them
    engine.addMidiController( midiKeys, midiIn ); // add midi processing to the engine
#endif

    engine.listDevices();
    engine.setDeviceID(1); // REMEMBER TO SET THIS AT THE RIGHT INDEX!!!!
    engine.setup( 44100, 512, 3);
    
    //Maps stuff
    coordinates =
    {
        { 42.2610492, -91.4501953 },
        { 43.0046471, -90.4833984 },
        { 43.0367759, -89.3847656 },
        { 41.9676592, -88.4619141 },
        { 41.2117215, -89.0332031 },
        { 40.5805847, -90.1318359 },
        { 40.6806380, -91.1865234 },
        { 41.1124688, -92.4169922 },
        { 42.1959688, -93.2958984 },
        { 43.2932003, -92.1972656 },
        { 44.0560117, -90.7470703 }
    };
    
    ofJson json = ofLoadJson("provider.json");
    
    tileProvider = std::make_shared<ofxMaps::MapTileProvider>(ofxMaps::MapTileProvider::fromJSON(json));
    Poco::ThreadPool::defaultPool().addCapacity(64);
    bufferCache = std::make_shared<ofxMaps::MBTilesCache>(*tileProvider, "cache/");
    tileSet = std::make_shared<ofxMaps::MapTileSet>(1024,
                                                    tileProvider,
                                                    bufferCache);
    
    tileLayer = std::make_shared<ofxMaps::MapTileLayer>(tileSet, 1 * 1920, 1 * 1080);
    
    ofxGeo::Coordinate chicago(41.8827, -87.6233);
    ofxGeo::Coordinate bethel(45.0579, -93.1605);
    
    tileLayer->setCenter(coordinates[3], 21);

    
}

//--------------------------------------------------------------
void ofApp::update(){
    // Maps stuff
    tileLayer->update();
    
    if (!ofIsFloatEqual(animation, 0.f))
        tileLayer->setCenter(tileLayer->getCenter().getNeighbor(animation, 0));

}

//--------------------------------------------------------------
void ofApp::draw(){
    // Maps stuff
    ofPushMatrix();
    //ofTranslate(-tileLayer->getWidth() / 2, -tileLayer->getHeight() / 2);
    tileLayer->draw(0, 0);
    ofPopMatrix();
    ofPushStyle();
    ofNoFill();
    ofSetColor(0, 255, 0);
    
    for (auto coordinate: coordinates)
    {
        auto tc = tileLayer->geoToPixels(coordinate);
        ofDrawCircle(tc.x, tc.y, 20);
    }
    ofPopStyle();
    
    //    cam.end();
    
    ofDrawBitmapStringHighlight(tileLayer->getCenter().toString(0), 14, ofGetHeight() - 32);
    ofDrawBitmapStringHighlight("Task Queue:" + ofx::TaskQueue::instance().toString(), 14, ofGetHeight() - 16);
    ofDrawBitmapStringHighlight("Connection Pool: " + bufferCache->toString(), 14, ofGetHeight() - 2);

    // draw GUI. PDSP stuff
    gui.draw();
    
    ofSetColor(ofRandom(255),ofRandom(255),ofRandom(255));
    float t = ofGetElapsedTimef();
    ofDrawBitmapString("A TEST CHANGE", (sin(t)*0.5+0.5)*ofGetWidth()*0.9,(cos(t)*0.5+0.5)*ofGetHeight());
    ofSetColor(255);

#ifndef OFAPP_KARPLUS_MIDI
    // draw some keyboard keys and infos
    keyboard.draw(14, 330, 200, 180);
#endif

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
#ifndef OFAPP_KARPLUS_MIDI
    // sends key messages to ofxPDSPComputerKeyboard
    keyboard.keyPressed( key );
#endif
    // maps stuff
    if (key == 'f' || key == 'F')
    {
        ofToggleFullscreen();
    }
    else if (key == '-')
    {
        tileLayer->setCenter(tileLayer->getCenter().getZoomedBy(-1));
    }
    else if (key == '=')
    {
        tileLayer->setCenter(tileLayer->getCenter().getZoomedBy(1));
    }
    else if (key == 'w')
    {
        tileLayer->setCenter(tileLayer->getCenter().getNeighborUp());
    }
    else if (key == 'a')
    {
        tileLayer->setCenter(tileLayer->getCenter().getNeighborLeft());
    }
    else if (key == 's')
    {
        tileLayer->setCenter(tileLayer->getCenter().getNeighborDown());
    }
    else if (key == 'd')
    {
        tileLayer->setCenter(tileLayer->getCenter().getNeighborRight());
    }
    else if (key == '1')
    {
        animation -= 0.01;;
    }
    else if (key == '2')
    {
        animation += 0.01;;
    }
    else if (key == '3')
    {
        animation = 0;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
#ifndef OFAPP_KARPLUS_MIDI
    // sends key messages to ofxPDSPComputerKeyboard
    keyboard.keyReleased( key );
#endif
    
    
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
