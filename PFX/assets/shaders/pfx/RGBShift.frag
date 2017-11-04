#version 410 core

uniform sampler2D uBaseFBO;
uniform float uTime;

in vec2 vTexCoord0;

out vec4 fragColor;

void main(void)
{
    vec2 uv = vTexCoord0;
    
    float amount = 0.0;
    
    amount = (1.0 + sin(uTime * 6.0)) * 0.5;
    amount *= 1.0 + sin(uTime * 16.0) * 0.5;
    amount *= 1.0 + sin(uTime * 19.0) * 0.5;
    amount *= 1.0 + sin(uTime * 27.0) * 0.5;
    amount = pow(amount, 3.0);
    
    amount *= 0.05;
    
    vec3 c;
    c.r = texture(uBaseFBO, vec2(uv.x + amount, uv.y)).r;
    c.g = texture(uBaseFBO, uv).g;
    c.b = texture(uBaseFBO, vec2(uv.x - amount, uv.y)).b;
    
    c *= (1.0 - amount * 0.5);
    
    fragColor = vec4(c, 1.0);
}
