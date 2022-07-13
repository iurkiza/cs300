#version 400

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 u_enviromentMapView;
uniform mat4 u_enviromentMapModel;
uniform mat4 u_enviromentMapPrespective;
uniform mat4 u_enviromentMapMVP;


void main()
{
    gl_Position = u_enviromentMapMVP * aPosition;

}