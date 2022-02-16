// texture shader.cpp
#include "ShadowTexture.h"


ShadowTexture::ShadowTexture(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"shadow_vs.cso", L"MasterShadowPixelShader.cso");
}


ShadowTexture::~ShadowTexture()
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


void ShadowTexture::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC timeBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
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
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

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

void ShadowTexture::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, const XMMATRIX LightViewMatrix[13], const XMMATRIX LightprojectionMatrix[13],
	ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap[13], Light* light[8], bool ToggleNormals, float time)
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
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

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

	// Send control data to pixel shader
	PixelBufferType* PixelPtr;
	deviceContext->Map(PixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	PixelPtr = (PixelBufferType*)mappedResource.pData;
	if (ToggleNormals)
	{
		PixelPtr->ToggleNormals = 1;
	}
	else
	{
		PixelPtr->ToggleNormals = 0;
	}
	PixelPtr->WaterSettings.x = 0.0f;
	PixelPtr->WaterSettings.y = time;
	PixelPtr->ExtraPadding = 0.0f;
	deviceContext->Unmap(PixelBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &PixelBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 13, depthMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}

