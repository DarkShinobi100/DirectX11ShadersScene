// Vertical blur shader handler
// Loads vertical blur shaders (vs and ps)
// Passes screen height to shaders, for sample coordinate calculation
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class VerticalBlurShader : public BaseShader
{
private:
	//The order the data should be sent to the shaders
	struct ScreenSizeBufferType
	{
		float screenHeight;
		XMFLOAT3 padding;
	};

public:

	VerticalBlurShader(ID3D11Device* device, HWND hwnd);
	~VerticalBlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture[2], float width);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	//buffers in shaders receive data
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;
};
