//geometry shader for leaves
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class GeometryShader : public BaseShader
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

	struct RNGBufferType
	{
		XMFLOAT4 RNG;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient[8];
		XMFLOAT4 diffuse[8];
		XMFLOAT4 position[8];
		XMFLOAT4 attenuation[8];
	};

	struct PixelBufferType
	{
		int ToggleNormals;
		XMFLOAT2 WaterSettings;
		float ExtraPadding;
	};

public:

	GeometryShader(ID3D11Device* device, HWND hwnd);
	~GeometryShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, const XMMATRIX LightViewMatrix[13], const XMMATRIX LightprojectionMatrix[13],
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap[13], Light* light[8], bool Normals, float RNG, ID3D11ShaderResourceView* Bumptexture,float time);
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
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* PixelBuffer;
	ID3D11Buffer* RNGBuffer;

	//light values
	float constantFactor;
	float linearFactor;
	float quadraticFactor;
	float lightType; //0 = point light, 1 = directional
};
