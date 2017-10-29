#version 410 core

#include "util.glsl"

uniform mat4 ciModelViewProjection;

uniform vec4 uStartColor;
uniform vec4 uEndColor;

in vec4 ciColor;
in vec4 ciPosition;

layout (location = 1) in vec3  iPosition;
layout (location = 3) in float iAge;
layout (location = 4) in float iLifespan;

out vec4 vColor;

void main(void)
{
    float elapsedTime = clamp(float(iAge) / float(max(iLifespan, 1.0)), 0.0, 1.0);
    vec3 c = map(vec3(elapsedTime), vec3(0.0), vec3(1.0), uStartColor.rgb, uEndColor.rgb);
    vColor = vec4(c, 0.5);
    
    gl_Position = ciModelViewProjection * vec4(iPosition, 1.0);
    gl_PointSize = 1.0;
}
