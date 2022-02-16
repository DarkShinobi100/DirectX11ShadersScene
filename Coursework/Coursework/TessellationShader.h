// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class TessellationShader : public BaseShader
{
private:
	struct TesselationBufferType
	{
		float tesselationLeft;
		float tesselationRight;
		float tesselationBottom;
		float tesselationInside;
		float time;
		XMFLOAT3 padding;
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

	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, XMFLOAT4 TesselationValues, float time, XMFLOAT4 CameraPosition);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* TesselationBuffer;
	ID3D11Buffer* TimeBuffer;
	ID3D11Buffer* CameraBuffer;
};
