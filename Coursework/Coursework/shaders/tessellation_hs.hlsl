// Tessellation Hull Shader
// Prepares control points for tessellation
struct InputType
{
    float3 position : POSITION;
    float4 colour : COLOR;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
    float4 colour : COLOR;
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


//Default Triangle
ConstantOutputType PatchConstantFunction(InputPatch<InputType, 3> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;
    float distance = ((inputPatch[patchId].position.x - cameraPosition.x) + (inputPatch[patchId].position.y - cameraPosition.y) + (inputPatch[patchId].position.z - cameraPosition.z) / 3);

    // Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = tesselationBottom/distance;
	output.edges[1] = tesselationLeft/distance;
	output.edges[2] = tesselationRight/distance;

    // Set the tessellation factor for tessallating inside the triangle.
	output.inside = tesselationInside/distance;

    return output;
}

/*
//Quad
ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;

    float distance = ((inputPatch[patchId].position.x - cameraPosition.x) + (inputPatch[patchId].position.y - cameraPosition.y) + (inputPatch[patchId].position.z - cameraPosition.z) / 3);
    //distance = 1 - distance;

    // Set the tessellation factors for the three edges of the triangle.
    output.edges[0] = tesselationBottom / distance;
    output.edges[1] = tesselationLeft / distance;
    output.edges[2] = tesselationRight / distance;
    output.edges[3] = tesselationRight / distance;

    // Set the tessellation factor for tessallating inside the triangle.
    output.inside[0] = tesselationInside / distance;
    output.inside[1] = tesselationInside / distance;

    return output;
}
*/



/*
[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
*/

[domain("tri")]
[partitioning("integer")]
//[partitioning("fractional_even")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;


    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    // Set the input colour as the output colour.
    output.colour = patch[pointId].colour;

    return output;
}