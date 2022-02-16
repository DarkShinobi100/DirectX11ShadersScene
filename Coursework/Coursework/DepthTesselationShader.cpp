// depth shader.cpp
#include "DepthTesselationShader.h"

DepthTesselationShader::DepthTesselationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"DepthTesselationShader_vs.cso", L"DepthTesselationShader_hs.cso", L"DepthTesselationShader_ds.cso", L"DepthMasterPixelShader.cso");
}

DepthTesselationShader::~DepthTesselationShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void DepthTesselationShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Setup the description of the tesselation buffer size.
	D3D11_BUFFER_DESC tesselationBufferDesc;
	tesselationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tesselationBufferDesc.ByteWidth = sizeof(TesselationBufferType);
	tesselationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tesselationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tesselationBufferDesc.MiscFlags = 0;
	tesselationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tesselationBufferDesc, NULL, &TesselationBuffer);

	// Setup the description of the Time buffer size.
	D3D11_BUFFER_DESC TimeBufferDesc;
	TimeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	TimeBufferDesc.ByteWidth = sizeof(TimeBufferType);
	TimeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	TimeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TimeBufferDesc.MiscFlags = 0;
	TimeBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&TimeBufferDesc, NULL, &TimeBuffer);

	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Setup the description of the Camera buffer size.
	D3D11_BUFFER_DESC CameraBufferDesc;
	CameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	CameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	CameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CameraBufferDesc.MiscFlags = 0;
	CameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&CameraBufferDesc, NULL, &CameraBuffer);
}

void DepthTesselationShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}
void DepthTesselationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, XMFLOAT4 TesselationValues,
	ID3D11ShaderResourceView* Bumptexture, XMFLOAT4 time, XMFLOAT4 CameraPosition,ID3D11ShaderResourceView* texture)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	// Send tesselation values to hull shader
	TesselationBufferType* Tesselationptr;
	deviceContext->Map(TesselationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	Tesselationptr = (TesselationBufferType*)mappedResource.pData;
	Tesselationptr->tesselationLeft = TesselationValues.x;
	Tesselationptr->tesselationRight = TesselationValues.y;
	Tesselationptr->tesselationBottom = TesselationValues.z;
	Tesselationptr->tesselationInside = TesselationValues.w;
	deviceContext->Unmap(TesselationBuffer, 0);
	deviceContext->HSSetConstantBuffers(1, 1, &TesselationBuffer);

	// Send Camera values to hull shader
	CameraBufferType* Cameraptr;
	deviceContext->Map(CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	Cameraptr = (CameraBufferType*)mappedResource.pData;
	Cameraptr->cameraPosition = CameraPosition;
	deviceContext->Unmap(CameraBuffer, 0);
	deviceContext->HSSetConstantBuffers(2, 1, &CameraBuffer);

	// Send time values to domain shader
	TimeBufferType* timeptr;
	deviceContext->Map(TimeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timeptr = (TimeBufferType*)mappedResource.pData;
	timeptr->time = time;
	deviceContext->Unmap(TimeBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &TimeBuffer);

	// Set shader texture resource in the Domain shader.
	deviceContext->DSSetShaderResources(1, 1, &Bumptexture);
	deviceContext->DSSetSamplers(0, 1, &sampleState);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}
