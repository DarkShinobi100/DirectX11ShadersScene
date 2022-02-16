//Texture to sample to get correct image colours 
Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture[13] : register(t1);

SamplerState diffuseSampler  : register(s0);
SamplerState shadowSampler : register(s1);

//Data received from Cpp file in correct order
cbuffer LightBuffer : register(b0)
{
    float4 ambient[8];
    float4 diffuse[8];
    float4 position[8];
    float4 attenuation[8];
};

cbuffer PixelBuffer : register(b1)
{
    int ToggleNormals;
    float2 WaterSettings;
    float ExtraPadding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos[13] : TEXCOORD2;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
    // Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

    // Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float4 main(InputType input) : SV_TARGET
{
    float shadowMapBias = 0.005f;
    float4 colour = float4(0.f, 0.f, 0.f, 1.f);
    // Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    float4 textureColour = float4(0.f, 0.f, 0.f, 1.f);
   
    //if it is the water shader
    if (WaterSettings.x !=0.0f)
    {// move where the sampler checks each frame to create a moving water effect
        textureColour = shaderTexture.SampleLevel(diffuseSampler, input.tex + float2 (WaterSettings.y / 500, WaterSettings.y / 500), 0);
    }
    else
    {// sample at the correct texture coordinate
        textureColour = shaderTexture.Sample(diffuseSampler, input.tex);
    }
    
    float4 lightFinalColour[9];

    if (ToggleNormals > 0)
    {
        return float4(input.normal, 1.f);
    }
    else {

    }// Shadow test. Is or isn't in shadow
    for (int i = 0; i < 13; i++)
    {
        // Calculate the projected texture coordinates.
        float2 pTexCoord = getProjectiveCoords(input.lightViewPos[i]);

        if (hasDepthData(pTexCoord))
        {
            // Has depth map data
            if (!isInShadow(depthMapTexture[i], pTexCoord, input.lightViewPos[i], shadowMapBias))
            {
                // is NOT in shadow, therefore light
                //new method
                for (int j = 0; j < 8; j++)
                {
                    float lightType = attenuation[j].w;

                    //call calculateLighting to handle diffuse,
                    if (lightType == 0.0f) //Spotlight
                    {
                        //lightvector
                        float3 lightVector = position[j] - input.worldPosition;

                        //normalise lightvector
                        lightVector = normalize(lightVector);
                        lightFinalColour[j] = pow(max(calculateLighting(lightVector, input.normal, diffuse[j]), 0.0f), 20.0f);
                        lightFinalColour[j] = saturate(lightFinalColour[j] + ambient[j]);
                    }
                    else if (lightType == 1.0f) //directional
                    {
                        lightFinalColour[j] = calculateLighting(-position[j], input.normal, diffuse[j]);
                        lightFinalColour[j] = lightFinalColour[j] + ambient[j];
                    }
                    else
                    {
                        //lightvector
                        float3 lightVector = position[j] - input.worldPosition;
                        //attenuation
                        float constantFactor = attenuation[j].x;
                        float linearFactor = attenuation[j].y;
                        float quadraticFactor = attenuation[j].z;

                        float distance = length(lightVector);
                        float attenuationValue = 1 / (constantFactor + (linearFactor * distance) + (quadraticFactor * pow(distance, 2)));

                        //normalise lightvector
                        lightVector = normalize(lightVector);

                        lightFinalColour[j] = calculateLighting(lightVector, input.normal, diffuse[j]);

                        //add the ambient
                        lightFinalColour[j] = lightFinalColour[j] + ambient[j];
                        //multiply lightColour by the attenuation.
                        lightFinalColour[j] = lightFinalColour[j] * attenuationValue;
                    }
                }
                lightFinalColour[8] = lightFinalColour[0] + lightFinalColour[1] + lightFinalColour[2] + lightFinalColour[3] + lightFinalColour[4] + lightFinalColour[5] + lightFinalColour[6] + lightFinalColour[7];

                //return lightColour ;
                return saturate(lightFinalColour[8] * textureColour);
            }
    }
    }
    colour = saturate(colour + ambient[0]);
    return saturate(colour) * textureColour;    
}