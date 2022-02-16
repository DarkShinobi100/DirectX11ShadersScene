//shader for generating and shadowing Tesselated geometry
#ifndef _SHADOWTESSELATION_H_
#define _SHADOWTESSELATION_H_

#include "DXF.h"

using namespace std;
using namespace DirectX;


class ShadowTesselation : public BaseShader
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

	struct TesselationBufferType
	{
		float tesselationLeft;
		float tesselationRight;
		float tesselationBottom;
		float tesselationInside;
	};
	struct LightBufferType
	{
		XMFLOAT4 ambient[8];
		XMFLOAT4 diffuse[8];
		XMFLOAT4 position[8];
		XMFLOAT4 attenuation[8];
	};

	struct TimeBufferType
	{
		XMFLOAT4 time;
	};

	struct CameraBufferType
	{
		XMFLOAT4 cameraPosition;
	};

	struct PixelBufferType
	{
		int ToggleNormals;
		XMFLOAT2 WaterSettings;
		float ExtraPadding;
	};

public:

	ShadowTesselation(ID3D11Device* device, HWND hwnd);
	~ShadowTesselation();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, const XMMATRIX LightViewMatrix[13], const XMMATRIX LightprojectionMatrix[13], XMFLOAT4 TesselationValues,
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Bumptexture, ID3D11ShaderResourceView* DepthMap[13], Light* light[8], XMFLOAT4 time, XMFLOAT4 CameraPosition, bool Normals);
	void setAttenuation(float x, float y, float z, float w)
	{
		constantFactor = x;
		linearFactor = y;
		quadraticFactor = z;
		lightType = w; //0 = point light, 1 = directional
	}

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	//buffers in shaders receive data
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* TesselationBuffer;
	ID3D11Buffer* TimeBuffer;
	ID3D11Buffer* CameraBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* PixelBuffer;

	//light values
	float constantFactor;
	float linearFactor;
	float quadraticFactor;
	float lightType; //0 = point light, 1 = directional
};

#endif