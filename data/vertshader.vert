#version 400

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 u_MVP;
uniform mat4 u_M2W;
uniform mat4 u_View;


//out 
out vec3 color;
out vec2 UV;
out vec3 Normal;
out vec3 Position;

void main()
{
   gl_Position = u_MVP * aPosition;
   Position = vec3(u_MVP * aPosition);
   color = vec3(aUV, 0);
   UV = aUV;

   Normal = mat3(transpose(inverse(u_M2W))) * aNormal;
   
};

















