#version 410 core

uniform sampler2D uTex0;
uniform float uBrightnessThresh;

in vec2 vTexCoord0;

out vec4 fragColor;

void main(void)
{
    vec2 uv = vTexCoord0.xy;
    vec4 c = texture(uTex0, uv.st);
    vec4 finalColor = max(c - vec4(vec3(uBrightnessThresh), 0.0), vec4(0.0));
    fragColor = finalColor;
}
