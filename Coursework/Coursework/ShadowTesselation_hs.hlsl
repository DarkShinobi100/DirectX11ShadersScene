// Tessellation Hull Shader
// Prepares control points for tessellation
//Data received from Cpp file in correct order
struct InputType
{
    float4 position : POSITION;
    float4 colour : COLOR;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float4 position : POSITION;
    float4 colour : COLOR;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};


cbuffer TesselationBuffer : register(b1)
{
    float tesselationLeft;
    float tesselationRight;
    float tesselationBottom;
    float tesselationInside;
};

cbuffer CameraBuffer : register(b2)
{
    float4 cameraPosition;
};

//Quad
ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;

    //calculate distance from the plane
    float distance = (pow((inputPatch[patchId].position.x - cameraPosition.x), 2) + pow((inputPatch[patchId].position.y - cameraPosition.y), 2) + pow((inputPatch[patchId].position.z - cameraPosition.z), 2));
    distance = sqrt(distance) / 3;

    // Set the tessellation factors for the edges.
    output.edges[0] = tesselationBottom / distance;
    output.edges[1] = tesselationLeft / distance;
    output.edges[2] = tesselationRight / distance;
    output.edges[3] = tesselationRight / distance;

    // Set the tessellation factor for tessallating inside the quad.
    output.inside[0] = tesselationInside / distance;
    output.inside[1] = tesselationInside / distance;

    return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    // Set the input colour as the output colour.
    output.colour = patch[pointId].colour;
    //pass it along
    output.tex = patch[pointId].tex;
    output.normal = patch[pointId].normal;

    //save world position before converting to screenspace
    output.worldPosition = patch[pointId].worldPosition;

    return output;
}