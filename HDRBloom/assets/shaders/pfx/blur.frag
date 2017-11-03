#version 410 core

uniform sampler2D uTex0;
uniform int uDirection;
uniform vec2 uResolution;
uniform float uOffsetScale;
uniform float uAttenuation;

in vec2 vTexCoord0;

out vec4 fragColor;

void main(void)
{
    vec2 uv = vTexCoord0.xy;
    
    vec2 sampleOffset;
    if(uDirection == 0) sampleOffset = vec2(1.0 / uResolution.x, 0.0); // horizontal
    else sampleOffset = vec2(0.0, 1.0 / uResolution.y); // vertical
    sampleOffset *= uOffsetScale;
    
    vec3 sum = vec3(0.0, 0.0, 0.0);
    sum += texture(uTex0, uv + -10.0 * sampleOffset).rgb * 0.009167927656011385;
    sum += texture(uTex0, uv +  -9.0 * sampleOffset).rgb * 0.014053461291849008;
    sum += texture(uTex0, uv +  -8.0 * sampleOffset).rgb * 0.020595286319257878;
    sum += texture(uTex0, uv +  -7.0 * sampleOffset).rgb * 0.028855245532226279;
    sum += texture(uTex0, uv +  -6.0 * sampleOffset).rgb * 0.038650411513543079;
    sum += texture(uTex0, uv +  -5.0 * sampleOffset).rgb * 0.049494378859311142;
    sum += texture(uTex0, uv +  -4.0 * sampleOffset).rgb * 0.060594058578763078;
    sum += texture(uTex0, uv +  -3.0 * sampleOffset).rgb * 0.070921288047096992;
    sum += texture(uTex0, uv +  -2.0 * sampleOffset).rgb * 0.079358891804948081;
    sum += texture(uTex0, uv +  -1.0 * sampleOffset).rgb * 0.084895951965930902;
    sum += texture(uTex0, uv +   0.0 * sampleOffset).rgb * 0.086826196862124602;
    sum += texture(uTex0, uv +  +1.0 * sampleOffset).rgb * 0.084895951965930902;
    sum += texture(uTex0, uv +  +2.0 * sampleOffset).rgb * 0.079358891804948081;
    sum += texture(uTex0, uv +  +3.0 * sampleOffset).rgb * 0.070921288047096992;
    sum += texture(uTex0, uv +  +4.0 * sampleOffset).rgb * 0.060594058578763078;
    sum += texture(uTex0, uv +  +5.0 * sampleOffset).rgb * 0.049494378859311142;
    sum += texture(uTex0, uv +  +6.0 * sampleOffset).rgb * 0.038650411513543079;
    sum += texture(uTex0, uv +  +7.0 * sampleOffset).rgb * 0.028855245532226279;
    sum += texture(uTex0, uv +  +8.0 * sampleOffset).rgb * 0.020595286319257878;
    sum += texture(uTex0, uv +  +9.0 * sampleOffset).rgb * 0.014053461291849008;
    sum += texture(uTex0, uv + +10.0 * sampleOffset).rgb * 0.009167927656011385;
    
    sum *= uAttenuation;
    fragColor = vec4(sum, 1.0);
}
