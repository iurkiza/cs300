#version 400

in vec3 color;
in vec2 UV;
in vec3 Normal;
in vec4 pixPos;


out vec4 outputColor;

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
uniform Light uLight[1];

uniform sampler2D u_Texture;
uniform int u_TextureBool;
uniform int u_DrawNormalCol;
uniform vec3 u_camPos;

void main()
{
    //Light Vector
    vec3 L = uLight[0].uPos - vec3(pixPos);
    L = normalize(L);
    
    //View Vector
    vec3 V = u_camPos - vec3(pixPos);
    V = normalize(V);
    
    //Reflected Vector
    //vec3 R = 2 * (Dot(Normal, L)) * Normal - L;
    
    
    // I calculations
    
    //float Iambient = uAmb;
    
    
    
    
    
    
    //Obj drawing
    if(u_TextureBool == 1)
    {
        outputColor = texture(u_Texture, UV) * uLight[0].uAmb;
    }
    else
    {
        outputColor = vec4(color, 1.0f) * 0.2;
    }
    
    //Normal Drawing
    if(u_DrawNormalCol == 1)
    {
        outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}