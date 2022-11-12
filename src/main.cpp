#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
//    ofSetupOpenGL(640, 480, OF_WINDOW);            // <-------- setup the GL context
//
//    // this kicks off the running of my app
//    // can be OF_WINDOW or OF_FULLSCREEN
//    // pass in width and height too:
//    ofRunApp(new ofApp());

    ofGLWindowSettings settings;
    settings.setSize(1280, 768);
    settings.setGLVersion(3, 2);
    settings.windowMode = OF_FULLSCREEN;
    auto window = ofCreateWindow(settings);
    auto app = std::make_shared<ofApp>();
    return ofRunApp(app);
}
