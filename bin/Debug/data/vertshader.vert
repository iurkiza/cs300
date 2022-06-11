#version 400

// struct definition
struct Light
{
    int  type;
    
    vec3 uPos;
    vec3 uDir;
    vec3 uCol;
    vec3 uAtt;
    float uAmb;
    float uInner;
    float uOuter;
    float falloff;
    
};


// uniform declarations

uniform int   uLightNum;
uniform Light uLight[LIGHT_NUM_MAX];

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 u_MVP;
uniform mat4 u_M2W;
out vec3 color;
out vec2 UV;

void main()
{
   gl_Position = u_MVP * aPosition;
   color = vec3(aUV, 0);
   UV = aUV;
};