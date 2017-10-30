#define PI 3.14159265358979

// Random function
float random(vec2 n){
    return fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

float random(float nx, float ny){
    return random(vec2(nx, ny));
}

//Map
float map(float value, float inputMin, float inputMax, float outputMin, float outputMax){
    return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

vec2 map(vec2 value, vec2 inputMin, vec2 inputMax, vec2 outputMin, vec2 outputMax){
    float x = map(value.x, inputMin.x, inputMax.x, outputMin.x, outputMax.x);
    float y = map(value.y, inputMin.y, inputMax.y, outputMin.y, outputMax.y);
    return vec2(x, y);
}

vec3 map(vec3 value, vec3 inputMin, vec3 inputMax, vec3 outputMin, vec3 outputMax){
    float x = map(value.x, inputMin.x, inputMax.x, outputMin.x, outputMax.x);
    float y = map(value.y, inputMin.y, inputMax.y, outputMin.y, outputMax.y);
    float z = map(value.z, inputMin.z, inputMax.z, outputMin.z, outputMax.z);
    return vec3(x, y, z);
}

vec4 map(vec4 value, vec4 inputMin, vec4 inputMax, vec4 outputMin, vec4 outputMax){
    float x = map(value.x, inputMin.x, inputMax.x, outputMin.x, outputMax.x);
    float y = map(value.y, inputMin.y, inputMax.y, outputMin.y, outputMax.y);
    float z = map(value.z, inputMin.z, inputMax.z, outputMin.z, outputMax.z);
    float w = map(value.w, inputMin.w, inputMax.w, outputMin.w, outputMax.w);
    return vec4(x, y, z, w);
}
