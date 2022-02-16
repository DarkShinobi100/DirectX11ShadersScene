// Depth pass for geometry shader
#pragma once

#include "DXF.h"
#include <cstdlib>

using namespace std;
using namespace DirectX;


class DepthGeometryShader : public BaseShader
{
private:
	//The order the data should be sent to the shaders
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightViewMatrix[13];
		XMMATRIX lightProjectionMatrix[13];
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[8];
		XMFLOAT4 diffuse[8];
		XMFLOAT4 position[8];
		XMFLOAT4 attenuation[8];
	};

	struct RNGBufferType
	{
		XMFLOAT4 RNG;
	};

	struct PixelBufferType
	{
		int ToggleNormals;
		XMFLOAT3 ExtraPadding;
	};

public:

	DepthGeometryShader(ID3D11Device* device, HWND hwnd);
	~DepthGeometryShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture,float RNG);
	void setAttenuation(float x, float y, float z, float w)
	{
		constantFactor = x;
		linearFactor = y;
		quadraticFactor = z;
		lightType = w; //0 = point light, 1 = directional
	}
private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* gsFilename, const wchar_t* psFilename);

private:
	//buffers in shaders receive data
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* PixelBuffer;
	ID3D11Buffer* RNGBuffer;

	//light values
	float constantFactor;
	float linearFactor;
	float quadraticFactor;
	float lightType; //0 = point light, 1 = directional
};
