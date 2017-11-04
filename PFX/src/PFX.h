#pragma once

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

class PFX {
public:
    void setup()
    {
    }

    void render(const gl::FboRef& baseFbo)
    {
        mBaseFbo = baseFbo;

        gl::clear();
        gl::setMatricesWindow(mBaseFbo->getSize());
        const gl::ScopedTextureBind tex0(mBaseFbo->getColorTexture(), 0);

        gl::ScopedModelMatrix scopedModelMatrix;
        gl::translate(app::getWindowCenter());
        gl::scale(app::getWindowSize());
        mRectBatch->getGlslProg()->uniform("uBaseFBO", 0);
        mRectBatch->draw();
    }

    void selectEffect(int mode)
    {
        switch (mode) {
        case 0:
            mRectBatch = loadBatch("shaders/pfx/RGBShift.frag");
            mRectBatch->getGlslProg()->uniform("uTime", float(app::getElapsedSeconds()));
            break;
        case 1:
            mRectBatch = loadBatch("shaders/pfx/vignette.frag");
            break;
        case 2:
            mRectBatch = loadBatch("shaders/pfx/symmetry.frag");
            mRectBatch->getGlslProg()->uniform("uSymMode", 0);
            break;
        default:
            break;
        }
    }

    gl::BatchRef& loadBatch(const fs::path &relativePath)
    {
        const gl::GlslProgRef glsl = gl::GlslProg::create(app::loadAsset("shaders/passthru.vert"), app::loadAsset(relativePath));
        const gl::VboMeshRef rect = gl::VboMesh::create(geom::Rect());
        mRectBatch = gl::Batch::create(rect, glsl);

        return mRectBatch;
    }

private:
    gl::FboRef mBaseFbo;
    gl::BatchRef mRectBatch;
};
