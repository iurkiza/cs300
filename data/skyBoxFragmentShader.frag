#version 400

out vec4 outputColor;
in vec3 UV;
uniform samplerCube u_cubeMap;

void main()
{
    outputColor = texture(u_cubeMap, UV);
}