#version 400

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

out vec3 color;
uniform mat4 u_MVP;
out vec2 UV;

void main()
{
   gl_Position = u_MVP * aPosition;
   color = vec3(aUV, 0);
   UV = aUV;
};