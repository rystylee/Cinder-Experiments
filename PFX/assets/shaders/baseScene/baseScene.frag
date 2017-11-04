#version 410 core

in vec2 vTexCoord0;
in vec3 vNormal;

out vec4 fragColor;

void main(void)
{
    vec3 c = vec3(1, 1, 1);
    
    const vec3 lightPos = vec3(0, 0, 1);
    float lambert = max(0.0, dot(normalize(vNormal), lightPos));
    c *= lambert;
    
    fragColor = vec4(c, 1.0);
}
