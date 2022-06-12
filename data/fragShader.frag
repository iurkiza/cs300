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
uniform int u_isLight;
uniform vec3 u_camPos;
uniform float u_sh;
uniform mat4 u_View;

void main()
{

    vec4 FragemtColor;
    if(u_TextureBool == 1)
    {
        FragemtColor = texture(u_Texture, UV);
    }
    else
    {
        FragemtColor = vec4(color, 1.0f);
    }

    //Light Vector
    vec3 L = uLight[0].uPos - vec3(pixPos);
    L = normalize(L);

    //View Vector
    vec3 V = - vec3(pixPos);
    V = normalize(V);
    
    //Normal Vector
    vec3 N = Normal;
    N = normalize(N);

    //Reflected Vector
    vec3 R = 2 * (dot(N, L)) * N - L;
    R = normalize(R);
    
    vec3 amb = vec3(FragemtColor) * uLight[0].uAmb;

    // I calculations
    if(uLight[0].type == 1)
    {
        //diffuse I
        vec3 difuseI = vec3(FragemtColor) * uLight[0].uCol * max(dot(N, L), 0);

        //specular I
        vec3 specullarI = vec3(1.0, 1.0, 1.0) * pow(max(dot(R,V), 0), u_sh);
        
        //attenuation
        float dist = length(uLight[0].uPos - vec3(pixPos));
        
        float atenuation = min(1 / (uLight[0].uAtt.x + (uLight[0].uAtt.y * dist) + (uLight[0].uAtt.z * dist * dist)), 1);

           //Calculate the color
        outputColor = vec4(amb,1.0) + atenuation *((vec4(difuseI, 1.0) + vec4(specullarI, 1.0)));
    }
    else if(uLight[0].type == 2)
    {
        L = -uLight[0].uDir;
        L = normalize(L);
        
        vec3 R = 2 * (dot(N, L)) * N - L;
        R = normalize(R);
        
        //diffuse I
        vec3 difuseI = vec3(FragemtColor) * uLight[0].uCol * max(dot(N, L), 0);

        //specular I
        vec3 specullarI = vec3(1.0, 1.0, 1.0) * pow(max(dot(R,V), 0), u_sh);
        
        //float intensity = max(0.0, dot(N, L));
        
        outputColor = vec4(amb,1.0) + ((vec4(difuseI, 1.0) + vec4(specullarI, 1.0)));
        //outputColor = FragemtColor * intensity;
        
    }
    else if(uLight[0].type == 3)
    {
       //diffuse I
        vec3 difuseI = vec3(FragemtColor) * uLight[0].uCol * max(dot(N, L), 0);

        //specular I
        vec3 specullarI = vec3(1.0, 1.0, 1.0) * pow(max(dot(R,V), 0), u_sh);
        
        //attenuation
        float dist = length(uLight[0].uPos - vec3(pixPos));
        
        float atenuation = min(1 / (uLight[0].uAtt.x + (uLight[0].uAtt.y * dist) + (uLight[0].uAtt.z * dist * dist)), 1);
        
        
        //float alpha = dot();
        
        float SpotlightEffect = pow( (dot(-L, uLight[0].uDir) - cos(radians(uLight[0].uOuter))) / (cos(radians(uLight[0].uInner)) - cos(radians(uLight[0].uOuter))) , uLight[0].falloff);
        //Calculate the color
        outputColor = vec4(amb,1.0) + SpotlightEffect * atenuation *((vec4(difuseI, 1.0) + vec4(specullarI, 1.0)));
    }

    


    //Normal Drawing
    if(u_DrawNormalCol == 1 ||u_isLight == 1 )
    {
        outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}