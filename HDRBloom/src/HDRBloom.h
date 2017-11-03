#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;

class HDRBloom {
public:
    void setup()
    {
        setupFBOs();
        setupShaders();
    }

    void render(gl::FboRef& baseFbo)
    {
        mBaseFbo = baseFbo;

        // Brightness Threshold Pass
        {
            const gl::ScopedFramebuffer fbo(mBrightnessThreshFbo);
            gl::clear();
            gl::setMatricesWindow(app::getWindowSize());

            gl::ScopedModelMatrix scopedModelMatrix;
            gl::translate(app::getWindowCenter());
            gl::scale(app::getWindowSize());

            const gl::ScopedTextureBind tex0(mBaseFbo->getColorTexture(), 0);
            mRectBatchBrightnessThresh->getGlslProg()->uniform("uBrightnessThresh", 0.6f);
            mRectBatchBrightnessThresh->draw();
        }

        // Horizontal Blur Pass
        {
            const gl::ScopedFramebuffer fbo(mBlurFbo[0]);
            gl::clear();
            gl::setMatricesWindow(app::getWindowSize());

            gl::ScopedModelMatrix scopedModelMatrix;
            gl::translate(app::getWindowCenter());
            gl::scale(app::getWindowSize());

            const gl::ScopedTextureBind tex0(mBrightnessThreshFbo->getColorTexture(), 0);
            mRectBatchBlur[0]->getGlslProg()->uniform("uDirection", 0);
            mRectBatchBlur[0]->getGlslProg()->uniform("uResolution", vec2(app::getWindowSize()));
            mRectBatchBlur[0]->getGlslProg()->uniform("uOffsetScale", mOffsetScale);
            mRectBatchBlur[0]->getGlslProg()->uniform("uAttenuation", mAttenuation);
            mRectBatchBlur[0]->draw();
        }

        // Vertical Blur Pass
        {
            const gl::ScopedFramebuffer fbo(mBlurFbo[1]);
            gl::clear();
            gl::setMatricesWindow(app::getWindowSize());

            gl::ScopedModelMatrix scopedModelMatrix;
            gl::translate(app::getWindowCenter());
            gl::scale(app::getWindowSize());

            const gl::ScopedTextureBind tex0(mBlurFbo[0]->getColorTexture(), 0);
            mRectBatchBlur[1]->getGlslProg()->uniform("uDirection", 1);
            mRectBatchBlur[1]->getGlslProg()->uniform("uResolution", vec2(app::getWindowSize()));
            mRectBatchBlur[1]->getGlslProg()->uniform("uOffsetScale", mOffsetScale);
            mRectBatchBlur[1]->getGlslProg()->uniform("uAttenuation", mAttenuation);
            mRectBatchBlur[1]->draw();
        }

        // Composite Pass
        {
            const gl::ScopedFramebuffer fbo(mCompositeFbo);
            gl::clear();
            gl::setMatricesWindow(app::getWindowSize());

            gl::ScopedModelMatrix scopedModelMatrix;
            gl::translate(app::getWindowCenter());
            gl::scale(app::getWindowSize());

            const gl::ScopedTextureBind tex0(mBaseFbo->getColorTexture(), 0);
            const gl::ScopedTextureBind tex1(mBlurFbo[1]->getColorTexture(), 1);
            mRectBatchComposite->getGlslProg()->uniform("uGamma", 2.2f);
            mRectBatchComposite->draw();
        }

        gl::ScopedBlendAdditive add;
        gl::draw(mCompositeFbo->getColorTexture());
        //gl::draw(mBlurFbo[1]->getColorTexture());
    }

    void setupFBOs()
    {
        int w = app::getWindowWidth();
        int h = app::getWindowHeight();

        // Brightness Threshold FBO
        try {
            gl::Texture2d::Format colorTextureFormat;
            colorTextureFormat.internalFormat(GL_RGBA16F);
            mBrightnessThreshFbo = gl::Fbo::create(w, h, gl::Fbo::Format().colorTexture(colorTextureFormat));
            const gl::ScopedFramebuffer fbo(mBrightnessThreshFbo);
            const gl::ScopedViewport viewport(ivec2(0), mBrightnessThreshFbo->getSize());
            gl::clear();
        }
        catch (const std::exception& e) {
            app::console() << "mFboLBuffer failed: " << e.what() << std::endl;
        }

        // Blur FBO
        for (int i = 0; i<2; i++) {
            try {
                gl::Texture2d::Format colorTextureFormat;
                colorTextureFormat.internalFormat(GL_RGBA16F);
                mBlurFbo[i] = gl::Fbo::create(w, h, gl::Fbo::Format().colorTexture(colorTextureFormat));
                const gl::ScopedFramebuffer fbo(mBlurFbo[i]);
                const gl::ScopedViewport viewport(ivec2(0), mBlurFbo[i]->getSize());
                gl::clear();
            }
            catch (const std::exception& e) {
                app::console() << "mFboLBuffer failed: " << e.what() << std::endl;
            }
        }

        // Composite FBO
        try {
            gl::Texture2d::Format colorTextureFormat;
            colorTextureFormat.internalFormat(GL_RGBA16F);
            mCompositeFbo = gl::Fbo::create(w, h, gl::Fbo::Format().colorTexture(colorTextureFormat).samples(8));
            const gl::ScopedFramebuffer fbo(mCompositeFbo);
            const gl::ScopedViewport viewport(ivec2(0), mCompositeFbo->getSize());
            gl::clear();
        }
        catch (const std::exception& e) {
            app::console() << "mFboLBuffer failed: " << e.what() << std::endl;
        }
    }

    void setupShaders()
    {
        // Load shader files from disk
        const DataSourceRef vertPassthru = app::loadAsset("shaders/passthru.vert");
        const DataSourceRef fragBrightnessThresh = app::loadAsset("shaders/pfx/brightnessThresh.frag");
        const DataSourceRef fragBlur = app::loadAsset("shaders/pfx/blur.frag");
        const DataSourceRef fragHDRBloom = app::loadAsset("shaders/pfx/hdrBloom.frag");

        const auto mBrightnessThreshShader = gl::GlslProg::create(vertPassthru, fragBrightnessThresh);
        const auto mBlurShader0 = gl::GlslProg::create(vertPassthru, fragBlur);
        const auto mBlurShader1 = gl::GlslProg::create(vertPassthru, fragBlur);
        const auto mHDRBloomShader = gl::GlslProg::create(vertPassthru, fragHDRBloom);

        const gl::VboMeshRef rect = gl::VboMesh::create(geom::Rect());

        mRectBatchBrightnessThresh = gl::Batch::create(rect, mBrightnessThreshShader);
        mRectBatchBlur[0] = gl::Batch::create(rect, mBlurShader0);
        mRectBatchBlur[1] = gl::Batch::create(rect, mBlurShader1);
        mRectBatchComposite = gl::Batch::create(rect, mHDRBloomShader);

        mRectBatchBrightnessThresh->getGlslProg()->uniform("uTex0", 0);
        mRectBatchBlur[0]->getGlslProg()->uniform("uTex0", 0);
        mRectBatchBlur[1]->getGlslProg()->uniform("uTex0", 0);
        mRectBatchComposite->getGlslProg()->uniform("uTex0", 0);
        mRectBatchComposite->getGlslProg()->uniform("uBlurTex", 1);
    }

    void setOffsetScale(float offsetScale) { mOffsetScale = offsetScale; }
    void setAttenuation(float attenuation) { mAttenuation = attenuation; }

private:
    gl::FboRef mBaseFbo, mBrightnessThreshFbo, mBlurFbo[2], mCompositeFbo;
    gl::BatchRef mRectBatchBrightnessThresh, mRectBatchBlur[2], mRectBatchComposite;
    float mOffsetScale, mAttenuation;
};

