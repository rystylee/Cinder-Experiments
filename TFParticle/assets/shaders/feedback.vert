#version 410 core

#include "util.glsl"

uniform float uAccelStep;
uniform vec3 uPrevEmitterPos;
uniform vec3 uEmitterPos;

layout (location = 0) in vec4  iColor;
layout (location = 1) in vec3  iPosition;
layout (location = 2) in vec3  iVelocity;
layout (location = 3) in float iAge;
layout (location = 4) in float iLifespan;

out vec4  oColor;
out vec3  oPosition;
out vec3  oVelocity;
out float oAge;
out float oLifespan;

void main(void)
{
    vec4 color     = iColor;
    vec3 pos       = iPosition;
    vec3 vel       = iVelocity;
    float age      = iAge;
    float lifespan = iLifespan;
    
    age += 0.3;
    
    if(age >= lifespan){
        age = 0.0;
        lifespan = 50.0 + 250.0 * abs(random(pos.xy));
        float theta = 2.0 * PI * random(pos.yy);
        float phi = PI * random(pos.zz);
        float r = 5.0 * random(pos.xy);
        vec3 startPos = uPrevEmitterPos + (uEmitterPos - uPrevEmitterPos) * random(pos.yz);
        pos = startPos + vec3(r * sin(theta) * cos(phi), r * sin(theta) * sin(phi), r * cos(theta));
        vel.xyz = vec3(0.0);
    }
    
    vec3 accel = normalize(uEmitterPos - pos);
    accel *= uAccelStep;
    vel += accel;
    pos += vel;
    
    
    oColor    = color;
    oPosition = pos;
    oVelocity = vel;
    oAge      = age;
    oLifespan = lifespan;
}
