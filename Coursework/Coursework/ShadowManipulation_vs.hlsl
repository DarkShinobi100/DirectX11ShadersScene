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


OutputType main(InputType input)
{
    OutputType output;

    //use hieght map data for y values
    input.position.y = 2 * texture1.SampleLevel(sampler0, input.tex, 0);

    //Calculate normals
    float3 right = (0.0, 0.0, 0.0);
    float3 left = (0.0, 0.0, 0.0);
    float3 above = (0.0, 0.0, 0.0);
    float3 below = (0.0, 0.0, 0.0);

    //calculate horizontal values
    right.x += 0.02f;
    right.y = 2 * texture1.SampleLevel(sampler0, input.tex + float2(0.02f, 0), 0);

    left.x -= 0.02f;
    left.y = 2 * texture1.SampleLevel(sampler0, input.tex - float2(0.02f, 0), 0);
    float3 Horizontal = normalize(right - left);

    //calculate vertical values
    above.z += 0.02f;
    above.y = 2 * texture1.SampleLevel(sampler0, input.tex + float2(0, 0.02f), 0);

    below.z -= 0.02f;
    below.y = 2 * texture1.SampleLevel(sampler0, input.tex - float2(0, 0.02f), 0);
    float3 Verical = normalize(below - above);

    //cross product of horizontal & vertical = correct values
    float3 vertexnormal = normalize(cross(Horizontal, Verical));

    //save correct normals
    input.normal = vertexnormal;
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    //save current position before conversion to screenspace
    output.worldPosition = mul(input.position, worldMatrix).xyz;

    for (int i = 0; i < 13; i++)
    {
        // Calculate the position of the vertice as viewed by the light source.
        output.lightViewPos[i] = mul(input.position, worldMatrix);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
        output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
    }

    return output;
}