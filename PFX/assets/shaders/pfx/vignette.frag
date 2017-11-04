#version 410 core

uniform sampler2D uBaseFBO;

in vec2 vTexCoord0;

out vec4 fragColor;

const float vignetteStrength = 1.5;

void main(void)
{
    vec2 uv = vTexCoord0;
    vec4 c = texture(uBaseFBO, uv.st);
    vec2 centeredCoord = uv - 0.5; // -0.5 ~ 0.5
    
    float distance = sqrt(dot(centeredCoord, centeredCoord));
    
    fragColor = vec4(mix(c.rgb, vec3(0), distance * vignetteStrength), c.a);
}
