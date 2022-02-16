// triangle_gs
// Geometry shader that generates a triangle for every vertex.
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

cbuffer RNGBuffer : register(b1)
{
    float4 RNG;
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

/*
[maxvertexcount(3)]
void main(point InputType input[1], inout TriangleStream<OutputType> triStream)
{
	OutputType output;
	
	// Move the vertex away from the point position
    output.position = input[0].position + float4(0.0, 1.0, 0.0, 0.0);
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.tex = input[0].tex;
    output.normal = mul(input[0].normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    triStream.Append(output);

	// Move the vertex away from the point position
    output.position = input[0].position + float4(-1.0, 0.0, 0.0, 0.0);
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.tex = input[0].tex;
    output.normal = mul(input[0].normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    triStream.Append(output);

	// Move the vertex away from the point position
    output.position = input[0].position + float4(1.0, 0.0, 0.0, 0.0);
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.tex = input[0].tex;
    output.normal = mul(input[0].normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    triStream.Append(output);

    triStream.RestartStrip();
}
*/

float4 Rotation(float4 Position, float Angle)
{
    //code from: https://stackoverflow.com/questions/28124548/rotate-quad-made-in-geometry-shader
    // This is the 3D position that we want to rotate:
    float4 p = Position;

    // Specify the axis to rotate about:
    float x = 0.0;
    float y = 1.0;
    float z = 0.0;

    // Specify the angle in radians:
    float angle = Angle * 3.14 / 180.0; // 90 degrees, CCW

    float4 q;
    q.x = p.x * (x * x * (1.0 - cos(angle)) + cos(angle))
        + p.y * (x * y * (1.0 - cos(angle)) + z * sin(angle))
        + p.z * (x * z * (1.0 - cos(angle)) - y * sin(angle));

    q.y = p.x * (y * x * (1.0 - cos(angle)) - z * sin(angle))
        + p.y * (y * y * (1.0 - cos(angle)) + cos(angle))
        + p.z * (y * z * (1.0 - cos(angle)) + x * sin(angle));

    q.z = p.x * (z * x * (1.0 - cos(angle)) + y * sin(angle))
        + p.y * (z * y * (1.0 - cos(angle)) - x * sin(angle))
        + p.z * (z * z * (1.0 - cos(angle)) + cos(angle));
    q.w = p.w;
    return q;
}

float4 CalculateNormals(point InputType input[1])
{
    //Calculate normals
    float3 right = (0.0, 0.0, 0.0);
    float3 left = (0.0, 0.0, 0.0);
    float3 above = (0.0, 0.0, 0.0);
    float3 below = (0.0, 0.0, 0.0);

    //calculate horizontal values
    right.x += 0.02f;
    right.y = 2 * texture1.SampleLevel(sampler0, input[0].tex + float2(0.02f, 0), 0);

    left.x -= 0.02f;
    left.y = 2 * texture1.SampleLevel(sampler0, input[0].tex - float2(0.02f, 0), 0);
    float3 Horizontal = normalize(right - left);

    //calculate vertical values
    above.z += 0.02f;
    above.y = 2 * texture1.SampleLevel(sampler0, input[0].tex + float2(0, 0.02f), 0);

    below.z -= 0.02f;
    below.y = 2 * texture1.SampleLevel(sampler0, input[0].tex - float2(0, 0.02f), 0);
    float3 Verical = normalize(below - above);

    //cross product of horizontal & vertical = correct values
    float3 vertexnormal = normalize(cross(Horizontal, Verical));


    // Calculate the position of the vertex against the world, view, and projection matrices.
    float4 position = mul(vertexnormal, worldMatrix);
    position = mul(position, viewMatrix);
    position = mul(position, projectionMatrix);

    return position;
}


[maxvertexcount(7)]
//Quad
void main(point InputType input[1], inout TriangleStream<OutputType> triStream)
{
    OutputType output;

    //generates Triangle around point 

    if (input[0].position.y > 175 && input[0].position.y < 450)
    {
        // Move the vertex away from the point position
        output.position = input[0].position + float4(-10.0, 10.0, 0.0, 0.0);
        //Rotate the quads randomly    
        output.position = Rotation(output.position, RNG.x);
        output.position = mul(output.position, worldMatrix);
        output.position = mul(output.position, viewMatrix);
        output.position = mul(output.position, projectionMatrix);
        output.tex = float2(0.0f, 0.0f);
        output.normal = CalculateNormals(input[0]);
        output.normal = normalize(output.normal);
        output.worldPosition = mul(input[0].position, worldMatrix).xyz;

        if (RNG.w != 1.0f) //if w = 1.0 it's the depth pass
        {
            for (int i = 0; i < 13; i++)
            {
                // Calculate the position of the vertice as viewed by the light source.
                output.lightViewPos[i] = mul(input[0].position, worldMatrix);
                output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
                output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
            }
        }

        triStream.Append(output);

        // Move the vertex away from the point position
        output.position = input[0].position + float4(-10.0, 0.0, 0.0, 0.0);
        //Rotate the quads randomly    
        output.position = Rotation(output.position, RNG.x);
        output.position = mul(output.position, worldMatrix);
        output.position = mul(output.position, viewMatrix);
        output.position = mul(output.position, projectionMatrix);
        output.tex = float2(0.0f, 1.0f);
        output.normal = CalculateNormals(input[0]);
        output.normal = normalize(output.normal);
        output.worldPosition = mul(input[0].position, worldMatrix).xyz;
        if (RNG.w != 1.0f)  //if w = 1.0 it's the depth pass
        {
            for (int i = 0; i < 13; i++)
            {
                // Calculate the position of the vertice as viewed by the light source.
                output.lightViewPos[i] = mul(input[0].position, worldMatrix);
                output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
                output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
            }
        }
        triStream.Append(output);

        // Move the vertex away from the point position
        output.position = input[0].position + float4(0.0, 0.0, 0.0, 0.0);
        //Rotate the quads randomly    
        output.position = Rotation(output.position, RNG.x);
        output.position = mul(output.position, worldMatrix);
        output.position = mul(output.position, viewMatrix);
        output.position = mul(output.position, projectionMatrix);
        output.tex = float2(1.0f, 1.0f);
        output.normal = CalculateNormals(input[0]);
        output.normal = normalize(output.normal);
        output.worldPosition = mul(input[0].position, worldMatrix).xyz;
        if (RNG.w != 1.0f)  //if w = 1.0 it's the depth pass
        {
            for (int i = 0; i < 13; i++)
            {
                // Calculate the position of the vertice as viewed by the light source.
                output.lightViewPos[i] = mul(input[0].position, worldMatrix);
                output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
                output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
            }
        }
        triStream.Append(output);

        triStream.RestartStrip();
        output.position = input[0].position + float4(0.0, 0.0, 0.0, 0.0);
        //Rotate the quads randomly    
        output.position = Rotation(output.position, RNG.x);
        output.position = mul(output.position, worldMatrix);
        output.position = mul(output.position, viewMatrix);
        output.position = mul(output.position, projectionMatrix);
        output.tex = float2(1.0f, 1.0f);
        output.normal = CalculateNormals(input[0]);
        output.normal = normalize(output.normal);
        output.worldPosition = mul(input[0].position, worldMatrix).xyz;
        if (RNG.w != 1.0f)  //if w = 1.0 it's the depth pass
        {
            for (int i = 0; i < 13; i++)
            {
                // Calculate the position of the vertice as viewed by the light source.
                output.lightViewPos[i] = mul(input[0].position, worldMatrix);
                output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
                output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
            }
        }
        triStream.Append(output);

        // Move the vertex away from the point position
        output.position = input[0].position + float4(0.0, 10.0, 0.0, 0.0);
        //Rotate the quads randomly    
        output.position = Rotation(output.position, RNG.x);
        output.position = mul(output.position, worldMatrix);
        output.position = mul(output.position, viewMatrix);
        output.position = mul(output.position, projectionMatrix);
        output.tex = float2(1.0f, 0.0f);
        output.normal = CalculateNormals(input[0]);
        output.normal = normalize(output.normal);
        output.worldPosition = mul(input[0].position, worldMatrix).xyz;
        if (RNG.w != 1.0f)  //if w = 1.0 it's the depth pass
        {
            for (int i = 0; i < 13; i++)
            {
                // Calculate the position of the vertice as viewed by the light source.
                output.lightViewPos[i] = mul(input[0].position, worldMatrix);
                output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
                output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
            }
        }
        triStream.Append(output);

        // Move the vertex away from the point position
        output.position = input[0].position + float4(-10.0, 10.0, 0.0, 0.0);
        //Rotate the quads randomly    
        output.position = Rotation(output.position, RNG.x);
        output.position = mul(output.position, worldMatrix);
        output.position = mul(output.position, viewMatrix);
        output.position = mul(output.position, projectionMatrix);
        output.tex = float2(0.0f, 0.0f);
        output.normal = CalculateNormals(input[0]);
        output.normal = normalize(output.normal);
        output.worldPosition = mul(input[0].position, worldMatrix).xyz;
        if (RNG.w != 1.0f)  //if w = 1.0 it's the depth pass
        {
            for (int i = 0; i < 13; i++)
            {
                // Calculate the position of the vertice as viewed by the light source.
                output.lightViewPos[i] = mul(input[0].position, worldMatrix);
                output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
                output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
            }
        }
        triStream.Append(output);

        triStream.RestartStrip();
    }
}