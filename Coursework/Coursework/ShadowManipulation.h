//shader for generating and shadowing vertex manipulated geometry
#ifndef _SHADOWMANUPULATION_H_
#define _SHADOWMANIPULATION_H_

#include "DXF.h"

using namespace std;
using namespace DirectX;


class ShadowManipulation : public BaseShader
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

	struct PixelBufferType
	{
		int ToggleNormals;
		XMFLOAT2 WaterSettings;
		float ExtraPadding;
	};

public:

	ShadowManipulation(ID3D11Device* device, HWND hwnd);
	~ShadowManipulation();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, const XMMATRIX LightViewMatrix[13], const XMMATRIX LightprojectionMatrix[13],
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* heightMap, ID3D11ShaderResourceView* depthMap[13], Light* light[8], bool toggleNormals,float time);
	void setAttenuation(float x, float y, float z, float w)
	{
		constantFactor = x;
		linearFactor = y;
		quadraticFactor = z;
		lightType = w; //0 = point light, 1 = directional
	}

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	//buffers in shaders receive data
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* PixelBuffer;

	//light values
	float constantFactor;
	float linearFactor;
	float quadraticFactor;
	float lightType; //0 = point light, 1 = directional
};

#endif