#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/params/Params.h"
#include "BaseScene.h"
#include "HDRBloom.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class HDRBloomApp : public App {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void setupParams();

    BaseScene mBaseScene;
    HDRBloom mBloom;

    params::InterfaceGlRef mParams;
    float mFrameRate;
    bool mIsBloom = false;
    float mAttenuation = 2.5f;
    float mOffsetScale = 1.0f;
};

void HDRBloomApp::setup()
{
    setupParams();

    mBaseScene.setup();
    mBloom.setup();
}

void HDRBloomApp::update()
{
    mFrameRate = getAverageFps();

    mBaseScene.update();
}

void HDRBloomApp::draw()
{
    gl::clear();

    if (mIsBloom) {
        gl::FboRef& fbo = mBaseScene.getFbo();
        mBloom.render(fbo);
        mBloom.setOffsetScale(mOffsetScale);
        mBloom.setAttenuation(mAttenuation);
    }
    else {
        mBaseScene.draw();
    }

    mParams->draw();
}

void HDRBloomApp::setupParams()
{
    mParams = params::InterfaceGl::create("Params", ivec2(200, 100));
    mParams->addParam("fps", &mFrameRate, "", true);
    mParams->addParam("Bloom", &mIsBloom, "", false);
    mParams->addParam("Blur Offset Scale", &mOffsetScale).min(0.0f).max(10.0f).step(1.0f);
    mParams->addParam("Attenuation", &mAttenuation).min(0.0f).max(3.0f).step(0.1f);
}

auto options = RendererGl::Options().version(4, 1).msaa(8);

CINDER_APP(HDRBloomApp, RendererGl(options), [](App::Settings* settings) {
    settings->setWindowSize(1024, 768);
})
