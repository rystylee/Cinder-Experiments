#version 440 core
#extension GL_ARB_shader_storage_buffer_object : require

#include "util.glsl"

uniform mat4 ciModelViewProjection;

uniform vec4 uStartColor;
uniform vec4 uEndColor;

layout (location = 0) in int particleID;

out vec4 vColor;

struct Particle {
    vec4  color;
    vec3  position;
    vec3  velocity;
    float age;
    float lifespan;
};

layout (std140, binding = 0) buffer Part
{
    Particle particles[];
};

void main(void)
{
    float elapsedTime = clamp(float(particles[particleID].age) / float(max(particles[particleID].lifespan, 1.0)), 0.0, 1.0);
    vec3 c = map(vec3(elapsedTime), vec3(0.0), vec3(1.0), uStartColor.rgb, uEndColor.rgb);
    vColor = vec4(c, 0.5);
    
    gl_Position = ciModelViewProjection * vec4(particles[particleID].position, 1.0);
    gl_PointSize = 1.0;
}
