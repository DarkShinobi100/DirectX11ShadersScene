//depth pass for tesselation shader
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthTesselationShader : public BaseShader
{
private:
	//The order the data should be sent to the shaders
	struct TesselationBufferType
	{
		float tesselationLeft;
		float tesselationRight;
		float tesselationBottom;
		float tesselationInside;
	};

	struct TimeBufferType
	{
		XMFLOAT4 time;
	};

	struct CameraBufferType
	{
		XMFLOAT4 cameraPosition;
	};

public:

	DepthTesselationShader(ID3D11Device* device, HWND hwnd);
	~DepthTesselationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, XMFLOAT4 TesselationValues,
		ID3D11ShaderResourceView* Bumptexture, XMFLOAT4 time, XMFLOAT4 CameraPosition, ID3D11ShaderResourceView* texture);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	//buffers in shaders receive data
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* TesselationBuffer;
	ID3D11Buffer* CameraBuffer;
	ID3D11Buffer* TimeBuffer;
};