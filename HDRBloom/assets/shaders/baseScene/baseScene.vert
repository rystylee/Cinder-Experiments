#version 410 core

uniform mat4  ciModelViewProjection;
uniform mat3  ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;
in vec2 ciTexCoord0;

out vec2 vTexCoord0;
out vec3 vNormal;

void main(void)
{
    vTexCoord0   = ciTexCoord0;
    vNormal      = ciNormalMatrix * ciNormal;
    gl_Position  = ciModelViewProjection * ciPosition;
}
