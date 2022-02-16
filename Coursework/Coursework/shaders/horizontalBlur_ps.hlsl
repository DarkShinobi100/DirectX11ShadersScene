Texture2D shaderTexture[2] : register(t0);
SamplerState SampleType : register(s0);

//Data received from Cpp file in correct order
cbuffer ScreenSizeBuffer : register(b0)
{
    float screenWidth;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 WeakBlur(InputType input, float4 colour, float4 DistanceValue)
{
    float weight0, weight1, weight2;

    // Create the weights that each neighbor pixel will contribute to the blur.
    weight0 = 0.4062f;
    weight1 = 0.2442f;
    weight2 = 0.0545f;

    float texelSize = 1.0f / screenWidth;
    // Add the horizontal pixels to the colour by the specific weight of each.
    colour += (shaderTexture[0].Sample(SampleType, input.tex + float2(texelSize * -2.0f, 0.0f)) * weight2) * (DistanceValue / DistanceValue.w);
    colour += (shaderTexture[0].Sample(SampleType, input.tex + float2(texelSize * -1.0f, 0.0f)) * weight1) * (DistanceValue / DistanceValue.w);
    colour += (shaderTexture[0].Sample(SampleType, input.tex) * weight0) * (DistanceValue / DistanceValue.w);
    colour += (shaderTexture[0].Sample(SampleType, input.tex + float2(texelSize * 1.0f, 0.0f)) * weight1) * (DistanceValue / DistanceValue.w);
    colour += (shaderTexture[0].Sample(SampleType, input.tex + float2(texelSize * 2.0f, 0.0f)) * weight2) * (DistanceValue / DistanceValue.w);

    return colour;
}

float4 StrongBlur(InputType input, float4 colour, float4 DistanceValue)
{
    float weight0, weight1, weight2, weight3, weight4;

    // Create the weights that each neighbor pixel will contribute to the blur.
    weight0 = 0.382928f;
    weight1 = 0.241732f;
    weight2 = 0.060598f;
    weight3 = 0.005977f;
    weight4 = 0.000229f;

    float texelSize = 1.0f / screenWidth;
    // Add the horizontal pixels to the colour by the specific weight of each.
    colour += (shaderTexture[0].Sample(SampleType, input.tex + float2(texelSize * -4.0f, 0.0f)) * weight4) * (DistanceValue / DistanceValue.w);
    colour += (shaderTexture[0].Sample(SampleType, input.tex + float2(texelSize * -3.0f, 0.0f)) * weight3) * (DistanceValue / DistanceValue.w);
    colour += (shaderTexture[0].Sample(SampleType, input.tex + float2(texelSize * -2.0f, 0.0f)) * weight2) * (DistanceValue / DistanceValue.w);
    colour += (shaderTexture[0].Sample(SampleType, input.tex + float2(texelSize * -1.0f, 0.0f)) * weight1) * (DistanceValue / DistanceValue.w);
    colour += (shaderTexture[0].Sample(SampleType, input.tex) * weight0) * (DistanceValue / DistanceValue.w);
    colour += (shaderTexture[0].Sample(SampleType, input.tex + float2(texelSize * 1.0f, 0.0f)) * weight1) * (DistanceValue / DistanceValue.w);
    colour += (shaderTexture[0].Sample(SampleType, input.tex + float2(texelSize * 2.0f, 0.0f)) * weight2) * (DistanceValue / DistanceValue.w);
    colour += (shaderTexture[0].Sample(SampleType, input.tex + float2(texelSize * 3.0f, 0.0f)) * weight3) * (DistanceValue / DistanceValue.w);
    colour += (shaderTexture[0].Sample(SampleType, input.tex + float2(texelSize * 4.0f, 0.0f)) * weight4) * (DistanceValue / DistanceValue.w);

    return colour;
}

float4 main(InputType input) : SV_TARGET
{
    float4 colour;
	// Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //distance blur
    float4 DistanceValue = shaderTexture[1].Sample(SampleType, input.tex).r;

    //if the current pixel is further away than the centre of the screen
    if (DistanceValue.x >= shaderTexture[1].Sample(SampleType, float2(0.5, 0.5)).r)
    {
        //if it is less than 50% darker than the centre
        if (DistanceValue.x <= 0.5)
        {
            colour = WeakBlur(input, colour, DistanceValue);
        }
        else // it must be far away, apply a stronger blur
        {
            colour = StrongBlur(input, colour, DistanceValue);
        }       
    }
    else
    {
        colour = shaderTexture[0].Sample(SampleType, input.tex);
    }

	// Set the alpha channel to one.
    colour.a = 1.0f;

    return colour;
}
