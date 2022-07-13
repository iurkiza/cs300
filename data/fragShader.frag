#version 400

//input
in vec3 color;
in vec2 UV;
in vec3 Normal;
in vec3 Position;


//output
out vec4 outputColor;

//textures
uniform sampler2D u_Texture;
uniform samplerCube u_enviromentMap;

//bools
uniform int u_TextureMode;
uniform int u_DrawNormalCol;
uniform int u_TangentCol;
uniform int u_BitangentCol;
uniform int u_refractor;

uniform float u_ior;
uniform vec3 u_cameraPos;


void main()
{
    vec4 finalColor;
    vec4 FragemtColor;
    
    if(u_refractor == 1)
    {
        if(u_TextureMode == 0)
        {
            outputColor = texture(u_Texture, UV);
            return;
        }
        else if(u_TextureMode == 1)
        {
            vec3 I = normalize(Position - u_cameraPos);
            vec3 R = reflect(I, normalize(Normal));
            outputColor = vec4(texture(u_enviromentMap, R).rgb, 1.0);

            return;
        }
        else if(u_TextureMode == 2)
        {
            float ratio = 1.0f / u_ior;
            vec3 I = normalize(Position - u_cameraPos);
            vec3 R = refract(I, normalize(Normal), ratio);
            outputColor = vec4(texture(u_enviromentMap, R).rgb, 1.0);

            return;

        }
        
    }

    if(u_refractor == 0)
    {
        FragemtColor = texture(u_Texture, UV);
    }

    outputColor = FragemtColor;



    //Normal Drawing
    if(u_DrawNormalCol == 1)
    {
        outputColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
        return;
    }
    if(u_BitangentCol == 1)
    {
        outputColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
        return;

    }
    if(u_TangentCol == 1)
    {
        outputColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        return;
    }
    
}