#pragma once

#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"

using namespace ci;

struct Particle{
    ColorA color;
    vec3   position;
    vec3   velocity;
    float  age;
    float  lifespan;
};

class TFInstancing{
public:
    void setup()
    {
        std::vector<Particle> particles;
        particles.assign(NUM_PARTICLES, Particle());
        for(int i=0; i<particles.size(); ++i){
            auto& p = particles.at(i);
            p.color    = ColorA(1.0f, 1.0f, 1.0f, 1.0f);
            p.position = vec3(Rand::randVec3());
            p.velocity = Rand::randVec3();
            p.age = randFloat(10.0f, 100.0f);
            p.lifespan = randFloat(1.0f, 150.0f);
        }
        mParticleBuffer[mSourceIndex] = gl::Vbo::create(GL_ARRAY_BUFFER, particles.size()*sizeof(Particle), particles.data(), GL_STATIC_DRAW);
        mParticleBuffer[mDestinationIndex] = gl::Vbo::create(GL_ARRAY_BUFFER, particles.size()*sizeof(Particle), nullptr, GL_STATIC_DRAW);
        
        for(int i=0; i<2; ++i){
            mAttributes[i] = gl::Vao::create();
            gl::ScopedVao vao(mAttributes[i]);
            
            gl::ScopedBuffer buffer(mParticleBuffer[i]);
            gl::enableVertexAttribArray(0);
            gl::enableVertexAttribArray(1);
            gl::enableVertexAttribArray(2);
            gl::enableVertexAttribArray(3);
            gl::enableVertexAttribArray(4);
            gl::vertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, color));
            gl::vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, position));
            gl::vertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, velocity));
            gl::vertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, age));
            gl::vertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, lifespan));
        }
        
        const DataSourceRef renderVert = app::loadAsset("shaders/render.vert");
        const DataSourceRef renderFrag = app::loadAsset("shaders/render.frag");
        const DataSourceRef TFVert = app::loadAsset("shaders/feedback.vert");
        
        gl::GlslProg::Format renderFormat = gl::GlslProg::Format().vertex(renderVert).fragment(renderFrag)
        .attribLocation("iPosition", 1)
        .attribLocation("iAge", 3)
        .attribLocation("iLifespan", 4);
        gl::GlslProg::Format updateFormat = gl::GlslProg::Format().vertex(TFVert)
        .feedbackFormat(GL_INTERLEAVED_ATTRIBS)
        .feedbackVaryings({"oColor", "oPosition", "oVelocity", "oAge", "oLifespan"})
        .attribLocation("iColor", 0)
        .attribLocation("iPosition", 1)
        .attribLocation("iVelocity", 2)
        .attribLocation("iAge", 3)
        .attribLocation("iLifespan", 4);
        
        mRenderProg = loadGlslProg(renderFormat);
        mUpdateProg = loadGlslProg(updateFormat);
        
        gl::VboMeshRef boxMesh = gl::VboMesh::create(geom::Cube().size(vec3(2.0f, 2.0f, 8.0f)).subdivisions(1));
        geom::BufferLayout layout;
        layout.append(geom::CUSTOM_1, 3, sizeof(Particle), offsetof(Particle, position), 1);
        layout.append(geom::CUSTOM_3, 1, sizeof(Particle), offsetof(Particle, age), 1);
        layout.append(geom::CUSTOM_4, 1, sizeof(Particle), offsetof(Particle, lifespan), 1);
        boxMesh->appendVbo(layout, mParticleBuffer[mSourceIndex]);
        
        gl::Batch::AttributeMapping map =
        {{geom::Attrib::CUSTOM_1, "iPosition"},
        {geom::Attrib::CUSTOM_3, "iAge"},
        {geom::Attrib::CUSTOM_4, "iLifespan"}};
        mBatch = gl::Batch::create(boxMesh, mRenderProg, map);
    }
    
    void update()
    {
        float time = app::getElapsedSeconds();
        mPrevEmitterPos = mEmitterPos;
        mEmitterPos = 300.0f * vec3(Perlin().noise(time*0.5, 0, 0), Perlin().noise(0, time*0.5, 0), Perlin().noise(0, 0, time*0.5));
        
        gl::ScopedGlslProg updater(mUpdateProg);
        gl::ScopedState rasterizer(GL_RASTERIZER_DISCARD, true);
        mUpdateProg->uniform("uAccelStep", mAccelStep);
        mUpdateProg->uniform("uPrevEmitterPos", mPrevEmitterPos);
        mUpdateProg->uniform("uEmitterPos", mEmitterPos);
        
        gl::ScopedVao source(mAttributes[mSourceIndex]);
        gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffer[mDestinationIndex]);
        gl::beginTransformFeedback(GL_POINTS);
        gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);
        gl::endTransformFeedback();
        
        std::swap(mSourceIndex, mDestinationIndex);
    }
    
    void draw()
    {
        gl::ScopedGlslProg render(mRenderProg);
        mRenderProg->uniform("uStartColor", mStartColor);
        mRenderProg->uniform("uEndColor", mEndColor);
        gl::ScopedVao vao(mAttributes[mSourceIndex]);
        gl::context()->setDefaultShaderVars();
        mBatch->drawInstanced(NUM_PARTICLES);
    }
    
    gl::GlslProgRef loadGlslProg(const gl::GlslProg::Format& format)
    {
        gl::GlslProgRef glslProg;
        try{
            glslProg = gl::GlslProg::create(format);
        }
        catch(const gl::GlslProgExc& e){
            app::console() << e.what() << std::endl;
        }
        return glslProg;
    }
    
    void setAccelStep(float accelStep){ mAccelStep = accelStep; }
    void setStartColor(ColorA startColor){ mStartColor = startColor; }
    void setEndColor(ColorA endColor){ mEndColor = endColor; }
    
private:
    static const int NUM_PARTICLES = 500000;
    gl::GlslProgRef mRenderProg;
    gl::GlslProgRef mUpdateProg;
    
    gl::VaoRef mAttributes[2];
    gl::VboRef mParticleBuffer[2];
    
    std::uint32_t mSourceIndex = 0;
    std::uint32_t mDestinationIndex = 1;
    
    gl::BatchRef mBatch;
    
    float mAccelStep;
    vec3 mPrevEmitterPos;
    vec3 mEmitterPos;
    ColorA mStartColor;
    ColorA mEndColor;
};
