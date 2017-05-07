#include "ofMain.h"
#include "ofApp.h"

int main( ){
    ofGLWindowSettings settings;
    settings.setGLVersion(4, 4);
    ofSetupOpenGL(768, 768, OF_WINDOW);

    /**
    * workaround for making the window float when using i3wm:
    * put this in your .i3/config:
    * for_window [title="^!float$"] floating enable
    */
    ofSetWindowTitle("!float"); // TODO find a way to make window float

    ofRunApp(new ofApp());
}
