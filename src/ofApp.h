#pragma once

#include "ofMain.h"
#include <string>

#define PALETTE_COUNT 32

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();

    enum Bindings {source, dest, render, resizeCheck, offsets};

    bool surface1IsSource;
    ofTexture surface1, surface2, renderSurface;
    ofShader sandToppler, expansionChecker, renderer;
    ofBufferObject expansionBuffer;
    ofBufferObject offsetBuffer;
};
