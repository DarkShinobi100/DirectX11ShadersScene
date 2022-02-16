//Depth pass for any vertex manipulated geometry 
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthManipulationShader : public BaseShader
{

public:

	DepthManipulationShader(ID3D11Device* device, HWND hwnd);
	~DepthManipulationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* Bumptexture, ID3D11ShaderResourceView* texture);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	//buffers in shaders receive data
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
};