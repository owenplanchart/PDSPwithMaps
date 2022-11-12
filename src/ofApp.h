#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxPDSP.h"
#include "ofxGui.h"
#include "synth/KarplusStrong.h"
#include "effect/Chorus.h"
#include "ofxMaps.h"

#define OFAPP_KARPLUS_MIDI

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
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        
        ofxPDSPEngine   engine;
        
        #ifdef OFAPP_KARPLUS_MIDI
        ofxPDSPMidiIn        midiIn;
        ofxPDSPMidiKeys      midiKeys;
        #else 
        ofxPDSPComputerKeyboard keyboard;
        #endif

        ofxPanel             gui;
        
        np::synth::KarplusStrong ksynth;
        np::effect::Chorus chorus;
    
    // Maps Stuff
    std::shared_ptr<ofxMaps::MBTilesCache> bufferCache;
    std::shared_ptr<ofxMaps::MapTileLayer> tileLayer;
    std::shared_ptr<ofxMaps::MapTileSet> tileSet;
    std::shared_ptr<ofxMaps::MapTileProvider> tileProvider;
    
    std::vector<ofxGeo::Coordinate> coordinates;
    
    std::vector<std::string> sets;
    int setsIndex = 0;
    
    float animation = 0;
    
    ofEasyCam cam;
};
