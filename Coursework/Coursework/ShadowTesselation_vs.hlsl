// Tessellation vertex shader.
// Doesn't do much, could manipulate the control points
// Pass forward data, strip out some values not required for example.

//Data received from Cpp file in correct order
struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

OutputType main(InputType input)
{
    OutputType output;

    // Pass the vertex position into the hull shader.
    output.position = input.position;

    //pass it along
    output.tex = input.tex;
    output.normal = input.normal;

    return output;
}
