#include "ofApp.h"
#include "ofConstants.h"

//TODO panning/zooming

void ofApp::setup(){
    ofSetBackgroundAuto(false);
    ofSetBackgroundColor(0, 0, 0);

    surface1IsSource = true;
    const size_t initSize = 128;
    ofSetWindowShape(512, 512);
    ofSetVerticalSync(false);

    // Load and compile shaders
    // shaders are stored in the sandpile/bin/data/ directory
    sandToppler.setupShaderFromFile(GL_COMPUTE_SHADER, "sandToppler.glsl");
    sandToppler.linkProgram();
    expansionChecker.setupShaderFromFile(GL_COMPUTE_SHADER, "expansionChecker.glsl");
    expansionChecker.linkProgram();
    renderer.setupShaderFromFile(GL_COMPUTE_SHADER, "renderer.glsl");
    renderer.linkProgram();

    // allocate and bind rendersurface
    renderSurface.allocate(initSize, initSize, GL_RGBA8);
    renderSurface.bindAsImage(Bindings::render, GL_WRITE_ONLY);
    renderSurface.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

    // set the initial state of the sandpile
    ofPixels data;
    data.allocate(initSize, initSize, OF_IMAGE_COLOR_ALPHA);
    data.setColor(ofColor(0, 0, 0, 0)); // fill data with 0
    surface2.loadData(data);
    data.setColor(initSize/2, initSize/2, ofColor(0, 0, 1, 0)); // set a point in the center to 65536 (1 << 16)
    surface1.loadData(data);

    // allocate two 32-bit uint to store the offset for resizing
    offsetBuffer.allocate(sizeof(uint32_t)*2, NULL, GL_DYNAMIC_DRAW);
    offsetBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, Bindings::offsets);

    // allocate a 32-bit uint as an expansion needed flag
    uint32_t expandNeeded = 1;
    expansionBuffer.allocate(sizeof(expandNeeded), &expandNeeded, GL_DYNAMIC_DRAW);
    expansionBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, Bindings::resizeCheck);
}

void ofApp::update(){
    uint32_t currentSeconds = static_cast<uint32_t>(ofGetSeconds());
    static uint32_t lastSeconds = currentSeconds;
    if (currentSeconds != lastSeconds) {
        lastSeconds = currentSeconds;
        ofSetWindowTitle("ips: " + to_string(ofGetFrameRate()));
    }
}

void ofApp::draw(){
    auto &sourceBuffer = (surface1IsSource ? surface1 : surface2);
    auto &destBuffer = (surface1IsSource ? surface2 : surface1);
    surface1IsSource = !surface1IsSource;
    sourceBuffer.bindAsImage(Bindings::source, GL_READ_ONLY);
    destBuffer.bindAsImage(Bindings::dest, GL_READ_WRITE);
    unsigned int currentSize = static_cast<unsigned int>(destBuffer.getWidth());

    // check if we need to resize the texture
    expansionChecker.begin();
    expansionChecker.dispatchCompute(currentSize/32+1, 1, 1);
    expansionChecker.end();
    uint32_t expandNeeded = 0;
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(expandNeeded), &expandNeeded);
    if (expandNeeded > 0) {
        // double the size of the buffers and the render surface
        currentSize = currentSize * 2;
        ofPixels data;
        data.allocate(static_cast<size_t>(currentSize), static_cast<size_t>(currentSize), OF_IMAGE_COLOR_ALPHA);
        data.setColor(ofColor(0, 0, 0, 0));
        destBuffer.loadData(data);
        destBuffer.bindAsImage(Bindings::dest, GL_READ_WRITE);

        if (static_cast<unsigned int>(renderSurface.getWidth()) < currentSize) {
            renderSurface.allocate(static_cast<int>(currentSize), static_cast<int>(currentSize), GL_RGBA8);
            renderSurface.bindAsImage(Bindings::render, GL_WRITE_ONLY);
        }
    }

    sandToppler.begin();
    sandToppler.dispatchCompute(currentSize/32+1, currentSize/32+1, 1);
    sandToppler.end();

    // draw every 16.666 ms instead of every iteration to make it faster
    if (ofGetElapsedTimeMicros() >= 16666) {
        ofResetElapsedTimeCounter();
        renderer.begin();
        renderer.dispatchCompute(currentSize/32+1, currentSize/32+1, 1);
        renderer.end();
        renderSurface.draw(0, 0, ofGetWidth(), ofGetHeight());
    }
}
