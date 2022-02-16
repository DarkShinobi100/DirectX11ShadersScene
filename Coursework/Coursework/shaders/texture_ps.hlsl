// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D texture0 : register(t0);
//Texture2D texture1 : register(t1);

SamplerState Sampler0 : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	float4 textureColor;
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = texture0.Sample(Sampler0, input.tex);
	//textureColor += texture1.Sample(Sampler0, input.tex);
	//invert colours
	//return 1.0 - textureColor;
	
	//default colours
	//return float4(input.normal, 1.f);
	return textureColor;
}