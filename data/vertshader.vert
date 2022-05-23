#version 400
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
out vec3 color;
void main()
{
   gl_Position = aPosition;
   color = vec3(1.0);
};