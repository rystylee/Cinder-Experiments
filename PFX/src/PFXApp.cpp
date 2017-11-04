#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/params/Params.h"
#include "BaseScene.h"
#include "PFX.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PFXApp : public App {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void setupParams();

    BaseScene mBaseScene;
    PFX mPfx;
    bool mIsPfx = false;

    params::InterfaceGlRef mParams;
    float mFrameRate;
    int mMode = 0;
};

void PFXApp::setup()
{
    setupParams();

    mBaseScene.setup();
    mPfx.setup();
}

void PFXApp::update()
{
    mFrameRate = getAverageFps();

    mBaseScene.update();
}

void PFXApp::draw()
{
    gl::clear();

    if (mIsPfx) {
        mPfx.selectEffect(mMode);
        gl::FboRef& fbo = mBaseScene.getTexture();
        mPfx.render(fbo);
    }
    else {
        mBaseScene.draw();
    }

    mParams->draw();
}

void PFXApp::setupParams()
{
    mParams = params::InterfaceGl::create("Params", ivec2(200, 100));
    mParams->addParam("fps", &mFrameRate, "", true);
    mParams->addParam("use PFX", &mIsPfx, "", false);
    mParams->addParam("Effect Mode", &mMode, "", false);
}

auto options = RendererGl::Options().version(4, 1);

CINDER_APP(PFXApp, RendererGl(options), [](App::Settings* settings) {
    settings->setWindowSize(1024, 768);
})

