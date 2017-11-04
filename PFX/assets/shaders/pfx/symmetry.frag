#version 410 core

uniform sampler2D uBaseFBO;
uniform int uSymMode;

in vec2 vTexCoord0;

out vec4 fragColor;

void main(void)
{
    vec2 uv = vTexCoord0.xy * 2.0 - 1.0;

    if(uSymMode == 0){
        if (uv.s > 0.0) uv.s = - uv.s;
    }else {
        if (uv.t > 0.0) uv.t = - uv.t;
    }
    
    uv = (uv + 1.0) * 0.5;
    
    vec4 c = texture(uBaseFBO, uv);
    fragColor = c;
}
