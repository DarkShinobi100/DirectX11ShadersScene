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

    // pass the values through.
    output.position = input.position;
    output.tex = input.tex;
    output.normal = input.normal;

    return output;
}