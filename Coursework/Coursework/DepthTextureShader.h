//depth pass for textured geometry, models
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthTextureShader : public BaseShader
{

public:

	DepthTextureShader(ID3D11Device* device, HWND hwnd);
	~DepthTextureShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	//buffers in shaders receive data
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* matrixBuffer;
};