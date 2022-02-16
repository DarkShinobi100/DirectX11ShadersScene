// Horizontal blur shader handler
// Loads horizontal blur shaders (vs and ps)
// Passes screen width to shaders, for sample coordinate calculation
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class HorizontalBlurShader : public BaseShader
{
private:
	//The order the data should be sent to the shaders
	struct ScreenSizeBufferType
	{
		float screenWidth;
		XMFLOAT3 padding;
	};

public:

	HorizontalBlurShader(ID3D11Device* device, HWND hwnd);
	~HorizontalBlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture[2], float width);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	//buffers in shaders receive data
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;
};