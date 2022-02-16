// texture shader.cpp
#include "ShadowTesselation.h"


ShadowTesselation::ShadowTesselation(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"ShadowTesselation_vs.cso", L"ShadowTesselation_hs.cso", L"ShadowTesselation_ds.cso", L"MasterShadowPixelShader.cso");
}
ShadowTesselation::~ShadowTesselation()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void ShadowTesselation::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
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

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	// Setup the description of the Camera buffer size.
	D3D11_BUFFER_DESC CameraBufferDesc;
	CameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	CameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	CameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CameraBufferDesc.MiscFlags = 0;
	CameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&CameraBufferDesc, NULL, &CameraBuffer);

	// Setup Pixel Shader buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC PixelBufferDesc;
	PixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	PixelBufferDesc.ByteWidth = sizeof(LightBufferType);
	PixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	PixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	PixelBufferDesc.MiscFlags = 0;
	PixelBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&PixelBufferDesc, NULL, &PixelBuffer);

}

void ShadowTesselation::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}

void ShadowTesselation::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMMATRIX LightViewMatrix[13], const XMMATRIX LightprojectionMatrix[13],
	XMFLOAT4 TesselationValues,ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Bumptexture, ID3D11ShaderResourceView* depthMap[13], Light* light[8], XMFLOAT4 time, XMFLOAT4 CameraPosition, bool Normals)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* lightPtr;

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
		dataPtr->lightViewMatrix[i] = XMMatrixTranspose(LightViewMatrix[i]);
		dataPtr->lightProjectionMatrix[i] = XMMatrixTranspose(LightprojectionMatrix[i]);
	}


	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	//Additional
	//float lightType = 0.0f; //0 = point light, 1 = directional
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	// Send light data to pixel shader
	lightPtr = (LightBufferType*)mappedResource.pData;
	for (int i = 0; i < 8; i++)
	{
		lightPtr->ambient[i] = light[i]->getAmbientColour();
		lightPtr->diffuse[i] = light[i]->getDiffuseColour();
		lightPtr->position[i].x = light[i]->getPosition().x;
		lightPtr->position[i].y = light[i]->getPosition().y;
		lightPtr->position[i].z = light[i]->getPosition().z;
		lightPtr->position[i].w = 0.0f;

		lightPtr->attenuation[i].x = constantFactor;
		lightPtr->attenuation[i].y = linearFactor;
		lightPtr->attenuation[i].z = quadraticFactor;
		lightPtr->attenuation[i].w = lightType;

	}
	lightPtr->position[6].x = light[6]->getDirection().x;
	lightPtr->position[6].y = light[6]->getDirection().y;
	lightPtr->position[6].z = light[6]->getDirection().z;
	lightPtr->position[6].w = 0.0f;
	lightPtr->attenuation[6].w = 1.0f;
	lightPtr->attenuation[7].w = 2.0f;
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

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

	// Send control data to pixel shader
	PixelBufferType* PixelPtr;
	deviceContext->Map(PixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	PixelPtr = (PixelBufferType*)mappedResource.pData;
	if (Normals)
	{
		PixelPtr->ToggleNormals = 1;
	}
	else
	{
		PixelPtr->ToggleNormals = 0;
	}
	PixelPtr->WaterSettings.x = 1.0f;
	PixelPtr->WaterSettings.y = time.x;
	PixelPtr->ExtraPadding = 0.0f;
	deviceContext->Unmap(PixelBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &PixelBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 13, depthMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);

	// Set shader texture resource in the Domain shader.
	deviceContext->DSSetShaderResources(1, 1, &Bumptexture);
	deviceContext->DSSetSamplers(0, 1, &sampleState);
}

