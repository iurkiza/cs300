#version 400
in vec3 color;
in vec2 UV;
out vec4 outputColor;


uniform sampler2D u_Texture;
uniform int u_TextureBool;

void main()
{
    if(u_TextureBool == 1)
    {
        outputColor = texture(u_Texture, UV);
    }
    else
    {
        outputColor = vec4(color, 1.0f);
   }
}