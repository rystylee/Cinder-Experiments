#pragma once

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"
#include "cinder/gl/Ssbo.h"

using namespace ci;

#pragma pack(push, 1)
struct Particle {
    vec4  color;
    vec3  position;
    vec3  velocity;
    float age;
    float lifespan;
};
#pragma pack(pop)

class CSParticle {
public:
    void setup()
    {
        std::vector<Particle> particles;
        particles.assign(NUM_PARTICLES, Particle());
        for (int i = 0; i<particles.size(); ++i) {
            auto& p = particles.at(i);
            p.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
            p.position = vec3(Rand::randVec3());
            p.velocity = Rand::randVec3();
            p.age = randFloat(10.0f, 100.0f);
            p.lifespan = randFloat(1.0f, 150.0f);
        }

        ivec3 count = gl::getMaxComputeWorkGroupCount();
        CI_ASSERT(count.x >= (NUM_PARTICLES / WORK_GROUP_SIZE));

        mParticleSsbo = gl::Ssbo::create(particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW);
        gl::ScopedBuffer ssbo(mParticleSsbo);
        mParticleSsbo->bindBase(0);

        const DataSourceRef renderVert = app::loadAsset("shaders/render.vert");
        const DataSourceRef renderFrag = app::loadAsset("shaders/render.frag");
        gl::GlslProg::Format renderFormat = gl::GlslProg::Format().vertex(renderVert).fragment(renderFrag)
            .attribLocation("particleID", 0);
        mRenderProg = loadGlslProg(renderFormat);

        std::vector<GLuint> ids(NUM_PARTICLES);
        GLuint currID = 0;
        std::generate(ids.begin(), ids.end(), [&currID]() -> GLuint { return currID++; });

        mIdsVbo = gl::Vbo::create(GL_ARRAY_BUFFER, ids, GL_STATIC_DRAW);
        mAttributes = gl::Vao::create();
        gl::ScopedVao vao(mAttributes);
        gl::ScopedBuffer scopedIds(mIdsVbo);
        gl::enableVertexAttribArray(0);
        gl::vertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(GLuint), 0);

        const DataSourceRef updateCompute = app::loadAsset("shaders/update.comp");
        gl::GlslProg::Format updateFormat = gl::GlslProg::Format().compute(updateCompute);
        mUpdateProg = loadGlslProg(updateFormat);
    }

    void update()
    {
        float time = app::getElapsedSeconds();
        mPrevEmitterPos = mEmitterPos;
        mEmitterPos = 300.0f * vec3(Perlin().noise(time*0.5, 0, 0), Perlin().noise(0, time*0.5, 0), Perlin().noise(0, 0, time*0.5));

        gl::ScopedGlslProg updater(mUpdateProg);
        mUpdateProg->uniform("uAccelStep", mAccelStep);
        mUpdateProg->uniform("uPrevEmitterPos", mPrevEmitterPos);
        mUpdateProg->uniform("uEmitterPos", mEmitterPos);

        gl::ScopedBuffer scopedParicleSsbo(mParticleSsbo);
        gl::dispatchCompute(NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1);
        gl::memoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void draw()
    {
        gl::ScopedGlslProg render(mRenderProg);
        mRenderProg->uniform("uStartColor", mStartColor);
        mRenderProg->uniform("uEndColor", mEndColor);

        gl::ScopedBuffer scopedParicleSsbo(mParticleSsbo);
        gl::ScopedVao vao(mAttributes);
        gl::context()->setDefaultShaderVars();
        gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);

    }

    gl::GlslProgRef loadGlslProg(const gl::GlslProg::Format& format)
    {
        gl::GlslProgRef glslProg;
        try {
            glslProg = gl::GlslProg::create(format);
        }
        catch (const gl::GlslProgCompileExc& e) {
            app::console() << e.what() << std::endl;
        }
        return glslProg;
    }

    void setAccelStep(float accelStep) { mAccelStep = accelStep; }
    void setStartColor(ColorA startColor) { mStartColor = static_cast<vec4>(startColor); }
    void setEndColor(ColorA endColor) { mEndColor = static_cast<vec4>(endColor); }

private:
    enum { WORK_GROUP_SIZE = 128 };
    static const int NUM_PARTICLES = 1000000;
    gl::GlslProgRef mRenderProg;
    gl::GlslProgRef mUpdateProg;

    gl::SsboRef mParticleSsbo;
    gl::VboRef mIdsVbo;
    gl::VaoRef mAttributes;

    float mAccelStep;
    vec3 mPrevEmitterPos;
    vec3 mEmitterPos;
    vec4 mStartColor;
    vec4 mEndColor;
};