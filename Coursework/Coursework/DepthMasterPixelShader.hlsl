//Texture to sample for transparant images
Texture2D Texture : register(t0);
SamplerState Sampler  : register(s0);

//Data received from Cpp file in correct order
struct InputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float depthValue;
    float4 textureColour = Texture.Sample(Sampler, input.depthPosition);
    float4 comparison = (0.0f, 0.0f, 0.0f, 1.0f);
    if (textureColour.a < comparison.a)
    {
        clip(textureColour.a - .25);
        //check if the leaf has an alpha value less than 25% if so delete the pixel
        depthValue = 0.0f;
    }
    else 
    {
    // Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
    depthValue = input.depthPosition.z / input.depthPosition.w;
    }

    return float4(depthValue, depthValue, depthValue, 1.0f);

}