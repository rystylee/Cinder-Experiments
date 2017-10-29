#version 410 core

in vec4 vColor;
in vec3 vNormal;

out vec4 fragColor;

const vec3 lightPos = vec3(0, 0, 1);

void main(void)
{
    vec3 c = vColor.rgb;
    float lambert = max(0.0, dot(normalize(vNormal), lightPos));
    c *= lambert;
    
    fragColor = vec4(c, 1.0);
}
