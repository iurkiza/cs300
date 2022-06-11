#version 400

 struct definition
struct Light
{
    int  type;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    vec3 position;
    //  other variables 
    //  ...
};


// uniform declarations

uniform int   uLightNum;
//uniform Light uLight[LIGHT_NUM_MAX];

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 u_MVP;
out vec3 color;
out vec2 UV;

void main()
{
   gl_Position = u_MVP * aPosition;
   color = vec3(aUV, 0);
   UV = aUV;
};