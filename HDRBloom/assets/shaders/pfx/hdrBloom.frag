#version 410 core

uniform sampler2D uTex0;
uniform sampler2D uBlurTex;
uniform float uGamma;

in vec2 vTexCoord0;

out vec4 fragColor;

void main(void)
{
    vec2 uv = vTexCoord0.xy;
    
    vec3 hdrColor = texture(uTex0, uv).rgb;
    vec3 bloomColor = texture(uBlurTex, uv).rgb;
    hdrColor += bloomColor;
    
    vec3 result = pow(hdrColor, vec3(1.0 / uGamma));
    fragColor = vec4(result, 1.0);
}
