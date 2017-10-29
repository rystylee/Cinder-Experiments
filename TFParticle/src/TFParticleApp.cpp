#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"
#include "cinder/params/Params.h"
#include "TFParticle.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class TFParticleApp : public App {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void setupCamera();
    void setupParams();
    
    CameraPersp mCam;
    CameraUi mCamUi;
    
    params::InterfaceGlRef mParams;
    float mFrameRate;
    float mAccelStep;
    ColorA mStartColor;
    ColorA mEndColor;
    
    TFParticle mParticle;
};

void TFParticleApp::setup()
{
    setupCamera();
    setupParams();
    
    mParticle.setup();
}

void TFParticleApp::update()
{
    mFrameRate = getAverageFps();
    
    mParticle.setAccelStep(mAccelStep);
    mParticle.setStartColor(mStartColor);
    mParticle.setEndColor(mEndColor);
    mParticle.update();
}

void TFParticleApp::draw()
{
    gl::clear();
    gl::disableAlphaBlending();
    gl::enableAdditiveBlending();
    gl::setMatrices(mCam);
    gl::viewport(getWindowSize());
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    mParticle.draw();
    
    mParams->draw();
}

void TFParticleApp::setupCamera()
{
    const vec2 windowSize = toPixels(getWindowSize());
    mCam = CameraPersp(windowSize.x, windowSize.y, 60.0f, 0.01f, 3000.0f);
    mCam.setAspectRatio(getWindowAspectRatio());
    mCam.lookAt(vec3(0, 0, 600.0f) ,vec3(0));
    mCamUi = CameraUi(&mCam, getWindow(), -1);
}

void TFParticleApp::setupParams()
{
    mAccelStep = 0.8;
    mStartColor = ColorA(1.0, 1.0, 0.8, 0.5);
    mEndColor = ColorA(0.6, 1.0, 1.0, 0.5);
    
    mParams = params::InterfaceGl::create("Params", ivec2(200,200));
    mParams->addParam("fps", &mFrameRate, "", true);
    mParams->addParam("uStartColor", &mStartColor, "", false);
    mParams->addParam("uEndColor", &mEndColor, "", false);
    mParams->addParam("uAccelStep", &mAccelStep).step(0.01f).min(0.001f).max(1.5f);
}

auto options = RendererGl::Options().version(4, 1);

CINDER_APP( TFParticleApp, RendererGl(options), [](App::Settings* settings ){
    settings->setWindowSize(1024, 768);
});

