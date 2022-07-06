#version 400

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 u_LightView;
uniform mat4 u_M2W;
uniform mat4 u_ligjtProjection;
uniform mat4 u_lightMVP;


void main()
{
    gl_Position = u_lightMVP * aPosition;

}