// geometry shader.cpp
#include "DepthGeometryShader.h"

DepthGeometryShader::DepthGeometryShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"triangle_vs.cso", L"triangle_gs.cso", L"DepthMasterPixelShader.cso");
}

DepthGeometryShader::~DepthGeometryShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

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

void DepthGeometryShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
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
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);


	// Setup the description of the RNG buffer size.
	D3D11_BUFFER_DESC RNGBufferDesc;
	RNGBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	RNGBufferDesc.ByteWidth = sizeof(RNGBufferType);
	RNGBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	RNGBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	RNGBufferDesc.MiscFlags = 0;
	RNGBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&RNGBufferDesc, NULL, &RNGBuffer);


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
}

void DepthGeometryShader::initShader(const wchar_t* vsFilename, const wchar_t* gsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadGeometryShader(gsFilename);
}


void DepthGeometryShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture,float RNG)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);


	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	for (int i = 0; i < 13; i++)
	{
		dataPtr->lightViewMatrix[i] = tworld;
		dataPtr->lightProjectionMatrix[i] = tworld;
	}
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->GSSetConstantBuffers(0, 1, &matrixBuffer);

	// Send Rotation values to Geometry shader
	RNGBufferType* Geoptr;
	deviceContext->Map(RNGBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	Geoptr = (RNGBufferType*)mappedResource.pData;
	Geoptr->RNG = XMFLOAT4 (RNG,0.0f,0.0f,1.0f);

	deviceContext->Unmap(RNGBuffer, 0);
	deviceContext->GSSetConstantBuffers(1, 1, &RNGBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}
