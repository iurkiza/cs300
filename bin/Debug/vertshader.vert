#version 400

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 u_MVP;
uniform mat4 u_M2W;

//out 
out vec3 color;
out vec2 UV;
out vec3 Normal;
out vec4 pixPos;


void main()
{
   gl_Position = u_MVP * aPosition;
   color = vec3(aUV, 0);
   UV = aUV;
   
   Normal = aNormal;
   pixPos =  u_M2W * aPosition;
   
};


















