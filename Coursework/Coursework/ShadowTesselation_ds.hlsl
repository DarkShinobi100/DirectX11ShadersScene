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
    matrix lightViewMatrix[13];
    matrix lightProjectionMatrix[13];
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
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos[13] : TEXCOORD2;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float4 vertexPosition;
    OutputType output;

    float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    float2 texCoord = lerp(t1, t2, uvwCoord.x);

    // vertexPosition.y = amplitude * texture1.SampleLevel(sampler0, texCoord, 0);
    float4 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float4 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);

    //use hieght map data for y values
    vertexPosition.y = 5 * texture1.SampleLevel(sampler0, texCoord + float2 (time.x / 500, time.x / 500), 0);

    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(vertexPosition, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = texCoord;

    // Calculate the normal vector against the world matrix only and normalise.
    //Calculate normals
    float3 right = (0.0, 0.0, 0.0);
    float3 left = (0.0, 0.0, 0.0);
    float3 above = (0.0, 0.0, 0.0);
    float3 below = (0.0, 0.0, 0.0);

    //calculate horizontal values
    right.x += 0.02f;
    right.y = 5 * texture1.SampleLevel(sampler0, (texCoord + float2 (time.x / 500, time.x / 500)) + float2(0.02f, 0), 0);

    left.x -= 0.02f;
    left.y = 5 * texture1.SampleLevel(sampler0, (texCoord + float2 (time.x / 500, time.x / 500)) - float2(0.02f, 0), 0);
    float3 Horizontal = normalize(right - left);

    //calculate vertical values
    above.z += 0.02f;
    above.y = 5 * texture1.SampleLevel(sampler0, (texCoord + float2 (time.x / 500, time.x / 500)) + float2(0, 0.02f), 0);

    below.z -= 0.02f;
    below.y = 5 * texture1.SampleLevel(sampler0, (texCoord + float2 (time.x / 500, time.x / 500)) - float2(0, 0.02f), 0);
    float3 Verical = normalize(below - above);

    //cross product of horizontal & vertical = correct values
    float3 vertexnormal = normalize(cross(Horizontal, Verical));
    output.normal = vertexnormal;

    //save current position before conversion to screenspace
    output.worldPosition = mul(vertexPosition, worldMatrix).xyz;

    for (int i = 0; i < 13; i++)
    {
        // Calculate the position of the vertice as viewed by the light source.
        output.lightViewPos[i] = mul(vertexPosition, worldMatrix);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
    }

    return output;
}

