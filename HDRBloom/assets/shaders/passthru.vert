#version 410 core

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;
in vec2 ciTexCoord0;

out vec3 vNormal;
out vec2 vTexCoord0;

void main()
{
    vTexCoord0  = ciTexCoord0;
    vNormal     = ciNormalMatrix * ciNormal;
    gl_Position = ciModelViewProjection * ciPosition;
}
