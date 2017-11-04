#pragma once

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;

class BaseScene {
public:
    void setup()
    {
        mFbo = gl::Fbo::create(mWidth, mHeight);
        mShader = gl::GlslProg::create(app::loadAsset("shaders/baseScene/baseScene.vert"), app::loadAsset("shaders/baseScene/baseScene.frag"));

        gl::enableDepthRead();
        gl::enableDepthWrite();
    }

    void update()
    {
        CameraPersp cam(mFbo->getWidth(), mFbo->getHeight(), 60.0f);
        cam.setPerspective(60.0f, mFbo->getAspectRatio(), 0.1f, 2000.0f);
        cam.lookAt(vec3(0, 0, 600.0f), vec3(0));

        gl::ScopedFramebuffer fbo(mFbo);
        gl::clear(Color(0.3f, 0.3f, 0.3f));

        gl::ScopedViewport viewPort(ivec2(0), mFbo->getSize());

        gl::ScopedMatrices mat;
        gl::setMatrices(cam);

        mRotation *= rotate(0.06f, normalize(vec3(0.166f, 0.333f, 0.666f)));
        gl::multModelMatrix(mRotation);

        gl::ScopedGlslProg shader(mShader);
        gl::drawCube(vec3(0), vec3(150.0f));
    }

    void draw()
    {
        gl::clear(Color::gray(0.35f));
        gl::setMatricesWindow(mFbo->getSize());

        gl::draw(mFbo->getColorTexture(), mFbo->getBounds());
    }

    gl::FboRef& getTexture() { return mFbo; }

private:
    const int mWidth = app::getWindowWidth(), mHeight = app::getWindowHeight();
    gl::FboRef mFbo;
    gl::GlslProgRef mShader;
    mat4 mRotation = mat4(1);
};


