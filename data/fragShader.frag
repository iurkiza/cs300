#version 400

in vec3 color;
in vec2 UV;
in vec3 Normal;
in vec4 pixPos;
in vec3 Bitangent;
in vec3 Tangent;
in vec4 lightSpacePos;

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
const int LIGHT_NUM_MAX = 10;
uniform int   uLightNum;
uniform Light uLight[LIGHT_NUM_MAX];

uniform sampler2D u_Texture;
uniform sampler2D u_heightMap;
uniform sampler2D u_shadowMap;

uniform float u_bias;
uniform int u_pcf;

uniform int u_TextureBool;
uniform int u_DrawNormalCol;

uniform int u_TangentCol;
uniform int u_BitangentCol;

uniform int u_isLight;
uniform vec3 u_camPos;
uniform float u_sh;
uniform mat4 u_View;

float calculateShadows(vec4 lightSpPos)
{
    vec3 fragPosL = lightSpacePos.xyz / lightSpacePos.w;
    fragPosL = fragPosL * 0.5 + 0.5;
    vec2 texelOffset = 1.0 / textureSize(u_shadowMap, 0);

    float shadow = 0.0f;
    float count = 0.0f;

    for(int x = -u_pcf; x <= u_pcf; ++x)
    {
        for(int y = -u_pcf; y <= u_pcf; ++y)
        {
            float D = texture(u_shadowMap, fragPosL.xy + vec2(x, y) * texelOffset).r; 
            shadow += fragPosL.z - u_bias > D ? 0.0 : 1.0;        
            count++;
        }    
    }
    shadow /= count;
    return shadow;
}

void main()
{
    vec4 finalColor;
    vec4 FragemtColor;
    

    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    
    vec3 heightNormal = texture(u_heightMap, UV).rgb;
    heightNormal = normalize(heightNormal * 2.0f - 1);
    heightNormal = TBN * heightNormal;

    heightNormal = normalize(heightNormal);


    if(u_TextureBool == 0)
    {
        FragemtColor = texture(u_Texture, UV);
    }

    for(unsigned int i = 0; i < uLightNum; i++)
    {
        //Light Vector
        vec3 L = uLight[i].uPos - vec3(pixPos);
        L = normalize(L);

        //View Vector
        vec3 V = - vec3(pixPos);
        V = normalize(V);

        //Normal Vector
        vec3 N = heightNormal;
        N = normalize(N);

        //Reflected Vector
        vec3 R = 2 * (dot(N, L)) * N - L;
        R = normalize(R);

        vec3 amb = vec3(FragemtColor) * uLight[i].uAmb;

        float shadow = calculateShadows(lightSpacePos);

        // I calculations
        if(uLight[i].type == 1)
        {
            //diffuse I
            vec3 difuseI = vec3(FragemtColor) * uLight[i].uCol * max(dot(N, L), 0);

            //specular I
            vec3 specullarI = vec3(1.0, 1.0, 1.0) * pow(max(dot(R,V), 0), u_sh);

            //attenuation
            float dist = length(uLight[i].uPos - vec3(pixPos));

            float atenuation = min(1 / (uLight[i].uAtt.x + (uLight[i].uAtt.y * dist) + (uLight[i].uAtt.z * dist * dist)), 1);

            //Calculate the color
            finalColor += vec4(amb,1.0) + atenuation * shadow *((vec4(difuseI, 1.0) + vec4(specullarI, 1.0)));
        }
        else if(uLight[i].type == 2)
        {
            L = -uLight[i].uDir;
            L = normalize(L);

            vec3 R = 2 * (dot(N, L)) * N - L;
            R = normalize(R);

            //diffuse I
            vec3 difuseI = vec3(FragemtColor) * uLight[i].uCol * max(dot(N, L), 0);

            //specular I
            vec3 specullarI = vec3(1.0, 1.0, 1.0) * pow(max(dot(R,V), 0), u_sh);


            finalColor += vec4(amb,1.0) + (shadow * ((vec4(difuseI, 1.0) + vec4(specullarI, 1.0))));

        }
        else if(uLight[i].type == 3)
        {
           //diffuse I
            vec3 difuseI = vec3(FragemtColor) * uLight[i].uCol * max(dot(N, L), 0);

            //specular I
            vec3 specullarI = vec3(1.0, 1.0, 1.0) * pow(max(dot(R,V), 0), u_sh);

            //attenuation
            float dist = length(uLight[i].uPos - vec3(pixPos));

            float atenuation = min(1 / (uLight[i].uAtt.x + (uLight[i].uAtt.y * dist) + (uLight[i].uAtt.z * dist * dist)), 1);
            
            float SpotlightEffect;

            if(dot(-L, uLight[i].uDir) < cos(radians(uLight[i].uOuter)))
            {
                SpotlightEffect = 0.0f;
            }
            else if(dot(-L, uLight[i].uDir) > cos(radians(uLight[i].uInner)))
            {
                SpotlightEffect = 1.0f;
            }
            else
            {
                SpotlightEffect = pow( (dot(-L, uLight[i].uDir) - cos(radians(uLight[i].uOuter))) / (cos(radians(uLight[i].uInner)) - cos(radians(uLight[i].uOuter))) , uLight[i].falloff);

            }
            //Calculate the color
            finalColor += vec4(amb,1.0) + atenuation * SpotlightEffect  * shadow * ((vec4(difuseI, 1.0) + vec4(specullarI, 1.0)));
        }
    }
    
    outputColor = finalColor;

    

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
    
    if(u_isLight == 1 )
    {
        outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        return;
    }
    
    if(u_TextureBool == 1) //normals
    {
        outputColor = vec4(Normal, 1.0f);
        return;
    }
    else if(u_TextureBool == 2) //Tangent
    {
        outputColor = vec4(Tangent, 1.0f);
        return;
    }
    else if(u_TextureBool == 3) //Bitangent
    {
        outputColor = vec4(Bitangent, 1.0f);
        return;
    }

}