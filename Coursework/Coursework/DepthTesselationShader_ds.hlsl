// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
//Texture to use for vertex manipulation
Texture2D texture1 : register(t1);
SamplerState sampler0 : register(s0);

//Data received from Cpp file in correct order
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer TimeBuffer : register(b1)
{
    float4 time;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float4 vertexPosition;
    OutputType output;

    //lerp between the 2 points we have by a set amount to find the correct texture coordinates
    float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    float2 texCoord = lerp(t1, t2, uvwCoord.x);

    //lerp between the 2 points we have by a set amount to find the correct vertex positions
    float4 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float4 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);

    //use hieght map data for y values
    vertexPosition.y = 5 * texture1.SampleLevel(sampler0, texCoord + float2 (time.x / 500, time.x / 500), 0);

    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(vertexPosition, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the position value in a second input value for depth value calculations.
    output.depthPosition = output.position;
    return output;
}